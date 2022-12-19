cd ai
g++ src/*.cpp external/fmt/libfmt.a -Iinclude -Ilib/include -Iexternal -std=c++2a -O3 -march=native -pg -ggdb3 -DNDEBUG -g 
