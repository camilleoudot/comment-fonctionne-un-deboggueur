CFLAGS = -Wall -g -O
LDFLAGS = -g

default: break hello

all: break hello presentation.pdf

break: break.o break_utils.o
	gcc $(LDFLAGS) $^ -o $@

break.o: break.c break_utils.h
	gcc $(CFLAGS) -c $< -o $@
	
break_utils.o: break_utils.c break_utils.h
	gcc $(CFLAGS) -c $< -o $@

hello: hello_world.c
	gcc $(CFLAGS) $< -no-pie -o $@

presentation.pdf: presentation.md
	pandoc $< -t beamer -o $@
	
clean:
	rm -f break hello *.o presentation.pdf
