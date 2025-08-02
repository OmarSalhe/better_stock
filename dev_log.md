### Day 1 (not really)

- Added onto FEN reader to handle additional fields (en passant, castling rights, etc.)
- Implemented a stack and necessary flags for game history later on
- Improved file structure and updated compile script  


**Next**: Define flags, implement stack functions, and test making and undoing moves  
**Bugs**: Linking issues (constants have same name)  
  
  
### Day 3 (yikes)

- Defined flags
- Implemented stack functionality (push, pop)  
- Refactored Pieces.h due to linking issues  
  

**Next**: add undo functionality to board, test functions, defining piece moves  
**Bugs**: Seg fault somewhere (damn), mix of macros and consts is ugly though, might need refactor  
  

### Day 7 (yikes-er)

- Added comments for clarity
- Fixed seg fault error (index OOB + int overflow)
- Added undoing moves and turn-based moves  
  
  
**Next**: keep track of piece locations, define move patterns  
**Bugs**: Issue in state encoding, first move undone gets replaced with a queen (one move after start), incorrectly parsing FEN string  
  

### Day 10

- Fixed FEN parsing errors, state encoding, first move undone error
- Improved readability in pop function
- Implemented piece lists (piece tracking)
  
  
**Next**: actually start move generation, define move patterns  
**Bugs**: linking errors I'm putting off  
  
  
### Day 11

- Fixed linking errors (lots of refactoring)
  
  
**Next**: add move checks (so flags can actually come into play), hurry up and define moves and start generating moves, look into magic bitboards  
**Bugs**: None I found.  
  
  
### Day 15 (damn...)
- Did way too much reading on magic bitboards
  
  

### Day 17
- Still trying to understand magic bitboards
  

### Day 18 (i understand now)
- Defined pawn captures and knight moves  
  
***Next***: Finish up move boards, make attack_table for sliding piece (blocker configs)  
***Bugs***: maybe some UB missed b/c of all the shifting done (none found so far though)  