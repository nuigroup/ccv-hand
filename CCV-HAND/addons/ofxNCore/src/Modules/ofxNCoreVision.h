/*
 *  ofxNCoreVision.h
 *  NUI Group Community Core Vision
 *
 *  Created by NUI Group Dev Team A on 2/1/09.
 *  Copyright 2009 NUI Group/Inc. All rights reserved.
 *
 */

#ifndef _ofxNCoreVision_H
#define _ofxNCoreVision_H

//Main
#include "ofMain.h"
//Addons
#ifdef TARGET_WIN32
#include "ofxffmv.h"
#include "ofxPS3.h"
#include "ofxDSVL.h"
#endif
#include "ofxOpenCv.h"
#include "ofxDirList.h"
#include "ofxVectorMath.h"
#include "ofxNetwork.h"
#include "ofxOsc.h"
#include "ofxThread.h"
#include "ofxXmlSettings.h"

//Our Addon
#include "ofxNCore.h"

//height and width of the source/tracked draw window
#define MAIN_WINDOW_HEIGHT 240.0f
#define MAIN_WINDOW_WIDTH  320.0f

#define MAX_N_HANDS					5
#define MAX_N_TRACKED_FINGERS		20

#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>

// AAM-Library
#include "AAM_IC.h"
#include "AAM_Basic.h"

#include <time.h>

#include "Coord.h"

#include "BlobResult.h"
#include "cvaux.h"
#include "getBackground.h"
#include "extractBlob.h"
#include "videoHandConstants.h"
#include "videoHandFingerDetector.h"


class ofxNCoreVision : public ofxGuiListener//, public BlobManager
{
    //ofxGUI setup stuff
    enum
    {
        propertiesPanel,
        propertiesPanel_flipV,
        propertiesPanel_flipH,
        propertiesPanel_settings,
        propertiesPanel_pressure,

        gpuPanel,
        gpuPanel_use,

        optionPanel,
        optionPanel_tuio_osc,
        optionPanel_tuio_tcp,
        optionPanel_tuio_height_width,
        optionPanel_tuio_hand_info,

        calibrationPanel,
        calibrationPanel_calibrate,
        calibrationPanel_warp,

        sourcePanel,
        sourcePanel_cam,
        sourcePanel_nextCam,
        sourcePanel_previousCam,
        sourcePanel_video,

        backgroundPanel,
        backgroundPanel_remove,
        backgroundPanel_dynamic,
        backgroundPanel_learn_rate,
        backgroundPanel_colorBackforTemplate,

        smoothPanel,
        smoothPanel_use,
        smoothPanel_smooth,

        amplifyPanel,
        amplifyPanel_use,
        amplifyPanel_amp,

        highpassPanel,
        highpassPanel_use,
        highpassPanel_blur,
        highpassPanel_noise,

        trackedPanel,
        trackedPanel_darkblobs,
        trackedPanel_use,
        trackedPanel_threshold,
        trackedPanel_min_movement,
        trackedPanel_min_blob_size,
        trackedPanel_max_blob_size,
        trackedPanel_outlines,
        trackedPanel_ids,

        savePanel,
        kParameter_SaveXml,
        kParameter_File,

        hPanel,
        hPanel_use_AAM,
        hPanel_use_Vjones,
        hPanel_use_Template,
        hPanel_use_Kalman,
        hPanel_use_Camshift,
		hPanel_use_Malik,

    };

public:

    ofxNCoreVision()
    {
        ofAddListener(ofEvents.mousePressed, this, &ofxNCoreVision::_mousePressed);
        ofAddListener(ofEvents.mouseDragged, this, &ofxNCoreVision::_mouseDragged);
        ofAddListener(ofEvents.mouseReleased, this, &ofxNCoreVision::_mouseReleased);
        ofAddListener(ofEvents.keyPressed, this, &ofxNCoreVision::_keyPressed);
        ofAddListener(ofEvents.keyReleased, this, &ofxNCoreVision::_keyReleased);
        ofAddListener(ofEvents.setup, this, &ofxNCoreVision::_setup);
        ofAddListener(ofEvents.update, this, &ofxNCoreVision::_update);
        ofAddListener(ofEvents.draw, this, &ofxNCoreVision::_draw);
        ofAddListener(ofEvents.exit, this, &ofxNCoreVision::_exit);

        exited=false;

#ifdef TARGET_WIN32
        PS3  = NULL;
        ffmv = NULL;
        dsvl = NULL;
#endif

        vidGrabber = NULL;
        vidPlayer = NULL;
        //initialize filter
        filter = NULL;
        //fps and dsp calculation
        frames		= 0;
        fps			= 0;
        lastFPSlog	= 0;
        differenceTime = 0;
        //bools
        bCalibration= 0;
        bFullscreen = 0;
        bcamera = 0;
        bShowLabels = 1;
        bMiniMode = 0;
        bDrawOutlines = 1;
        bGPUMode = 0;
        bTUIOMode = 0;
        showConfiguration = 0;
        printfToFile = 0;
        //camera
        camRate = 30;
        camWidth = 320;
        camHeight = 240;
        //ints/floats
        backgroundLearnRate = .01;
        MIN_BLOB_SIZE = 2;
        MAX_BLOB_SIZE = 100;
        //if auto tracker is defined then the tracker automagically comes up
        //on startup..
#ifdef STANDALONE
        bStandaloneMode = true;
#else
        bStandaloneMode = false;
#endif

    }

