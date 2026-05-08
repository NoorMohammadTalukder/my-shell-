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

## lexer.c - add_token()

- adds one word into tokenlist
- grows the items array by using realloc

### Test:

- added ls and -la to tokenlist
- output: size: 0, token[0]: ls, token[1]: -la
- working correctly

## lexer.c - get_input()

- reads input using fgets
- handles any length input using realloc
- stops reading when newline found

### Bug:

- NUL typo instead of NULL
- strchr missing second argument '\n'

### Fix:

- changed NUL to NULL
- added '\n' as second argument to strchr

### Test:

- input: test test
- output: your input is: test test
- working correctly

## lexer.c - get_tokens()

- splits input string into tokens using strtok

### Bug:

- missing ) in for loop in main.c
- logical error: get_tokens result not used
- was printing old tokenlist instead of new one from input

### Fix:

- added missing ) after i++
- replaced old tokenlist with get_tokens(input)

### Test:

- input: cd src
- output: token[0]: cd, token[1]: src
- working correctly

## lexer.c - free_tokens()

- frees each token string first
- then frees items array
- then frees tokenlist itself

### Test:

- input: cd src
- output: token[0]: cd, token[1]: src
- memory freed after use
- working correctly

## lexer.c - expand_env_vars()

- replaces $VAR tokens with actual env values

### Bug 1:

- used } instead of ] in tokens->items[i}
- compiler showed expected ] before } token

### Fix 1:

- changed } to ] in tokens->items[i]

### Bug 2:

- wrote getenv(tok) instead of getenv(tok + 1)
- tested echo $HOME and token showed empty

### Fix 2:

- realized getenv needs the name without $ sign
- tok + 1 moves pointer one step forward skipping $

### Test:

- input: echo $HOME
- output: token[0]: echo, token[1]: /root

## lexer.c - expand_tilde()

- handles ~ alone with home and ~/something cases

### Test:

- input: cd ~
- output: token[0]: cd, token[1]: /root
- working correctly

## exec.h

- declared find_executable
- declared run_foreground

### Test:

- commented out other code in main.c
- included exec.h and printed "working"
- compiled successfully
- output: working
- working fine

## exec.c - find_executable() and run_foreground()

- find_executable searches $PATH for command
- run_foreground forks child process to run command

### Bug 1 - syntax:

- missing semicolon after malloc in str_copy
- compiler showed: expected , or ; before if

### Bug 2 - syntax:

- missing semicolon after int status in run_foreground
- compiler showed: expected , or ; before waitpid

### Fix:

- added semicolons in str_copy and run_foreground

## main.c - testing exec.c

### Bug:

- fullpath variable was undeclared
- compiler showed: error fullpath undeclared

### Fix:

- added fullpath in main.c

### Test:

- input: ls
- output: found /usr/bin/ls, files listed correctly
- input: ls -la
- working correctly
