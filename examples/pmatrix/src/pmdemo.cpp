// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2015 King Abdullah Petroleum Studies and Research Center
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// --------------------------------------------



#include "pmdemo.h"

using std::cout;
using std::endl;
using std::flush; 


namespace PMatDemo {  
  
  // nothing yet

}
// end of namespace


// -------------------------------------------------


int main(int ac, char **av) {
    using KBase::dSeed;
    using KBase::PRNG;

    auto sTime = KBase::displayProgramStart();
    uint64_t seed = dSeed;
    bool run = true;  

    auto showHelp = []() {
        printf("\n");
        printf("Usage: specify one or more of these options\n");
        printf("--help            print this message\n"); 
        printf("--seed <n>        set a 64bit seed \n");
        printf("                  0 means truly random\n");
        printf("                  default: %020llu \n", dSeed);
    };

    // tmp args

    if (ac > 1) {
        for (int i = 1; i < ac; i++) {
            cout << "Argument " << i << " is -|" << av[i] << "|-" << endl << flush;
            if (strcmp(av[i], "--seed") == 0) {
                i++;
                seed = std::stoull(av[i]);
            } 
            else if (strcmp(av[i], "--help") == 0) {
                run = false;
            }
            else {
                run = false;
                printf("Unrecognized argument %s\n", av[i]);
            }
        }
    }

    if (!run) {
        showHelp();
        return 0;
    }

    PRNG * rng = new PRNG();
    seed = rng->setSeed(seed); // 0 == get a random number
    printf("Using PRNG seed:  %020llu \n", seed);
    printf("Same seed in hex:   0x%016llX \n", seed);
    
    cout << "Nothing yet implemented."<<endl<<flush;

    delete rng;
    KBase::displayProgramEnd(sTime);
    return 0;
}


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
