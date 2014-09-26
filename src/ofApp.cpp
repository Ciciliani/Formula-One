#include "ofApp.h"
#include "ciciLib.h"

//void ofApp::titleFunc(string aTitle){
//    static int frameRate = (int)ofGetFrameRate();
//    static float fade = 0.f;
//    static float fadeTime = 5.f;
//    static float stepValues = 1.f/(fadeTime * frameRate);
//    static float holdTime = 10.f;
//    static int holdCounter = 0;
//    
//    
//    if (fade < 1.f && holdCounter == 0) {
//        fade += stepValues;
//    } else if (fade >= 1.f && holdCounter < (holdTime * frameRate)) {
//        fade = 1.f;
//        holdCounter++;
//    }
//    
//    if (holdCounter >= (holdTime * frameRate) && fade >= 0) {
//        fade-= stepValues;
//    }
////    cout << (holdTime * frameRate) << " " << fade << " " << holdCounter << endl;
//    
//    ofDisableLighting();
//    ofSetColor(0, 0, 0, int(100 * fade));
//    ofRect(90, 110 - (titleFont.stringHeight(aTitle) + 20), titleFont.stringWidth(aTitle) + 20, titleFont.stringHeight(aTitle) + 20);
//    ofSetColor(255, 255, 220, int(180 * fade));
//    titleFont.drawStringAsShapes(aTitle, 100, 100);
//}

//--------------------------------------------------------------
void ofApp::setup(){
    
    forwardViewPlayer.loadMovie("ForwardView_mjpg.mov");
    backwardViewPlayer.loadMovie("BackwardView_mjpg.mov");
    onBoardFilmsAlpha = 0;
    onBoardShaderAlpha = 0.5;
    
    player1.loadMovie("Ferrari_Pit_Stop640x360mjpg.mov");
    player2.loadMovie("Mercedes_Pit_Stop640x360mjpg.mov");
    player3.loadMovie("Lotus_Pit_Stop640x360mjpg.mov");
    player4.loadMovie("RedBull_Pit_Stop640x360mjpg.mov");
    
    loCrashPlayer.loadMovie("LoRegCrashMJPG.mov");
    midCrashPlayer.loadMovie("MidRegCrashMJPG.mov");
    hiCrashPlayer.loadMovie("HiRegCrashMJPG.mov");
    
    ofSetWindowShape((int)forwardViewPlayer.getWidth(), (int)forwardViewPlayer.getHeight());// set window to video Size
    fullScreen = false;
    
    forwardFrame = 0;
    backwardFrame = 0;
    forwardViewPlayer.setFrame(forwardFrame);
    
    cameraID = 1;
    camera.setDeviceID(cameraID);
    camera.initGrabber(ofGetWidth()/2, ofGetHeight()/2);
    
    //sections
    pit_stop = false;
    on_board = false;
    sarabande = false;
    b_section = false;
    flags = false;
    doNotAllowToReturnToOnBoard = false;
    
    frontOrBack = 0;
    shaderGrowth = 0;
    
    if ((forwardViewPlayer.getWidth() != backwardViewPlayer.getWidth())||
        forwardViewPlayer.getHeight() != backwardViewPlayer.getHeight()) {
        cout << "ERROR: videos don't match in size!!!" << endl;
    } else {
        videoWidth = (int)forwardViewPlayer.getWidth();
        videoHeight = (int)forwardViewPlayer.getHeight();
    }
    
    shader1.load("shader");
//    shader1.begin();
//    shader1.setUniform2f("size", videoWidth, videoHeight);
//    shader1.end();
    
    shader2.load("shader2");

    imageRatio = ofGetWidth()/(float)videoWidth;
    cout << "videoWidth: " << videoWidth << "; videoHeight: " << videoHeight << "; screenWidth: " << ofGetWidth() << "; ImageRatio: " << imageRatio << endl;
    
    waveTableX.resize(WAVETABLE_SIZE);
    waveTableY.resize(WAVETABLE_SIZE);
    
    for (int i = 0; i < 40; i++) {
        history[i].allocate(videoWidth/4, videoHeight/4, GL_RGBA);
    }
    
    drawingPixels.allocate(videoWidth/4, videoHeight/4, 4);
    blackPixels.allocate(videoWidth/4, videoHeight/4, 4);
    dataPixels.allocate(videoWidth, videoHeight, 4);
    
    latestTexture = 0;
    eraseTextureCounter = 0;
    onTarget = false;
    tracking1 = false;//wird ueberfluessig!!!
    tracking2 = true;//wird ueberfluessig!!!
    
    for (int i = 0; i < 25; i++) {
        smoothingArray[i] = 0.f;
    }
    
    xOffset = 0.f;
    yOffset = 0.f;
    
    time0 = ofGetElapsedTimef();
    freezeTarget = false;
    
    oscReceiver.setup(12345);
    oscSender.setup(HOST, PORT);
    amplitude = 1.f;
    
#ifndef SC
    isPlaying = true;
#else
    isPlaying = false;
#endif
    
    playingSynths = "0";
    
    ofEnableAlphaBlending();
    ofDisableLighting();
    ofSetBackgroundColor(255, 255, 255, 255);
    
    font.loadFont("Eurostile Bold", 40, true, false, true);
    titleFont.loadFont("Eurostile Bold", 40, true, false, true);
    
    for(int x = 0; x < videoWidth/4; x++){
        for (int y = 0; y < videoHeight/4; y++) {
            blackPixels.setColor(x, y, ofColor(0, 0, 0, 0));
        }
    }
    
    for (int i = 0; i < 40; i++) {
        history[i].loadData(blackPixels);
    }
    
    xFadeForHistory = 0;
    
    // pit-stop
    
//    ofEnableDepthTest();
    ofEnableLighting();
    lighting.enable();
    lighting.setDirectional();
    lighting.setPosition(-150, -100, 10);
    ofSetSmoothLighting(true);
    lighting.setDiffuseColor(ofFloatColor(0.7f, 0.6f, 0.5f));
    
    bubblePositions.resize(NUM);
    radius.resize(NUM);
    perlinRandom.resize(NUM);
    playPosition1 = 0;
    playPosition2 = 0;
    playPosition3 = 0;
    playPosition4 = 0;
    barbCam = false;
    screenBlend = false;
    sect2Start = false;
    
    for (int i = 0; i < NUM; i++) {
        bubblePositions[i] = ofPoint((int)ofRandom(0, videoWidth), (int)ofRandom(0, videoHeight));
        radius[i] = (int)ofRandom(40, 150);
        perlinRandom[i] = ofRandom(10000);
    }
    
    curserNoise = ofVec2f(0, 0);
    
    grabTexture.allocate(1280, 720, GL_RGBA);
//    grabTexture.allocate(1280, 720, GL_RGBA);
//    grabTexture2.allocate(1280, 720, GL_RGBA);
    
//    ownCar.addVertex(582, 379);
    ownCar.addVertex(388, 716);
    ownCar.addVertex(465, 597);
    ownCar.addVertex(229, 586);
    ownCar.addVertex(237, 392);
    ownCar.addVertex(159, 347);
    ownCar.addVertex(113, 267);
    ownCar.addVertex(125, 197);
    ownCar.addVertex(155, 150);
    ownCar.addVertex(210, 113);
    ownCar.addVertex(261, 102);
    ownCar.addVertex(332, 124);
    ownCar.addVertex(368, 154);
    ownCar.addVertex(387, 190);
    ownCar.addVertex(433, 193);
    ownCar.addVertex(450, 369);
    ownCar.addVertex(593, 341);
    ownCar.addVertex(620, 283);
    ownCar.addVertex(739, 282);
    ownCar.addVertex(808, 354);
    ownCar.addVertex(942, 358);
    ownCar.addVertex(956, 318);
    ownCar.addVertex(1140, 309);
    ownCar.addVertex(1106, 611);
    ownCar.addVertex(1031, 644);
    ownCar.addVertex(1137, 715);
    ownCar.close();
    
    // flag and start variables
    flagPlayer1.loadMovie("Red_FlagMJPG320x180.mov");
    flagPlayer2.loadMovie("Green_FlagMJPG320x180.mov");
    flagPlayer3.loadMovie("Yellow_FlagMJPG320x180.mov");
    flagPlayer4.loadMovie("Blue_FlagMJPG320x180.mov");
    startSequencePlayer.loadMovie("F1_Start_SequenceMJPG.mov");
    startSequencePlayer.setFrame(0);
    
    flagPlayer1.setSpeed(1);
    flagPlayer2.setSpeed(0.5);
    flagPlayer3.setSpeed(0.5);
    flagPlayer4.setSpeed(0.5);
    
    redOrRedFlag = true;
    greenOrGreenFlag = true;
    blueOrBlueFlag = true;
    yelloOryelloFlag = true;
    blackUL = blackUR = blackLL = blackLR = true;
    startSequence = false;
    
    upTrigger = 1;// ready to receive upTrigger
    lowTrigger = 0;
    flagSzene = 0;
    semaphoreCounter = 0;
    semaphoreCounting = true;
    startSequenceMotionBlur = false;
    
    flagSceneBools.resize(NUMSCENES);
    for (int i = 0; i < NUMSCENES; i++) {
        flagSceneBools[i] = true;
    }
    
    screenFlagFBO.allocate(1280, 720);
    screenFlagFBO2.allocate(1280, 720);
    
    fadeToEndBlack = false;
    flagShader = true;
    
    section = 100;
    titles.resize(5);
    
    for (int i = 0; i < 5; i++) {
        titles[i] = titleClass();
    }
    
};

