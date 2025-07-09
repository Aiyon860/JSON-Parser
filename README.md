# JSON Parser in C

A minimalist JSON parser implemented in C. This project aims to parse and validate JSON strings according to the [ECMA-404 JSON Standard](https://www.json.org/json-en.html), supporting detailed error reporting, proper AST (Abstract Syntax Tree) generation, and tokenization.

---

## 📖 Overview

This project is a JSON parser built from scratch in C, designed for educational and experimental purposes. It includes:

- A tokenizer that processes JSON input into a sequence of tokens
- A recursive descent parser that validates and constructs an abstract syntax tree (AST)
- Error reporting with line and column positions
- AST pretty-printing for inspection

---

## 🛠 Tools & Technologies Used

| Tool           | Description                                                     |
|----------------|-----------------------------------------------------------------|
| `C`            | Core language                                                   |
| `Make`         | Build automation                                                |
| `GCC`  | Default C compiler used in this project                                                      |
| `POSIX`        | For directory and file handling (I/O and system-level calls)    |

---

## ⚙️ Requirements

- A C compiler  
  > This project uses GCC by default. If you prefer to use another compiler, please modify the `CC` variable in the Makefile accordingly.

- Windows OS (only supported on Windows for now)
- `make` installed (e.g., via Git Bash, MSYS2, or WSL)

## 🛠️ Build & Run (Windows Only)

Make sure you have GCC and `make` available in your environment (e.g., Git Bash or MSYS2 terminal). Then run:

### 🔧 Build the Project

```bash
make
```

### ▶️ Run the Program

```bash
make run
```

### ⚙️ Run with Custom Arguments
You can customize the execution by specifying:
- JSON_FOLDER: the folder containing JSON files to be tested
- COLOR_ENABLED: whether to enable colored output (true or leave empty)

```bash
make run JSON_FOLDER=tests/early_tests/step1
make run JSON_FOLDER=tests/early_tests/step1 COLOR_ENABLED=true
make run JSON_FOLDER=tests/early_tests/step2
```

### 🧹 Clean the Build Output

```bash
make clean
```

> ℹ️ The compiled binary will be placed inside the build/ directory.

## Project Structure

```
.
├── include/
│   ├── error.h
│   ├── helper.h
│   ├── json.h
│   ├── parser.h
│   ├── read_file.h
│   ├── token_type.h
│   └── tokenizer.h
├── src/
│   ├── error.c
│   ├── helper.c
│   ├── json.c
│   ├── parser.c
│   ├── read_file.c
│   ├── token_type.c
│   └── tokenizer.c
├── tests/
│   ├── early_tests
│   │   ├── step1
│   │   ├── step2
│   │   ├── step3
│   │   └── step4
│   ├── full_tests
│   │   ├── pass
│   │   ├── test1
│   │   ├── test2
│   └── └── test3
├── build/
├── Makefile
└── README.md
```