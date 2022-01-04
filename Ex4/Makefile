all: Measure sender

Measure: Measure.c
	gcc -o Measure Measure.c

sender: sender.c
	gcc -o sender sender.c

clean:
	rm -f *.o Measure sender

runs:
	./Measure

runc:
	./sender

runs-strace:
	strace -f ./Measure

runc-strace:
	strace -f ./sender