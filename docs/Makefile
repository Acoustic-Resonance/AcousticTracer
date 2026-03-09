report.pdf: report.md header.yaml
	pandoc header.yaml report.md -o report.pdf --lua-filter=filter-headings.lua --shift-heading-level-by=-1

release: report.md header.yaml
	pandoc header.yaml report.md -o report.pdf --lua-filter=filter-headings.lua --shift-heading-level-by=-1 --toc=true --toc-depth=2
