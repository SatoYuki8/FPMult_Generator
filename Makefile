BIN=gen

all:	comp

comp:	src/main.c
	gcc -o $(BIN) $^ -lm -Wall -O2

src/main.c:
	gcc -o $(BIN) $@ -lm -Wall -O2


exec:
	./$(BIN) FPMult exp=5 frac=10 stdout Wrapper


clean:
	rm -rf *~ *.sfl $(BIN)
