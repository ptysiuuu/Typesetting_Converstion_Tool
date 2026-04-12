import argparse
# import converter_bindings ->  compiled cpp module

def main():
    parser = argparse.ArgumentParser(description="Convert GFM to Typst or LaTeX")
    parser.add_argument("source", help="Path to the source markdown file")
    parser.add_argument("--format", choices=["latex", "typst"], required=True, help="Target output format")
    parser.add_argument("--output", help="Optional output destination path")

    args = parser.parse_args()

    try:
        # Call to the C++ dispatcher
        pass
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()