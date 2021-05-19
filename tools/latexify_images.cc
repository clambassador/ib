#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "ib/csv_table.h"
#include "ib/fileutil.h"
#include "ib/formatting.h"
#include "ib/logger.h"
#include "ib/set.h"
#include "ib/tokenizer.h"
#include "ib/run.h"

using namespace std;
using namespace ib;

int main(int argc, char** argv) {
	string s;
	if (argc != 3) {
		Logger::error("usage: % htmlfile outdir", argv[0]);
		return -1;
	}
	if (argc == 1) {
		Fileutil::read_stream(cin, &s);
	} else {
		Fileutil::read_file(argv[1], &s);
	}
	int r;
	string preamble = R"--(
\documentclass[border=2mm]{standalone}
\usepackage[utf8]{inputenc}

\usepackage{amssymb}
\usepackage{amsmath}
\usepackage{amsthm}
\usepackage{pdfpages}


\usepackage{tikz}
\newtheorem{theorem}{Theorem}
\newtheorem{lemma}{Lemma}

\newcommand{\lbr}{\lbrace}
\newcommand{\rbr}{\rbrace}
\pagestyle{empty}
\begin{document}
)--";
	string postamble = "\n\\end{document}\n";
	while (s.length()) {
		string tmp;
		string name, text;
		r = Tokenizer::extract("%<img data-latex=true style=\"vertical-align:middle\" src=\"%\" alt=\"%\">%", s,
				       nullptr, &name, &text, &tmp);
		s = tmp;
		if (r == 4) {
			assert(name.size());
			assert(text.size());
			cout << name << " " << text << endl;
			string outfile = string(argv[2]) + "/" + name;
			if (Fileutil::exists(outfile)) continue;

			Fileutil::write_file("/tmp/latexify_image.tex",
					     preamble + text + postamble);
			unlink("/tmp/latexify_image.pdf");
			{
				Run r("pdflatex -output-directory=/tmp /tmp/latexify_image.tex");
				r();
				r.result();
			}
			{
				Run r("pdftoppm /tmp/latexify_image.pdf | pnmtopng");
				r();
				r.redirect(outfile);
			}
		}
	}
	return 0;
}
