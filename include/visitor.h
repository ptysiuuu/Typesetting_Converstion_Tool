class Paragraph;
class Heading;
class BoldText;
class ItalicText;
class InlineCode;
class FencedCodeBlock;
class UnorderedList;
class OrderedList;
class Link;
class ThematicBreak;
class BlockQuote;
class Image;
class Table;
class Text;
class Document;

class WriterVisitor {
  public:
	WriterVisitor() = default;
	virtual ~WriterVisitor() = default;
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
};