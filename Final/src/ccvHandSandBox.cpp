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

void ccvHandSandBox::setup()
{

    printf("CCV HAND TRACKING SANDBOX STARTED\n");
    ofSetWindowTitle("CCV HAND TRACKING SANDBOX");
    verdana.loadFont("verdana.ttf", 8, true, true);
    bigTitle.loadFont("verdana.ttf", 20, true, true);
    info.loadFont("verdana.ttf", 12, true, true);
    ofBackground(0,200,255);
    background.loadImage("images/background.jpg");
    //Load Settings from config.xml file
    loadXMLSettings();

    facedet.LoadCascade("haarcascade.xml");

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
    blobsCheck.allocate(camWidth, camHeight);

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
    bTUIOMode = true;
    initBg  = true;
    selected = false;

    /**************
    For the Kalman Filter
    **************/

    muX=0;
    muY=0;
    meanKalmanDistance=0;
    meanCondenseDistance=0;
    meanStdDXcondens=0;
    meanStdDYcondens=0;
    stdDXcondens=0;
    stdDYcondens=0;
    CvSize axesCondens;

    predict = NULL;

    theta=0;
    /*---------*/
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

        time(&baam);

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

        if (blur)processedImg.blur( 3 );
        if (dilate)processedImg.dilate( );
        if (erode)processedImg.erode( );
        if (eqHist)processedImg.equalizeHist();
        if (highpass)processedImg.highpass(12,3);
        if (amplify)processedImg.amplify(processedImg, 300);


        contourFinder.findContours(processedImg,  (MIN_BLOB_SIZE * 2) + 1, ((camWidth * camHeight) * .4) * (MAX_BLOB_SIZE * .001), maxBlobs, false);

        tracker.track(&contourFinder);

        if (initBg)
        {
            initBackgroundModel(&bkgdMdl,sourceImg.getCvImage(), &paramMoG);
            initBg  = false;
        }
        else
        {

            blobsCheck = sourceImg;
             //cvSub(blobsCheck.getCvImage(), grayBg.getCvImage(), blobsCheck.getCvImage());


            binaryForeground = updateBackground(bkgdMdl,blobsCheck.getCvImage());
            blobsVector = getBlobs2(blobsCheck.getCvImage(),binaryForeground);
            blobs_total = blobsVector.GetNumBlobs();
/*
            if (blur)blobsCheck.blur( 3 );
            if (dilate)blobsCheck.dilate( );
            if (erode)blobsCheck.erode( );
            if (eqHist)blobsCheck.equalizeHist();
            if (highpass)blobsCheck.highpass(12,3);
            if (amplify)blobsCheck.amplify(blobsCheck, 300);
*/

            if ( blobs_total > 0 )
            {
                if (selected == false)
                {

#if DEBUG
                    printf("How many: %d\n", blobs_total);
#endif
                    drawInitialBlobs(blobsCheck.getCvImage(), blobsVector);

                    coord selectedCoord;

                    selectedCoord = extractBlob( blobsVector, selectedCoord);


                    //Init Kalman
                    printf("Kalman has started\n");
                    kalman = initKalman(indexMat, selectedCoord);
                    printf("Passed from Kalman\n");

                    state=cvCreateMat(kalman->DP,1,CV_32FC1);
                    measurement = cvCreateMat( kalman->MP, 1, CV_32FC1 );
                    process_noise = cvCreateMat(kalman->DP, 1, CV_32FC1);

                    coordReal=selectedCoord;
                    coordPredict=coordReal;
                    muX=camWidth/2;
                    muY=camHeight/2;

                    selected = true;


                } //end from selection if
                else
                {
                    printf("How many: %d\n", blobs_total);
                    coordReal = extractBlob( blobsVector, coordReal);


                    drawBlob(blobsCheck.getCvImage(), coordReal, 255,255,255);
                    printf("Before Kalman> %lf, %lf, %lf, %lf,%d, %d, %d, %d \n", coordPredict.cX, coordPredict.cY, coordReal.cX, coordReal.cY, coordReal.MaxX, coordReal.MaxY, coordReal.MinX, coordReal.MinY);

                    predict = updateKalman(kalman,coordReal);


                    coordPredict.set (coordReal.MaxX, coordReal.MinX, coordReal.MaxY, coordReal.MinY);

                    coordPredict.set ((int)predict[0], (int)predict[1]);
                    printf("After Kalman> %lf, %lf, %lf, %lf,%d, %d, %d, %d \n", coordPredict.cX, coordPredict.cY, coordReal.cX, coordReal.cY, coordReal.MaxX, coordReal.MaxY, coordReal.MinX, coordReal.MinY);

                    muX = sqrt(kalman->error_cov_pre->data.fl[0]);
                    muY = sqrt(kalman->error_cov_pre->data.fl[5]);

                    axes = cvSize( muX , muY );

                    printf("Ellipse 2\n");
                    cvEllipse(blobsCheck.getCvImage(), cvPoint(coordPredict.cX,coordPredict.cY), axes, theta, 0, 360, CV_RGB(255,255,0),1);
                    cvLine( blobsCheck.getCvImage(),  cvPoint(coordPredict.cX,coordPredict.cY), cvPoint(coordPredict.cX,coordPredict.cY), CV_RGB(255,0, 0), 4, 8, 0 );
                    cvLine( blobsCheck.getCvImage(),  cvPoint(coordPredict.cX,coordPredict.cY), cvPoint(coordReal.cX,coordReal.cY), CV_RGB(255,0, 0), 1, 8, 0 );


                    //distance from real
                    resultDistance = sqrt((double)(coordReal.cX - coordPredict.cX)*(coordReal.cX - coordPredict.cX) + (coordReal.cY - coordPredict.cY)*(coordReal.cY - coordPredict.cY));
                    meanKalmanDistance+=resultDistance;
                }

                flag = model.InitShapeFromDetBox(Shape,blobsCheck.getCvImage(),facedet);
                /*
                modelIC.InitParams(blobsCheck.getCvImage());
*/
                if (flag == false)
                {
                   printf("False model fitting\n");
                }
                else
                {
             //  model.Fit(blobsCheck.getCvImage(), Shape, 30, false);
               //printf("Pyramid OK\n");
               /*
               modelIC.Fit(blobsCheck.getCvImage(), Shape, 30, false);
               printf ("Inverse Compositional OK\n");
               */
              //model.Draw(blobsCheck.getCvImage(), Shape, 0);
              /*
              modelIC.Draw(blobsCheck.getCvImage(), Shape, 1);
*/
                }

                    //Working on ROI-Camshift
        HandROIAdjust(coordReal.cX,coordReal.cY, processedImg.getCvImage());
        cvCamShift(processedImg.getCvImage(), handSelectionRect, cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ), &handComp, &handBox);
        handSelectionRect = handComp.rect;

        cvEllipseBox( processedImg.getCvImage(), handBox, CV_RGB(255,255,255), 3, CV_AA, 0 );
            }
        }


        if (bTUIOMode)
        {
            //Start sending OSC
            myTUIO.bTCPMode = true;
            myTUIO.sendTUIO(&getBlobs());
        }


    }
    time(&afAaam);

    #if DEBUG
    printf ("Time elapsed for total calculations %lf . \n", elapsed2);
    #endif


}