    ~ofxNCoreVision()
    {
        if ( filter != NULL )
        {
            delete filter;
        }
        if ( vidGrabber != NULL )
        {
            delete vidGrabber;
        }
        if ( vidPlayer != NULL )
        {
            delete vidPlayer;
        }
#ifdef TARGET_WIN32
        if ( dsvl != NULL )
        {
            delete dsvl;
        }
        if ( ffmv != NULL )
        {
            delete ffmv;
        }
#endif
    }

    /****************************************************************
     *						Public functions
     ****************************************************************/
    //Basic Events called every frame
    void _setup(ofEventArgs &e);
    void _update(ofEventArgs &e);
    void _draw(ofEventArgs &e);
    void _exit(ofEventArgs &e);
    //Mouse Events
    void _mousePressed(ofMouseEventArgs &e);
    void _mouseDragged(ofMouseEventArgs &e);
    void _mouseReleased(ofMouseEventArgs &e);
    //Key Events
    void _keyPressed(ofKeyEventArgs &e);
    void _keyReleased(ofKeyEventArgs &e);

    //GUI
    void setupControls();
    void		handleGui(int parameterId, int task, void* data, int length);
    ofxGui*		controls;

    //image processing stuff
    void initDevice();
    void getPixels();
    void grabFrameToCPU();
    void grabFrameToGPU(GLuint target);

    //drawing
    void drawFingerOutlines();
    void drawMiniMode();
    void drawFullMode();

    //Load/save settings
    void loadXMLSettings();
    void saveSettings();

    //Getters
    std::map<int, Blob> getBlobs();
        std::map<int, Blob> getHandBlobs();

    /*************************************
      Hand-Tracking Stuff
      *************************************/

    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage 	originalBg;
    ofxCvColorImage         processedImgColor;

    ofxCvShortImage		    floatBgImg;
//    ofxCvColorImage         BgImgColor;
//    ofImage				    background;

    ofImage fileImage;

    float               fLearnRate;

    //  ofTrueTypeFont		verdana;
    ofTrueTypeFont		bigTitle;
    ofTrueTypeFont		info;

    //int 				threshold;
    bool				bLearnBackground2;
    bool                bDynamicBG;
    // bool                bTUIOMode;

    bool                newFrame;

    CPUImageFilter      processedImgHand;
//
//    ofxCvColorImage		sourceImg;
    CPUImageFilter	blobsCheck;
    CPUImageFilter  camShiftImage;
    CPUImageFilter  aamImage;
    ofxCvColorImage		colorBg;


    //void loadXMLSettings();
    void learnBackGround(ofxCvColorImage& img);

    //string videoFileName;
    string  aamModelFileName;

    //XML Settings Vars
    //  ofxXmlSettings		XML;
//    string				message;

//    bool				bDrawOutlines;
//    bool				bShowLabels;

    bool blur,erode, dilate, eqHist, highpass, amplify;

    bool                vJones;
    bool                aamTracking;
    bool                bTemplate;
    bool                bKalman;
    bool                bCamshift;

//    ContourFinder       contourFinder;
    ContourFinder 	handContourFinder;
//    BlobTracker			tracker;

    //int					MIN_BLOB_SIZE;
    //int					MAX_BLOB_SIZE;

    //Camshift / Mean-Shift variables
    CvBox2D             handBox;
    CvRect              handRect;
    CvConnectedComp     handComp;
    CvRect              handSelectionRect;
    CvPoint             handPoint;
    CvPoint            templateRightMin;
    CvPoint            templateRightMax;
    CvPoint            templateLeftMin;
    CvPoint            templateLeftMax;




    void                HandROIAdjust(int x , int y, IplImage* image);

