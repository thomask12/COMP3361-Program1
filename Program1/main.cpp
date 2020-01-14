/* 
 * File:   main.cpp
 * Author: kt
 *
 * Created on April 24, 2019, 8:18 AM
 */

#include <cstdlib>
#include <iostream>
#include "RoundRobinPriority.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    RoundRobinPriority r(argv[1], argv[2], argv[3]);
    return 0;
}

