BIN=gen
OPT=-lm -Wall -O2
GCC=gcc
HEAD=src/mul_gen.h
OBJ=src/main.o src/step_measurement.o src/arg_check.o src/sub_module_function.o src/fpmultiplier_generator.o

all:	comp

comp:	$(OBJ)
	$(GCC) -o $(BIN) $^ -lm -Wall -O2

src/main.o:	src/main.c $(HEAD)
src/main.c:
	$(GCC) -c $@ $(OPT)

src/step_measurement.o:	src/step_measurement.c $(HEAD)
src/step_measurement.c:
	$(GCC) -c $@ $(OPT)

src/arg_check.o: src/arg_check.c $(HEAD)
src/arg_check.c:
	$(GCC) -c $@ $(OPT)

src/sub_module_function.o:	src/sub_module_function.c $(HEAD)
src/sub_module_function.c:
	$(GCC) -c $@ $(OPT)

src/fpmultiplier_generator.o:	src/fpmultiplier_generator.c $(HEAD)
src/fpmultiplier_generator.c:
	$(GCC) -c $@ $(OPT)

exec:
	./$(BIN) FPMult exp=5 frac=10 stdout Wrapper


clean:
	rm -rf *~ *.sfl

distclean:
	rm -rf *~ *.sfl $(BIN) src/*.o
