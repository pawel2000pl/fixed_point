#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "lib/fixedpoint.h"
#include "lib/taylormath.h"

int main() {

    fixedpoint<int, int, 20> fp = 0ll;
    fixedpoint<int, int, 16> k = 0.2f;

    std::cout << k << std::endl;

    fp = 1000;
    while (fp > 0) {
        std::cout << fp << std::endl;
        fp *= k;
    }

    
    // std::cin >> fp;
    std::cout << fp << std::endl;
    std::cout << sin(0.16f) << std::endl;
    std::cout << taylor::sin<fixedpoint<long long int>>(0.16f) << std::endl; //0.1593182066142460
    std::cout << taylor::sqrt<fixedpoint<long long int>>(2) << std::endl; //1.414213562373095
    std::cout << taylor::asin<fixedpoint<long long int>>(0.5) << std::endl; //0.5235987755982989
    std::cout << taylor::asin<fixedpoint<long long int>>(0.95) << std::endl; //1.253235897503375

    std::cout << taylor::ln<fixedpoint<long long int>>(0.5) << std::endl; //-0,6931471805599453
    std::cout << taylor::ln<fixedpoint<long long int>>(2) << std::endl; //0,6931471805599453
    std::cout << taylor::ln<fixedpoint<long long int>>(3) << std::endl; //1.098612288668110
    std::cout << taylor::ln<fixedpoint<long long int>>(10) << std::endl; //2.302585092994046

    std::cout << taylor::exp<fixedpoint<long long int>>(10) << std::endl; //22026.46579480672
    std::cout << taylor::exp<fixedpoint<long long int>>(2) << std::endl; //7.389056098930650
    std::cout << taylor::exp<fixedpoint<long long int>>(0.5) << std::endl; //1.648721270700128
    std::cout << taylor::exp<fixedpoint<long long int>>(-0.5) << std::endl; //0.6065306597126334
    std::cout << taylor::exp<fixedpoint<long long int>>(-2) << std::endl; //0.1353352832366127

    return 0;
}
