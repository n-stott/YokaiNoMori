~/git/emsdk/upstream/emscripten/em++ src/*.cpp -Iinclude -Ilib/include\
    -std=c++2a -O3 -march=native -DNDEBUG\
    -o ./bin/yokai-lib.js\
    -s SINGLE_FILE\
    -s WASM=0\
    -s EXPORTED_FUNCTIONS='["_validAction", "_playAction", "_searchBestMove", "_board", "_reserve0", "_reserve1"]'\
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'

    #-s MODULARIZE\