void ofApp::update(){
    
#ifndef SC
    xPos = (int)mapping(mouseX, 0.f, ofGetWidth(), 0.f, videoWidth-1);
    yPos = (int)mapping(mouseY, 0, ofGetHeight(), 0, videoHeight-6);

    xPos = (int)ofClamp(xPos, 0, videoWidth - 1);
    yPos = (int)ofClamp(yPos, 0, videoHeight - 6);
#endif

    ofxOscMessage sendOSC1, sendOSC2, feedback2SC, toSC1, toSC2;
    
    static bool thirdPhraseOfPitStop = false;
    static bool thirdPhraseTransition = false;
    static float durThirdPhrase;
    
    while(oscReceiver.hasWaitingMessages()){
        ofxOscMessage m;
        
        oscReceiver.getNextMessage(&m);
        if (m.getAddress() == "\vlnData") {
            float pitch = m.getArgAsFloat(0);
            
            amplitude = m.getArgAsFloat(1);//amp with lag
            yPos = (int)((1.f - amplitude) * float(videoHeight - 6));
        
            if (m.getArgAsFloat(2) > 0.05) {//amp without lag
                isPlaying = 1;
//                if (amplitude > 0.1) {
                    xPos = (int)(pitch * float(videoWidth - 1));
//                }
                
            } else if (m.getArgAsFloat(2) <= 0.01 && isPlaying){
                isPlaying = 0;
                shaderGrowth = 0;
            }
            
            playingSynths = m.getArgAsString(3);
            
            if (xPos > (0.1 * videoWidth) && !frontOrBack && !doNotAllowToReturnToOnBoard) {
                frontOrBack = 1;
                b_section = false;
                forwardViewPlayer.setFrame(forwardFrame);
                forwardViewPlayer.play();
                backwardFrame = backwardViewPlayer.getCurrentFrame();
                backwardViewPlayer.stop();
            }
            if (xPos <= (0.1 * videoWidth) && frontOrBack) {
                b_section = true;
                frontOrBack = 0;
                feedback2SC.setAddress("/bSection");
                feedback2SC.addIntArg(1);
                oscSender.sendMessage(feedback2SC);
                
                backwardViewPlayer.setFrame(backwardFrame);
                backwardViewPlayer.play();
                forwardFrame = forwardViewPlayer.getCurrentFrame();
                forwardViewPlayer.stop();
            }
            
        }
        else if (m.getAddress() == "barbCam" && !flags) {
            if (m.getArgAsInt32(0) > 0) {
                barbCam = true;
//                if (!thirdPhraseOfPitStop) {
//                    thirdPhraseOfPitStop = true;
//                    durThirdPhrase = ofGetElapsedTimef();
                    // during pit_stop: exchange film in top right rectangle with on-board film! Equivalent to pressing '2' on the keyboard
//                    if (pit_stop) {
//                        sect2Start = true;
//                        forwardViewPlayer.setSpeed(0.25);
//                        forwardViewPlayer.setFrame(0);
//                    }
//                }
//                if (ofGetElapsedTimef() - durThirdPhrase >=15 && !thirdPhraseTransition) {
//                    thirdPhraseTransition = true;
                    // x-fade from pit-stop to on-board. Equivalent to pressing '3' on the keyboard
//                    screenBlend = !screenBlend;
//                    sect2Start = true;
//                    screenBlendIndex = 0;
//                    forwardViewPlayer.setSpeed(1);
//                    on_board = true;
//                    toSC1.setAddress("/pit_stop");
//                    toSC1.addIntArg(0);
//                    oscSender.sendMessage(toSC1);
//                    toSC2.setAddress("/on_board");
//                    toSC2.addIntArg(1);
//                    oscSender.sendMessage(toSC2);
//                }
            } else {
                barbCam = false;
            }
        }
        else if (m.getAddress() == "upTrigger" && upTrigger == 1 && flags) {
            
            upTrigger = 0;
            lowTrigger = 1;
            timer2.startTiming();
            cout << "upTrigger" << endl;
            cursorColor = ofFloatColor(0.f, 1.f, 0.f);
        }
        else if (m.getAddress() == "lowTrigger" && lowTrigger == 1) {
            cout << "lowTrigger" << endl;
            
            lowTrigger = 0;
            upTrigger = 1;
            cursorColor = ofFloatColor(1.f, 1.f, 0.f);
            
            if (timer2.getTime() <= 2.f && flagSzene < 13) {
                flagSzene++;
                cout << flagSzene << endl;
                timer2.stop();
            } else {
                timer2.stop();
            }
        }
    }
    
    if (on_board||sect2Start) {
        if(forwardViewPlayer.isPlaying()) forwardViewPlayer.update();
        if(backwardViewPlayer.isPlaying()) backwardViewPlayer.update();
    }
    
    if (on_board) {
        
        // the main chunk of the on-board section
        
        
        if (forwardViewPlayer.isFrameNew()||backwardViewPlayer.isFrameNew()) {
            
            ofPixels &pixels = (forwardViewPlayer.isPlaying()) ? forwardViewPlayer.getPixelsRef() : backwardViewPlayer.getPixelsRef();
            
            sendOSC1.setAddress("/waveTableX");
            
            float widthRatio = (float)videoWidth/(float)WAVETABLE_SIZE; // needed to stretch from WAVETABLE_SIZE to width of video
            float heightRatio = (float)videoHeight/(float)WAVETABLE_SIZE; // needed to stretch from WAVETABLE_SIZE to height of video

            
            for (int i = 0; i < WAVETABLE_SIZE; i++) {
                unsigned int pixelBasedInd = int(yPos * pixels.getWidth() * pixels.getBytesPerPixel() + (i * widthRatio * pixels.getBytesPerPixel()));
                int r = pixels[pixelBasedInd];
                int g = pixels[pixelBasedInd + 1];
                int b = pixels[pixelBasedInd + 2];
                
                float v = (float)(r + g + b)/3; // lightness = average of r, g and b
                
                //            float v = pixels.getColor(i * 2, yPos).getLightness();
                waveTableX.at(i) = (float)(v/128) - 1.f;
                
                sendOSC1.addFloatArg(waveTableX.at(i));
                
            }
            
            for (int i = 0; i < WAVETABLE_SIZE; i++) {
                //            float v = pixels.getColor(xPos, int(i * 1.5)).getBrightness();
                unsigned int pixelBasedInd = int(i * heightRatio) * pixels.getWidth() * pixels.getBytesPerPixel() + int(xPos * pixels.getBytesPerPixel());
                int r = pixels[pixelBasedInd];
                int g = pixels[pixelBasedInd + 1];
                int b = pixels[pixelBasedInd + 2];
                
                float v = (float)(r + g + b)/3; // lightness = average of r, g and b
                waveTableY.at(i) = (float)(v/128) - 1.f;
                
                sendOSC1.addFloatArg(waveTableY.at(i));
            }
            oscSender.sendMessage(sendOSC1);
            
            
            if (forwardViewPlayer.isPlaying()) {
                
                eraseTextureCounter = 0;
                // motion draw
                hue += 3.f;
                latestColor.setHsb(hue, 255, 255, 100);
                if (hue > 55.f) {
                    hue = 0;
                }
                ofColor clear(0, 0, 0, 0);
                
                for (int x = 0; x < videoWidth/4; x++) {
                    for (int y = 0; y < videoHeight/4; y++) {
                        unsigned int pixelBasedInd = y * 4 * pixels.getWidth() * pixels.getBytesPerPixel() + x * 4 * pixels.getBytesPerPixel();
                        int r = pixels[pixelBasedInd] - dataPixels[pixelBasedInd];
                        int g = pixels[pixelBasedInd + 1] - dataPixels[pixelBasedInd + 1];
                        int b = pixels[pixelBasedInd + 2] - dataPixels[pixelBasedInd + 2];
                        
                        int diff = r + g + b;
                        
                        if (diff > (10 + amplitude * 80)) {
                            drawingPixels.setColor(x, y, latestColor);
                        } else {
                            drawingPixels.setColor(x, y, clear);
                        }
                    }
                }
                dataPixels = pixels;
                
                history[latestTexture].loadData(drawingPixels);
                latestTexture++;
                if (latestTexture > 39) {
                    latestTexture = 0;
                }
                
                short greyPixels = 0;
                
                for (int i = 0; i < 9; i++) {
                    for (int j = 0; j < 9; j++) {
                        if ((xPos > 3)&&(yPos > 3)) {
                            unsigned int pixelInd = int((xPos - 4 + i) * pixels.getBytesPerPixel() + (yPos - 4 + j) * pixels.getBytesPerPixel() * pixels.getWidth());
                            int pixelRed = pixels[pixelInd];
                            int pixelGreen = pixels[pixelInd + 1];
                            int pixelBlue = pixels[pixelInd + 2];
                            
                            if (tracking1) {
                                if ((abs(pixelRed - pixelGreen) < 25)&&(abs(pixelRed - pixelBlue) < 25)&&(abs(pixelBlue - pixelGreen) < 25)) {// if all colors show less difference than 15, it's assumed that the color is grey
                                    if (((pixelRed > 40)&&(pixelRed < 130))&&((pixelGreen > 40)&&(pixelGreen < 130))&&((pixelBlue > 40)&&(pixelBlue < 130))) {
                                        greyPixels++;
                                    }
                                    
                                }
                            }//tracking 1 kann wahrscheinlich abgeschafft werden
                            if (tracking2) {
                                float sat = ofColor(pixelRed, pixelGreen, pixelBlue).getSaturation();
                                //                        cout << sat << endl;
                                if (sat < 30) {
                                    greyPixels++;
                                }
                            }
                        }
                    }
                }
                
                if (!freezeTarget && (greyPixels < 81) && (amplitude > 0) && !ownCar.inside(xPos, yPos)) {// check if the curser's on target, if it is, it is freezed for one second
                        onTarget = true;
                        freezeTarget = true;
                        time0 = ofGetElapsedTimef();
                }
                if (greyPixels >= 81 && (ofGetElapsedTimef() - time0) > 1.f) {
                    onTarget = false;
                    freezeTarget = false;
                    startTimeOnTarget = ofGetElapsedTimef() - time0;//muss immer >1.0 sein
                }
                
                if (!pit_stop) { // send the osc messages only, if transition from pit-stop is over
                    feedback2SC.setAddress("/onTarget");
                    feedback2SC.addIntArg(onTarget);
                    
                    if (onTarget) {
                        feedback2SC.addFloatArg(greyPixels);
                    } else {
                        feedback2SC.addFloatArg(ofGetElapsedTimef() - time0); // deltaTime //ofGetElapsedTimef() - time0
                        feedback2SC.addFloatArg(ofGetElapsedTimef());//timeStamp
                    }
                    //            feedback2SC.addFloatArg(greyPixels);
                    oscSender.sendMessage(feedback2SC);
                }
                
                static int counter = 0;
                if (onTarget) {
                    smoothingArray[counter] = greyPixels;
                    counter++;
                    counter %= 25;
                    
                    float sum = 0;
                    for (int i = 0; i < 25; i++) {
                        sum = sum + smoothingArray[i];
                    }
                    dia = mapping(sum / 25, 0, 80, 300, 50);// nice but it has to be smoothed
                } else { dia = 100;}
            }
            
            if (backwardViewPlayer.isPlaying()) {
                onTarget = false;
                
                feedback2SC.setAddress("/onTarget");
                feedback2SC.addIntArg(onTarget);
                feedback2SC.addFloatArg(0);
                oscSender.sendMessage(feedback2SC);
                
                if (eraseTextureCounter < 40) {
                    history[(int)eraseTextureCounter].loadData(blackPixels);
                }
                eraseTextureCounter = eraseTextureCounter + 0.1f;
            }
        }
        if (!isPlaying) {
            camera.update();
        }
    }
    
    float time = ofGetElapsedTimef() * 0.25f;
    
    if (pit_stop) {
    
        // pit-stop
        player1.update();
        player2.update();
        barbCam ? camera.update() : player3.update();
        player4.update();
        
        for (int i = 0; i < NUM; i++) {
            ofClamp(bubblePositions[i].x += (ofSignedNoise(time + (ofSignedNoise(time + i * 100)) + perlinRandom[i]) * 0.5), 0, videoWidth);
            ofClamp(bubblePositions[i].y += (ofSignedNoise(time + (ofSignedNoise(time + i * 150)) + perlinRandom[NUM - 1 - i]) * 0.5), 0, videoHeight);
        }
        
        curserNoise = ofVec2f(ofSignedNoise(time * 2 + perlinRandom[NUM]) * 50, ofSignedNoise(time * 2 + perlinRandom[NUM-1]) * 50);
        
        playPosition1 = ofNoise(time/2 + 2376);
        
        if (sect2Start && !forwardViewPlayer.isPlaying()) {
            forwardViewPlayer.setFrame(0);
            forwardViewPlayer.play();
        }
        else {
            playPosition2 = ofNoise(time/2 + 3678);
            player2.setPosition(playPosition2);
        }
        
        playPosition3 = ofNoise(time/2 + 9664);
        playPosition4 = ofNoise(time/2 + 245);
        player1.setPosition(playPosition1);
        player3.setPosition(playPosition3);
        player4.setPosition(playPosition4);
    }
    
    if (sarabande) {
        if (xPos <= (1.0/3.0 * (float)videoWidth)) {
            loCrashPlayer.update();
        } else if (xPos <= (2.0/3.0 * (float)videoWidth))
        {
            midCrashPlayer.update();
        } else {
            hiCrashPlayer.update();
        }
        
        float fadeTime = 5.f; //seconds
        float steps = fadeTime * ofGetFrameRate();
        float fadeValue = 255.f/steps;
        
        if (!isPlaying && fadeToBlack >=0) {
            fadeToBlack -= fadeValue;
        } else if (isPlaying && fadeToBlack <= 255) {
            fadeToBlack += fadeValue;
        }
        float position = fmod((((xPos) * 3.f)/videoWidth), 1.f) + ((ofSignedNoise(time + 200)/15) * (255.f - fadeToBlack)/255.f);
        
        if (xPos <= (1.0/3.0 * (float)videoWidth)) {
            loCrashPlayer.setPosition(position);
        } else if (xPos <= (2.0/3.0 * (float)videoWidth))
        {
            midCrashPlayer.setPosition(position);
        } else {
            hiCrashPlayer.setPosition(position);
        }
    }
    ///////////////////////// SARABANDE END /////////////////////////
    //////////////////////// FLAGS BEGINNING ////////////////////////
    
    if (flags) {
        if (upTrigger == 0 && timer2.getTime() > 3) {
            upTrigger = 1;
            cursorColor = ofFloatColor(1.f, 1.f, 0.f);
        } // falls versehentlich upTrigger getaetigt wurde, wird es hier nach 3 Sekunden resettet
        
        ofxOscMessage toSC;
        
        switch (flagSzene) {
            case 0:
                if (flagSceneBools[0] == true) {
                    flagSceneBools[0] = false;
                    blackUL = blackUR = blackLL = blackLR = true;
                    toSC.setAddress("/flags");
                    toSC.addIntArg(0);
                    flagPlayer1.play();// erst starten wenn sie auch wirklich dran sind?!
                    flagPlayer2.play();
                    flagPlayer3.play();
                    flagPlayer4.play();
                }
                break;
                
            case 1:
                if (flagSceneBools[1] == true) {
                    flagSceneBools[1] = false;
                    blackUL = false;
                    redOrRedFlag = false; // red color in UL, rest black
                    blackUR = blackLL = blackLR = true;
                    toSC.setAddress("/flags");
                    toSC.addIntArg(1);
                }
                break;
                
            case 2:
                if (flagSceneBools[2] == true) {
                    flagSceneBools[2] = false;
                    blackUL = false;
                    redOrRedFlag = false;
                    blackLR = false;
                    blueOrBlueFlag = false;
                    blackUR = blackLL = true; // red and blue panel, rest black
                    toSC.setAddress("/flags");
                    toSC.addIntArg(2);
                }
                break;
                
            case 3:
                if (flagSceneBools[3] == true) {
                    flagSceneBools[3] = false;
                    blackUL = false;
                    redOrRedFlag = false;
                    blackUR = true;
                    blackLL = false;
                    yelloOryelloFlag = false;
                    blackLR = false;
                    blueOrBlueFlag = true; // red and yellow panel, blue film, ur black
                    toSC.setAddress("/flags");
                    toSC.addIntArg(3);
                }
                break;
                
            case 4:
                if (flagSceneBools[4] == true) {
                    flagSceneBools[4] = false;
                    blackUL = false;
                    redOrRedFlag = true;
                    blackUR = true;
                    blackLL = false;
                    yelloOryelloFlag = false;
                    blackLR = false;
                    blueOrBlueFlag = true; // yellow panel, red and blue film, ur black
                    toSC.setAddress("/flags");
                    toSC.addIntArg(4);
                }
                break;
                
            case 5:
                if (flagSceneBools[5] == true) {
                    flagSceneBools[5] = false;
                    blackUL = false;
                    redOrRedFlag = true;
                    blackUR = false;
                    greenOrGreenFlag = true;
                    blackLL = false;
                    yelloOryelloFlag = false;
                    blackLR = false;
                    blueOrBlueFlag = false; // yellow and blue panel, red and green film
                    toSC.setAddress("/flags");
                    toSC.addIntArg(5);
                }
                break;
                
            case 6:
                if (flagSceneBools[6] == true) {
                    flagSceneBools[6] = false;
                    blackUL = false;
                    redOrRedFlag = false;
                    blackUR = false;
                    greenOrGreenFlag = true;
                    blackLL = false;
                    barbCam = false;
                    yelloOryelloFlag = true;
                    blackLR = false;
                    blueOrBlueFlag = false; // red and blue panel, yellow and green film
                    toSC.setAddress("/flags");
                    toSC.addIntArg(6);
                }
                break;
                
            case 7:
                if (flagSceneBools[7] == true) {
                    flagSceneBools[7] = false;
                    blackUL = false;
                    redOrRedFlag = false;
                    blackUR = false;
                    greenOrGreenFlag = true;
                    blackLL = false;
                    barbCam = false;
                    yelloOryelloFlag = true;
                    blackLR = false;
                    blueOrBlueFlag = true; // red panel, yellow, green and blue film
                    toSC.setAddress("/flags");
                    toSC.addIntArg(7);
                }
                break;
                
            case 8:
                if (flagSceneBools[8] == true) {
                    flagSceneBools[8] = false;
                    blackUL = true;
                    blackUR = false;
                    greenOrGreenFlag = false;
                    blackLL = false;
                    barbCam = false;
                    yelloOryelloFlag = true;
                    blackLR = false;
                    blueOrBlueFlag = true; // UL black, green panel, yellow and blue film
                    toSC.setAddress("/flags");
                    toSC.addIntArg(8);
                }
                break;
                
            case 9:
                if (flagSceneBools[9] == true) {
                    flagSceneBools[9] = false;
                    blackUL = true;
                    blackUR = false;
                    greenOrGreenFlag = false;
                    blackLL = false;
                    yelloOryelloFlag = false;
                    blackLR = true; // UL and LR black, green and yellow panel
                    toSC.setAddress("/flags");
                    toSC.addIntArg(9);
                }
                break;
                
            case 10:
                if (flagSceneBools[10] == true) {
                    flagSceneBools[10] = false;
                    blackUL = true;
                    blackUR = false;
                    greenOrGreenFlag = false;
                    blackLL = false;
                    barbCam = true;
                    yelloOryelloFlag = true;
                    blackLR = true; // UL and LR black, green panel, yellow BarbCam
                    toSC.setAddress("/flags");
                    toSC.addIntArg(10);
                }
                break;
                
            case 11:
                if (flagSceneBools[11] == true) {
                    flagSceneBools[11] = false;
                    blackUL = false;
                    redOrRedFlag = false;
                    blackUR = false;
                    greenOrGreenFlag = true;
                    blackLL = false;
                    barbCam = true;
                    yelloOryelloFlag = true;
                    blackLR = false; // UL and LR black, green and yellow panel
                    blueOrBlueFlag = false;
                    toSC.setAddress("/flags"); // yellow BarbCam,  red and blue panel, green film
                    toSC.addIntArg(11);
                }
                break;
                
            case 12:
                if (flagSceneBools[12] == true) {
                    flagSceneBools[12] = false;
                    blackUL = false;
                    redOrRedFlag = true;
                    blackUR = false;
                    greenOrGreenFlag = true;
                    blackLL = false;
                    barbCam = true;
                    yelloOryelloFlag = true;
                    blackLR = false;
                    blueOrBlueFlag = true;
                    toSC.setAddress("/flags"); // yellow BarbCam,  red, blue and green film
                    toSC.addIntArg(12);
                }
                break;
                
            case 13:
                if (flagSceneBools[13] == true) {
                    flagSceneBools[13] = false;
                    blackUL = true;
                    blackUR = true;
                    blackLL = false;
                    barbCam = true;
                    yelloOryelloFlag = false;
                    blackLR = true;
                    toSC.setAddress("/flags"); // yellow BarbCam,  red, blue and green film
                    toSC.addIntArg(13);
                }
                break;
                
            case 14:
                if (flagSceneBools[14] == true) {
                    flagSceneBools[14] = false;
                    toSC.setAddress("/flags"); // yellow BarbCam,  red, blue and green film
                    toSC.addIntArg(14);
                }
                break;
            case 15:
                if (flagSceneBools[15] == true) {
                    flagSceneBools[15] = false;
                    toSC.setAddress("/flags"); // this releases the synths in SC and starts the motionblur for the end
                    toSC.addIntArg(15);
                    startSequenceMotionBlur = true;
                    section = 4;
                }
                break;
                
            default:
                blackUL = blackUR = blackLL = blackLR = true;
                break;
        }
        
        oscSender.sendMessage(toSC);
        
        if (redOrRedFlag) {
            flagPlayer1.update();
        }
        if (greenOrGreenFlag) {
            flagPlayer2.update();
        }
        if (blueOrBlueFlag) {
            flagPlayer4.update();
        }
        if (yelloOryelloFlag) {
            if (barbCam) {
                camera.update();
            } else {
                flagPlayer3.update();
            }
        }
        if (startSequence) {
            startSequencePlayer.update();
        }
    }
    
    if (startSequenceMotionBlur) {
        
        if (startSequencePlayer.isFrameNew()) {
            ofPixels &pixels = startSequencePlayer.getPixelsRef();
            eraseTextureCounter = 0;
            // motion draw
            hue += 3.f;
            latestColor.setHsb(hue, 255, 255, 100);
            if (hue > 55.f) {
                hue = 0;
            }
            ofColor clear(0, 0, 0, 0);
            
            for (int x = 0; x < videoWidth/4; x++) {
                for (int y = 0; y < videoHeight/4; y++) {
                    unsigned int pixelBasedInd = y * 4 * pixels.getWidth() * pixels.getBytesPerPixel() + x * 4 * pixels.getBytesPerPixel();
                    int r = pixels[pixelBasedInd] - dataPixels[pixelBasedInd];
                    int g = pixels[pixelBasedInd + 1] - dataPixels[pixelBasedInd + 1];
                    int b = pixels[pixelBasedInd + 2] - dataPixels[pixelBasedInd + 2];
                    
                    int diff = r + g + b;
                    
                    if (diff > (10 + amplitude * 80 * 1.5)) {
                        drawingPixels.setColor(x, y, latestColor);
                    } else {
                        drawingPixels.setColor(x, y, clear);
                    }
                }
            }
            dataPixels = pixels;
            
            history[latestTexture].loadData(drawingPixels);
            latestTexture++;
            if (latestTexture > 39) {
                latestTexture = 0;
            }
            
            short greyPixels = 0;
            
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if ((xPos > 3)&&(yPos > 3)) {
                        unsigned int pixelInd = int((xPos - 4 + i) * pixels.getBytesPerPixel() + (yPos - 4 + j) * pixels.getBytesPerPixel() * pixels.getWidth());
                        int pixelRed = pixels[pixelInd];
                        int pixelGreen = pixels[pixelInd + 1];
                        int pixelBlue = pixels[pixelInd + 2];
                        
                        if (tracking1) {
                            if ((abs(pixelRed - pixelGreen) < 25)&&(abs(pixelRed - pixelBlue) < 25)&&(abs(pixelBlue - pixelGreen) < 25)) {// if all colors show less difference than 15, it's assumed that the color is grey
                                if (((pixelRed > 40)&&(pixelRed < 130))&&((pixelGreen > 40)&&(pixelGreen < 130))&&((pixelBlue > 40)&&(pixelBlue < 130))) {
                                    greyPixels++;
                                }
                                
                            }
                        }//tracking 1 kann wahrscheinlich abgeschafft werden
                        if (tracking2) {
                            float sat = ofColor(pixelRed, pixelGreen, pixelBlue).getSaturation();
                            //                        cout << sat << endl;
                            if (sat < 30) {
                                greyPixels++;
                            }
                        }
                    }
                }
            }
            static int counter = 0;

            smoothingArray[counter] = greyPixels;
            counter++;
            counter %= 25;
            
            float sum = 0;
            for (int i = 0; i < 25; i++) {
                sum = sum + smoothingArray[i];
            }
            dia = mapping(sum / 25, 0, 80, 300, 50);// nice but it has to be smoothed
        }
    }
}

