g++ -fPIC -c -Wall -o libroute.o libroute.cpp
g++ -shared -Wl -o libroute.so libroute.o -lc
