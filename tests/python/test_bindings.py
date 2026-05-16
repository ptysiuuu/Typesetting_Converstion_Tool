import os
import sys

sys.path.append(os.path.abspath("build"))
import pytest

import converter_bindings


def test_basic_conversion_call():
    result = converter_bindings.convert_markdown("# Title", "latex")
    assert isinstance(result, str)
    assert len(result) > 0


def test_unsupported_format():
    with pytest.raises(Exception) as excinfo:
        converter_bindings.convert_markdown("# Test", "html")
    assert "Unsupported target format" in str(excinfo.value)


def test_empty_input():
    result = converter_bindings.convert_markdown("", "typst")
    assert isinstance(result, str)


def test_invalid_markdown_content():
    with pytest.raises(Exception) as excinfo:
        converter_bindings.convert_markdown("<html>not markdown</html>", "latex")

    assert "unsupported" in str(excinfo.value).lower()
