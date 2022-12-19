cd ai
g++ src/*.cpp -Iinclude -Ilib/include -Iexternal -std=c++2a -O3 -march=native -pg -ggdb3 -DNDEBUG -g  -DFMT_HEADER_ONLY
