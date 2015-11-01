//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
// start of a simple Tetris clone
//---------------------------------------------

#include <string.h>
#include "tutils.h"
#include "pvcanvas.h"
#include "tmain.h"


namespace Tetris {

  PVCanvas::PVCanvas(int x, int y, int w, int h, const char * l) : Canvas(x, y, w, h, l) {
    // nothing, yet
  }


  PVCanvas::~PVCanvas() {
    // nothing, yet
  }

  void PVCanvas::onMove(int x, int y) {
    return;
  }
  void PVCanvas::onDrag(int x, int y) {
    return;
  }
  void PVCanvas::onPush(int x, int y, int b) {
    return;
  }
  void PVCanvas::onRelease(int x, int y, int b) {
    return;
  }
  void PVCanvas::onKeyDown(int x, int y, int k) {
    return;
  }
  void PVCanvas::draw() {
    const int px = x();
    const int py = y();
    const int pw = w();
    const int ph = h();
    clearMaps();
    xMap = new KGraph::CoordMap(px, 0.5, px+pw, 5.5);
    yMap = new KGraph::CoordMap(py + ph, 0.5, py, 3.5);
    Canvas::draw();

    fl_push_clip(px, py, pw, ph); 
    Shape ns = TApp::theApp->board->nextShape;
    TCode nc = ns.getShape();
    const Fl_Color clr = TApp::theApp->color((unsigned int)nc);
    const int ci = 2;
    const int cj = 3;
    for (int k = 0; k < 4; k++) {
      TApp::theApp->board->drawUnitSquare(clr,
        ci + ns.y(k), cj + ns.x(k),
        false, FL_WHITE, this);
    } 
    fl_pop_clip();
    return;
  }

}; // end of namespace

//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
