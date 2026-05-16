# Typesetting Conversion Tool

A command-line tool for converting **GitHub Flavored Markdown (GFM)** to **LaTeX** or **Typst** format. The core conversion logic is implemented in C++23 and exposed to Python, which handles the CLI.

---

## Architecture Overview

The project is a hybrid C++/Python application:

- **C++ core** - GFM parsing (via `cmark-gfm`), AST construction, and output rendering (LaTeX / Typst writers).
- **Python bindings** - `pybind11` exposes `convert_markdown(source_text, target_format)` to Python.
- **Python CLI** - `cli/main.py` provides a command-line interface for end users.

---

## Prerequisites

- **CMake** >= 3.20
- **Conan** 2.x — `pip install conan`
- **Python** 3.11+
- **uv** — `pip install uv`
- **just** (recommended task runner) — see [just github](https://github.com/casey/just)

## Quick Start

The project uses just making setup very easy and straightforward:

```bash
just setup      # install all dependencies + build the C++ module
just test       # run all tests (C++ + Python)
just lint       # lint Python code with ruff
just format     # auto-format Python (black) and C++ (clang-format)
```

Run `just` with no arguments to list all available recipes.

---

## Tools & Code Quality

### Python

| Tool | Role | Config |
|------|------|--------|
| [black](https://black.readthedocs.io/) | Auto-formatter | `pyproject.toml` |
| [ruff](https://docs.astral.sh/ruff/) | Linter | `pyproject.toml` |
| [pytest](https://pytest.org/) | Test runner | — |

**Format Python code:**
```bash
uv run black .
# or via just:
just format-py
```

**Lint Python code:**
```bash
uv run ruff check .
# or via just:
just lint-py
```

### C++

| Tool | Role | Config |
|------|------|--------|
| [clang-format](https://clang.llvm.org/docs/ClangFormat.html) | Auto-formatter | `.clang-format` |
| [clang-tidy](https://clang.llvm.org/extra/clang-tidy/) | Linter / static analyser | `.clang-tidy` (optional) |

**Format C++ code:**
```bash
just format-cpp
```

**Lint C++ code** (requires a compilation database which is generated automatically by CMake):
```bash
just lint-cpp
```

---

## Building

### 1. Configure a Conan profile

```bash
conan profile detect --force
```

Project-provided profiles:

- **Linux / clang-18:** `profiles/ubuntu-llvm`
- **macOS / Apple Silicon:** `profiles/macos-llvm`

### 2. Install C++ dependencies via Conan

**Linux:**
```bash
conan install . --build=missing -pr profiles/ubuntu-llvm --output-folder=build
```

**macOS:**
```bash
conan install . --build=missing -pr profiles/macos-llvm --output-folder=build
```

### 3. Configure CMake

**Linux (clang-18):**
```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
  -DCMAKE_CXX_COMPILER=clang++-18 \
  -DCMAKE_C_COMPILER=clang-18 \
  -DCMAKE_BUILD_TYPE=Release
```

**macOS:**
```bash
cmake --preset macos-llvm
```

### 4. Compile

```bash
cmake --build build
```

This produces:

- `build/cpp_tests`
- `build/converter_bindings*.so`

---

## Running Tests

### C++ tests (Catch2)

```bash
./build/cpp_tests
# or:
just test-cpp
```

### Python tests (pytest)

Install Python dependencies first:

```bash
uv sync
```

Then run:

```bash
uv run pytest tests/python/ -v
# or:
just test-py
```

### All tests

```bash
just test
```

---

## Code Documentation

Generate C++ API documentation with Doxygen:

```bash
doxygen Doxyfile
# or:
just docs
```

---

## Project Structure

```
.
├── bindings/          # pybind11 Python bindings
├── cli/               # Python CLI entry point
├── include/           # C++ public headers
│   ├── parsers/
│   ├── writers/
│   └── utils/
├── src/               # C++ implementation
│   ├── parsers/
│   └── writers/
├── tests/
│   ├── cpp/           # Catch2 unit tests
│   └── python/        # pytest tests
├── profiles/          # Conan build profiles
├── justfile           # Task runner recipes
├── CMakeLists.txt
├── conanfile.txt
└── pyproject.toml
```

---

## Example Usage

### Output to file

```bash
uv run cli/main.py README.md --format typst --output readme.typ
```

### Print to stdout

```bash
uv run cli/main.py README.md --format latex
```
