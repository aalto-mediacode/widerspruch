#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"

#include "ofMain.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
private:
    ofSoundPlayer mySound;
    int bufferSize;
    int sampleRate;
    
    ofMesh myMesh;
    ofEasyCam mainCam;
    int width, height;
    float perlinRange;
    
    ofTrueTypeFont fontBold;
    ofTrueTypeFont fontMedium;
    ofTrueTypeFont fontMediumItalic;
    
    float zOffset;
    bool isReadyToPlay;
    bool isLoaded;
    int nBands;
    
    void setupMesh();
    void drawText();
    void drawRightScreen();
    void drawInitialBlop();
    void drawMainBlops(float *spectrum);
    void drawMainMesh(float *spectrum);
    void drawMouseCursor();
    enum class Blop { INITIAL, MAIN_LEFT, MAIN_RIGHT };
    void drawBlopShape(float translateX, float translateY, Blop, float *spectrum);
};
