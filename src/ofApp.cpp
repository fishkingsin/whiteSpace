#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(25);
//    ofSetBackgroundAuto(false);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableAlphaBlending();
    ofEnableArbTex();
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    kinect.setCameraTiltAngle(0);

    kinect.setUseTexture(true);
    kinect.setLed(ofxKinect::LED_GREEN);
    
    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    fbo.allocate(kinect.width, kinect.height);
    grayImage.setUseTexture(true);
    
    gui.setup();
    gui.add(nearThreshold.set("nearThreshold",230, 1,1000));
    gui.add(farThreshold.set("farThreshold",70, 1,1000));
    gui.loadFromFile("settings.xml");
//    if(ofGetGLProgrammableRenderer()){
//        string vertex = "#version 150\n\
//        \n\
//        uniform mat4 projectionMatrix;\n\
//        uniform mat4 modelViewMatrix;\n\
//        uniform mat4 modelViewProjectionMatrix;\n\
//        \n\
//        \n\
//        in vec4  position;\n\
//        in vec2  texcoord;\n\
//        \n\
//        out vec2 texCoordVarying;\n\
//        \n\
//        void main()\n\
//        {\n\
//        texCoordVarying = texcoord;\
//        gl_Position = modelViewProjectionMatrix * position;\n\
//        }";
//        string fragment = "#version 150\n\
//        \n\
//        uniform sampler2DRect tex0;\
//        uniform sampler2DRect maskTex;\
//        in vec2 texCoordVarying;\n\
//        \
//        out vec4 fragColor;\n\
//        void main (void){\
//        vec2 pos = texCoordVarying;\
//        \
//        vec3 src = texture(tex0, pos).rgb;\
//        float mask = texture(maskTex, pos).r;\
//        \
//        fragColor = vec4( src , mask);\
//        }";
//        shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
//        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
//        shader.bindDefaults();
//        shader.linkProgram();
//    }else{
        string shaderProgram = "#version 120\n \
        #extension GL_ARB_texture_rectangle : enable\n \
        \
        uniform sampler2DRect tex0;\
        uniform sampler2DRect maskTex;\
        \
        void main (void){\
        vec2 pos = gl_TexCoord[0].st;\
        \
        vec3 src = texture2DRect(tex0, pos).rgb;\
        float mask = texture2DRect(maskTex, pos).r;\
        \
        gl_FragColor = vec4( src , mask);\
        }";
        shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
        shader.linkProgram();
//    }
    imageSequence.loadSequence("./");
}
void ofApp::exit(){
    
    kinect.setLed(ofxKinect::LED_OFF);
    kinect.close();
}
//--------------------------------------------------------------
void ofApp::update(){

    kinect.update();
    
    
    if(kinect.isFrameNew()) {
        
        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds

        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        
        // update the cv images
        grayImage.flagImageChanged();
        
    }
    grayImage.draw(0,0,640,480);
    fbo.begin();
    // Cleaning everthing with alpha mask on 0 in order to make it transparent for default
    ofClear(0, 0, 0, 0);
    
    shader.begin();
    shader.setUniformTexture("maskTex", grayImage.getTextureReference(), 1 );
    
    kinect.draw(0,0);
        
    shader.end();
    fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    ofEnableAlphaBlending();
    imageSequence.getTextureForFrame(ofGetFrameNum()%imageSequence.getTotalFrames()).draw(0,0);
//    drawFbo.begin();
//    
//    ofClear(0, 0, 0,0.1);
    fbo.draw(0,0);
//    drawFbo.end();
//    drawFbo.draw(kinect.getWidth(),0,-kinect.getWidth(),kinect.getHeight());
//    ofPixels pixels;
//    fbo.readToPixels(pixels);
//    ofSaveImage(pixels, ofGetTimestampString()+".png");
    gui.draw();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
