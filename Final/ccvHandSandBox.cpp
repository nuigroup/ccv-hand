/***************************************************************************
*
*  ccvHandSandbox.cpp
*  NUI Group Community Core Vision Hand Tracking
*  Google Summer of Code 2009
*  Student: Thiago de Freitas Oliveira Araújo
*  Mentor: Laurence Muller
*
*  Copyright 2009 NUI Group/Inc. All rights reserved.
*
*   License GPL v2.0
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer as
* the first lines of this file unmodified.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY NUI GROUP ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL BEN WOODHOUSE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

* Web: http://nuigroup.com
*      http://ccv.nuigroup.com/
*      http://thiagodefreitas.wordpress.com
*      http://www.multigesture.net
*
* Still under development...
***************************************************************************/

#include "ccvHandSandBox.h"
#include <cv.h>
#include <highgui.h>
#include <time.h>

#define DEBUG 0

void ccvHandSandBox::setup(){

    printf("CCV HAND TRACKING SANDBOX STARTED\n");
    printf("CCV HAND TRACKING SANDBOX STARTED\n");
    ofSetWindowTitle("CCV HAND TRACKING SANDBOX");
    verdana.loadFont("verdana.ttf", 8, true, true);
    bigTitle.loadFont("verdana.ttf", 20, true, true);
    info.loadFont("verdana.ttf", 12, true, true);
    ofBackground(0,200,255);
    background.loadImage("images/background.jpg");
    //Load Settings from config.xml file
	loadXMLSettings();

    vidPlayer.loadMovie(videoFileName);
    vidPlayer.play();
    vidPlayer.setLoopState(OF_LOOP_NORMAL);
    printf("Running Video, OK!\n");

	processedImg.allocate(camWidth,camHeight);
	sourceImg.setUseTexture(false);
	grayBg.allocate(camWidth,camHeight);
	grayDiff.allocate(camWidth,camHeight);
    sourceImg.allocate(camWidth,camHeight);
    sourceImg.setUseTexture(false);
    floatBgImg.allocate(camWidth, camHeight);

    amplify = true;
    blur = true;
    erode = false;
    dilate = false;
    eqHist = false;
    bLearnBackground = false;
    bDynamicBG = false;
    highpass = true;
    bDrawOutlines = true;
    bShowLabels = true;
    aamTracking = false;
    vJones  =  false;
    //Setup Calibration
	calib.setup(camWidth, camHeight, &tracker);

    MIN_BLOB_SIZE = 2;
    MAX_BLOB_SIZE = 50;

	threshold = 11;

	histDims = 16;

    float hranges_arr[] = {0,180};

	hranges = hranges_arr;

	hist = cvCreateHist( 1, &histDims, CV_HIST_ARRAY, &hranges, 1 );

	histImg.allocate(camWidth,camHeight);
    cvZero(histImg.getCvImage() );

    h_plane = cvCreateImage(cvGetSize(histImg.getCvImage()), 8 , 1);
    planes = &h_plane;

    //Checking if lots of time is being consumed during the model load

    time(&before);

    model.ReadModel(aamModelFileName);

    time(&after);

    elapsed = difftime(before, after);

    #if DEBUG
    printf ("Time elapsed to load model %lf . \n", elapsed);
    #endif
}
//--------------------------------------------------------------
void ccvHandSandBox::update()
{

      newFrame = false;

      vidPlayer.idleMovie();

	if (newFrame = vidPlayer.isFrameNew())
	{

        sourceImg.setFromPixels(vidPlayer.getPixels(), camWidth,camHeight);
        processedImg = sourceImg;

        h_plane = processedImg.getCvImage();
        planes = &h_plane;

        cvSub(processedImg.getCvImage(), grayBg.getCvImage(), processedImg.getCvImage());

        #if DEBUG
        printf("Got problems here(?)\n");
        #endif
        cvCalcHist( planes, hist, 0, 0 ); // Compute histogram

        #if DEBUG
        printf("Ok!\n");
        #endif
        cvCalcBackProject( planes, histImg.getCvImage(), hist );

        processedImg.threshold(threshold);

		processedImg.flagImageChanged();

         if(blur)processedImg.blur( 3 );
         if(dilate)processedImg.dilate( );
         if(erode)processedImg.erode( );
         if(eqHist)processedImg.equalizeHist();
         if(highpass)processedImg.highpass(12,3);
         if(amplify)processedImg.amplify(processedImg, 300);


         contourFinder.findContours(processedImg,  (MIN_BLOB_SIZE * 2) + 1, ((camWidth * camHeight) * .4) * (MAX_BLOB_SIZE * .001), maxBlobs, false);

         tracker.track(&contourFinder);

        //Working on ROI-Camshift
         HandROIAdjust(15,200, processedImg.getCvImage());
         cvCamShift(processedImg.getCvImage(), handSelectionRect, cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ), &handComp, &handBox);
         handSelectionRect = handComp.rect;

         cvEllipseBox( processedImg.getCvImage(), handBox, CV_RGB(255,0,0), 3, CV_AA, 0 );


   }


}

