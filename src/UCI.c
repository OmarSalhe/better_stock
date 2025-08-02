#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Board.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TRUE    1
#define FALSE   0

#define max(x, y) (((x) >= (y))? (x): (y))
#define min(x, y) (((x) <= (y)) ? (x): (y))

#define VECTOR_COMPONENT 2     // Number of componenets in a 2-space position vector
#define LINE_VERTICES    9     // Number of vertices on a line in a closed 8x8 grid
#define TRI_VERTICES     3     // Number of vertices necessary for rendering a triangle
#define TRI_PER_SQUARE   2     // Number of triangles a square is comprised of

#define COLOR_COMPONENT 3                        // Number of components in a color vector (following RBG)
#define DARK_SQUARE     0.63f, 0.53f, 0.46f      // Coloring for dark squares in RGB format
#define LIGHT_SQUARE    0.84f, 0.77f, 0.72f      // Coloring for light squares in RGB format

#define MSG_LENGTH   512    // Length of error messages
#define MIN_WIN_SIZE 200    // Min dimension for window
#define MAX_WIN_SIZE 900    // Max dimension for window

#define glCheckError() \
GLenum error_code;\
while ((error_code = glGetError()) != GL_NO_ERROR) {\
    char *error;\
    switch(error_code) {\
        case GL_INVALID_ENUM:                   error = "INVALID ENUM"; break;\
        case GL_INVALID_VALUE:                  error = "INVALID VALUE"; break;\
        case GL_INVALID_OPERATION:              error = "INVALID OPERATION"; break;\
        case GL_STACK_OVERFLOW:                 error = "STACK OVERFLOW"; break;\
        case GL_STACK_UNDERFLOW:                error = "STACK UNDERFLOW"; break;\
        case GL_OUT_OF_MEMORY:                  error = "OUT OF MEMORY"; break;\
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID FRAMEBUFFER OPERATION"; break;\
    }\
\
    printf("%s | %s %d\n", error, __FILE__, __LINE__);\
}\

// Shader programs
const char *board_vertex_shader_source = "\
#version 330 core \
layout (location = 0) in vec2 aPos;\
layout (location = 1) in vec3 aColor;\
\
flat out vec3 color;\
\
void main() {\
    gl_Position = vec4(aPos, 0.0, 1.0);\
    color = aColor;\
}\0";

const char *board_frag_shader_source = "\
#version 330 core \
flat in vec3 color;\
\
out vec4 FragColor;\
\
void main() {\
    FragColor = vec4(color, 1.0f);\
}\0";

const char *piece_vertex_shader_source = "\
#version 330 core \
layout (location = 0) in vec2 aPos;\
layout (location = 1) in vec2 aTexCoord;\
layout (location = 2) in vec2 posOffset;\
layout (location = 3) in vec2 texOffset;\
\
out vec2 fTexCoord;\
\
void main() {\
    gl_Position = vec4(aPos + posOffset, 0.0, 1.0);\
    fTexCoord = aTexCoord + texOffset;\
}\0";

const char *piece_frag_shader_source = "\
#version 330 core \
in vec2 fTexCoord;\
\
uniform sampler2D pieceTexture;\
\
out vec4 FragColor;\
\
void main() {\
    FragColor = texture(pieceTexture, fTexCoord);\
}\0";

const char *a_piece_vertex_shader_source = "\
#version 330 core \
layout (location = 0) in vec2 aPos;\
layout (location = 1) in vec2 aTexCoord;\
\
out vec2 fTexCoord;\
\
void main() {\
    gl_Position = vec4(aPos, 0.0, 1.0);\
    fTexCoord = aTexCoord;\
}\0";

const char *a_piece_frag_shader_source = "\
#version 330 core\
\
in vec2 fTexCoord;\
\
out vec4 FragColor;\
\
uniform sampler2D pieceTexture;\
\
void main() {\
    FragColor = texture(pieceTexture, fTexCoord);\
}\0";


struct active_piece {
    // type of piece
    uint8_t type;

    // origin square
    int o_sq;

    // texture for piece
    float tex_x;
    float tex_y;
} typedef a_piece;

struct window_info {
    int width;

    double cursor_x;
    double cursor_y;

    int a_click;
} typedef w_info;


// Callback for context resizing; updates the window dimensions
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);

// Callback for cursor position; updates cursor position
void cursor_pos_callback(GLFWwindow* window, double x_pos, double y_pos);

// Callback for clicking
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Callback for pressing key
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Creates a window context for later rendering
GLFWwindow* create_context();

// Create shader program for board
GLuint compile_board_shader();

// Create shader program for pieces
GLuint compile_piece_shader();

// Create shader program for active piece
GLuint compile_a_piece_shader();

// Generate textures for pieces
GLuint gen_texture();

// Generate grid_indices necessary for forming 
void gen_grid_indices();

// Generates vertices necessary for an 8x8 grid 
void gen_grid_vertices();

// Generate vertices necessary for piece tiles
void gen_piece_offsets();

void gen_board_buffer(GLuint *boardVAO, GLuint *boardVBO, GLuint *boardEBO);

void gen_piece_buffer(GLuint *piecesVAO, GLuint *piecesEBO, GLuint *piecesVBO, GLuint *instanceVBO);

void gen_a_piece_buffer(GLuint *a_pieceVAO, GLuint *piecesEBO, GLuint *piecesVBO, GLuint *a_pieceVBO);

a_piece piece;
w_info win = {1200, 0, 0, FALSE};

/**
 * current indexing doesn't map correctly for sprites
 * 
 * ==piece index==     ==sprite index==
 *      PAWN        ->      QUEEN 
 *      KNIGHT      ->      KING
 *      BISHOP      ->      ROOK
 *      ROOK        ->      KNIGHT
 *      QUEEN       ->      BISHOP
 *      KING        ->      PAWN
 */
const int SPRITE_MAP[] = {-1, 5, 3, 4, 2, 0, 1};

const int square_indices[] = {
    0, 1, 3,  // First triangle
    0, 3, 2   // Second triangle
};

const float square_vertices[] = {
//     x       y        texture 
    0.0f,    0.0f,  0.0f,      0.0f,
    0.25f,   0.0f,  1.0f / 6,  0.0f, 
    0.0f,   0.25f,  0.0f,      0.5f,
    0.25f,  0.25f,  1.0f / 6,  0.5f
};

float board_vertices[LINE_VERTICES * LINE_VERTICES * (VECTOR_COMPONENT + COLOR_COMPONENT)];
int board_indices[SQUARES * TRI_PER_SQUARE * TRI_VERTICES];
float offsets[SQUARES * (VECTOR_COMPONENT + VECTOR_COMPONENT)];

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
    win.width = width;
    glViewport(0, 0, width, width);
}

