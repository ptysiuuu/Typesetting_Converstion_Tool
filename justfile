# Typesetting Conversion Tool runner

# Show available recipes
default:
	@just --list

# Setup

# Install all dependencies, build C++ module (via uv) and compile C++ tests
setup: conan-install sync build-cpp-tests

# Install Conan C++ dependencies
conan-install:
	conan install . --build=missing -pr profiles/ubuntu-llvm -s build_type=Release --output-folder=build

# Install Python dev dependencies and build C++ bindings (scikit-build-core)
sync:
	uv sync

# Configure CMake in Release mode (needed for C++ tests and compile_commands.json)
cmake-configure:
	cmake -B build \
		-DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
		-DCMAKE_PREFIX_PATH=build \
		-DCMAKE_CXX_COMPILER=clang++-18 \
		-DCMAKE_C_COMPILER=clang-18 \
		-DCMAKE_BUILD_TYPE=Release

# Build pure C++ native tests
build-cpp-tests: cmake-configure
	cmake --build build --target cpp_tests

# Testing

# Run all tests
test: test-cpp test-py

# Run C++ Catch2 tests
test-cpp:
	./build/cpp_tests

# Run Python pytest tests
test-py:
	uv run pytest tests/python/ -v

# Formatting

# Format all code (Python + C++)
format: format-py format-cpp

# Format Python code with black
format-py:
	uv run black .

# Format C++ code with clang-format
format-cpp:
	find src include bindings tests/cpp -name '*.cpp' -o -name '*.h' \
	  | xargs clang-format-18 -i

# Linting

# Lint all code (Python + C++)
lint: lint-py lint-cpp

# Lint Python code with ruff
lint-py:
	uv run ruff check .

# Lint C++ code with clang-tidy (requires compile_commands.json in build/)
lint-cpp:
	run-clang-tidy -p build

# Documentation

# Generate Doxygen C++ API documentation
docs:
	doxygen Doxyfile

# Utilities

# Remove all build artefacts
clean:
	rm -rf build/
	rm -rf _skbuild/

# Remove build artefacts and Python virtual environment
clean-all: clean
	rm -rf .venv/