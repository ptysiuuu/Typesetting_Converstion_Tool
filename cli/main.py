import argparse
import os
import sys

project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
build_path = os.path.join(project_root, "build")
sys.path.insert(0, build_path)

try:
    import converter_bindings
except ImportError as e:
    print(f"DEBUG: Full error message: {e}")
    sys.exit(1)


def main():
    parser = argparse.ArgumentParser(description="Convert GFM to Typst or LaTeX")
    parser.add_argument("source", help="Path to the source markdown file")
    parser.add_argument(
        "--format",
        choices=["latex", "typst"],
        required=True,
        help="Target output format",
    )
    parser.add_argument("--output", help="Optional output destination path")

    args = parser.parse_args()

    try:
        # Call to the C++ dispatcher
        if not os.path.exists(args.source):
            print(f"Error: File '{args.source}' does not exist.")
            return
        with open(args.source, encoding="utf-8") as f:
            content = f.read()
        result = converter_bindings.convert_markdown(content, args.format)

        if args.output:
            with open(args.output, "w", encoding="utf-8") as f:
                f.write(result)
            print(f"Conversion successful! Output saved to '{args.output}'")
        else:
            print(result)
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