//--------------------------------------------------------------

void ofApp::draw(){
    
    
    ofScale(imageRatio, imageRatio);
    ofTranslate(xOffset, yOffset);
    ofPushMatrix();
    ofRect(-xOffset, 0, ofGetWidth()+ xOffset, 1000);
    
    if (pit_stop) {
        
        float xFadeTime = 15.f;
        float xFadeSteps = xFadeTime * ofGetFrameRate(); // total number of Frames for xFade
        
        if (screenBlend) {
            if (screenBlendIndex == 0) {
                yAxisBlend = xPos;
                xAxisBlend = yPos;//(width - yAxisBlend)  * ((float)height/(float)width);
                
                xComp = xAxisBlend;
                yComp = yAxisBlend;
                xStart = xAxisBlend;
                screenBlendIndex++;
                rightTop = ofColor(0, 255, 0);
            }
            else if (xComp < videoHeight && xComp >=0 && yComp < videoWidth && yComp >=0)
            {
                float xGrow, yGrow;
                xGrow = (videoHeight - xAxisBlend) / xFadeSteps;
                //            xAxisBlend += xGrow;
                xComp += xGrow; //= xAxisBlend;
                
                yGrow = (yAxisBlend) / xFadeSteps;
                //            yAxisBlend -= yGrow;
                yComp -= yGrow;//yAxisBlend;
                rightTop.r = (char)mapping((int)xComp, xStart, videoHeight, 0, 255);
                rightTop.g = (char)mapping((int)xComp, xStart, videoHeight, 255, 100);
                rightTop.b = (char)mapping((int)xComp, xStart, videoHeight, 0, 100);
                xFadeForHistory = (int)mapping((int)xComp, xStart, videoHeight, 0, 255);
                if (xComp >= videoHeight) {
                    pit_stop = false;
                    on_board = true; // transition to next section!!!
                }
            };
        } else {
            xComp = yPos;
            yComp = xPos;
            rightTop = ofColor(0, 255, 0);
        }
        
        ofSetColor(255, 0, 0);
        player1.draw(0, 0, yComp, xComp);
        ofSetColor(rightTop);
        if (!sect2Start) {
            player2.draw(yComp, 0, videoWidth - yComp, xComp);
        } else {
            forwardViewPlayer.draw(yComp, 0, videoWidth - yComp, xComp);
        }
        
        ofSetColor(255, 255, 0);
        if (barbCam) {
            camera.draw(0, xComp, yComp, videoHeight - xComp);
        } else {
            player3.draw(0, xComp, yComp, videoHeight - xComp);
        }
        ofSetColor(100, 100, 255);
        player4.draw(yComp, xComp, videoWidth - yComp, videoHeight - xComp);
        
        ofSetColor(255, 0, 0, 100);
//        ofSetColor(255, 50, 50, 255);
        ofRect(0, (yPos - 2), videoWidth, 5);
        ofRect((xPos - 2), 0, 5, videoHeight);
        
        grabTexture.loadScreenData(0, 0, ofGetWidth(), ofGetHeight());
        
        shader1.begin();
        
        shader1.setUniform1i("texture0", 0);
        //    grabTexture.bind();
        
        ofDisableDepthTest();
        
        for ( int i = 0; i<10; i++ ) {
            ofVec2f center;// = ofVec2f( ofRandom(videoPlayer.width), ofRandom(videoPlayer.height) );
            float rad;
            if(i < (10-1))
            {
                center = bubblePositions[i];
                rad = radius[i];
                shader1.setUniform4f("colorDef", 1.f, 1.f, 1.f, (255.f - xFadeForHistory)/255.f);
                
            } else if(i == (10-1)) {// navigating blop
                center = ofVec2f(xPos, yPos) + curserNoise;
                rad = 120;
                shader1.setUniform4f("colorDef", 1.f, 0.f, 0.f, (255.f - xFadeForHistory)/255.f);
            }
            ofMesh rect;
            rect.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x-rad,center.y-rad));
            rect.addTexCoord(ofVec2f(center.x-rad,center.y-rad));//
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x+rad,center.y-rad));
            rect.addTexCoord(ofVec2f(center.x+rad,center.y-rad));//
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x+rad,center.y+rad));
            rect.addTexCoord(ofVec2f(center.x+rad,center.y+rad));//
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x-rad,center.y+rad));
            rect.addTexCoord(ofVec2f(center.x-rad,center.y+rad));//
            
            shader1.setUniform2f("center", center.x, center.y);
            shader1.setUniform2f("size", rad*2, rad*2);
            
            rect.draw();
        }
        shader1.end();
    }
    ///////////////////////// PIT-STOP END /////////////////////////
    //////////////////////// ON-BOARD BEGIN ////////////////////////
    ofxOscMessage sarabandeMessage;
    
    if (on_board) {

        ofSetColor(255, 100, 100, 255);
        if(forwardViewPlayer.isPlaying()&&!pit_stop) forwardViewPlayer.draw(0, 0, videoWidth, videoHeight);
        if(backwardViewPlayer.isPlaying()&&!pit_stop) backwardViewPlayer.draw(0, 0, videoWidth, videoHeight);
        
        ofSetColor(100, 50, 0, 187);
        ofRect(204, 243, 100, 60);
        
        stringstream convert(playingSynths);
        int playingSynthsInt;
        convert >> playingSynthsInt;
        
        ofColor firstColor, secondColor;
        
        if (playingSynthsInt < 6){
            firstColor = ofColor(255, 200, 127, 255/2);
            secondColor = ofColor(127, 200, 255, 255/2);
        } else if (playingSynthsInt < 11){
            firstColor = ofColor(255, 100, 0, 255/2);
            secondColor = ofColor(100, 255, 0, 255/2);
        } else {
            firstColor = ofColor(255, 55, 0, 255);
            secondColor = ofColor(255, 200, 0, 255);
        }
        float rand1, rand2, rand3, rand4;
        
        rand1 = ofRandom(-playingSynthsInt, playingSynthsInt);
        rand2 = ofRandom(-playingSynthsInt, playingSynthsInt);
        rand3 = ofRandom(-playingSynthsInt, playingSynthsInt);
        rand4 = ofRandom(-playingSynthsInt, playingSynthsInt);
        
        ofBlendMode(OF_BLENDMODE_ADD);
        if (!pit_stop) {
            ofSetColor(firstColor);
            font.drawStringAsShapes(playingSynths, 251 + rand1/3 - font.stringWidth(playingSynths)/2, 294 + rand2/3);
            ofSetColor(secondColor);
            font.drawStringAsShapes(playingSynths, 251 + rand3/3 - font.stringWidth(playingSynths)/2, 294 + rand4/3);
            section = 1;//just for title;
        }
        
        ofSetColor(0, 0, 0, (int)onBoardFilmsAlpha);
        ofRect(0, 0, videoWidth, videoHeight); // fade possibility for pit-stop stuff, except shader
        
        ofSetColor(255, 255, 255, xFadeForHistory);
        for (int i = 0; i < 40; i++) {
            history[i].draw(0, 0, videoWidth, videoHeight);
        }

        if (!isPlaying) {
            float rad;
            shaderGrowth++;
            rad = ofClamp(pow((float)shaderGrowth, 1.3f), 0, 10000);
            
            if (testTimeB) {
                testTimeB = false;
                timeOnB = ofGetElapsedTimef();
            }
            
            
            shader1.begin();
            shader1.setUniform4f("colorDef", 1.f, 0.39f, 0.39f, onBoardShaderAlpha); // same color as for films
//            ofTexture &videoTexture = forwardViewPlayer.getTextureReference();
            ofTexture &videoTexture = camera.getTextureReference();
            shader1.setUniform1i("texture0", 0);
            videoTexture.bind();

            ofDisableDepthTest();
            
            static bool shaderFadeHasStarted = false;

            ofVec2f center = ofVec2f(xPos, yPos);
            
            if(shaderFadeHasStarted){
                shaderRadius *= 0.994;
            } else {shaderRadius = rad;}
            
            ofMesh bubbleMesh;
            bubbleMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            bubbleMesh.addNormal(ofVec3f(0, 0, 1));
            bubbleMesh.addVertex(ofVec3f(center.x-shaderRadius,center.y-shaderRadius));
            bubbleMesh.addTexCoord(ofVec2f(center.x/2-shaderRadius/2,center.y/2-shaderRadius/2));
            bubbleMesh.addNormal(ofVec3f(0, 0, 1));
            bubbleMesh.addVertex(ofVec3f(center.x+shaderRadius,center.y-shaderRadius));
            bubbleMesh.addTexCoord(ofVec2f(center.x/2+shaderRadius/2,center.y/2-shaderRadius/2));
            bubbleMesh.addNormal(ofVec3f(0, 0, 1));
            bubbleMesh.addVertex(ofVec3f(center.x+shaderRadius,center.y+shaderRadius));
            bubbleMesh.addTexCoord(ofVec2f(center.x/2+shaderRadius/2,center.y/2+shaderRadius/2));
            bubbleMesh.addNormal(ofVec3f(0, 0, 1));
            bubbleMesh.addVertex(ofVec3f(center.x-shaderRadius,center.y+shaderRadius));
            bubbleMesh.addTexCoord(ofVec2f(center.x/2-shaderRadius/2,center.y/2+shaderRadius/2));
            
            shader1.setUniform2f("center", center.x, center.y);
            shader1.setUniform2f("size", shaderRadius * 2, shaderRadius * 2);
            
            bubbleMesh.draw();
            shader1.end();
            
            float fadeTime = 15.f; //seconds
            float steps = fadeTime * ofGetFrameRate();
            float fadeValue = 255.f/steps;
            
            static bool transitionToSarabandeHasStarted = false;
            
            if ((ofGetElapsedTimef() - timeOnB) > 5.0 && b_section) { // transition to Section 3 !!!!!!!!!!!!!!!!!!!!!!!!!!!
//                transitionToSarabandeHasStarted = true;//warum hatte ich dies drin?!?
                doNotAllowToReturnToOnBoard = true;
                onBoardFilmsAlpha += fadeValue;
                if(onBoardFilmsAlpha >= 255.f){
                    onBoardFilmsAlpha = 255.f;
                    onBoardShaderAlpha -= fadeValue/255.f;
                    if (!shaderFadeHasStarted) {
                        shaderFadeHasStarted = true;
                        sarabandeMessage.setAddress("/sarabande1");
                        sarabandeMessage.addIntArg(1);
                        oscSender.sendMessage(sarabandeMessage);
                    }
                    if (onBoardShaderAlpha <= 0.f) {
                        on_board = false;
                        sarabande = true;
                        section = 2;
                    }
                }
            }
        } else {
            testTimeB = true;
        }
        
        ofSetColor(255, 0, 0, 100);
        ofRect(0, (yPos - 2), videoWidth, 5);
        ofRect((xPos - 2), 0, 5, videoHeight);
        
        ofSetColor(125, 255, 255, int(xFadeForHistory * 0.7843137254902)); // alpha goes up to 200
        
        ofMesh lineMesh;
        ofSetLineWidth(3);
        
        lineMesh.setMode(OF_PRIMITIVE_LINES);
        for (int i = 0; i < videoWidth - 2; i++) {
            //        ofRect(i, yPos - waveTableX.at(int(i/2)) * 100, 3, 3);
            lineMesh.addVertex(ofVec3f((i), fold(yPos - (waveTableX.at(int(i * (float)WAVETABLE_SIZE/(float)videoWidth)) * (amplitude * xFadeForHistory/255.f)) * 100, 0, videoHeight), 0));//amplitude is scaled for xfade between pit-stop and on-board
            lineMesh.addVertex(ofVec3f((i + 1), fold(yPos - (waveTableX.at(int((i + 1) * WAVETABLE_SIZE/(float)videoWidth)) * (amplitude * xFadeForHistory/255.f)) * 100, 0, videoHeight), 0));
        }
        
        for (int i = 0; i < videoHeight - 1; i++) {
            //        ofLine(xPos + (waveTableY.at(int(i * 0.66)) * amplitude) * 100, i, xPos + (waveTableY.at(int((i + 1) * 0.66)) * amplitude) * 100, i + 1);
            lineMesh.addVertex(ofVec3f(fold(xPos + (waveTableY.at(int((i) * WAVETABLE_SIZE/(float)videoHeight)) * (amplitude * xFadeForHistory/255.f)) * 100, 0, videoWidth), (i), 0));
            lineMesh.addVertex(ofVec3f(fold(xPos + (waveTableY.at(int((i + 1) * WAVETABLE_SIZE/(float)videoHeight)) * (amplitude * xFadeForHistory/255.f)) * 100, 0, videoWidth), (i + 1), 0));
        }
        
        lineMesh.draw();
        
        ofColor vertexColor, targetColor;
        
        if (onTarget && !ownCar.inside(xPos, yPos)) {
            vertexColor = ofColor(50, 150, 0, xFadeForHistory);
            targetColor = ofColor(50, 150, 0, int(xFadeForHistory * 0.3921568627451));// alpha goes up to 100;
        } else {
            vertexColor = ofColor(255, 0, 0, int(xFadeForHistory/2));
            targetColor = ofColor(255, 255, 100, int(xFadeForHistory * 0.27450980392157)); // alpha goes up to 70
        }
        
        ofMesh frame;
        
        ofVec3f tl, tr, t_l, t_r, bl, br, b_l, b_r, _t_l, _t_r, _b_l, _b_r;
        
        float delta = 3;
        float targetX = xPos - dia/2;
        float targetY = yPos - dia/2;
        
        tl = ofVec3f(targetX, targetY, 0); // 0
        tr = ofVec3f(targetX + dia, targetY, 0); // 1
        t_l = ofVec3f(targetX, targetY + delta, 0); // 2
        t_r = ofVec3f(targetX + dia, targetY + delta, 0); // 3
        bl = ofVec3f(targetX, targetY + dia, 0); // 4
        br = ofVec3f(targetX + dia, targetY + dia, 0); // 5
        b_l = ofVec3f(targetX, targetY + dia - delta, 0); // 6
        b_r = ofVec3f(targetX + dia, targetY + dia - delta, 0); // 7
        _t_l = ofVec3f(targetX + delta, targetY + delta, 0); // 8
        _t_r = ofVec3f(targetX + dia - delta, targetY + delta, 0); // 9
        _b_l = ofVec3f(targetX + delta, targetY + dia - delta, 0); // 10
        _b_r = ofVec3f(targetX + dia - delta, targetY + dia - delta, 0); // 11
        
        if(amplitude > 0){
            frame.setMode(OF_PRIMITIVE_TRIANGLES);

            frame.addColor(vertexColor);
            frame.addVertex(tl);
            frame.addColor(vertexColor);
            frame.addVertex(tr);
            frame.addColor(vertexColor);
            frame.addVertex(t_l);
            frame.addColor(vertexColor);
            frame.addVertex(t_r);
            frame.addColor(vertexColor);
            frame.addVertex(bl);
            frame.addColor(vertexColor);
            frame.addVertex(br);
            frame.addColor(vertexColor);
            frame.addVertex(b_l);
            frame.addColor(vertexColor);
            frame.addVertex(b_r);
            frame.addColor(vertexColor);
            frame.addVertex(_t_l);
            frame.addColor(vertexColor);
            frame.addVertex(_t_r);
            frame.addColor(vertexColor);
            frame.addVertex(_b_l);
            frame.addColor(vertexColor);
            frame.addVertex(_b_r);
            
            frame.addColor(targetColor);
            frame.addVertex(_t_l);
            frame.addColor(targetColor);
            frame.addVertex(_t_r);
            frame.addColor(targetColor);
            frame.addVertex(_b_l);
            frame.addColor(targetColor);
            frame.addVertex(_b_r);
            
            frame.addTriangle(0, 1, 2);
            frame.addTriangle(2, 3, 1);
            frame.addTriangle(4, 5, 6);
            frame.addTriangle(6, 7, 5);
            frame.addTriangle(2, 6, 8);
            frame.addTriangle(6, 10, 8);
            frame.addTriangle(9, 3, 11);
            frame.addTriangle(3, 7, 11);
            
            frame.addTriangle(14, 13, 12);
            frame.addTriangle(14, 15, 13);
            
            frame.drawFaces();
        }
    }
    
    ////////////////////////// ON-BOARD END /////////////////////////
    //////////////////////// SARABANDE BEGIN ////////////////////////
    
    if (sarabande) {
        static bool sarabande2HasStarted = false, sarabande3HasStarted = false;
        
        ofSetColor(255, 255, 255, 255);
        if (xPos <= (1.0/3.0 * (float)videoWidth)) {
            loCrashPlayer.draw(0, 0);
        } else if (xPos <= (2.0/3.0 * (float)videoWidth))
        {
            midCrashPlayer.draw(0, 0);
            if (!sarabande2HasStarted) {
                sarabande2HasStarted = true;
//                sarabandeMessage.setAddress("/sarabande2");
//                sarabandeMessage.addIntArg(1);
//                oscSender.sendMessage(sarabandeMessage);
            }
        } else if (xPos <= (float)videoWidth){
            hiCrashPlayer.draw(0, 0);
            if (!sarabande3HasStarted) {
                sarabande3HasStarted = true;
//                sarabandeMessage.setAddress("/sarabande3");
//                sarabandeMessage.addIntArg(1);
//                oscSender.sendMessage(sarabandeMessage);
            }
        }
        
        shader1.begin();
        ofTexture &videoTexture = xPos <= (1.0/3.0 * videoWidth) ? loCrashPlayer.getTextureReference() :
        (xPos <= (2.0/3.0 * videoWidth) ? midCrashPlayer.getTextureReference() : hiCrashPlayer.getTextureReference());
        
        shader1.setUniform1i("texture0", 0);
        videoTexture.bind();
        
        ofSeedRandom(200);
        ofDisableDepthTest();
        
        for ( int i = 0; i<NUM; i++ ) {
            ofVec2f center;// = ofVec2f( ofRandom(videoPlayer.width), ofRandom(videoPlayer.height) );
            float rad;
            if(i > 0 && i < (NUM-1))
            {
                center = bubblePositions[i];
                rad = radius[i];
                shader1.setUniform4f("colorDef", 1.f, 1.f, 1.f, 1.f);
            } else if (i == 0){
                center = ofVec2f(videoWidth/2, videoHeight/2);
                rad = videoWidth * 3;
                shader1.setUniform4f("colorDef", 1.f, 1.f, 1.f, 1.f);
            } else if(i == (NUM-1)) {// navigating blop
                center = ofVec2f(xPos, yPos) + curserNoise;
                rad = 120;
                shader1.setUniform4f("colorDef", 1.f, 0.f, 0.f, 1.0f);
            }
            ofMesh rect;
            rect.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x-rad,center.y-rad));
            rect.addTexCoord(ofVec2f(center.x-rad,center.y-rad));
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x+rad,center.y-rad));
            rect.addTexCoord(ofVec2f(center.x+rad,center.y-rad));
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x+rad,center.y+rad));
            rect.addTexCoord(ofVec2f(center.x+rad,center.y+rad));
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x-rad,center.y+rad));
            rect.addTexCoord(ofVec2f(center.x-rad,center.y+rad));
            
            shader1.setUniform2f("center", center.x, center.y);
            shader1.setUniform2f("size", rad*2, rad*2);
            
            rect.draw();
        }
        shader1.end();
        
        ofSetColor(0, 0, 0, 255 - (int)fadeToBlack);
        ofRect(0, 0, loCrashPlayer.width, loCrashPlayer.height);
    }
    
    ///////////////////////// SARABANDE END /////////////////////////
    //////////////////////// FLAGS BEGINNING ////////////////////////
    
    static float fadeValue = 0.f;
    
    if (flags) {
        
        
        screenFlagFBO.begin();
        ofDisableLighting();
        ofClear(0);

        ofColor black = ofColor(0, 0, 0, 255);
        
        if (blackUL) {
            ofSetColor(black);
        } else {
            ofSetColor(255, 128, 128, 255);
        }
        if (redOrRedFlag) {
            flagPlayer1.draw(0, 0, videoWidth/2, videoHeight/2);
        } else {
            ofRect(0, 0, videoWidth/2, videoHeight/2);
        }
        
        if (blackUR) {
            ofSetColor(black);
        } else {
            ofSetColor(0, 255, 128, 255);
        }
        if (greenOrGreenFlag) {
            flagPlayer2.draw(videoWidth/2, 0, videoWidth/2, videoHeight/2);
        } else {
            ofRect(videoWidth/2, 0, videoWidth/2, videoHeight/2);
        }
        
        if (blackLL) {
            ofSetColor(black);
        } else {
            ofSetColor(255, 255, 0, 255);
        }
        
        if (yelloOryelloFlag) {
            if (barbCam) {
                camera.draw(0, videoHeight/2, videoWidth/2, videoHeight/2);
            } else {
                flagPlayer3.draw(0, videoHeight/2, videoWidth/2, videoHeight/2);
            }
        } else {
            ofRect(0, videoHeight/2, videoWidth/2, videoHeight/2);
        }
        
        if (blackLR) {
            ofSetColor(black);
        } else {
            ofSetColor(128, 128, 255, 255);
        }
        
        if (blueOrBlueFlag) {
            flagPlayer4.draw(videoWidth/2, videoHeight/2, videoWidth/2, videoHeight/2);
        } else {
            ofRect(videoWidth/2, videoHeight/2, videoWidth/2, videoHeight/2);
        }
        
        ofSetColor(125, 255, 255, 200);
        ofSetLineWidth(3);
        float amplitude = 1.0;
        
        if (semaphoreCounting) {
            if (!timer1.isTiming() && !startSequence) {
                timer1.startTiming();
            } else if (timer1.getTime() > 15.f) {
                timer1.resetTiming();
            }
            
            if (timer1.getTime() < 7 && timer1.isTiming()) {
                semaphoreCounter = (int)ofClamp(timer1.getTime(), 0, 5);
            } else {
                semaphoreCounter = 0;
                if (startSequence) {
                    timer1.stop();
                    semaphoreCounting = false;
                    flagSzene = 14;
                    //                ofxOscMessage toSC;
                    //                toSC.setAddress("/flags"); // yellow BarbCam,  red, blue and green film
                    //                toSC.addIntArg(14);
                    //                oscSender.sendMessage(toSC);
                }
            }
        }
        
        ofSetColor(cursorColor);
        ofCircle(xPos, yPos, 25);
        
        if (semaphoreCounter == 1 && flagSzene == NUMSCENES - 3) {// part 5 ist started!
            startSequence = true;
            startSequencePlayer.play();
        }
        
        static float fadeSteps = 15 * ofGetFrameRate();
        static int countFade = 0;
        if (startSequence) {
            ofSetColor(255,
                       (int)mapping(countFade/fadeSteps, 0, 1, 255, 100),
                       (int)mapping(countFade/fadeSteps, 0, 1, 0, 100));
            if (countFade <= fadeSteps) {
                countFade++;
            }
            startSequencePlayer.draw(0, 0);
            flagPlayer1.stop();
            flagPlayer2.stop();
            flagPlayer3.stop();
            flagPlayer4.stop();
        }
        
        if (semaphoreCounting) {
            ofSetColor(255, 0, 0, 255);
            ofRect(0, 360, 1280, 5); // red Cross
            ofRect(640, 0, 5, 720);// is only drawn during "Flags" until first start
        }

        screenFlagFBO.end();
        
        screenFlagFBO2.begin();
        ofDisableLighting();
        
        ofSetColor(255, 255,  255, 255);
        screenFlagFBO.draw(0, 0);
        
        if (flagShader) {
            shader1.begin();
            shader1.setUniform1i("texture0", 0);
            screenFlagFBO.getTextureReference().bind();
            
            
            ofDisableDepthTest();
            
            for ( int i = 0; i < semaphoreCounter + 1; i++ ) {
                ofVec2f center;// = ofVec2f( ofRandom(videoPlayer.width), ofRandom(videoPlayer.height) );
                float rad;
                
                if (i == 0) {
                    center = ofVec2f(xPos, yPos);// cursor
                    rad = 30;
                    shader1.setUniform4f("colorDef", 1.0, 1.0, 1.f, 1.f);
                } else {
                    center = ofVec2f((videoWidth * (i)/ 6), videoHeight/2);
                    rad = 120;
                    shader1.setUniform4f("colorDef", 1.f, 0.f, 0.f, 1.f);
                }
                ofMesh rect;
                rect.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
                rect.addNormal(ofVec3f(0, 0, 1));
                rect.addVertex(ofVec3f(center.x-rad, center.y-rad));
                rect.addTexCoord(ofVec3f(center.x-rad, center.y-rad));//
                rect.addNormal(ofVec3f(0, 0, 1));
                rect.addVertex(ofVec3f(center.x+rad, center.y-rad));
                rect.addTexCoord(ofVec2f(center.x+rad, center.y-rad));//
                rect.addNormal(ofVec3f(0, 0, 1));
                rect.addVertex(ofVec3f(center.x+rad, center.y+rad));
                rect.addTexCoord(ofVec2f(center.x+rad, center.y+rad));//
                rect.addNormal(ofVec3f(0, 0, 1));
                rect.addVertex(ofVec3f(center.x-rad, center.y+rad));
                rect.addTexCoord(ofVec2f(center.x-rad, center.y+rad));//
                
                shader1.setUniform2f("center", center.x, center.y);
                shader1.setUniform2f("size", rad*2, rad*2);
                
                rect.draw();
            }
            shader1.end();

        } else {
            shader2.begin();
            shader2.setUniform1i("texture0", 0);
            ofTexture &videoTexture = camera.getTextureReference();
            videoTexture.bind();
            
            ofDisableDepthTest();
            
            ofVec2f center;// = ofVec2f( ofRandom(videoPlayer.width), ofRandom(videoPlayer.height) );
            float rad;
        
            center = ofVec2f(xPos, yPos);// cursor
            rad = dia * 4;
            shader2.setUniform4f("colorDef", 1.0, 1.0, 0.f, fadeValue/2);
            shader2.setUniform1f("time", ofGetElapsedTimef() * 15);

            ofMesh rect;
            rect.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x-rad, center.y-rad));
            rect.addTexCoord(ofVec3f(center.x-rad, center.y-rad));//
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x+rad, center.y-rad));
            rect.addTexCoord(ofVec2f(center.x+rad, center.y-rad));//
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x+rad, center.y+rad));
            rect.addTexCoord(ofVec2f(center.x+rad, center.y+rad));//
            rect.addNormal(ofVec3f(0, 0, 1));
            rect.addVertex(ofVec3f(center.x-rad, center.y+rad));
            rect.addTexCoord(ofVec2f(center.x-rad, center.y+rad));//
            
            shader2.setUniform2f("center", center.x, center.y);
            shader2.setUniform2f("size", rad*2, rad*2);
            
            rect.draw();
            
            shader2.end();
        }
                ////
        screenFlagFBO2.end();
        ofSetColor(255, 255, 255);
        screenFlagFBO2.draw(0, 0);
        
        ofPixels pixels2;
    
        screenFlagFBO2.readToPixels(pixels2);
        
        ofxOscMessage sendOSC1, sendOSC2;
        
        sendOSC1.setAddress("/waveTableX");
        
        float widthRatio = (float)videoWidth/(float)WAVETABLE_SIZE; // needed to stretch from WAVETABLE_SIZE to width of video
        float heightRatio = (float)videoHeight/(float)WAVETABLE_SIZE; // needed to stretch from WAVETABLE_SIZE to height of video
        
        for (int i = 0; i < WAVETABLE_SIZE; i++) {
            unsigned int pixelBasedInd = int(yPos * pixels2.getWidth() * pixels2.getBytesPerPixel() + (round(i * widthRatio) * pixels2.getBytesPerPixel()));
            int r = pixels2[pixelBasedInd];
            int g = pixels2[pixelBasedInd + 1];
            int b = pixels2[pixelBasedInd + 2];
            
            float v = (float)(r + g + b)/3.f; // lightness = average of r, g and b
            waveTableX.at(i) = (float)(v/128) - 1.f;
            
            sendOSC1.addFloatArg(waveTableX.at(i));
            
        }
        
        for (int i = 0; i < WAVETABLE_SIZE; i++) {
            //            float v = pixels.getColor(xPos, int(i * 1.5)).getBrightness();
            unsigned int pixelBasedInd = int(i * heightRatio) * pixels2.getWidth() * pixels2.getBytesPerPixel() + int(xPos * pixels2.getBytesPerPixel());
            int r = pixels2[pixelBasedInd];
            int g = pixels2[pixelBasedInd + 1];
            int b = pixels2[pixelBasedInd + 2];
            
            float v = (float)(r + g + b)/3.f; // lightness = average of r, g and b
            
            waveTableY.at(i) = (float)(v/128) - 1.f;
            
            sendOSC1.addFloatArg(waveTableY.at(i));
        }
        oscSender.sendMessage(sendOSC1);
        
        static float fadeTime = 10.f;
        static float fadeStep = 1.f/(fadeTime * ofGetFrameRate());
        if (startSequenceMotionBlur) {
            if (fadeValue <= 1.f) {
                fadeValue += fadeStep;
            }
        }
        
        
        ofSetColor(125, 255, 255, int(200 * (1.f - fadeValue))); // alpha goes up to 200
        
        ofMesh lineMesh;
        
        lineMesh.setMode(OF_PRIMITIVE_LINES);
        for (int i = 0; i < videoWidth - 2; i++) {
            lineMesh.addVertex(ofVec3f((i), fold(yPos - (waveTableX.at(int(i * (float)WAVETABLE_SIZE/(float)videoWidth)) * (amplitude * 255.f)) * 0.5f, 0, videoHeight), 0));
            lineMesh.addVertex(ofVec3f((i + 1), fold(yPos - (waveTableX.at(int((i + 1) * WAVETABLE_SIZE/(float)videoWidth)) * (amplitude * 255.f)) * 0.5f, 0, videoHeight), 0));
        }
        
        for (int i = 0; i < videoHeight - 1; i++) {
            
            lineMesh.addVertex(ofVec3f(fold(xPos + (waveTableY.at(int((i) * WAVETABLE_SIZE/(float)videoHeight)) * (amplitude * 255.f)) * 0.5f, 0, videoWidth), (i), 0));
            lineMesh.addVertex(ofVec3f(fold(xPos + (waveTableY.at(int((i + 1) * WAVETABLE_SIZE/(float)videoHeight)) * (amplitude * 255.f)) * 0.5f, 0, videoWidth), (i + 1), 0));
        }
        
        lineMesh.draw();
    }
    
    if (startSequenceMotionBlur) {
        ofSetColor(255, 255, 255, 255);
        for (int i = 0; i < 40; i++) {
            history[i].draw(0, 0, videoWidth, videoHeight);
        }
        
        flagShader = false;
        
        ofMesh frame;
        ofVec3f tl, tr, t_l, t_r, bl, br, b_l, b_r, _t_l, _t_r, _b_l, _b_r;
        
        ofColor vertexColor, targetColor;
        vertexColor = ofColor(50, 150, 0, 255 * fadeValue);
        targetColor = ofColor(50, 150, 0, 100 * fadeValue);
        
        if (dia > 200) {
            vertexColor = ofColor(50, 150, 0, 255 * fadeValue);
            targetColor = ofColor(50, 150, 0, 100 * fadeValue);// alpha goes up to 100;
        } else {
            vertexColor = ofColor(255, 0, 0, 255 * fadeValue);
            targetColor = ofColor(255, 255, 100, 100 * fadeValue); // alpha goes up to 70
        }
        
        float delta = 3;
        float targetX = xPos - dia/2;
        float targetY = yPos - dia/2;
        
        tl = ofVec3f(targetX, targetY, 0); // 0
        tr = ofVec3f(targetX + dia, targetY, 0); // 1
        t_l = ofVec3f(targetX, targetY + delta, 0); // 2
        t_r = ofVec3f(targetX + dia, targetY + delta, 0); // 3
        bl = ofVec3f(targetX, targetY + dia, 0); // 4
        br = ofVec3f(targetX + dia, targetY + dia, 0); // 5
        b_l = ofVec3f(targetX, targetY + dia - delta, 0); // 6
        b_r = ofVec3f(targetX + dia, targetY + dia - delta, 0); // 7
        _t_l = ofVec3f(targetX + delta, targetY + delta, 0); // 8
        _t_r = ofVec3f(targetX + dia - delta, targetY + delta, 0); // 9
        _b_l = ofVec3f(targetX + delta, targetY + dia - delta, 0); // 10
        _b_r = ofVec3f(targetX + dia - delta, targetY + dia - delta, 0); // 11
        
//        if(amplitude > 0){
            frame.setMode(OF_PRIMITIVE_TRIANGLES);
            
            frame.addColor(vertexColor);
            frame.addVertex(tl);
            frame.addColor(vertexColor);
            frame.addVertex(tr);
            frame.addColor(vertexColor);
            frame.addVertex(t_l);
            frame.addColor(vertexColor);
            frame.addVertex(t_r);
            frame.addColor(vertexColor);
            frame.addVertex(bl);
            frame.addColor(vertexColor);
            frame.addVertex(br);
            frame.addColor(vertexColor);
            frame.addVertex(b_l);
            frame.addColor(vertexColor);
            frame.addVertex(b_r);
            frame.addColor(vertexColor);
            frame.addVertex(_t_l);
            frame.addColor(vertexColor);
            frame.addVertex(_t_r);
            frame.addColor(vertexColor);
            frame.addVertex(_b_l);
            frame.addColor(vertexColor);
            frame.addVertex(_b_r);
            
            frame.addColor(targetColor);
            frame.addVertex(_t_l);
            frame.addColor(targetColor);
            frame.addVertex(_t_r);
            frame.addColor(targetColor);
            frame.addVertex(_b_l);
            frame.addColor(targetColor);
            frame.addVertex(_b_r);
            
            frame.addTriangle(0, 1, 2);
            frame.addTriangle(2, 3, 1);
            frame.addTriangle(4, 5, 6);
            frame.addTriangle(6, 7, 5);
            frame.addTriangle(2, 6, 8);
            frame.addTriangle(6, 10, 8);
            frame.addTriangle(9, 3, 11);
            frame.addTriangle(3, 7, 11);
            
            frame.addTriangle(14, 13, 12);
            frame.addTriangle(14, 15, 13);
            
            frame.drawFaces();

    }
    
    static float endFade = 0;
    static float fadeSteps = 5 * ofGetFrameRate();
    static float fadeAmount = 255.f/fadeSteps;
    if (fadeToEndBlack) {
        if (endFade >= 255) {
            endFade = 255;
        } else {
        endFade += fadeAmount;
        }
        ofSetColor(0, 0, 0, (int)endFade);
        ofRect(0, 0, ofGetWidth(), ofGetHeight());
    }
    
    ofSetColor(0, 0, 0, 255);
    //    ofRect(-xOffset, -yOffset, , ofGetHeight() + yOffset);
    ofRect(0, 0, ofGetWidth()+ xOffset, -1000);
    ofRect(0, videoHeight, ofGetWidth()+ xOffset, 1000);
    
    switch (section) {
        case 0:
            titles[section].fadeTitle("Pimp Up", titleFont);
            break;
        case 1:
            titles[section].fadeTitle("On the Hunt", titleFont);
            break;
        case 2:
            titles[section].fadeTitle("Court Dancing", titleFont);
            break;
        case 3:
            titles[section].fadeTitle("'Sig-nals'", titleFont);
            break;
        case 4:
            titles[section].fadeTitle("A Night Out", titleFont);
            break;
        default:
            break;
    }

    ofSetColor(0, 0, 0);
    ofPopMatrix();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    ofxOscMessage toSC1, toSC2;
    
