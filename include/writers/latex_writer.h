#pragma once

#include "ast.h"
#include "visitor.h"
#include <string>

class LaTeXWriter : public WriterVisitor {
  public:
	LaTeXWriter(bool standalone = true) : standalone(standalone) {};

	std::string getOutput() const override { return output; }

	void visitDocument(const Document &d) override;
	void visitParagraph(const Paragraph &p) override;
	void visitHeading(const Heading &h) override;
	void visitBoldText(const BoldText &b) override;
	void visitItalicText(const ItalicText &i) override;
	void visitInlineCode(const InlineCode &c) override;
	void visitFencedCodeBlock(const FencedCodeBlock &f) override;
	void visitUnorderedList(const UnorderedList &u) override;
	void visitOrderedList(const OrderedList &o) override;
	void visitLink(const Link &l) override;
	void visitThematicBreak(const ThematicBreak &t) override;
	void visitBlockQuote(const BlockQuote &b) override;
	void visitImage(const Image &i) override;
	void visitTable(const Table &t) override;
	void visitText(const Text &t) override;
	void visitListItem(const ListItem &li) override;
	void visitTableRow(const TableRow &tr) override;
	void visitTableCell(const TableCell &tc) override;
	void visitLineBreak(const LineBreak &lb) override;

  private:
	std::string output;
	bool standalone;
	std::string escapeLatex(const std::string text);
};