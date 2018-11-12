BIN=gen
OPT=-lm -Wall -O2
GCC=gcc

all:	comp

comp:	src/main.o src/step_measurement.o
	$(GCC) -o $(BIN) $^ -lm -Wall -O2

src/main.o:	src/main.c src/generator.h
src/main.c:
	$(GCC) -c $@ $(OPT)

src/step_measurement.o:	src/step_measurement.c src/generator.h
src/step_measurement.c:
	$(GCC) -c $@ $(OPT)

exec:
	./$(BIN) FPMult exp=5 frac=10 stdout Wrapper


clean:
	rm -rf *~ *.sfl $(BIN) src/*.o
