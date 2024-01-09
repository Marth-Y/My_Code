ls : main.o
	g++ -o ls main.o -g -O0

main.o : ls.cc
	g++ -c ls.cc -o main.o -g -O0

.PHONY:clean
clean:
	rm *.o
