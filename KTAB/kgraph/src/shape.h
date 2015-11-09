//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
// start of a simple Tetris clone
//---------------------------------------------
#ifndef TETRIS_SHAPE_H
#define TETRIS_SHAPE_H
//---------------------------------------------

namespace Tetris {
  
  enum TCode { // Character codes for the canonical Tetronimoes
    N = 0, I, J, L, O, S, T, Z
  };

  class Shape { // any valid transformation of a canonical Tetronimo
  public:
    static unsigned int shapeCounter;
    Shape() { idNum = shapeCounter++; setShape(TCode::N); }
    Shape(TCode p) { idNum = shapeCounter++; setShape(p); }
    void setShape(TCode p);
    void setRandomShape();
    TCode getShape() const { return tName; }
    char getName() const { return name;}
    int x(int index) const { assert(0 <= index); assert(index <= 3); return coords[index][0]; }
    int y(int index) const { assert(0 <= index); assert(index <= 3); return coords[index][1]; }

    int minX() const;
    int maxX() const;
    int minY() const;
    int maxY() const;

    void showCoords() const;

    Shape lrot() const;
    Shape rrot() const;

    unsigned int idNum = 0;

  private:
    void setX(int index, int x) { coords[index][0] = x; }
    void setY(int index, int y) { coords[index][1] = y; }
    TCode tName = N; 
    char name = 'N';
    int coords[4][2];
  };

}; // end of namespace

//---------------------------------------------
#endif
//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
