CFLAGS = -Wall -g -O0
LDFLAGS =
DOC?=presentation
IMAGES=${shell sed -n 's/^!\[\](\([^)]*\)).*$$/\1/p' $(DOC).md}
PANDOCFLAGS = -fmarkdown-implicit_figures -t beamer -V theme:default -V colortheme:seagull

default: break hello

all: break hello hello-rust $(DOC)-43.pdf $(DOC)-169.pdf

break: break.o break_utils.o
	gcc $(LDFLAGS) $^ -o $@

break.o: break.c break_utils.h
	gcc $(CFLAGS) -c $< -o $@
	
break_utils.o: break_utils.c break_utils.h
	gcc $(CFLAGS) -c $< -o $@

hello: hello_world.c
	gcc $(CFLAGS) $< -no-pie -o $@

hello-rust: hello_world.rs
	rustc -C debuginfo=1 $< -o $@

$(DOC)-43.pdf: $(DOC).md $(IMAGES)
	pandoc $(PANDOCFLAGS) -s $< -t beamer -o $@
	
$(DOC)-169.pdf: $(DOC).md $(IMAGES)
	pandoc $(PANDOCFLAGS) -s $< -o $@ --variable "classoption:aspectratio=169"

img/%.pdf: img/%.odg
	libreoffice -env:UserInstallation=file:///$(HOME)/.libreoffice-headless/ --headless --convert-to pdf --outdir img $<

clean:
	rm -f break hello hello-rust *.o $(DOC)-*.pdf img/*.pdf
