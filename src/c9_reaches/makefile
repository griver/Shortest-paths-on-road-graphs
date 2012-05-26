DST_NAME=../../data/c9_reaches
SFILES=*.cpp
HFILES=*.h ../shared/*.h ../reach_preprocessing/reach_graph.h


XX=g++
FLAGS=-Wall -g -O2 -std=c++0x

${DST_NAME}: $(SFILES) $(HFILES)
	$(XX) $(FLAGS) ${SFILES} -o $(DST_NAME)



clean:
	rm -rf ${DST_NAME} 

.PHONY: clean
