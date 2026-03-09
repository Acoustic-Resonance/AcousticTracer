function Header(el)
	if el.level == 1 then
		el.classes:insert("unnumbered")
		el.classes:insert("unlisted")
		local reset = pandoc.RawBlock("latex", "\\setcounter{section}{0}")
		return { el, reset }
	end
end
