CC = g++ $(CFLAGS)
CFLAGS = -Wno-error=unused-variable -g -O0

all: tick100 badNumbers catalanTst checkZero

tick100: tick100.o fullTree.o catalan.o
	$(CC) -o tick100 tick100.o fullTree.o catalan.o

catalanTst: catalanTst.o fullTree.o catalan.o
	$(CC) -o catalanTst catalanTst.o fullTree.o catalan.o

tick100.o: tick100.cpp fullTree.h catalan.h
	$(CC) -c tick100.cpp

ticks100.o: ticks100.cpp fullTree.h catalan.h
	$(CC) -c ticks100.cpp

fullTree.o: fullTree.h fullTree.cpp
	$(CC) -c fullTree.cpp

catalan.o: catalan.cpp fullTree.h catalan.h
	$(CC) -c catalan.cpp

catalanTst.o: catalanTst.cpp fullTree.h catalan.h
	$(CC) -c catalanTst.cpp

checkZero: checkZero.cpp
	$(CC) -o checkZero checkZero.cpp

badNumbers: badNumbers.cpp fullTree.o catalan.o
	$(CC) -o badNumbers badNumbers.cpp fullTree.o catalan.o

clean:
	rm -f *.o
	rm -f tick100 badNumbers catalanTst checkZero