void cursor_pos_callback(GLFWwindow* window, double mouse_x, double mouse_y) {
    win.cursor_x = mouse_x;
    win.cursor_y = mouse_y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (!win.a_click && (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS) {
        /**
         *  if piece clicked
         *      save origin square and piece
         *      remove piece from origin square
         *      place piece on mouse
         */


        int file = 2 * ROWS * win.cursor_x / win.width;
        int rank = ROWS * (1 - 2 * win.cursor_y / win.width);

        int t_sq = rank * ROWS + file;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        printf("%d %d %d\n", win.width, width, height);
        printf("%f %f\n", win.cursor_x / win.width, win.cursor_y / win.width);
        printf("rank: %d, file: %d, sq: %d, piece: %d\n", rank, file, t_sq, board[t_sq]);

        if (board[t_sq] && is_white(board[t_sq]) == white_turn) {
            win.a_click = TRUE;

            piece = (a_piece){board[t_sq], t_sq, SPRITE_MAP[piece_type(board[t_sq])] / 6.0f, is_white(board[t_sq]) / 2.0f};
            board[t_sq] = NONE;
        }
    }
    else if (win.a_click && (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_RELEASE) {
        /**
         *  if piece released
         *      if released onto legal square
         *          put piece on square
         *          update boards
         *      else
         *          put back to origin square
         *      
         */
        
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        int file = 2 * ROWS * win.cursor_x / win.width;
        int rank = ROWS * (1 - 2 * win.cursor_y / win.width);

        if (win.cursor_x > 0 && win.cursor_x < win.width && win. cursor_y > 0 && win.cursor_y < win.width) { // if legal move
            int t_sq = rank * ROWS + file;

            push_move(move_made(piece.o_sq, t_sq, 0), board[t_sq]);
            
            board[t_sq] = piece.type;
            
            int board_idx = piece_type(piece.type) + is_white(piece.type) * 6 - 1;
            printf("rank: %d, file: %d, bitboard: %d\n", rank, file, board_idx);
            printf("start: %d, end: %d\n", piece.o_sq, t_sq);

            bitboards[board_idx] &= ~(1ULL << piece.o_sq);
            bitboards[board_idx] |= 1ULL << t_sq;
            
            occupancy_board &= ~(1ULL << piece.o_sq);
            occupancy_board |= 1ULL << t_sq;

            white_turn = !white_turn;
            ply_count++;
            move_count += !white_turn;
        }
        else
            board[piece.o_sq] = piece.type;

        win.a_click = FALSE;
    }   
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!win.a_click && key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        printf("You pressed left\n");
        pop_move();
    }
}

GLFWwindow* create_context() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_FLOATING, GL_TRUE);
    glfwWindowHint(GLFW_POSITION_X, 1080);
    glfwWindowHint(GLFW_POSITION_Y, 450);

    GLFWwindow* window = glfwCreateWindow(600, 600, "Chess", NULL, NULL);
    if (window == NULL) {
        printf("ERROR::Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("ERROR::Failed to initialize GLAD\n");
        return NULL;
    } 

    glfwSetWindowAspectRatio(window, 1, 1);
    glfwSetWindowSizeLimits(window, MIN_WIN_SIZE, MIN_WIN_SIZE, MAX_WIN_SIZE, MAX_WIN_SIZE);

    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetKeyCallback(window, key_callback);
    return window;
}

