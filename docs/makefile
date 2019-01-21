HEADER = ../src/parameters.hpp
GENERATOR = ./generate-macros
IMAGES = logo.jpg output.png raw.jpg

main.pdf: main.tex macros.tex ${IMAGES} makefile
	pdflatex -halt-on-error $<

macros.tex: ${HEADER} ${GENERATOR}
	${GENERATOR}