//--------------------------------------------------------------
void ccvHandSandBox::draw()
{


    processedImg.draw(20,20, 320, 240);
    blobsCheck.draw(360,280);
    grayBg.draw(20,280,320,240);
    vidPlayer.draw(350,20,320,240);


    drawFingerOutlines();

    drawROIRect();

    ofSetColor(0xffffff);

    char sandBoxStr[1024];

    ofSetColor(0x000000);
    sprintf(sandBoxStr, " t  Hand info enabled at TUIO:    %s",myTUIO.bHandInfo? "true" : "false");
    verdana.drawString(sandBoxStr, 20, 540);

    ofSetColor(0xFFFFFF);
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
void ccvHandSandBox::keyPressed  (int key)
{

    switch (key)
    {
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
    case 't':
        myTUIO.bHandInfo=!myTUIO.bHandInfo;
        if (myTUIO.bHandInfo)
            printf("Mode changed to inform hand information in the TUIO packet\n");
        else
            printf("Mode changed to not inform hand information in the TUIO packet\n");
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
void ccvHandSandBox::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ccvHandSandBox::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ccvHandSandBox::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ccvHandSandBox::mouseReleased()
{

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

    videoFileName = XML.getValue("CONFIG:VIDEO:FILENAME", "test_videos/video_test1.avi");
    camWidth					= XML.getValue("CONFIG:CAMERA_0:WIDTH", 640);
    camHeight					= XML.getValue("CONFIG:CAMERA_0:HEIGHT", 480);
    tmpLocalHost				= XML.getValue("CONFIG:NETWORK:LOCALHOST", "localhost");
    tmpPort						= XML.getValue("CONFIG:NETWORK:TUIOPORT_OUT", 3333);
    tmpFlashPort				= XML.getValue("CONFIG:NETWORK:TUIOFLASHPORT_OUT", 3000);
    maxBlobs					= XML.getValue("CONFIG:BLOBS:MAXNUMBER", 20);
    aamModelFileName			= XML.getValue("CONFIG:AAM:MODEL", "hand.amf");
    myTUIO.bHandInfo			= XML.getValue("CONFIG:BOOLEAN:HANDINFO",0);

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
