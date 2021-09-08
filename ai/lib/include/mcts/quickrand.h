#ifndef QUICKRAND_H
#define QUICKRAND_H

#include <cstdlib>

class Quickrand {
    int seed;
    public:
        Quickrand(int s = 0) : seed(s) {
            // std::cout << "Quickrand with seed : " << seed << std::endl;
            srand(s);
        }

        int next(int n) {
            int res = rand() % n;
            return res;
        }
};

#endif