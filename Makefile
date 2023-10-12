.PHONY: all
all: scheduler.out

scheduler.out: scheduler.c 
	gcc -o scheduler.out scheduler.c 

.PHONY: clean
clean: 
	rm -f scheduler.out

.PHONY: test, test2, test3, test4, test5, test6
test: scheduler.out
	./scheduler.out 0 STCF example_stcf.in 10

test2: scheduler.out
	./scheduler.out 1 STCF example_stcf.in 5

test3: scheduler.out
	./scheduler.out 0 RR example_rr.in 5

test4: scheduler.out
	./scheduler.out 1 RR example_rr.in 5

test5: scheduler.out
	./scheduler.out 0 LT example_lt.in 5

test6: scheduler.out
	./scheduler.out 1 LT example_lt.in 5

