.DELETE_ON_ERROR:

defaultTargets = header.html footer.html style.css
doxygenHtmlTemplateTargets = header.html.in footer.html.in style.css

default: $(defaultTargets)

$(doxygenHtmlTemplateTargets) &:
	doxygen -w html $(doxygenHtmlTemplateTargets)

%.html: %.html.in
	sed -E \
	  -e 's:\$$relpath\^::g' \
	  -e 's:(\$$(doxygen|project|search))([a-z]):\1\U\3:g' \
	  -e 's:\$$([a-zA-Z]+):{{ \1 }}:g' \
	  $< > $@

.PHONY: clean
clean:
	${RM} *.bak *.css *.html *.in

.PHONY: install
install: ${defaultTargets}
	cp -a $^ ../doxygen-both
