#pragma once

#include "ast.h"
#include <string>

class WriterVisitor {
  public:
	WriterVisitor() = default;
	virtual ~WriterVisitor() = default;
	std::string virtual getOutput() const = 0;
	void virtual visitDocument(const Document &d) = 0;
	void virtual visitParagraph(const Paragraph &p) = 0;
	void virtual visitHeading(const Heading &h) = 0;
	void virtual visitBoldText(const BoldText &b) = 0;
	void virtual visitItalicText(const ItalicText &i) = 0;
	void virtual visitInlineCode(const InlineCode &c) = 0;
	void virtual visitFencedCodeBlock(const FencedCodeBlock &f) = 0;
	void virtual visitUnorderedList(const UnorderedList &u) = 0;
	void virtual visitOrderedList(const OrderedList &o) = 0;
	void virtual visitLink(const Link &l) = 0;
	void virtual visitThematicBreak(const ThematicBreak &t) = 0;
	void virtual visitBlockQuote(const BlockQuote &b) = 0;
	void virtual visitImage(const Image &i) = 0;
	void virtual visitTable(const Table &t) = 0;
	void virtual visitText(const Text &t) = 0;
	void virtual visitListItem(const ListItem &li) = 0;
	void virtual visitTableRow(const TableRow &tr) = 0;
	void virtual visitTableCell(const TableCell &tc) = 0;
	void virtual visitLineBreak(const LineBreak &lb) = 0;

  protected:
	void visitChildren(const ContainerNode &node) {
		for (auto it = node.childrenBegin(); it != node.childrenEnd(); ++it) {
			(*it)->accept(*this);
		}
	}
};