all:	comp

comp:	main.c
	gcc -o gen main.c -lm -Wall -O2

exec:
	./gen FPMult exp=5 frac=10 stdout Wrapper


clean:
	rm -rf *~ *.sfl gen
