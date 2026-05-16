#include "ast.h"
#include "writers/typst_writer.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <memory>

using Catch::Matchers::ContainsSubstring;

TEST_CASE("TypstWriter generates correct Typst code", "[writer][typst]") {
	auto doc = std::make_unique<Document>();
	auto table = std::make_unique<Table>(2);
	auto row = std::make_unique<TableRow>();

	auto cell1 = std::make_unique<TableCell>();
	cell1->addChild(std::make_unique<Text>("Product"));

	auto cell2 = std::make_unique<TableCell>();
	auto bold = std::make_unique<BoldText>();

	bold->addChild(std::make_unique<Text>("Price [#1]"));
	cell2->addChild(std::move(bold));

	row->addChild(std::move(cell1));
	row->addChild(std::move(cell2));
	table->addChild(std::move(row));
	doc->addChild(std::move(table));

	SECTION("Standalone mode adds basic document configuration") {
		TypstWriter writer(true);
		doc->accept(writer);
		std::string output = writer.getOutput();

		CHECK_THAT(output, ContainsSubstring("#set page(paper: \"a4\")"));
		CHECK_THAT(output,
				   ContainsSubstring("#set text(font: \"linux libertine\""));
		CHECK_THAT(output, ContainsSubstring("#set par(justify: true)"));
	}

	SECTION("Table formatting and escaping are correct") {
		TypstWriter writer(false);
		doc->accept(writer);
		std::string output = writer.getOutput();

		CHECK_THAT(output, ContainsSubstring("#table("));
		CHECK_THAT(output, ContainsSubstring("columns: 2"));

		CHECK_THAT(output, ContainsSubstring("[Product]"));

		CHECK_THAT(output, ContainsSubstring("[*Price \\[\\#1\\]*]"));
	}

	SECTION("Inline formatting handles escaping properly") {
		auto doc2 = std::make_unique<Document>();
		auto para = std::make_unique<Paragraph>();
		auto italic = std::make_unique<ItalicText>();

		italic->addChild(std::make_unique<Text>("Typst_Test"));
		para->addChild(std::move(italic));
		doc2->addChild(std::move(para));

		TypstWriter writer2(false);
		doc2->accept(writer2);
		std::string output2 = writer2.getOutput();

		CHECK_THAT(output2, ContainsSubstring("_Typst\\_Test_"));
	}
}