all: front-end.o back-end.o
	gcc -o csdb front-end.o back-end.o	

clean:
	rm -f front-end.o back-end.o csdb
