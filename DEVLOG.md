# Development Log

## Project Setup

- Created GitHub repository and initialized project

## Folder Structure

- Created src, include, bin, obj folders
- Added empty source files: main.c, shell.c, exec.c
- Added empty header files: lexer.h, exec.h, shell.h

## .gitignore

- Added .gitignore to exclude compiled files and editor files

## Makefile

- Added Makefile to automate compilation

### Test:

- ran make and got undefined reference to main error
- added temporary main() to verify build works
- ran make again and compiled successfully
- bin/shell created and runs correctly

### Bug:

- empty main.c caused linker error

### Fix:

- added temporary main() function

## lexer.h

- defined tokenlist struct with items array and size
- declared all function signatures

### Bugs Found:

- forgot to include lexer.h in main.c
- typo tokemlist instead of tokenlist in new_tokenlist
- missing semicolon after free_tokens declaration
- missing ) in free_tokens declaration

### Fix:

- added #include "lexer.h" in main.c
- corrected tokemlist to tokenlist
- added missing semicolon
- added missing ) in free_tokens
- removed extra semicolon after main()

### Test:

- compiled successfully
- lexer.h loaded successfully in main.c

## lexer.c - new_tokenlist()

- creates empty tokenlist
- allocates memory for items array

### Bug:

- type caste error for using %d with size_t type

### Fix:

- cast size_t to int

### Test:

- ran ./bin/shell
- output: size: 0
- working correctly
