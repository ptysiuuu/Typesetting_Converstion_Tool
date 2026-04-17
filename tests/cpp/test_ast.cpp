#include <ast.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AST architecture test", "[ast]") {
	class MockVisitor : public WriterVisitor {
	  public:
		std::string result;

		void visitDocument(const Document &) override {
			result += "START_DOC ";
		}
		void visitParagraph(const Paragraph &) override {
			result += "START_PARAGRAPH ";
		}
		void visitText(const Text &) override { result += "PLAIN_TEXT "; }
		void visitBoldText(const BoldText &) override {
			result += "BOLD_TEXT ";
		}
		void visitItalicText(const ItalicText &) override {}
		void visitInlineCode(const InlineCode &) override {}
		void visitFencedCodeBlock(const FencedCodeBlock &) override {}
		void visitUnorderedList(const UnorderedList &) override {}
		void visitOrderedList(const OrderedList &) override {}
		void visitLink(const Link &) override {}
		void visitThematicBreak(const ThematicBreak &) override {}
		void visitBlockQuote(const BlockQuote &) override {}
		void visitImage(const Image &) override {}
		void visitTable(const Table &) override { result += "TABLE "; }
		void visitHeading(const Heading &) override {}
	};

	Document doc;
	Paragraph paragraph;
	Text text("Hello, World!");
	BoldText boldText;
	Table table;

	MockVisitor visitor;
	doc.accept(visitor);
	paragraph.accept(visitor);
	text.accept(visitor);
	boldText.accept(visitor);
	table.accept(visitor);

	REQUIRE(visitor.result ==
			"START_DOC START_PARAGRAPH PLAIN_TEXT BOLD_TEXT TABLE ");
}