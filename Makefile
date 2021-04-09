CSRC=$(wildcard *.c)
COBJ=$(CSRC:.c=.o)

CFLAGS=-W -Wall -Wextra -pedantic -Ofast -std=c99 -march=native -mtune=native
LDFLAGS=-pthread -lpthread

EXEC=hffmn

.phony: par-batch

$(EXEC): $(COBJ)
	$(CC) $(LDFLAGS) $^ -o $@

prepare:
	ls ./ex | grep zip | xargs -n1 unzip -d ./ex --;
	rm ex/*.zip

clean:
	rm $(EXEC)
	rm -rfd *.o *.out
	rm -rfd ./ex/*.hf ./ex/*.zip ./ex/*.7z

seq-batch: $(EXEC)
	time ./$^ ./ex/test.1.txt --concurrency s;
	time ./$^ ./ex/test.2.txt --concurrency s;
	time ./$^ ./ex/test.3.txt --concurrency s;
	time ./$^ ./ex/test.4.txt --concurrency s;

par-batch: $(EXEC)
	time ./$^ ./ex/test.1.txt --concurrency p --thread 128;
	time ./$^ ./ex/test.2.txt --concurrency p --thread 128;
	time ./$^ ./ex/test.3.txt --concurrency p --thread 128;
	time ./$^ ./ex/test.4.txt --concurrency p --thread 128;

batch: $(EXEC)
	time ./$^ ./ex/test.1.txt --concurrency s;
	time ./$^ ./ex/test.1.txt --concurrency p --thread 128;
	time ./$^ ./ex/test.2.txt --concurrency s;
	time ./$^ ./ex/test.2.txt --concurrency p --thread 128;
	time ./$^ ./ex/test.3.txt --concurrency s;
	time ./$^ ./ex/test.3.txt --concurrency p --thread 128;
	time ./$^ ./ex/test.4.txt --concurrency s;
	time ./$^ ./ex/test.4.txt --concurrency p --thread 128;

seq-low: $(EXEC)
	time ./$^ ./ex/test.1.txt --concurrency s;

seq-soft: $(EXEC)
	time ./$^ ./ex/test.2.txt --concurrency s;

seq-norm: $(EXEC)
	time ./$^ ./ex/test.3.txt --concurrency s;

seq-hard: $(EXEC)
	time ./$^ ./ex/test.4.txt --concurrency s;

par-low: $(EXEC)
	time ./$^ ./ex/test.1.txt --concurrency p --thread 128;

par-soft: $(EXEC)
	time ./$^ ./ex/test.2.txt --concurrency p --thread 128;

par-norm: $(EXEC)
	time ./$^ ./ex/test.3.txt --concurrency p --thread 128;

par-hard: $(EXEC)
	time ./$^ ./ex/test.4.txt --concurrency p --thread 128;
