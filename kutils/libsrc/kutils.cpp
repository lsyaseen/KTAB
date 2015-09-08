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
// Define a few generally useful functions.
// -------------------------------------------------

#include <assert.h>
#include <iostream>
#include <tuple>

#include "kutils.h"
#include "prng.h"

namespace KBase {
  
  using std::cout;
  using std::endl;
  using std::flush;

  std::chrono::time_point <std::chrono::system_clock, std::chrono::system_clock::duration> displayProgramStart() {
    using std::chrono::high_resolution_clock;
    using std::chrono::system_clock;

    auto sTime = high_resolution_clock::now();
    time_t st = system_clock::to_time_t(sTime);
    cout << endl << endl;
    cout << "  Start time: " << ctime(&st);
    cout << endl << flush;
    return sTime;
  }

  void displayProgramEnd(std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>  & sTime) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;

    auto fTime = high_resolution_clock::now();
    time_t ft = system_clock::to_time_t(fTime);
    auto eTime = (duration_cast<milliseconds>(fTime - sTime).count()) / 1000.00;
    cout << endl << endl;
    cout << "  End time: " << ctime(&ft);
    printf("  Seconds elapsed: %.4f \n", eTime);
    cout << endl << flush;
    return;
  }

  
  double rescale(double x, double x0, double x1, double y0, double y1) {
    assert ((x0 < x1) ||( x1 < x0));
    const double f = (x-x0)/(x1-x0);
    return y0 + (y1-y0)*f;
  }
  
 
  // -------------------------------------------------
  KException::KException(string m) { msg = m; }
  KException::~KException() { }
  // -------------------------------------------------


}; // namespace


// --------------------------------------------
// Copyright KAPSARC. Open source MIT License.
// --------------------------------------------
