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

	visitChildren(d);

	if (standalone) {
		output += R"(
\end{document}
)";
	}
}

void LaTeXWriter::visitParagraph(const Paragraph &p) {
	if (listItemDepth == 0)
		output += "\n";
	visitChildren(p);
	if (listItemDepth == 0)
		output += "\n\n";
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
	visitChildren(h);
	output += "}\n";
}

void LaTeXWriter::visitBoldText(const BoldText &b) {
	output += R"(\textbf{)";
	visitChildren(b);
	output += "}";
}

void LaTeXWriter::visitItalicText(const ItalicText &i) {
	output += R"(\textit{)";
	visitChildren(i);
	output += "}";
}

void LaTeXWriter::visitInlineCode(const InlineCode &ic) {
	output += R"(\texttt{)";
	output += escapeLatex(ic.getContent());
	output += "}";
}

void LaTeXWriter::visitFencedCodeBlock(const FencedCodeBlock &fcb) {
	const std::string &lang = fcb.getLanguage();
	if (!lang.empty()) {
		output += R"(\begin{lstlisting}[language=)" + lang + "]\n";
	} else {
		output += R"(\begin{lstlisting})"
				  "\n";
	}
	output += "]\n";
	output += fcb.getContent();
	output += R"(\end{lstlisting})"
			  "\n";
}

void LaTeXWriter::visitUnorderedList(const UnorderedList &ul) {
	output += R"(\begin{itemize})"
			  "\n";
	visitChildren(ul);
	output += R"(\end{itemize})"
			  "\n";
}

void LaTeXWriter::visitOrderedList(const OrderedList &ol) {
	output += R"(\begin{enumerate})"
			  "\n";

	visitChildren(ol);

	output += R"(\end{enumerate})"
			  "\n";
}

void LaTeXWriter::visitListItem(const ListItem &li) {
	output += R"(\item )";
	++listItemDepth;
	visitChildren(li);
	--listItemDepth;
	output += "\n";
}

void LaTeXWriter::visitLink(const Link &l) {
	output += R"(\href{)";
	output += l.getUrl();
	output += "}{";
	visitChildren(l);
	output += "}";
}

void LaTeXWriter::visitImage(const Image &i) {
	output += R"(\begin{figure}[h] \centering)";
	output += R"(\includegraphics[width=\textwidth]{)";
	output += i.getUrl();
	output += "}\n";
	output += R"(\caption{)";
	visitChildren(i);
	output += R"(} \end{figure})";
}

void LaTeXWriter::visitBlockQuote(const BlockQuote &bq) {
	output += R"(\begin{quote})";
	visitChildren(bq);
	output += R"(\end{quote})";
}

void LaTeXWriter::visitThematicBreak(const ThematicBreak &) {
	output += R"(\noindent\rule{\textwidth}{0.4pt})";
	output += "\n\n";
}

void LaTeXWriter::visitTable(const Table &t) {
	if (t.getColCount() == 0)
		return;

	output += R"(\begin{tabular}{)";
	for (size_t i = 0; i < t.getColCount(); ++i)
		output += "l";
	output += "}\n";
	output += R"(\toprule)"
			  "\n";

	bool isFirstRow = true;
	for (auto it = t.childrenBegin(); it != t.childrenEnd(); ++it) {
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
	bool first = true;
	for (auto it = tr.childrenBegin(); it != tr.childrenEnd(); ++it) {
		if (!first) {
			output += " & ";
		}
		(*it)->accept(*this);
		first = false;
	}
	output += R"( \\)"
			  "\n";
}

void LaTeXWriter::visitTableCell(const TableCell &tc) { visitChildren(tc); }

void LaTeXWriter::visitText(const Text &t) {
	std::string rawText = t.getText();

	std::string safeText = escapeLatex(rawText);

	output += safeText;
}

void LaTeXWriter::visitLineBreak(const LineBreak &) { output += "\\\\\n"; }

std::string LaTeXWriter::escapeLatex(const std::string &text) {
	std::string result = "";
	result.reserve(text.size());
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