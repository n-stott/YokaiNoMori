cd ai
g++ -m32 src/*.cpp -Iinclude -Ilib/include -Iexternal -std=c++2a -march=i386 -DNDEBUG -g -DFMT_HEADER_ONLY\
    -O0\
    -ffunction-sections -fdata-sections\
    -Wl,--gc-sections\
    -fno-exceptions\
    #-mstringop-strategy=libcall\
    #-static-libstdc++ -static-libgcc 
