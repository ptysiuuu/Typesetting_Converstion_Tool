#pragma once

#include <memory>
#include <vector>

class WriterVisitor;

class Node {
  public:
	Node() = default;
	virtual ~Node() = default;
	void virtual accept(WriterVisitor &v) = 0;
};

class ContainerNode : public Node {
  public:
	~ContainerNode() = default;
	void addChild(std::unique_ptr<Node> pNode) {
		children.push_back(std::move(pNode));
	};

	auto childrenBegin() const { return children.begin(); };
	auto childrenEnd() const { return children.end(); };

  private:
	std::vector<std::unique_ptr<Node>> children;
};

class LeafNode : public Node {
  public:
	~LeafNode() = default;
	LeafNode() = default;
};

class Document : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class Paragraph : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class Heading : public ContainerNode {
  public:
	Heading(int level) : level(level) {};
	void accept(WriterVisitor &v) override;
	int getLevel() const { return level; }

  private:
	int level;
};

class BoldText : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class ItalicText : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class UnorderedList : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class OrderedList : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class Link : public ContainerNode {
  public:
	Link(std::string url) : url(url) {};
	void accept(WriterVisitor &v) override;
	std::string getUrl() const { return url; }

  private:
	std::string url;
};

class BlockQuote : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class Table : public ContainerNode {
  public:
	Table(size_t colCount = 0) : colCount(colCount) {}
	void accept(WriterVisitor &v) override;
	size_t getColCount() const { return colCount; }

  private:
	size_t colCount;
};

class ListItem : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class TableRow : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class TableCell : public ContainerNode {
  public:
	void accept(WriterVisitor &v) override;
};

class Image : public ContainerNode {
  public:
	Image(std::string url) : url(url) {};
	void accept(WriterVisitor &v) override;
	std::string getUrl() const { return url; }

  private:
	std::string url;
};

class Text : public LeafNode {
  public:
	Text(std::string text) : text(text) {};
	void accept(WriterVisitor &v) override;
	std::string getText() const { return text; };

  private:
	std::string text;
};

class ThematicBreak : public LeafNode {
  public:
	void accept(WriterVisitor &v) override;
};

class InlineCode : public LeafNode {
  public:
	InlineCode(std::string content) : content(content) {};
	void accept(WriterVisitor &v) override;
	std::string getContent() const { return content; };

  private:
	std::string content;
};

class FencedCodeBlock : public LeafNode {
  public:
	FencedCodeBlock(std::string language, std::string content)
		: language(language), content(content) {};
	void accept(WriterVisitor &v) override;
	std::string getLanguage() const { return language; }
	std::string getContent() const { return content; };

  private:
	std::string language;
	std::string content;
};

class LineBreak : public LeafNode {
  public:
	void accept(WriterVisitor &v) override;
};