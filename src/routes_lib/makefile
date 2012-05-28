DST_NAME=../../server2/lib/libroute.o
SO_DST_NAME=../../server2/lib/libroute.so
SFILES=*.cpp
HFILES=*.h ../shared/*.h ../reach_preprocessing/reach_graph.h

XX=g++
FLAGS=-fPIC -c -Wall -g -O2 -std=c++0x
FLAGS2=-shared -Wl

prog: $(SFILES) $(HFILES)
	$(XX) $(FLAGS) ${SFILES}
	
lib: interface.o routes_lib.o routes.o stdafx.o 
	$(XX) $(FLAGS2) -o $(SO_DST_NAME) *.o


clean:
	rm -rf ${DST_NAME} 

.PHONY: clean

