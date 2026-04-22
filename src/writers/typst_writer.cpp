#include "writers/typst_writer.h"

void TypstWriter::visitDocument(const Document &d) {
    if(standalone) {
        output += R"(#set page(paper: "a4")
        #set text(font: "linux libertine", size: 11pt)
        #set par(justify: true) )";
    } else {
        output += "# Required Typst settings: page, text, par\n\n";
    }

    for (auto it = d.childrenBegin(); it != d.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
}

void TypstWriter::visitParagraph(const Paragraph &p) {
    if (!insideListItem)
        output += "\n";
    
    for (auto it = p.childrenBegin(); it != p.childrenEnd(); ++it)
        (*it)->accept(*this);
    
    if (!insideListItem)
        output += "\n\n";
}

void TypstWriter::visitHeading(const Heading &h) {
    output += "\n";
    for (int i = 0; i < h.getLevel(); ++i) {
        output += "=";
    }
    output += " ";
    
    for (auto it = h.childrenBegin(); it != h.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "\n";
}

void TypstWriter::visitBoldText(const BoldText &b) {
    output += "*";
    for (auto it = b.childrenBegin(); it != b.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "*";
}

void TypstWriter::visitItalicText(const ItalicText &i) {
    output += "_";
    for (auto it = i.childrenBegin(); it != i.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "_";
}

void TypstWriter::visitInlineCode(const InlineCode &ic) {
    output += "`";
    output += ic.getContent();
    output += "`";
}


void TypstWriter::visitFencedCodeBlock(const FencedCodeBlock &fcb) {
    output += "```";
    output += fcb.getLanguage();
    output += "\n";
    output += fcb.getContent();
    output += "```\n";
}

void TypstWriter::visitUnorderedList(const UnorderedList &ul) {
    bool previousState = isInsideOrderedList; 
    
    isInsideOrderedList = false; 

    output += "\n";
    for (auto it = ul.childrenBegin(); it != ul.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "\n";

    isInsideOrderedList = previousState; 
}

void TypstWriter::visitOrderedList(const OrderedList &ol) {
    bool previousState = isInsideOrderedList;
    isInsideOrderedList = true; 

    output += "\n";
    for (auto it = ol.childrenBegin(); it != ol.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "\n";

    isInsideOrderedList = previousState;
}

void TypstWriter::visitListItem(const ListItem &li) {
    if (isInsideOrderedList) {
        output += "+ "; 
    } else {
        output += "- "; 
    }
    insideListItem = true;
    for (auto it = li.childrenBegin(); it != li.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    insideListItem = false;
    
    output += "\n";
}

void TypstWriter::visitLink(const Link &l) {
    output += "#link(\"";
    output += l.getUrl();
    output += "\")[";
    for (auto it = l.childrenBegin(); it != l.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "]";
}

void TypstWriter::visitImage(const Image &i) {
    output += "#figure(\n  image(\"";
    output += i.getUrl();
    output += "\"),\n  caption: [";
    for (auto it = i.childrenBegin(); it != i.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "],\n)\n";
}

void TypstWriter::visitBlockQuote(const BlockQuote &bq) {
    output += "#quote(block: true)[";
    for (auto it = bq.childrenBegin(); it != bq.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "]\n";
}

void TypstWriter::visitThematicBreak(const ThematicBreak &) {
    output += "\n#line(length: 100%)\n\n";
}

void TypstWriter::visitTable(const Table &t) {
    if (t.getColCount() == 0) return;

    output += "#table(\n";
    output += "  columns: " + std::to_string(t.getColCount()) + ",\n";
    output += "  inset: 10pt,\n";
    output += "  align: horizon,\n";

    for (auto it = t.childrenBegin(); it != t.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += ")\n";
}

void TypstWriter::visitTableRow(const TableRow &tr) {
    for (auto it = tr.childrenBegin(); it != tr.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
}

void TypstWriter::visitTableCell(const TableCell &tc) {
    output += "  [";
    for (auto it = tc.childrenBegin(); it != tc.childrenEnd(); ++it) {
        (*it)->accept(*this);
    }
    output += "],\n";
}

void TypstWriter::visitText(const Text &t) {
    output += escapeTypst(t.getText());
}


void TypstWriter::visitLineBreak(const LineBreak &) {
    output += "\n";
}

std::string TypstWriter::escapeTypst(const std::string &text) {
    std::string result = "";
    for (auto character : text) {
        switch (character) {
            case '#':  result += "\\#"; break;
            case '*':  result += "\\*"; break;
            case '_':  result += "\\_"; break;
            case '`':  result += "\\`"; break;
            case '[':  result += "\\["; break;
            case ']':  result += "\\]"; break;
            default:   result += character; break;
        }
    }
    return result;
}