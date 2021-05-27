#include "ofApp.h"

void ofApp::setup(){
    ofBackground(241, 239, 213);
    fontBold.load("EBGaramond-Bold.ttf", 48);
    fontMedium.load("EBGaramond-Medium.ttf", 12);
    fontMediumItalic.load("EBGaramond-MediumItalic.ttf", 12);
    
    zOffset = 0;
    isReadyToPlay = false;
    isLoaded = false;
    nBands = 128;
    
    sampleRate = 44100;
    bufferSize= 512;
    
    setupMesh();
}

void ofApp::setupMesh() {
    width = 50;
    height = 50;
    perlinRange = 1.0;
    mainCam.setDistance(60);
    mainCam.setRelativeYAxis(true);
    
    for (int y = 0; y < height; y++){
        for (int x = 0; x<width; x++){
            myMesh.addVertex(ofPoint(x-width/2,y-height/2,0));
            myMesh.addColor(ofFloatColor(90,90,90));
        }
    }
    
    // here we loop through and join the vertices together as indices to make rows of triangles to make the wireframe grid
    for (int y = 0; y<height-1; y++){
        for (int x=0; x<width-1; x++){
            myMesh.addIndex(x+y*width);                // 0
            myMesh.addIndex((x+1)+y*width);            // 1
            myMesh.addIndex(x+(y+1)*width);            // 10
            
            myMesh.addIndex((x+1)+y*width);            // 1
            myMesh.addIndex((x+1)+(y+1)*width);        // 11
            myMesh.addIndex(x+(y+1)*width);            // 10
        }
    }
}

void ofApp::update(){
    int i=0;
    float *mySpectrum = ofSoundGetSpectrum(nBands);
    for (int y = 0; y<height; y++){
        for (int x=0; x<width; x++){
            int freqBand = ofMap(x, 0, width, 0, nBands-(nBands/4));
            float freqMagnitude = mySpectrum[freqBand];
            ofVec3f newPosition = myMesh.getVertex(i);
            float factor = ofMap(freqBand, 0, nBands, 0, nBands/5);
            newPosition.z = ofNoise(ofMap(x, 0, width, 0, perlinRange),  ofMap(y, 0, height, 0, perlinRange+mouseX/200) ) * freqMagnitude*factor*10;
            myMesh.setVertex(i, newPosition);
            i++;
        }
    }
}

void ofApp::draw(){
    drawText();
    drawInitialBlop();
    
    float *mySpectrum = ofSoundGetSpectrum(nBands);
    for (int i = 0; i < nBands; i++) {
        mySpectrum[i]*=2;
    }
    
    drawRightScreen();
    drawMainMesh(mySpectrum);
    drawMainBlops(mySpectrum);
    drawMouseCursor();
}

void ofApp::drawText() {
    if(!isReadyToPlay) {
        ofSetColor(0, 77, 226);
        fontBold.drawString("widerspruch", 150, 180);
        fontMediumItalic.drawString("en: contradiction; a situation in which inconsistent elements are present", 155, 220);
        if(!isLoaded) {
            fontMedium.drawString("hit spacebar to", ofGetWidth()/1.5+150, ofGetHeight()/1.5);
            fontMedium.drawString("upload your audio file", ofGetWidth()/1.5+150, ofGetHeight()/1.5+18);
        } else {
            fontMedium.drawString("hit 'p' to start the story", ofGetWidth()/1.5+150, ofGetHeight()/1.5);
        }
    }
}

void ofApp::drawInitialBlop() {
    if(!isReadyToPlay) {
        drawBlopShape(ofGetWidth()/1.5, ofGetHeight()/1.5, Blop::INITIAL, NULL);
        zOffset += 0.01;
    }
}

void ofApp::drawMainBlops(float *spectrum) {
    if(isReadyToPlay) {
        const int xLocationOffset = 330;
        drawBlopShape(xLocationOffset, ofGetHeight()/2, Blop::MAIN_LEFT, spectrum);
        ofSetColor(241, 239, 213);
        drawBlopShape(ofGetWidth()-xLocationOffset, ofGetHeight()/2, Blop::MAIN_RIGHT, spectrum);
        zOffset += 0.01;
    }
}

void ofApp::drawBlopShape(float translateX, float translateY, Blop blop, float *spectrum) {
    ofPushMatrix();
    ofTranslate(translateX, translateY);
    ofBeginShape();
    for (float a = 0; a < TWO_PI; a+=0.01) {
        float xOffset = ofMap(cosf(a), -1, 1, 0, 1);
        float yOffset = ofMap(sinf(a), 1, -1, 0, 1);
        float r = 0;
        
        switch(blop) {
            case Blop::MAIN_LEFT:
                r = ofMap(ofNoise(xOffset, yOffset, zOffset), 0, 1, 80+spectrum[int(ofRandom(20))]*100, 90+spectrum[int(ofRandom(20))]*200);
                break;
            case Blop::MAIN_RIGHT:
                r = ofMap(ofNoise(xOffset, yOffset, zOffset), 0, 1, 90+spectrum[int(ofRandom(30,50))]*400, 80+spectrum[int(ofRandom(30,50))]*200);
                break;
            default:
                r = ofMap(ofNoise(xOffset, yOffset, zOffset), 0, 1, 60+(mouseX/70), 70+(mouseY/10));
        }
        
        glm::vec2 buttonCircleV = {r*cos(a), r*sin(a)};
        ofVertex(buttonCircleV);
    }
    ofEndShape();
    ofPopMatrix();
}

void ofApp::drawRightScreen() {
    if(isReadyToPlay) {
        ofSetColor(0, 77, 226);
        ofDrawRectangle(ofGetWidth()/2, 0, ofGetWidth()/2, ofGetWidth());
    }
}

void ofApp::drawMainMesh(float *spectrum) {
    if(isReadyToPlay) {
        ofPushMatrix();
        mainCam.begin();
        myMesh.drawWireframe();
        mainCam.end();
        ofPopMatrix();
    }
}

void ofApp::drawMouseCursor() {
    if (isReadyToPlay) {
        if (mouseX < ofGetWidth() / 2) {
            ofSetColor(0, 77, 226);
        } else {
            ofSetColor(241, 239, 213);
        }
    }
    ofDrawCircle(mouseX, mouseY, 10);
    ofHideCursor();
}

void ofApp::keyPressed(int key){
    if (key == ' ') {
        ofFileDialogResult result = ofSystemLoadDialog("Load file");
        if (result.bSuccess) {
            mySound.load(ofToDataPath(result.getPath()));
            isLoaded = true;
        }
    }
    
    if (key == 'p') {
        isReadyToPlay = true;
        mySound.play();
    }
}
