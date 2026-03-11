default: report.md header.yaml
	pandoc header.yaml report.md -o report.pdf --pdf-engine=pdflatex

numbered: report.md header.yaml
	pandoc header.yaml report.md -o report.pdf --lua-filter=filter-headings.lua --shift-heading-level-by=-1 --number-sections --pdf-engine=pdflatex

release: report.md header.yaml
	pandoc header.yaml report.md -o report.pdf --lua-filter=filter-headings.lua --shift-heading-level-by=-1 --toc --toc-depth=2 --number-sections --pdf-engine=pdflatex
