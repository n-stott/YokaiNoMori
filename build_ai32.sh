cd ai
g++ -m32 src/*.cpp -Iinclude -Ilib/include -Iexternal -std=c++2a -march=i386 -DNDEBUG -g -DFMT_HEADER_ONLY\
    -O0\
    -DENABLE_HUMAN_PLAYER=0\
    -DENABLE_INTERACTIVE=0\
    -DENABLE_ENUMERATOR=0\
    -ffunction-sections -fdata-sections\
    -Wl,--gc-sections\
    -fno-exceptions\
    -mstringop-strategy=loop\
    #-static-libstdc++ -static-libgcc 
