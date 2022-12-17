FNAME = test
CC    = gcc

spinlock: src/test_lock.c src/lock.h src/lock.c
	$(CC) -o $(FNAME) src/test_lock.c src/lock.h \
		-pthread

clean:
	rm $(FNAME)

run:
	./$(FNAME)	