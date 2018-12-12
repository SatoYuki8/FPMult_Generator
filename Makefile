BIN=gen
OPT=-lm -Wall -O2
GCC=gcc
HEAD=src/mul_gen.h
OBJ=src/main.o src/fpmult_step_measurement.o src/fpmult_arg_check.o src/fpmult_sub_module.o src/fpmult_generator.o
_SRC=$@:.o=.c
SRC=$($(_SRC))

all:	comp

comp:	$(OBJ)
	$(GCC) -o $(BIN) $^ -lm -Wall -O2

src/main.o:	$(SRC) $(HEAD)
src/main.c:
	$(GCC) -c $@ $(OPT)

src/fpmult_step_measurement.o:	src/fpmult_step_measurement.c $(HEAD)
src/fpmult_step_measurement.c:
	$(GCC) -c $@ $(OPT)

src/fpmult_arg_check.o: src/fpmult_arg_check.c $(HEAD)
src/fpmult_arg_check.c:
	$(GCC) -c $@ $(OPT)

src/fpmult_sub_module.o:	src/fpmult_sub_module.c $(HEAD)
src/fpmult_sub_module.c:
	$(GCC) -c $@ $(OPT)

src/fpmult_generator.o:	src/fpmult_generator.c $(HEAD)
src/fpmult_generator.c:
	$(GCC) -c $@ $(OPT)

exec:
	./$(BIN) FPMult exp=5 frac=10 stdout Wrapper


clean:
	rm -rf *~ *.sfl

distclean:
	rm -rf *~ *.sfl $(BIN) src/*.o