void gen_grid_indices() {
    for (int i = 0, j = 0, n = SQUARES * TRI_PER_SQUARE * TRI_VERTICES; i < n; i += TRI_PER_SQUARE * TRI_VERTICES) {
        /**
         * j + 9 ------- j + 10 
         *  |              |
         *  |              |
         *  |              |
         *  |              |
         *  j --------- j + 1
         */

        //lower triangle
        board_indices[i] =        j += (j % LINE_VERTICES == (LINE_VERTICES - 1)); // form triangle with current vertex, if not at end of grid; else form triangle on next layer (i.e. the next vertex)
        board_indices[i + 1] =    j + 1;
        board_indices[i + 2] =    j + LINE_VERTICES + 1;

        // upper triangle
        board_indices[i + 3] =    j;
        board_indices[i + 4] =    j + LINE_VERTICES + 1;
        board_indices[i + 5] =    j + LINE_VERTICES;

        ++j; // go to next vertex
    }
}

void gen_grid_vertices() {
    const float OFFSET = 0.25f; // distance between every vertice

    for (int i = 0; i < LINE_VERTICES * LINE_VERTICES * (VECTOR_COMPONENT + COLOR_COMPONENT); i += (VECTOR_COMPONENT + COLOR_COMPONENT)) {
        int row = i / (LINE_VERTICES * (VECTOR_COMPONENT + COLOR_COMPONENT));
        int col = (i / (VECTOR_COMPONENT + COLOR_COMPONENT)) % LINE_VERTICES;

        // position
        board_vertices[i] =       -1.0f + OFFSET * col;
        board_vertices[i + 1] =   -1.0f + OFFSET * row;  
    
        // color
        int is_light = (row + col) & 1;
        board_vertices[i + 2] =   0.84f * is_light + 0.63f * !is_light;
        board_vertices[i + 3] =   0.77f * is_light + 0.53f * !is_light;
        board_vertices[i + 4] =   0.72f * is_light + 0.46f * !is_light;
    }
}

