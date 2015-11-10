//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
// start of a simple set of common graphics for KTAB.
// We use the MVC pattern:
// Model: contains the state information, has internal utility methods.
//        for example, the state of a Tetris game or an SMP simulation.
// View: outputs information on the state. 
//    For example, an FLTK window displaying the state of a Tetris game,
//    the probability distribution in SMP, the quad chart of SMP, etc.
//    Crucially, M->V is one-to-many mapping. I may want several views of
//    the same model, but (at a given moment), each View is only associated with one Model.
//    Hence, each Canvas (V) has a pointer to is Picture (M).
// Controller: this modifies the state of the Model.
//---------------------------------------------
#ifndef KGRAPH_CANVAS_H
#define KGRAPH_CANVAS_H

#include <assert.h>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <functional>
#include <future>
#include <math.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Group.H> 

//#include "kutils.h"

using std::cout;
using std::endl;
using std::flush;
using std::vector;

//---------------------------------------------
namespace KGraph {
  class Canvas;
  class Picture;

  class CoordMap {
  public:
    CoordMap(int s1, double d1, int s2, double d2);
    virtual ~CoordMap();

    int d2s(double d);
    double s2d(int s);

  protected:
    double as = 0;
    double bs = 0;
    double ad = 0;
    double bd = 0;

  private:
    const bool testMap = true; // for a while
    int cmRound(double x);
  };


  // part of the reason for separating Canvas and Picture is 
  // to make editing easy.
  // The concrete class derived from Canvas has to be named
  // in FLTK's Fluid, but the editor provided is pretty basic.
  // Having a separate Picture object moves the editting out
  // of Fluid and into the editor/IDE of your choice.

  class Canvas : public Fl_Box {
    /// Abstract base class
  public:
    Canvas(int x, int y, int w, int h, const char * l = 0);
    virtual ~Canvas();
    void end();

    void updateMaps();
    void clearMaps();

    virtual int handle(int ev);

    // these just print information: override them.
    virtual void onMove(int x, int y);
    virtual void onDrag(int x, int y);
    virtual void onPush(int x, int y, int b);
    virtual void onRelease(int x, int y, int b);
    virtual void onKeyDown(int x, int y, int k);

    Picture* pict = nullptr;
    CoordMap* xMap = nullptr;
    CoordMap* yMap = nullptr;

  protected:

  private:
  };


  class Picture  {
  public:
    Picture();
    virtual ~Picture();

    void add(Canvas * c); // just adds to list
    void update() const; // just walks down the list

    // you will need to customize these
    virtual void connect(Canvas * c); //  initial configuration of the canvas
    virtual void update(Canvas * c) const; //  set the state of the canvas to whatever is needed

    double minX = 0;
    double maxX = 1;
    double minW = 1E-6; // deal with minX == maxX
    double minY = 0;
    double maxY = 1;
    double minH = 1E-6; // deal with minY == mxaY

  protected:
    vector<Canvas*> canvases = {}; // all the views 

  };

}; // end of namespace

//---------------------------------------------
#endif
//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
