g++ -fPIC -g -c -Wall -o example.o example.cpp
g++ -shared -Wl -o example.so example.o -lc
