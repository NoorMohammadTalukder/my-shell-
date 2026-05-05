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
- ran make again and  compiled successfully
- bin/shell created and runs correctly

### Bug:
- empty main.c caused linker error
### Fix:
- added temporary main() function
