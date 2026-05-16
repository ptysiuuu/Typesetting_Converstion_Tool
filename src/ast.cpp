#include "ast.h"
#include "visitor.h" // Tutaj bezpiecznie dołączamy pełną definicję wizytatora

void Document::accept(WriterVisitor &v) { v.visitDocument(*this); }

void Paragraph::accept(WriterVisitor &v) { v.visitParagraph(*this); }

void Heading::accept(WriterVisitor &v) { v.visitHeading(*this); }

void BoldText::accept(WriterVisitor &v) { v.visitBoldText(*this); }

void ItalicText::accept(WriterVisitor &v) { v.visitItalicText(*this); }

void UnorderedList::accept(WriterVisitor &v) { v.visitUnorderedList(*this); }

void OrderedList::accept(WriterVisitor &v) { v.visitOrderedList(*this); }

void Link::accept(WriterVisitor &v) { v.visitLink(*this); }

void BlockQuote::accept(WriterVisitor &v) { v.visitBlockQuote(*this); }

void Table::accept(WriterVisitor &v) { v.visitTable(*this); }

void ListItem::accept(WriterVisitor &v) { v.visitListItem(*this); }

void TableRow::accept(WriterVisitor &v) { v.visitTableRow(*this); }

void TableCell::accept(WriterVisitor &v) { v.visitTableCell(*this); }

void Image::accept(WriterVisitor &v) { v.visitImage(*this); }

void Text::accept(WriterVisitor &v) { v.visitText(*this); }

void ThematicBreak::accept(WriterVisitor &v) { v.visitThematicBreak(*this); }

void InlineCode::accept(WriterVisitor &v) { v.visitInlineCode(*this); }

void FencedCodeBlock::accept(WriterVisitor &v) {
	v.visitFencedCodeBlock(*this);
}

void LineBreak::accept(WriterVisitor &v) { v.visitLineBreak(*this); }