//--------------------------------------------------------------
void ccvHandSandBox::draw(){


	processedImg.draw(20,20, 320, 240);
    vidPlayer.draw(360,280);
    grayBg.draw(20,280,320,240);
    histImg.draw(350,20,320,240);


    drawFingerOutlines();

    drawROIRect();

	ofSetColor(0xffffff);

	char sandBoxStr[1024];
	sprintf(sandBoxStr, "+/- THRESHOLD: %5i",threshold);
	verdana.drawString(sandBoxStr, 20, 560);

	sprintf(sandBoxStr, " a  Amplify:    %s",amplify? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 720);

	sprintf(sandBoxStr, " d  DILATE:    %s",dilate? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 580);

	sprintf(sandBoxStr, " e  ERODE:     %s",erode? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 600);

	sprintf(sandBoxStr, " b  BLUR:      %s",blur? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 620);

	sprintf(sandBoxStr, " q  EQHIST:    %s",eqHist? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 640);

	sprintf(sandBoxStr, " h  HIGHPASS:    %s",highpass? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 660);

	sprintf(sandBoxStr, " y  Dynamic Background:    %s",bDynamicBG? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 680);

    ofSetColor(0x000000);
	sprintf(sandBoxStr, " Press k for background subtract:    %s",bLearnBackground? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 700);

    ofSetColor(0xFFFFFF);
	sprintf(sandBoxStr, " m Activate AAM:    %s",aamTracking? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 740);

	sprintf(sandBoxStr, " v Activate Viola and Jones:    %s",vJones? "true" : "false");
	verdana.drawString(sandBoxStr, 20, 760);

	verdana.drawString("Tracking View", 20, 10);

	verdana.drawString("Subtracted Background", 20, 270);

	verdana.drawString("Histogram", 350, 10);


    ofSetColor(0x000000);
    info.drawString("GOOGLE SUMMER OF CODE 2009", 685, 100);
    ofSetColor(0xFFFFFF);
    bigTitle.drawString("CCV HAND TRACKING", 685, 150);
    ofSetColor(0x000000);
    verdana.drawString("Student: Thiago de Freitas Oliveira Araujo", 725, 170);
    verdana.drawString("Mentor: Laurence Muller", 725, 190);

	ofSetColor(0x000000);
	verdana.drawString("Follow the development at:", 485, 270);
	ofSetColor(0xFFFFFF);
	verdana.drawString("nuicode.com |  ~  | thiagodefreitas.wordpress.com", 655, 270);
}

//--------------------------------------------------------------
void ccvHandSandBox::keyPressed  (int key){

	switch (key){
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 5) threshold = 5;
			break;
        case 'a':
			amplify=!amplify;
        break;
        case 'b':
			blur=!blur;
        break;
		case 'd':
			dilate = !dilate;
			break;
        case 'e':
			erode=!erode;
        break;
        case 'h':
			highpass=!highpass;
        break;
        case 'k':
            bLearnBackground = true;
            learnBackGround(sourceImg);
        break;
        case 'q':
			eqHist=!eqHist;
        break;
        case 'y':
            bDynamicBG = !bDynamicBG;
        break;
        case 'm':
            aamTracking = !aamTracking;
        break;
        case 'v':
            vJones = !vJones;
        break;
	}

}


//--------------------------------------------------------------
void ccvHandSandBox::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ccvHandSandBox::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ccvHandSandBox::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ccvHandSandBox::mouseReleased(){

}

