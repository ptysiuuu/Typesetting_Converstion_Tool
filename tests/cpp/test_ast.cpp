#include <ast.h>
#include <catch2/catch_test_macros.hpp>
#include <type_traits>

namespace {

std::size_t childCount(const ContainerNode &node) {
	return static_cast<std::size_t>(
		std::distance(node.childrenBegin(), node.childrenEnd()));
}

} // namespace

TEST_CASE("AST architecture test", "[ast]") {
	STATIC_REQUIRE(std::is_base_of_v<ContainerNode, Document>);
	STATIC_REQUIRE(std::is_base_of_v<ContainerNode, Paragraph>);
	STATIC_REQUIRE(std::is_base_of_v<ContainerNode, BoldText>);
	STATIC_REQUIRE(std::is_base_of_v<LeafNode, Text>);
	STATIC_REQUIRE(std::is_base_of_v<ContainerNode, Image>);

	class MockVisitor : public WriterVisitor {
	  public:
		std::string result;

		void visitDocument(const Document &d) override {
			result += "START_DOC ";
			for (auto it = d.childrenBegin(); it != d.childrenEnd(); ++it) {
				(*it)->accept(*this);
			}
			result += "END_DOC";
		}
		void visitParagraph(const Paragraph &p) override {
			result += "START_PARAGRAPH ";
			for (auto it = p.childrenBegin(); it != p.childrenEnd(); ++it) {
				(*it)->accept(*this);
			}
			result += "END_PARAGRAPH ";
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
		void visitListItem(const ListItem &) override {}
		void visitTableRow(const TableRow &) override {}
		void visitTableCell(const TableCell &) override {}
		void visitLineBreak(const LineBreak &) override {}
		std::string getOutput() const override { return ""; }
	};

	Document doc;
	auto paragraph = std::make_unique<Paragraph>();
	paragraph->addChild(std::make_unique<Text>("Hello, World!"));
	paragraph->addChild(std::make_unique<BoldText>());
	doc.addChild(std::move(paragraph));
	doc.addChild(std::make_unique<Table>());

	REQUIRE(childCount(doc) == 2);

	auto it = doc.childrenBegin();
	const auto *firstChild = it->get();
	REQUIRE(dynamic_cast<const Paragraph *>(firstChild) != nullptr);

	const auto *storedParagraph = dynamic_cast<const Paragraph *>(firstChild);
	REQUIRE(storedParagraph != nullptr);
	REQUIRE(childCount(*storedParagraph) == 2);

	auto paragraphIt = storedParagraph->childrenBegin();
	const auto *paragraphFirst = paragraphIt->get();
	const auto *storedText = dynamic_cast<const Text *>(paragraphFirst);
	REQUIRE(storedText != nullptr);
	CHECK(storedText->getText() == "Hello, World!");

	++paragraphIt;
	const auto *paragraphSecond = paragraphIt->get();
	REQUIRE(dynamic_cast<const BoldText *>(paragraphSecond) != nullptr);

	++it;
	REQUIRE(dynamic_cast<const Table *>(it->get()) != nullptr);

	Heading heading(3);
	CHECK(heading.getLevel() == 3);

	Link link("https://example.com");
	CHECK(link.getUrl() == "https://example.com");

	Image image("https://example.com/image.png");
	CHECK(image.getUrl() == "https://example.com/image.png");

	InlineCode inlineCode("x = 1");
	CHECK(inlineCode.getContent() == "x = 1");

	FencedCodeBlock fencedCode("cpp", "int main() {}");
	CHECK(fencedCode.getLanguage() == "cpp");
	CHECK(fencedCode.getContent() == "int main() {}");

	MockVisitor visitor;
	doc.accept(visitor);

	REQUIRE(visitor.result == "START_DOC START_PARAGRAPH PLAIN_TEXT BOLD_TEXT "
							  "END_PARAGRAPH TABLE END_DOC");
}