#include "gfm_parser.h"
#include "ast.h"
#include <cmark-gfm-core-extensions.h>
#include <cmark-gfm.h>
#include <memory>
#include <stack>
#include <stdexcept>
#include <string>

std::unique_ptr<Document> GFMParser::parse(const std::string &markdown_text) {
	cmark_gfm_core_extensions_ensure_registered();

	cmark_parser *parser = cmark_parser_new(CMARK_OPT_DEFAULT);

	cmark_syntax_extension *table_extension =
		cmark_find_syntax_extension("table");
	if (table_extension != nullptr) {
		cmark_parser_attach_syntax_extension(parser, table_extension);
	}
	// In future extensions can be added here in the same way

	cmark_parser_feed(parser, markdown_text.c_str(), markdown_text.length());
	cmark_node *raw_root = cmark_parser_finish(parser);

	cmark_parser_free(parser);

	if (raw_root == nullptr) {
		throw std::runtime_error(
			"Error: cmark-gfm failed to parse the document");
	}

	auto deleter = [](cmark_node *node) { cmark_node_free(node); };
	std::unique_ptr<cmark_node, decltype(deleter)> safe_root(raw_root, deleter);

	auto document = std::make_unique<Document>();
	std::stack<ContainerNode *> node_stack;
	node_stack.push(document.get());

	cmark_iter *iter = cmark_iter_new(safe_root.get());
	cmark_event_type ev_type;

	while ((ev_type = cmark_iter_next(iter)) != CMARK_EVENT_DONE) {

		cmark_node *cur = cmark_iter_get_node(iter);
		cmark_node_type type = cmark_node_get_type(cur);

		if (ev_type == CMARK_EVENT_ENTER) {
			switch (type) {
			case CMARK_NODE_DOCUMENT:
				break;

			case CMARK_NODE_PARAGRAPH: {
				auto para_node = std::make_unique<Paragraph>();
				ContainerNode *raw_para = para_node.get();
				node_stack.top()->addChild(std::move(para_node));
				node_stack.push(raw_para);
				break;
			}
			case CMARK_NODE_TEXT: {
				const char *literal = cmark_node_get_literal(cur);
				std::string text_content = literal ? literal : "";

				auto text_node = std::make_unique<Text>(text_content);
				node_stack.top()->addChild(std::move(text_node));
				break;
			}
				// TODO: Add all cases which are implemented in the AST.h
			case CMARK_NODE_HEADING: {
				int level = cmark_node_get_heading_level(cur);
				auto heading_node = std::make_unique<Heading>(level);
				ContainerNode *raw_heading = heading_node.get();
				node_stack.top()->addChild(std::move(heading_node));
				node_stack.push(raw_heading);
				break;
			}
			case CMARK_NODE_LIST: {
				int list_type = cmark_node_get_list_type(cur);
				std::unique_ptr<ContainerNode> list_node;

				if (list_type == CMARK_ORDERED_LIST) {
					list_node = std::make_unique<OrderedList>();
				} else {
					list_node = std::make_unique<UnorderedList>();
				}

				ContainerNode *raw_list = list_node.get();
				node_stack.top()->addChild(std::move(list_node));
				node_stack.push(raw_list);
				break;
			}
			case CMARK_NODE_ITEM: {
				auto item_node = std::make_unique<ListItem>();
				ContainerNode *raw_item = item_node.get();
				node_stack.top()->addChild(std::move(item_node));
				node_stack.push(raw_item);
				break;
			}
			case CMARK_NODE_CODE_BLOCK: {
				const char *literal = cmark_node_get_literal(cur);
				std::string code_content = literal ? literal : "";

				const char *info = cmark_node_get_fence_info(cur);
				std::string language = info ? info : "";

				auto code_block_node =
					std::make_unique<FencedCodeBlock>(language, code_content);
				node_stack.top()->addChild(std::move(code_block_node));
				break;
			}
			case CMARK_NODE_BLOCK_QUOTE: {
				auto block_quote_node = std::make_unique<BlockQuote>();
				ContainerNode *raw_block_quote = block_quote_node.get();
				node_stack.top()->addChild(std::move(block_quote_node));
				node_stack.push(raw_block_quote);
				break;
			}
			case CMARK_NODE_THEMATIC_BREAK: {
				auto thematic_break_node = std::make_unique<ThematicBreak>();
				node_stack.top()->addChild(std::move(thematic_break_node));
				break;
			}
			case CMARK_NODE_LINK: {
				const char *destination = cmark_node_get_url(cur);
				std::string url = destination ? destination : "";

				auto link_node = std::make_unique<Link>(url);
				ContainerNode *raw_link = link_node.get();
				node_stack.top()->addChild(std::move(link_node));
				node_stack.push(raw_link);
				break;
			}
			case CMARK_NODE_IMAGE: {
				const char *destination = cmark_node_get_url(cur);
				std::string url = destination ? destination : "";

				auto image_node = std::make_unique<Image>(url);
				ContainerNode *raw_image = image_node.get();
				node_stack.top()->addChild(std::move(image_node));
				node_stack.push(raw_image);
				break;
			}
			case CMARK_NODE_STRONG: {
				auto bold_node = std::make_unique<BoldText>();
				ContainerNode *raw_bold = bold_node.get();
				node_stack.top()->addChild(std::move(bold_node));
				node_stack.push(raw_bold);
				break;
			}
			case CMARK_NODE_EMPH: {
				auto italic_node = std::make_unique<ItalicText>();
				ContainerNode *raw_italic = italic_node.get();
				node_stack.top()->addChild(std::move(italic_node));
				node_stack.push(raw_italic);
				break;
			}
			case CMARK_NODE_CODE: {
				const char *literal = cmark_node_get_literal(cur);
				std::string code_content = literal ? literal : "";

				auto inline_code_node =
					std::make_unique<InlineCode>(code_content);
				node_stack.top()->addChild(std::move(inline_code_node));
				break;
			}
			case CMARK_NODE_SOFTBREAK: {
				auto text_node = std::make_unique<Text>(" ");
				node_stack.top()->addChild(std::move(text_node));
				break;
			}
			case CMARK_NODE_LINEBREAK: {
				auto break_node = std::make_unique<LineBreak>();
				node_stack.top()->addChild(std::move(break_node));
				break;
			}
			default: {
				std::string ext_name = cmark_node_get_type_string(cur);

				if (ext_name == "table") {
					auto table_node = std::make_unique<Table>();
					ContainerNode *raw_table = table_node.get();
					node_stack.top()->addChild(std::move(table_node));
					node_stack.push(raw_table);
				} else if (ext_name == "table_row" ||
						   ext_name == "table_header") {
					auto row_node = std::make_unique<TableRow>();
					ContainerNode *raw_row = row_node.get();
					node_stack.top()->addChild(std::move(row_node));
					node_stack.push(raw_row);
				} else if (ext_name == "table_cell") {
					auto cell_node = std::make_unique<TableCell>();
					ContainerNode *raw_cell = cell_node.get();
					node_stack.top()->addChild(std::move(cell_node));
					node_stack.push(raw_cell);
				} else {
					throw std::runtime_error("Error: Unsupported node type "
											 "encountered in GFMParser: " +
											 ext_name);
				}
				break;
			}
			}
		} else if (ev_type == CMARK_EVENT_EXIT) {
			std::string ext_name = cmark_node_get_type_string(cur);

			if (ext_name == "table" || ext_name == "table_row" ||
				ext_name == "table_header" || ext_name == "table_cell") {
				node_stack.pop();
			} else {
				switch (type) {
				case CMARK_NODE_PARAGRAPH:
				case CMARK_NODE_HEADING:
				case CMARK_NODE_LIST:
				case CMARK_NODE_ITEM:
				case CMARK_NODE_BLOCK_QUOTE:
				case CMARK_NODE_IMAGE:
				case CMARK_NODE_LINK:
				case CMARK_NODE_STRONG:
				case CMARK_NODE_EMPH:
					node_stack.pop();
					break;
				default:
					// leaf nodes
					break;
				}
			}
		}
	}

	cmark_iter_free(iter);

	return document;
};