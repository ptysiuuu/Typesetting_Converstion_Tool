#include "ast.h"
#include "parsers/gfm_parser.h"
#include "visitor.h"
#include <catch2/catch_test_macros.hpp>
#include <string>

class TestVisitor : public WriterVisitor {
  public:
	std::string result;
	std::string getOutput() const override { return ""; }
	void visitDocument(const Document &d) override {
		result += "[DOC] ";
		for (auto it = d.childrenBegin(); it != d.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/DOC]";
	}
	void visitParagraph(const Paragraph &p) override {
		result += "[P] ";
		for (auto it = p.childrenBegin(); it != p.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/P] ";
	}
	void visitHeading(const Heading &h) override {
		result += "[H" + std::to_string(h.getLevel()) + "] ";
		for (auto it = h.childrenBegin(); it != h.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/H] ";
	}
	void visitText(const Text &t) override {
		result += "TEXT(" + t.getText() + ") ";
	}
	void visitBoldText(const BoldText &b) override {
		result += "[B] ";
		for (auto it = b.childrenBegin(); it != b.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/B] ";
	}
	void visitItalicText(const ItalicText &i) override {
		result += "[I] ";
		for (auto it = i.childrenBegin(); it != i.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/I] ";
	}
	void visitInlineCode(const InlineCode &c) override {
		result += "CODE(" + c.getContent() + ") ";
	}
	void visitFencedCodeBlock(const FencedCodeBlock &f) override {
		result +=
			"[BLOCK:" + f.getLanguage() + "] " + f.getContent() + " [/BLOCK] ";
	}
	void visitLineBreak(const LineBreak &) override { result += "[BR] "; }
	void visitUnorderedList(const UnorderedList &u) override {
		result += "[UL] ";
		for (auto it = u.childrenBegin(); it != u.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/UL] ";
	}
	void visitOrderedList(const OrderedList &o) override {
		result += "[OL] ";
		for (auto it = o.childrenBegin(); it != o.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/OL] ";
	}
	void visitListItem(const ListItem &l) override {
		result += "[LI] ";
		for (auto it = l.childrenBegin(); it != l.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/LI] ";
	}
	void visitLink(const Link &l) override {
		result += "[LINK:" + l.getUrl() + "] ";
		for (auto it = l.childrenBegin(); it != l.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/LINK] ";
	}
	void visitImage(const Image &i) override {
		result += "[IMG:" + i.getUrl() + "] ";
		for (auto it = i.childrenBegin(); it != i.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/IMG] ";
	}
	void visitBlockQuote(const BlockQuote &b) override {
		result += "[QUOTE] ";
		for (auto it = b.childrenBegin(); it != b.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/QUOTE] ";
	}
	void visitThematicBreak(const ThematicBreak &) override {
		result += "[HR] ";
	}
	void visitTable(const Table &t) override {
		result += "[TABLE] ";
		for (auto it = t.childrenBegin(); it != t.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/TABLE] ";
	}
	void visitTableRow(const TableRow &r) override {
		result += "[TR] ";
		for (auto it = r.childrenBegin(); it != r.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/TR] ";
	}
	void visitTableCell(const TableCell &c) override {
		result += "[TD] ";
		for (auto it = c.childrenBegin(); it != c.childrenEnd(); ++it)
			(*it)->accept(*this);
		result += "[/TD] ";
	}
};

TEST_CASE("GFM Parser correctly builds the AST", "[parser]") {
	GFMParser parser;
	TestVisitor visitor;

	SECTION("Basic formatting (Bold, Italic, Code)") {
		auto doc = parser.parse("Hello **strong** and *italic* with `code`.");
		doc->accept(visitor);
		REQUIRE(
			visitor.result ==
			"[DOC] [P] TEXT(Hello ) [B] TEXT(strong) [/B] TEXT( and ) [I] "
			"TEXT(italic) [/I] TEXT( with ) CODE(code) TEXT(.) [/P] [/DOC]");
	}

	SECTION("Headings of different levels") {
		auto doc = parser.parse("# H1\n## H2");
		doc->accept(visitor);
		REQUIRE(visitor.result ==
				"[DOC] [H1] TEXT(H1) [/H] [H2] TEXT(H2) [/H] [/DOC]");
	}

	SECTION("Unordered list with nesting") {
		auto doc = parser.parse("* Item 1\n* Item 2");
		doc->accept(visitor);
		REQUIRE(visitor.result ==
				"[DOC] [UL] [LI] [P] TEXT(Item 1) [/P] [/LI] [LI] [P] "
				"TEXT(Item 2) [/P] [/LI] [/UL] [/DOC]");
	}

	SECTION("Links and Images") {
		auto doc =
			parser.parse("[Google](https://google.com) and ![Logo](img.png)");
		doc->accept(visitor);
		REQUIRE(visitor.result ==
				"[DOC] [P] [LINK:https://google.com] TEXT(Google) [/LINK] "
				"TEXT( and ) [IMG:img.png] TEXT(Logo) [/IMG] [/P] [/DOC]");
	}

	SECTION("Fenced Code Blocks") {
		auto doc = parser.parse("```cpp\nint main() {}\n```");
		doc->accept(visitor);
		REQUIRE(visitor.result ==
				"[DOC] [BLOCK:cpp] int main() {}\n [/BLOCK] [/DOC]");
	}

	SECTION("GFM Tables") {
		std::string table_md = "| A | B |\n"
							   "|---|---|\n"
							   "| 1 | 2 |";
		auto doc = parser.parse(table_md);
		doc->accept(visitor);

		REQUIRE(
			visitor.result ==
			"[DOC] [TABLE] [TR] [TD] TEXT(A) [/TD] [TD] TEXT(B) [/TD] [/TR] "
			"[TR] [TD] TEXT(1) [/TD] [TD] TEXT(2) [/TD] [/TR] [/TABLE] [/DOC]");
	}
}