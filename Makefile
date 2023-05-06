override CFLAGS := -Wall -Werror -std=gnu99 -O0 -g $(CFLAGS) -I.

# Build the myshell exec
myshell: myshell.o myshell_parser.o

# Build the parser .o file
myshell_parser.o: myshell_parser.c myshell_parser.h

# Build the myshell .o file
myshell.o: myshell.c

clean:
	rm myshell.o myshell_parser.o myshell