# Typesetting Conversion Tool

A command-line tool for converting **GitHub Flavored Markdown (GFM)** to **LaTeX** or **Typst** format. The core conversion logic is implemented in C++23 and exposed to Python, which handles the CLI.

---

## Architecture Overview

The project is a hybrid C++/Python application:

- **C++ core** - GFM parsing (via `cmark-gfm`), AST construction, and output rendering (LaTeX / Typst writers).
- **Python bindings** - `pybind11` exposes `convert_markdown(source_text, target_format)` to Python.
- **Python CLI** - `cli/main.py` provides a command-line interface for end users.

## Prerequisites

### Common

- **CMake** >= 3.20
- **Conan** 2.x - `pip install conan`
- **Python** 3.11+
- **uv** - `pip install uv`

---

## Building

### Configure a Conan profile

```bash
conan profile detect --force
```

There are also project provided profiles:

- **Linux / clang-18:** `profiles/ubuntu-llvm`
- **macOS / Apple Silicon:** `profiles/macos-llvm`

### Install C++ dependencies via Conan

**Linux:**
```bash
conan install . --build=missing -pr profiles/ubuntu-llvm --output-folder=build
```

### Configure CMake

**Linux (clang-18):**
```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
  -DCMAKE_CXX_COMPILER=clang++-18 \
  -DCMAKE_C_COMPILER=clang-18 \
  -DCMAKE_BUILD_TYPE=Debug
```

### Compile

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
```

### Python tests (pytest)

Install Python dependencies first:

```bash
uv sync
```

Then run:

```bash
uv run pytest tests/python/ -v
```

## Code Documentation

You can use doxygen for C++ API documentation

```bash
doxygen Doxyfile
```

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
├── CMakeLists.txt
├── conanfile.txt
└── pyproject.toml
```

## Example usage

### Creating a output file:

`python3 cli/main.py README.md --format typst --output readme.typ`

### Print to stdout:

`python3 cli/main.py README.md --format latex`