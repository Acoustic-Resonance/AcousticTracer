report.pdf: report.md
	pandoc header.yaml report.md -o report.pdf --lua-filter=filter-headings.lua --shift-heading-level-by=-1