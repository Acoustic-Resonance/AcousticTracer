report.pdf: report.md header.yaml
	pandoc header.yaml report.md -o report.pdf --pdf-engine=lualatex

numbered: report.pdf header.yaml
	pandoc header.yaml report.md -o report.pdf --lua-filter=filter-headings.lua --shift-heading-level-by=-1 --number-sections --pdf-engine=lualatex

release: report.md header.yaml
	pandoc header.yaml report.md -o report.pdf --lua-filter=filter-headings.lua --shift-heading-level-by=-1 --toc=true --toc-depth=2 --number-sections --pdf-engine=lualatex
