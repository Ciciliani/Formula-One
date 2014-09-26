#pragma once

#include "ofMain.h"
#include "ofxOSC.h"
#include "ciciLib.h"

#define HOST "localhost"
#define PORT 57120

#define WAVETABLE_SIZE 512
#define SC
#define NUM 30
#define NUMSCENES 16

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    
    ofVideoPlayer forwardViewPlayer, backwardViewPlayer;
    float onBoardFilmsAlpha, onBoardShaderAlpha;
    ofVideoPlayer player1, player2, player3, player4;
    int forwardFrame, backwardFrame;
    bool frontOrBack; // 0 fuer Front, 1 fuer Back
    bool b_section;
    
    int videoWidth, videoHeight;
    float imageRatio;
    float xOffset, yOffset;
    vector<float> waveTableX, waveTableY;
    
    ofxOscReceiver oscReceiver;
    ofxOscSender oscSender;
    
    int xPos, yPos;
    float amplitude;
    
    float time0, startTimeOnTarget;
    bool freezeTarget;
    float dia;
    float smoothingArray[25];
    
    ofTexture history[40];
    ofPixels drawingPixels, dataPixels, blackPixels;
    int latestTexture;
    float eraseTextureCounter;
    ofColor latestColor;
    float hue;
    bool onTarget;
    bool tracking1, tracking2;
    bool fullScreen;
    bool isPlaying;
    
    ofShader shader1, shader2;
    float shaderRadius;
    
    bool doNotAllowToReturnToOnBoard;
    
    int shaderGrowth;

    ofTrueTypeFont font, titleFont;
    string playingSynths;
    // for turning on and off the sections
    bool pit_stop, on_board, sarabande, flags;
    unsigned int xFadeForHistory;
    
    //variables for pit-stop
    ofLight lighting;
    ofImage image;
    ofVideoGrabber camera;
    int cameraID;
    
    vector<ofPoint> bubblePositions;
    vector<int> radius;
    vector<float> perlinRandom;
    float playPosition1, playPosition2, playPosition3, playPosition4;
    ofVec2f curserNoise;
    bool barbCam, screenBlend, sect2Start;
    float screenBlendIndex;
    float xComp, yComp;
    float xAxisBlend, yAxisBlend;
    ofColor rightTop;
    float xStart;
    ofTexture grabTexture, grabTexture2;
    float timeOnB;
    bool testTimeB;
    
    ofPolyline ownCar;
    
    //Sarabande
    float fadeToBlack;
    
    ofVideoPlayer loCrashPlayer, midCrashPlayer, hiCrashPlayer;
    
    // Flags + Start
    ofVideoPlayer flagPlayer1, flagPlayer2, flagPlayer3, flagPlayer4, startSequencePlayer;
    bool redOrRedFlag, blueOrBlueFlag, greenOrGreenFlag, yelloOryelloFlag, blackUL, blackUR, blackLL, blackLR, startSequence, semaphoreCounting, startSequenceMotionBlur, fadeToEndBlack, flagShader;
    int flagSzene;
    int upTrigger, lowTrigger;
    timer timer1, timer2;
    int semaphoreCounter;
    ofColor cursorColor;
    vector<bool> flagSceneBools;
    int section;
    
    ofFbo screenFlagFBO, screenFlagFBO2;
    
    vector<titleClass> titles;
};