void gen_piece_offsets() {
    const float OFFSET = 0.25f; // distance between every corner

    int sq = 0;
    for (int i = 0, stride = VECTOR_COMPONENT + VECTOR_COMPONENT; i < SQUARES * stride ; i += stride) {
        /**
         * TL;DR
         * go through piece bitboards and provide an offset for the position and texture based on the board and specific square; only 64 squares, so 64 offsets needed
         * 
         *    ==position offsets==
         *  like the grid vertices -> just calculate where each bottom right corner is for every square 
         * 
         *    ==texture offsets==
         * apsect ratio = 2280x960 -> 2280p gets split into 6 tiles so 2280/6, 960 gets split into 2 tiles so 960/2 (scales for x and y)
         * 
         * -> using scales, piecePosX = pieceType * scaleX
         *                  piecePosY = is_white ? scaleY: 0
         * 
         * texture is projected onto 2D grid from [0, 1] x [0, 1]
         *      -> texCoordX = piecePosX / 2280
         *         texCoordY = piecePosY / 960
         *     
         * texture offset is just the shifting of vertices by a defined amount
         * + texCoord being the x,y coordinate of the bottom left corner of a texture
         * 
         *      -> texOffsetX = baseX * tileUVScaleX + texCoordX
         *         texOffsetY = baseY * tileUVScaleY + texCoordY
         * 
         * base needs to be some unit square that can be fit over all textures (just a 1x1 scaled down by the tiles scale)
         * visual:  
         *        (1,0) ----------- (1,1)
         *          |                  |
         *          |                  |                                        c ------- d
         *          |                  |                  ->                    |         |
         *          |                  |                                        |         |
         *          |                  |                                        |         |
         *          |                  |                                        a ------- b
         *        (0,0) ------------ (0,1)                                        
         *
         *  -> tileUVScaleX = tileWidth / textureWidth = (textureWidth / 6) / textureWidth = 1/6
         *     tileUCScaleY = tileHeight / textureHeight = (textureHeight / 2) / textureHeight = 1/2
         * 
         * 
         *  ==final eq==
         * texOffset = base * (1/6, 1/2) + (pieceType / 6, is_white? 1/2: 0)
         */
               
        // position of piece on board
        offsets[i] =       -1 + (sq % ROWS) * OFFSET;
        offsets[i + 1] =   -1 + (sq / ROWS) * OFFSET;

        // position of piece on the texture
        uint8_t piece = piece_type(board[sq]);
        uint8_t color = is_white(board[sq]);

        offsets[i + 2] = SPRITE_MAP[piece] / 6.0f;
        offsets[i + 3] = color / 2.0f;
        
        // printf("%d. %d -> %d %d\n", sq, board[sq], piece, color);
        // printf("%d. %d -> %d %d\n", sq, piece, SPRITE_MAP[piece], color);

        ++sq; // go to next square
    }
}

void gen_board_buffer(GLuint *boardVAO, GLuint *boardVBO, GLuint *boardEBO) {
    gen_grid_vertices();
    gen_grid_indices();

    glGenVertexArrays(1, boardVAO);
    glBindVertexArray(*boardVAO);
    
    glGenBuffers(1, boardVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *boardVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(board_vertices), board_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, boardEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *boardEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(board_indices), board_indices, GL_STATIC_DRAW);

    /**
     *      position    |        color          
     * -------------------------------------------
     *  |   x   |   y   |   R   |   G   |   B   |  
     * -------------------------------------------
     */

    // Position
    glVertexAttribPointer(0, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + COLOR_COMPONENT) * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, COLOR_COMPONENT, GL_FLOAT, GL_FALSE,  (VECTOR_COMPONENT + COLOR_COMPONENT) * sizeof(float), (void *)(VECTOR_COMPONENT * sizeof(float)));
    glEnableVertexAttribArray(1);

    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
}

