#include "dispatcher.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(converter_bindings, m) {
	m.doc() = "C++ core bindings for MarkdownConverter tool";

	m.def("convert_markdown", &convert_markdown,
		  "Converts GFM Markdown string to LaTeX or Typst format",
		  py::arg("source_text"), py::arg("target_format"));

	py::register_exception<std::runtime_error>(m, "ConverterError");
}