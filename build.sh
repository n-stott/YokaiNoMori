cd ai
clang++-11 src/*.cpp -Iinclude -Ilib/include -std=c++2a -O3 -march=native -DNDEBUG
