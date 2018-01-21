bfjit: bfjit.o
	gcc -llightning bfjit.o -o bfjit
bfjit.o: bfjit.c
	gcc -O2 bfjit.c -c
test: bfjit tester.py
	python3 tester.py