//--------------------------------------------------------------
void ccvHandSandBox::loadXMLSettings()
{
    // TODO: a separate XML to map keyboard commands to action
	message = "Loading config.xml...";
	// Can this load via http?
	if ( XML.loadFile("config.xml"))
		message = "Settings Loaded!\n\n";
	else
		message = "No Settings Found...\n\n"; //FAIL

    videoFileName = XML.getValue("CONFIG:VIDEO:FILENAME", "test_videos/t1.avi");
    camWidth					= XML.getValue("CONFIG:CAMERA_0:WIDTH", 640);
	camHeight					= XML.getValue("CONFIG:CAMERA_0:HEIGHT", 480);
	tmpLocalHost				= XML.getValue("CONFIG:NETWORK:LOCALHOST", "localhost");
	tmpPort						= XML.getValue("CONFIG:NETWORK:TUIOPORT_OUT", 3333);
	tmpFlashPort				= XML.getValue("CONFIG:NETWORK:TUIOFLASHPORT_OUT", 3000);
	maxBlobs					= XML.getValue("CONFIG:BLOBS:MAXNUMBER", 20);
	aamModelFileName			= XML.getValue("CONFIG:AAM:MODEL", "hand.amf");

	myTUIO.setup(tmpLocalHost.c_str(), tmpPort, tmpFlashPort);

}

void ccvHandSandBox::learnBackGround(ofxCvColorImage& img)
{
        //Capture full background
        if (bLearnBackground == true)
        {
            floatBgImg = img;
			cvConvertScale( floatBgImg.getCvImage(), grayBg.getCvImage(), 255.0f/65535.0f, 0 );
			grayBg.flagImageChanged();
            bLearnBackground = false;
        }
}

/*****************************************************************************
* Getters
*****************************************************************************/

std::map<int, Blob> ccvHandSandBox::getBlobs()
{
	return tracker.getTrackedBlobs();
}

/*****************************************************************************
* Drawing functions
*****************************************************************************/

void ccvHandSandBox::drawFingerOutlines()
{
	//Find the blobs for drawing
	for (int i=0; i<contourFinder.nBlobs; i++)

	{
//	    char debugNumber[1024];
//	    sprintf(debugNumber, "quant: %i", contourFinder.nBlobs);
//        verdana.drawString(debugNumber, 365, contourFinder.blobs[i].boundingRect.height/2 + 185);

		if (bDrawOutlines)
		{
			//Draw contours (outlines) on the source image
			contourFinder.blobs[i].drawContours(20, 20, camWidth, camHeight, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
		}
		if (bShowLabels) //Show ID label;
		{
			float xpos = contourFinder.blobs[i].centroid.x * (MAIN_WINDOW_WIDTH/camWidth);
			float ypos = contourFinder.blobs[i].centroid.y * (MAIN_WINDOW_HEIGHT/camHeight);

			if (i > 0)
			{
			//float distance = sqrt(pow(contourFinder.blobs[i].centroid.x - contourFinder.blobs[i-1].centroid.x, 2)+pow(contourFinder.blobs[i].centroid.x - contourFinder.blobs[i-1].centroid.x,2));

		//	char distStr[1024];
		//	sprintf(distStr, "dist: %f", distance);
		//	verdana.drawString(distStr, xpos + 15, ypos + contourFinder.blobs[i].boundingRect.height/2 + 85);

			}
       //     ofCircle(contourFinder.blobs[i].centroid.x* (MAIN_WINDOW_WIDTH/camWidth), contourFinder.blobs[i].centroid.y* (MAIN_WINDOW_HEIGHT/camHeight), 10);
			ofSetColor(0xCCFFCC);
			char idStr[1024];
			sprintf(idStr, "id: %i", contourFinder.blobs[i].id);
			verdana.drawString(idStr, xpos + 15, ypos + contourFinder.blobs[i].boundingRect.height/2 + 45);

//			cvEllipseBox(sourceImg, contourFinder.track_box, CV_RGB(255,0,0), 3, CV_AA, 0);
		}
	}
	ofSetColor(0xFFFFFF);
}

void ccvHandSandBox::HandROIAdjust(int x, int y, IplImage* image)
{
    handSelectionRect.x = MIN(x,handPoint.x);
    handSelectionRect.y = MIN(y,handPoint.y);
    handSelectionRect.width = handSelectionRect.x + CV_IABS(x - handPoint.x);
    handSelectionRect.height = handSelectionRect.y + CV_IABS(y - handPoint.y);
    handSelectionRect.x = MAX( handSelectionRect.x, 0 );
    handSelectionRect.y = MAX( handSelectionRect.y, 0 );
    handSelectionRect.width = MIN( handSelectionRect.width, image->width );
    handSelectionRect.height = MIN( handSelectionRect.height, image->height );
    handSelectionRect.width -= handSelectionRect.x;
    handSelectionRect.height -= handSelectionRect.y;
}

void ccvHandSandBox::drawROIRect()
{
//
}

void ccvHandSandBox::aamSearch()
{

}
