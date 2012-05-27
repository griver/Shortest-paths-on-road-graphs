DST_NAME=../../server2/lib/libroute.o
SFILES=*.cpp
HFILES=*.h ../shared/*.h ../reach_preprocessing/reach_graph.h

XX=g++
FLAGS=-fPIC -Wall -g -O2 -std=c++0x
FLAGS2=-shared -Wl -nostdlib

${DST_NAME}: $(SFILES) $(HFILES)
	$(XX) $(FLAGS) ${SFILES} -o $(DST_NAME)
	
libroute.so: $(DST_NAME) 
	$(XX) $(FLAGS2) -o libroute.so $(DST_NAME)


clean:
	rm -rf ${DST_NAME} 

.PHONY: clean