//    if (key == '1') {
//        tracking1 = true;
//        tracking2 = false;
//    }
//    else if (key == '2') {
//        tracking1 = false;
//        tracking2 = true;
//    }
    if (key == '1') {// start the piece with "pit-stop
        pit_stop = true;
        on_board = false;
        toSC1.setAddress("/pit_stop");
        toSC1.addIntArg(1);
        oscSender.sendMessage(toSC1);
        section = 0;
    }
    else if (key == '2') {// during pit_stop: exchange film in top right rectangle with on-board film!
        if (pit_stop) {
            sect2Start = true;
            forwardViewPlayer.setSpeed(0.25);
            forwardViewPlayer.setFrame(0);
        }
    }
    else if (key == 'B') {// on-board camera
        barbCam = !barbCam;
    }
    else if (key == '3') {// x-fade from pit-stop to on-board (shift-1)
        screenBlend = !screenBlend;
        sect2Start = true;
        screenBlendIndex = 0;
        forwardViewPlayer.setSpeed(1);
        on_board = true;
        toSC1.setAddress("/pit_stop");
        toSC1.addIntArg(0);
        oscSender.sendMessage(toSC1);
        toSC2.setAddress("/on_board");
        toSC2.addIntArg(1);
        oscSender.sendMessage(toSC2);
    }
    else if (key == '4') {
        sarabande = true;
        pit_stop = false;
        on_board = false;
    }
    else if (key == '5') {
        toSC1.setAddress("/sarabande2");
        toSC1.addIntArg(1);
        oscSender.sendMessage(toSC1);
    }
    else if (key == '6') {
        toSC1.setAddress("/sarabande3");
        toSC1.addIntArg(1);
        oscSender.sendMessage(toSC1);
    }
    else if (key == '7') {
        flags = true;
        sarabande = false;
        pit_stop = false;
        on_board = false;
        section = 3;
    }
    else if (key == '8') {
        fadeToEndBlack = true;
    }
    else if (key == 'O') {//start on-board directly (for rehearsals only)
        pit_stop = false;
        on_board = true;
    }
    else if (key == 'b') {// just for testing: backward view
        backwardViewPlayer.setFrame(backwardFrame);
        backwardViewPlayer.play();
        forwardFrame = forwardViewPlayer.getCurrentFrame();
        forwardViewPlayer.stop();
    }
    else if (key == 'f') {// just for testing: forward view
        forwardViewPlayer.setFrame(forwardFrame);
        forwardViewPlayer.play();
        backwardFrame = backwardViewPlayer.getCurrentFrame();
        backwardViewPlayer.stop();
    }
    else if (key == 'a'){
        static bool toggle = 1;
        shaderGrowth = 0;
        toggle = !toggle;
        amplitude = toggle;
    }
    else if (key == ' '){// Full Screen
        ofToggleFullscreen();
        imageRatio = ofGetWidth()/(float)videoWidth;// fullscreen scaled according to width (default)
        cout << "videoWidth: " << videoWidth << "; screenWidth: " << ofGetWidth() << "; screenHeight: " << ofGetHeight() << "; ImageRatio: " << imageRatio << endl;
        xOffset = 0;
        yOffset = (ofGetHeight() - (videoHeight * imageRatio))/3; // warum 3??? und nicht 2????
        grabTexture.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
        grabTexture2.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    }
    else if (key == 'h'){// Full Screen proportions according to height (default)
        imageRatio = ofGetHeight()/(float)videoHeight; // fullscreen scaled according to height
        cout << "videoHeight: " << videoHeight << "; screenHeight: " << ofGetHeight() << "; ImageRatio: " << imageRatio << endl;
        yOffset = 0;
        xOffset = (ofGetWidth() - (videoWidth * imageRatio))/3;
    }
    else if (key == 'w'){// Full Screen proportions according to width
        imageRatio = ofGetWidth()/(float)videoWidth; // fullscreen scaled according to width
        cout << "videoWidth: " << videoWidth << "; screenWidth: " << ofGetWidth() << "; ImageRatio: " << imageRatio << endl;
        xOffset = 0;
        yOffset = (ofGetHeight() - (videoHeight * imageRatio))/3;
    }
    else if (key == 357){// in case camera input doesn't match
        cameraID = 0;
        camera.setDeviceID(cameraID);
        camera.initGrabber(ofGetWidth()/2, ofGetHeight()/2);
        cout << "cameraID: " << cameraID << endl;
    }
    else if (key == 359){
        cameraID = 1;
        camera.setDeviceID(cameraID);
        camera.initGrabber(ofGetWidth()/2, ofGetHeight()/2);
        cout << "cameraID: " << cameraID << endl;
    }
    else if (key == 358){
        cameraID = 2;
        camera.setDeviceID(cameraID);
        camera.initGrabber(ofGetWidth()/2, ofGetHeight()/2);
        cout << "cameraID: " << cameraID << endl;
    }
    if (key == 'z') {
        flagSzene--;
        cout << flagSzene << endl;
    }
    if (key == 'x') {
        flagSzene++;
        cout << flagSzene << endl;
    }
    if (key == 'c') {
        toSC1.setAddress("/flags"); // yellow BarbCam,  red, blue and green film
        toSC1.addIntArg(14);
        oscSender.sendMessage(toSC1);
    }
    if (key == 'v') {
        toSC1.setAddress("/flags"); // yellow BarbCam,  red, blue and green film
        toSC1.addIntArg(15);
        oscSender.sendMessage(toSC1);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
//    cout << x << ", " << y << endl;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
