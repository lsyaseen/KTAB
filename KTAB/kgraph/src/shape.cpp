//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------

#include "kutils.h"
//#include <algorithm> // get min, max
//#include "tutils.h"
#include "shape.h"
#include "tmain.h"

//---------------------------------------------

namespace Tetris {
  unsigned int Shape::shapeCounter = 0;

  void Shape::setShape(TCode p) {
    static const int coordsTable[8][4][2] = {
      { { 0, 0 }, {  0,  0 }, { 0, 0 }, { 0, 0 } }, // N, occupying just one cell
      { { 0, 0 }, { -1,  0 }, {-2, 0},  {+1, 0 } }, // I
      { { 0, 0 }, { -1,  0 }, { 1, 0 }, { 1,-1 } }, // J
      { { 0, 0 }, { -1,  0 }, { 1, 0 }, {-1,-1 } }, // L
      { { 0, 0 }, { -1,  0 }, { 0,-1 }, {-1,-1 } }, // O
      { { 0, 0 }, {  0, -1 }, { 1, 0 }, {-1,-1 } }, // S
      { { 0, 0 }, { -1,  0 }, { 1, 0 }, { 0,-1 } }, // T
      { { 0, 0 }, {  0, -1 }, {-1, 0 }, { 1,-1 } }  // Z
    };


    const char * nameTable = "NIJLOSTZ"; 

    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 2; ++j)
        coords[i][j] = coordsTable[p][i][j];
    }
    tName = p;
    name = nameTable[p]; 
  }


  void Shape::setRandomShape() {
    auto ta = TApp::theApp;
    assert(nullptr != ta);
    unsigned int i = 1 + (ta->rng->uniform() % 7);
    setShape(TCode(i));
  }

  int Shape::minX() const {
    int m = coords[0][0];
    for (unsigned int i = 0; i < 4; i++) { 
      m = (x(i) < m) ? x(i) : m;
    }
    return m;
  }

  int Shape::maxX() const {
    int m = coords[0][0];
    for (unsigned int i = 0; i < 4; i++) { 
      m = (m < x(i)) ? x(i) : m;
    }
    return m;
  }

  int Shape::minY() const {
    int m = coords[0][1];
    for (unsigned int i = 0; i < 4; i++) { 
      m = (y(i) < m) ? y(i) : m;
    }
    return m;
  }

  int Shape::maxY() const {
    int m = coords[0][1];
    for (unsigned int i = 0; i < 4; i++) { 
      m = (m < y(i)) ? y(i) : m;
    }
    return m;
  }

  void Shape::showCoords() const { 
    printf("[%c %05i ", name, idNum);
    for (unsigned int k = 0; k < 4; k++){ 
      printf("(%+i, %+i)", x(k), y(k));
      if (3 != k) { printf(" "); }
    }
    printf("]");
    return;
  }

  Shape Shape::lrot() const {
    Shape result;
    result.tName = tName;
    for (int i = 0; i < 4; ++i) {
      result.setX(i, -y(i));
      result.setY(i, x(i));
    }
    return result;
  }

  Shape Shape::rrot() const {
    Shape result;
    result.tName = tName;
    for (int i = 0; i < 4; ++i) {
      result.setX(i, y(i));
      result.setY(i, -x(i));
    }
    return result;
  }

}; // end of namespace

//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