void gen_piece_buffer(GLuint *piecesVAO, GLuint *piecesEBO, GLuint *piecesVBO, GLuint *instanceVBO) {
    gen_piece_offsets();

    if (!*piecesVAO) {
        glGenVertexArrays(1, piecesVAO);
        glGenBuffers(1, piecesEBO);
        glGenBuffers(1, piecesVBO);
        glGenBuffers(1, instanceVBO);
    }
    
    glBindVertexArray(*piecesVAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *piecesEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    // Vertex buffer for static geometry
    glBindBuffer(GL_ARRAY_BUFFER, *piecesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

    

    /**
     *      base pos    |    base tex    |   pos offset     |    tex offset
     * ----------------------------------------------------------------------------
     *  |   x   |   y   |   u   |   v   |    x1   |    y1   |    u1   |    v1   |
     * ----------------------------------------------------------------------------
     *                                            ...                 ...
     */

    // Base geometry
    glVertexAttribPointer(0, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Base texture
    glVertexAttribPointer(1, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (void *)(VECTOR_COMPONENT * sizeof(float)));
    glEnableVertexAttribArray(1);
    

    // Vertex buffer for each instance of geometry
    glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(offsets), offsets, GL_STREAM_DRAW);
    
    // Position offset
    glVertexAttribPointer(2, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (void *)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    // Texture offsets
    glVertexAttribPointer(3, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (void *)(VECTOR_COMPONENT * sizeof(float)));
    glVertexAttribDivisor(3, 1);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void gen_a_piece_buffer(GLuint *a_pieceVAO, GLuint *pieceEBO, GLuint *pieceVBO, GLuint *a_pieceVBO) {
    if (!*a_pieceVAO) {
        glGenVertexArrays(1, a_pieceVAO);
        glGenBuffers(1, a_pieceVBO);
    }

    glBindVertexArray(*a_pieceVAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *pieceEBO);
    glBindBuffer(GL_ARRAY_BUFFER, *pieceVBO);

    glVertexAttribPointer(0, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (void *)(VECTOR_COMPONENT * sizeof(float)));
    glEnableVertexAttribArray(1);

    float board_x = 2 * ROWS * win.cursor_x / win.width;
    float board_y = ROWS * (1 - 2 * win.cursor_y / win.width);

    glBindBuffer(GL_ARRAY_BUFFER, *a_pieceVBO);
    glBufferData(GL_ARRAY_BUFFER, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (float[]){-1.125f + .25f * board_x, -1.125f + .25 * board_y, piece.tex_x, piece.tex_y}, GL_STREAM_DRAW);

    glVertexAttribPointer(2, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (void *)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, VECTOR_COMPONENT, GL_FLOAT, GL_FALSE, (VECTOR_COMPONENT + VECTOR_COMPONENT) * sizeof(float), (void *)(VECTOR_COMPONENT * sizeof(float)));
    glVertexAttribDivisor(3, 1);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint gen_texture() {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    const float border_color[] = {1.0f, 0.0f, 0.0f, 0.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);  

    int width, height, num_channels;
    uint8_t *data = stbi_load("pieces.png", &width, &height, &num_channels, 0);

    if (data == NULL) {
        printf("ERROR::TEXTURE::LOADING:Failed to load texture\n");
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture);

    stbi_image_free(data);
    
    return texture;
}

GLuint compile_board_shader() {
    GLuint board_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(board_vertex_shader, 1, &board_vertex_shader_source, NULL);
    glCompileShader(board_vertex_shader);

    int success;
    char info_log[MSG_LENGTH];
    glGetShaderiv(board_vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(board_vertex_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s\n", info_log);
        return 0;
    }

    GLuint board_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(board_fragment_shader, 1, &board_frag_shader_source, NULL);
    glCompileShader(board_fragment_shader);

    glGetShaderiv(board_fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(board_fragment_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED %s\n", info_log);
        return 0;
    }

    GLuint board_shader = glCreateProgram();
    glAttachShader(board_shader, board_vertex_shader);
    glAttachShader(board_shader, board_fragment_shader);
    glLinkProgram(board_shader);

    // check for linking errors
    glGetProgramiv(board_shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(board_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED %s\n", info_log);
        return 0;
    }

    glDeleteShader(board_vertex_shader);
    glDeleteShader(board_fragment_shader);

    return board_shader;
}

GLuint compile_piece_shader() {
    int success;
    char info_log[MSG_LENGTH];
    GLuint piece_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(piece_vertex_shader, 1, &piece_vertex_shader_source, NULL);
    glCompileShader(piece_vertex_shader);

    glGetShaderiv(piece_vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(piece_vertex_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::PIECE::SHADER::VERTEX::COMPILATION_FAILED %s\n", info_log);
        return 0;
    }

    GLuint piece_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(piece_frag_shader, 1, &piece_frag_shader_source, NULL);
    glCompileShader(piece_frag_shader);

    glGetShaderiv(piece_frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(piece_frag_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::PIECE::SHADER::FRAGMENT::COMPILATION_FAILED %s\n", info_log);
        return 0;
    }

    GLuint piece_shader = glCreateProgram();
    glAttachShader(piece_shader, piece_vertex_shader);
    glAttachShader(piece_shader, piece_frag_shader);
    glLinkProgram(piece_shader);

    // check for linking errors
    glGetProgramiv(piece_shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(piece_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::PIECE::SHADER::PROGRAM::LINKING_FAILED %s\n", info_log);
        return 0;
    }

    glDeleteShader(piece_vertex_shader);
    glDeleteShader(piece_frag_shader);

    return piece_shader;
}

GLuint compile_a_piece_shader() {
    int success;
    char info_log[MSG_LENGTH];
    GLuint a_piece_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(a_piece_vertex_shader, 1, &a_piece_vertex_shader_source, NULL);
    glCompileShader(a_piece_vertex_shader);

    glGetShaderiv(a_piece_vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(a_piece_vertex_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::ACTIVE PIECE::SHADER::VERTEX::COMPILATION_FAILED %s\n", info_log);
        return 0;
    }

    GLuint a_piece_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(a_piece_frag_shader, 1, &a_piece_frag_shader_source, NULL);
    glCompileShader(a_piece_frag_shader);

    glGetShaderiv(a_piece_frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(a_piece_frag_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::ACTIVE PIECE::SHADER::FRAGMENT::COMPILATION_FAILED %s\n", info_log);
        return 0;
    }

    GLuint a_piece_shader = glCreateProgram();
    glAttachShader(a_piece_shader, a_piece_vertex_shader);
    glAttachShader(a_piece_shader, a_piece_frag_shader);
    glLinkProgram(a_piece_shader);

    // check for linking errors
    glGetProgramiv(a_piece_shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(a_piece_shader, MSG_LENGTH, NULL, info_log);
        printf("ERROR::ACTIVE PIECE::SHADER::PROGRAM::LINKING_FAILED %s\n", info_log);
        return 0;
    }

    glDeleteShader(a_piece_vertex_shader);
    glDeleteShader(a_piece_frag_shader);

    return a_piece_shader;
}


int main() {
    //   for debugging
    // float *vertices = gen_grid_vertices();
    // for (uint i = 0, n = LINE_VERTICES * LINE_VERTICES * (VECTOR_COMPONENT + COLOR_COMPONENT); i < n; i += (VECTOR_COMPONENT + COLOR_COMPONENT))
    //     printf("%d. pos: %f %f color: %f %f %f\n", i/(VECTOR_COMPONENT + COLOR_COMPONENT), vertices[i], vertices[i + 1], vertices[i + 2], vertices[i + 3], vertices[i + 4]);
    //
    // free(vertices);
    //
    // uint *indices = gen_indices();
    // for (uint i = 0; i < SQUARES * TRI_PER_SQUARE * TRI_VERTICES; i += TRI_PER_SQUARE * TRI_VERTICES)
    //     printf("%d. T1: %d %d %d T2: %d %d %d\n", i / (TRI_PER_SQUARE * TRI_VERTICES), indices[i], indices[i + 1], indices[i + 2], indices[i + 3], indices[i + 4], indices[i + 5]);
    //
    // free(indices);
    //
    // FEN_reader(NULL);
    // float *offsets = gen_piece_offsets();
    //
    // for (int i = 0; i < SQUARES * (VECTOR_COMPONENT + VECTOR_COMPONENT); i += (VECTOR_COMPONENT + VECTOR_COMPONENT))
    //     printf("%d. %f %f; %f %f \n", i / (VECTOR_COMPONENT + VECTOR_COMPONENT), offsets[i], offsets[i + 1], offsets[i + 2], offsets[i + 3]);
    // free(offsets);

    // Create window
    GLFWwindow *window = create_context();
    if (window == NULL)
        return -1;

    // Compile + Link shaders
    GLuint board_shader;
    if (!(board_shader = compile_board_shader()))
        return -1;

    GLuint piece_shader;
    if (!(piece_shader = compile_piece_shader()))
        return -1;

    GLuint a_piece_shader;
    if (!(a_piece_shader = compile_piece_shader()))
        return -1;

    GLuint texture;
    if (!(texture = gen_texture()))
        return -1;
    

    FEN_reader(NULL);
    //     test positions
    // FEN_reader("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    

    GLuint boardVAO = 0, boardVBO = 0, boardEBO = 0;
    gen_board_buffer(&boardVAO, &boardVBO, &boardEBO);

    GLuint piecesVAO = 0, piecesEBO = 0, piecesVBO = 0, instanceVBO = 0;
    gen_piece_buffer(&piecesVAO, &piecesEBO, &piecesVBO, &instanceVBO);

    GLuint a_pieceVAO = 0, a_pieceVBO = 0;

    

    // debugging
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Render board
        glUseProgram(board_shader);
        glBindVertexArray(boardVAO);
        glDrawElements(GL_TRIANGLES, SQUARES * TRI_PER_SQUARE * TRI_VERTICES, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        // // Render pieces
        
        gen_piece_buffer(&piecesVAO, &piecesEBO, &piecesVBO, &instanceVBO);
        glUseProgram(piece_shader);
        glBindVertexArray(piecesVAO);
        glDrawElementsInstanced(GL_TRIANGLES, TRI_PER_SQUARE * TRI_VERTICES, GL_UNSIGNED_INT, 0, SQUARES);
        
        glBindVertexArray(0);
        glUseProgram(0);
        
        if (win.a_click) {
            gen_a_piece_buffer(&a_pieceVAO, &piecesEBO, &piecesVBO, &a_pieceVBO);
            glUseProgram(a_piece_shader);
            glBindVertexArray(a_pieceVAO);
            glDrawElements(GL_TRIANGLES, TRI_PER_SQUARE * TRI_VERTICES, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        }
        // glfw: swap buffers and poll IO events (e.g. keys pressed/released, mouse moved, etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean up
    glDeleteVertexArrays(1, &boardVAO);
    glDeleteBuffers(1, &boardVBO);
    glDeleteBuffers(1, &boardEBO);
    glDeleteProgram(board_shader);

    glDeleteVertexArrays(1, &piecesVAO);
    glDeleteBuffers(1, &piecesVBO);
    glDeleteBuffers(1, &piecesEBO);
    glDeleteProgram(piece_shader);

    glDeleteVertexArrays(1, &a_pieceVAO);
    glDeleteBuffers(1, &a_pieceVBO);
    glDeleteProgram(a_piece_shader);
    
    glfwTerminate();
    return 0;
}