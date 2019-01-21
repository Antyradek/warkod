HEADER = ../src/parameters.hpp
GENERATOR = ./generate-macros
IMAGES = logo.jpg

main.pdf: main.tex macros.tex ${IMAGES}
	pdflatex -halt-on-error $<

macros.tex: ${HEADER} ${GENERATOR}
	${GENERATOR}
