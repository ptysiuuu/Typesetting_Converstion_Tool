#include "writers/latex_writer.h"

void LaTeXWriter::visitDocument(const Document &d) {
	if (standalone) {
		output += R"(\documentclass{article}
\usepackage[utf8]{inputenc}
\usepackage[T1]{fontenc}
\usepackage{graphicx}   % Required for images
\usepackage{hyperref}   % Required for links
\usepackage{listings}   % Required for code blocks
\usepackage{booktabs}   % Required for tables
\begin{document}
)";
	} else {
		output += "% Required LaTeX packages: graphicx, hyperref, listings, "
				  "booktabs\n\n";
	}

	for (auto it = d.childrenBegin(); it != d.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}

	if (standalone) {
		output += R"(
\end{document}
)";
	}
}

void LaTeXWriter::visitParagraph(const Paragraph &p) {
	output += R"(\n)";
	for (auto it = p.childrenBegin(); it != p.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += R"(\n\n)";
}

void LaTeXWriter::visitHeading(const Heading &h) {
	switch (h.getLevel()) {
	case 1: {
		output += R"(\section{)";
		break;
	}
	case 2: {
		output += R"(\subsection{)";
		break;
	}
	case 3: {
		output += R"(\subsubsection{)";
		break;
	}
	default: {
		output += R"(\paragraph{)";
		break;
	}
	}
	for (auto it = h.childrenBegin(); it != h.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += R"(}\n)";
}

void LaTeXWriter::visitBoldText(const BoldText &b) {
	output += R"(\textbf{)";
	for (auto it = b.childrenBegin(); it != b.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += "}";
}

void LaTeXWriter::visitItalicText(const ItalicText &i) {
	output += R"(\textit{)";
	for (auto it = i.childrenBegin(); it != i.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += "}";
}

void LaTeXWriter::visitInlineCode(const InlineCode &ic) {
	output += R"(\texttt{)";
	output += escapeLatex(ic.getContent());
	output += "}";
}

void LaTeXWriter::visitFencedCodeBlock(const FencedCodeBlock &fcb) {
	output += R"(\begin{lstlisting}[language=)";
	output += fcb.getLanguage();
	output += "]\n";
	output += fcb.getContent();
	output += R"(\end{lstlisting})"
			  "\n";
}

void LaTeXWriter::visitUnorderedList(const UnorderedList &ul) {
	output += R"(\begin{itemize})"
			  "\n";
	for (auto it = ul.childrenBegin(); it != ul.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += R"(\end{itemize})"
			  "\n";
}

void LaTeXWriter::visitOrderedList(const OrderedList &ol) {
	output += R"(\begin{enumerate})"
			  "\n";

	for (auto it = ol.childrenBegin(); it != ol.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}

	output += R"(\end{enumerate})"
			  "\n";
}

void LaTeXWriter::visitListItem(const ListItem &li) {
	output += R"(\item )";
	for (auto it = li.childrenBegin(); it != li.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += "\n";
}

void LaTeXWriter::visitLink(const Link &l) {
	output += R"(\href{)";
	output += l.getUrl();
	output += "}{";
	for (auto it = l.childrenBegin(); it != l.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += "}";
}

void LaTeXWriter::visitImage(const Image &i) {
	output += R"(\begin{figure}[h] \centering)";
	output += R"(\includegraphics[width=\textwidth]{)";
	output += i.getUrl();
	output += R"(\caption{)";
	for (auto it = i.childrenBegin(); it != i.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += R"(} \end{figure})";
}

void LaTeXWriter::visitBlockQuote(const BlockQuote &bq) {
	output += R"(\begin{quote})";
	for (auto it = bq.childrenBegin(); it != bq.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
	output += R"(end{quote})";
}

void LaTeXWriter::visitThematicBreak(const ThematicBreak &) {
	output += R"(\noindent\rule{\textwidth}{0.4pt})";
	output += "\n\n";
}

void LaTeXWriter::visitTable(const Table &t) {
	auto it = t.childrenBegin();
	if (it == t.childrenEnd())
		return;

	const Node *firstRow = it->get();
	const ContainerNode *rowContainer =
		dynamic_cast<const ContainerNode *>(firstRow);

	size_t colCount = 0;
	if (rowContainer) {
		colCount = std::distance(rowContainer->childrenBegin(),
								 rowContainer->childrenEnd());
	}

	output += R"(\begin{tabular}{)";
	for (size_t i = 0; i < colCount; ++i) {
		output += "l";
	}
	output += "}\n";
	output += R"(\toprule)"
			  "\n";

	bool isFirstRow = true;
	for (; it != t.childrenEnd(); ++it) {
		(*it)->accept(*this);

		if (isFirstRow) {
			output += R"(\midrule)"
					  "\n";
			isFirstRow = false;
		}
	}

	output += R"(\bottomrule)"
			  "\n";
	output += R"(\end{tabular})"
			  "\n";
}

void LaTeXWriter::visitTableRow(const TableRow &tr) {
	for (auto it = tr.childrenBegin(); it != tr.childrenEnd(); ++it) {
		(*it)->accept(*this);

		if (std::next(it) != tr.childrenEnd()) {
			output += " & ";
		}
	}
	output += R"( \\)"
			  "\n";
}

void LaTeXWriter::visitTableCell(const TableCell &tc) {
	for (auto it = tc.childrenBegin(); it != tc.childrenEnd(); ++it) {
		(*it)->accept(*this);
	}
}

void LaTeXWriter::visitText(const Text &t) {
	std::string rawText = t.getText();

	std::string safeText = escapeLatex(rawText);

	output += safeText;
}

void LaTeXWriter::visitLineBreak(const LineBreak &) {}

std::string LaTeXWriter::escapeLatex(const std::string text) {
	std::string result = "";
	for (auto character : text) {
		switch (character) {
		case '&':
			result += R"(\&)";
			break;
		case '$':
			result += R"(\$)";
			break;
		case '%':
			result += R"(\%)";
			break;
		case '#':
			result += R"(\#)";
			break;
		case '_':
			result += R"(\_)";
			break;
		case '{':
			result += R"(\{)";
			break;
		case '}':
			result += R"(\})";
			break;
		case '\\':
			result += R"(\textbackslash{})";
			break;
		case '~':
			result += R"(\textasciitilde{})";
			break;
		case '^':
			result += R"(\textasciicircum{})";
			break;
		default:
			result += character;
			break;
		}
	}
	return result;
}