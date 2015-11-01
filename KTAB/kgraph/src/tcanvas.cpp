//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
// start of a simple Tetris clone
//---------------------------------------------

#include <string.h>
#include "tutils.h"
#include "tcanvas.h"
#include "tmain.h"
#include "tetrisUI.h"


namespace Tetris {

TCanvas::TCanvas(int x, int y, int w, int h, const char * l) : Canvas(x, y, w, h, l) {
    // nothing, yet
}


TCanvas::~TCanvas() {
    // nothing, yet
}

void TCanvas::onMove(int x, int y) {
    auto tui = TetrisUI::theUI;
    if (nullptr != tui) {
        tui->playW->take_focus(); // at least try to get the keyboard events
        auto buff1 = newChar(20);
        sprintf(buff1, "%3i, %3i", x, y);
        std::string buff2 = buff1;
        tui->textCoordXY->value(buff2.c_str());
        tui->playW->redraw();
        delete buff1;
        buff1 = nullptr;
    }
    else {
        // printf("TCanvas::onMove at %i, %i \n", x, y);
    }

    return;
}
void TCanvas::onDrag(int x, int y) {
    //    printf("TCanvas::onDrag at %i, %i \n", x, y);
    return;
}
void TCanvas::onPush(int x, int y, int b) {
    //printf("TCanvas::onPush %i at %i, %i \n", b, x, y);
    auto tui = TetrisUI::theUI;
    if (nullptr != tui) {
        tui->playW->take_focus(); // at least try to get the keyboard events
    }
    return;
}
void TCanvas::onRelease(int x, int y, int b) {
    //    printf("TCanvas::onRelease %i at %i, %i \n", b, x, y);
}
void TCanvas::onKeyDown(int x, int y, int k) {
    //printf("TCanvas::onKeyDown %i at %i, %i \n", k, x, y);
    Tetris::TApp::theApp->processKey(x, y, k);
}
void TCanvas::draw() {
    Canvas::draw();
    if (nullptr != pict) {
        pict->update(this);
    }
    return;
}

}; // end of namespace

//---------------------------------------------
// Copyright Ben Paul Wise. All Rights Reserved.
//---------------------------------------------
