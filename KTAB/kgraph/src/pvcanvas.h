//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
// start of a simple Tetris clone
//---------------------------------------------
#ifndef TETRIS_PVCANVAS_H
#define TETRIS_PVCANVAS_H

#include "kutils.h"
#include "kgraph.h"

//---------------------------------------------

using std::cout;
using std::endl;
using std::flush;

using KGraph::Canvas;

//---------------------------------------------
namespace Tetris {


  class PVCanvas : public Canvas {
  public:
    PVCanvas(int x, int y, int w, int h, const char * l = 0);
    virtual ~PVCanvas();
    
    void onMove(int x, int y);
    void onDrag(int x, int y);
    void onPush(int x, int y, int b);
    void onRelease(int x, int y, int b);
    void onKeyDown(int x, int y, int k);
     
  protected:
    virtual void draw();
    // again, user should never call 'draw' directly.
    // just mark damage and/or call 'redraw'
  
  private:
  };

}; // end of namespace

//---------------------------------------------
#endif
//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
