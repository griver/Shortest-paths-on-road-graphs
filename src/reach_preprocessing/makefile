DST_NAME=reach_prep
SFILES=main.cpp penalties_preprocessor.cpp reach_updater.cpp shortcuts.cpp stdafx.cpp
HFILES=*.h ../shared/*.h


XX=g++
FLAGS=-Wall -g -std=c++0x

${DST_NAME}: $(SFILES) $(HFILES)
	$(XX) $(FLAGS) ${SFILES} -o $(DST_NAME)



clean:
	rm -rf ${DST_NAME} 

.PHONY: clean
