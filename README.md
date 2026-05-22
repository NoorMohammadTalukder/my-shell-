# my-shell

A simple Unix shell built in C for COP4610.

## Features

- Command execution via PATH search
- Input/output redirection (`<` and `>`)
- Piping (`|`)
- Background processing (`&`)
- Builtins: `cd`, `exit`, `jobs`
- Environment variable expansion (`$VAR`)
- Tilde expansion (`~`)

## Compile

```bash
make clean
make
```

## Run

```bash
./bin/shell
```
