import pytest
import subprocess
import os

def test_cli_help():
    result = subprocess.run(["python3", "cli/main.py", "--help"], capture_output=True, text=True)
    assert result.returncode == 0
    assert "Convert GFM to Typst or LaTeX" in result.stdout

def test_cli_missing_file():
    result = subprocess.run(["python3", "cli/main.py", "non_existent.md", "--format", "latex"], capture_output=True, text=True)
    assert "Error: File 'non_existent.md' does not exist." in result.stdout

def test_cli_conversion_to_file(tmp_path):
    d = tmp_path / "sub"
    d.mkdir()
    input_file = d / "test.md"
    output_file = d / "out.tex"
    input_file.write_text("# Hello")

    result = subprocess.run([
        "python3", "cli/main.py", str(input_file), 
        "--format", "latex", 
        "--output", str(output_file)
    ], capture_output=True, text=True)

    assert result.returncode == 0
    assert os.path.exists(output_file)

def test_cli_input_without_output(tmp_path):
    input_file = tmp_path / "test.md"
    input_file.write_text("# Hello")

    result = subprocess.run([
        "python3", "cli/main.py",
        str(input_file),
        "--format", "latex"
    ], capture_output=True, text=True)

    assert result.returncode == 0
    assert "Hello" in result.stdout


def test_cli_output_without_input():
    result = subprocess.run([
        "python3", "cli/main.py",
        "--format", "latex",
        "--output", "out.tex"
    ], capture_output=True, text=True)

    assert result.returncode == 2
    assert "usage:" in result.stderr.lower()
    assert "the following arguments are required: source" in result.stderr.lower()

def test_cli_invalid_markdown_content(tmp_path):
    input_file = tmp_path / "file.weird"
    input_file.write_text("<not markdown at all>")

    result = subprocess.run([
        "python3", "cli/main.py",
        str(input_file),
        "--format", "latex"
    ], capture_output=True, text=True)

    assert result.returncode == 0
    assert "unsupported node type" in result.stdout.lower()


def test_cli_txt_file_with_markdown(tmp_path):
    input_file = tmp_path / "file.txt"
    input_file.write_text("# Hello from TXT")

    result = subprocess.run([
        "python3", "cli/main.py",
        str(input_file),
        "--format", "latex"
    ], capture_output=True, text=True)

    assert result.returncode == 0
    assert "Hello" in result.stdout