    CvHistogram         *hist;
    int                 histDims;
    float*              hranges;
    ofxCvGrayscaleImage            histImg;
    IplImage*            h_plane;
    IplImage*               tmpl;
    IplImage*               tmpl_left;
    IplImage*     templateImg;
    IplImage*     templateImgLeft;
    CPUImageFilter templateImgDraw;

    void                drawROIRect();
    void                drawTemplateHandPos();
    void                drawMalikFingers();

    IplImage**          planes;

    void                aamSearch();

    AAM_Pyramid         model;
    // AAM_IC              modelIC;
    AAM_Basic           modelBasic;
    AAM_Shape           Shape;

    time_t      before, after;
    time_t      baam, afAaam;
    double      elapsed;
    double      elapsed2;

    coord coordReal;
    coord candidateCoordReal;
    coord coordPredict;
    coord predictConDens;

    //Declare the structure for the background subtraction
    CvGaussBGStatModelParams paramMoG;
    CvBGStatModel *bkgdMdl;
    IplImage* binaryForeground;

    bool selected;
    bool firstTime;
    bool    flag;

    CvKalman* kalman;
    CvConDensation* ConDens;
    CvMat* state;
    CvMat* measurement;
    CvMat* process_noise;
    CvMat* indexMat[6];

    float* predict;

    double theta;
    CvSize axes;
    int muX, muY, condenseFrame;

    float stdDXcondens, stdDYcondens, meanStdDXcondens, meanStdDYcondens, resultDistance, meanKalmanDistance, meanCondenseDistance ;

    CvSize axesCondens;

    int samples;

    bool initBg;

    int blobs_total;

    CBlobResult blobsVector;

    VJfacedetect facedet;

    string cascadeFileName;
    string tmplImageName;
    string tmplLeftName;

    double minF_left, maxF_left;
    double minF, maxF;

    CvRect  win;

    videoHandFingerDetector	fingerFinder;

    handBlob			hands[MAX_N_HANDS];
    trackedFinger		myFinger[MAX_N_TRACKED_FINGERS];

    int nHands;

    /******************************************************
    End of Hand Tracking definitions
    *******************************************************/

    /***************************************************************
     *					Video Capture Devices
     ***************************************************************/
#ifdef TARGET_WIN32
    ofxffmv*            ffmv; //for firefly mv
    ofxPS3*				PS3;  //for ps3
    ofxDSVL*			dsvl;
#endif
    ofVideoGrabber*		vidGrabber;
    ofVideoPlayer*		vidPlayer;

    /****************************************************************
     *            Variables in config.xml Settings file
     *****************************************************************/
    int					deviceID;
    int 				frameseq;
    int 				threshold;
    int					wobbleThreshold;
    int 				camRate;
    int 				camWidth;
    int 				camHeight;
    int					winWidth;
    int					winHeight;
    int					MIN_BLOB_SIZE;
    int					MAX_BLOB_SIZE;
    float				backgroundLearnRate;

    bool				showConfiguration;

    bool				printfToFile;

    bool				bcamera;
    bool  				bMiniMode;
    bool				bShowInterface;
    bool				bShowPressure;
    bool				bDrawOutlines;
    bool				bTUIOMode;
    bool  				bFullscreen;
    bool 				bCalibration;
    bool				bShowLabels;
    bool				bNewFrame;
    //filters
    bool				bAutoBackground;
    //modes
    bool				bGPUMode;

    //auto ~ standalone/non-addon
    bool                bStandaloneMode;

    //exit
    bool				exited;

    /****************************************************
     *End config.xml variables
     *****************************************************/
    //FPS variables
    int 				frames;
    int  				fps;
    float				lastFPSlog;
    int					differenceTime;

    //Fonts
    ofTrueTypeFont		verdana;
    ofTrueTypeFont      sidebarTXT;
    ofTrueTypeFont		bigvideo;

    //Images
    ofImage				background;

    //Blob Tracker
    BlobTracker			tracker;
    BlobTracker         trackerHand;

    /****************************************************************
     *						Private Stuff
     ****************************************************************/
    string videoFileName;

    int	maxBlobs;

    //Calibration
    Calibration calib;
    Calibration calib2;

    //Blob Finder
    ContourFinder	contourFinder;

    //Image filters
    Filters*			filter;
    CPUImageFilter      processedImg;
    ofxCvColorImage		sourceImg;

    //XML Settings Vars
    ofxXmlSettings		XML;
    string				message;

    //Communication
    TUIO				myTUIO;
    string				tmpLocalHost;
    int					tmpPort;
    int					tmpFlashPort;

    //Logging
    char				dateStr [9];
    char				timeStr [9];
    time_t				rawtime;
    struct tm *			timeinfo;
    char				fileName [80];
    FILE *				stream ;

	bool				bMalik;

};

#endif
