
// TODO:  still not implemented so maght be changed earlier
#include "dispatcher.h"
#include "parsers/gfm_parser.h"
#include "writers/latex_writer.h"
#include "writers/typst_writer.h"
#include <stdexcept>

std::string convert_markdown(const std::string &source_text,
							 const std::string &target_format) {
	GFMParser
		parser; //  TODO: TO BE CHANGED IN CASE THERE IS A DIFFERENT PARSER NAME
	auto document = parser.parse(source_text);

	if (!document) {
		throw std::runtime_error("Failed to parse Markdown source.");
	}

	if (target_format == "latex") {
		LaTeXWriter writer;
		document->accept(writer);
		return writer.getOutput();
	} else if (target_format == "typst") {
		TypstWriter writer;
		document->accept(writer);
		return writer.getOutput();
	} else {
		throw std::invalid_argument("Unsupported target format: " +
									target_format);
	}
}