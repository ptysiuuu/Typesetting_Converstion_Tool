#include "ast.h"
#include "writers/latex_writer.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <memory>

using Catch::Matchers::ContainsSubstring;

TEST_CASE("LaTeXWriter generates correct LaTeX code", "[writer][latex]") {
	auto doc = std::make_unique<Document>();
	auto table = std::make_unique<Table>(2);
	auto row = std::make_unique<TableRow>();

	auto cell1 = std::make_unique<TableCell>();
	cell1->addChild(std::make_unique<Text>("Product"));

	auto cell2 = std::make_unique<TableCell>();
	auto bold = std::make_unique<BoldText>();
	bold->addChild(std::make_unique<Text>("Price & Tax"));
	cell2->addChild(std::move(bold));

	row->addChild(std::move(cell1));
	row->addChild(std::move(cell2));
	table->addChild(std::move(row));
	doc->addChild(std::move(table));

	SECTION("Standalone mode adds preamble and packages") {
		LaTeXWriter writer(true);
		doc->accept(writer);
		std::string output = writer.getOutput();

		CHECK_THAT(output, ContainsSubstring("\\documentclass{article}"));
		CHECK_THAT(output, ContainsSubstring("\\usepackage{booktabs}"));
		CHECK_THAT(output, ContainsSubstring("\\usepackage{hyperref}"));
		CHECK_THAT(output, ContainsSubstring("\\begin{document}"));
		CHECK_THAT(output, ContainsSubstring("\\end{document}"));
	}

	SECTION("Table formatting and escaping are correct") {
		LaTeXWriter writer(false);
		doc->accept(writer);
		std::string output = writer.getOutput();

		CHECK_THAT(output, ContainsSubstring("\\begin{tabular}{ll}"));
		CHECK_THAT(output, ContainsSubstring("\\toprule"));

		CHECK_THAT(output,
				   ContainsSubstring("Product & \\textbf{Price \\& Tax}"));

		CHECK_THAT(output, ContainsSubstring("\\\\"));
	}
}