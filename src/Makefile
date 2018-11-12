all:	comp

comp:	main.o step_measurement.o
	gcc -o gen $^ -lm -Wall -O2

main.o:	main.c generator.h
main.c:
	gcc -c main.c -lm -Wall -O2

step_measurement.o:	step_measurement.c generator.h
step_measurement.c:
	gcc -c step_measurement.c -lm -Wall -O2

clean:
	rm -rf *~ *.sfl *.o gen
