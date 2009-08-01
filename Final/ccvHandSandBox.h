/***************************************************************************
*
*  ccvHandSandbox.h
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

#ifndef _CCVHAND_SANDBOX
#define _CCVHAND_SANDBOX

#include "ofMain.h"

#define OF_ADDON_USING_OFXOPENCV

#include "ofAddons.h"
#include "ofxXmlSettings.h"
#include "Filters/CPUImageFilter.h"

//Used for tracking algo
#include "Tracking/BlobManager.h"
#include "Tracking/Tracking.h"

#include "ofMain.h"

//Communications
#include "Communication/TUIO.h"

//Used other calibration
#include "Calibration/CalibrationUtils.h"
#include "Calibration/Calibration.h"


#include <cv.h>

//height and width of the source/tracked draw window
#define MAIN_WINDOW_HEIGHT 240.0f
#define MAIN_WINDOW_WIDTH  320.0f

// AAM-Library
#include "AAM_IC.h"
#include "AAM_Basic.h"

#include <time.h>

class ccvHandSandBox : public ofSimpleApp{

	public:

		void setup();
		void update();
		void draw();

		int camWidth;
		int camHeight;

		int	maxBlobs;

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();

        ofVideoGrabber 		vidGrabber;
        ofVideoPlayer       vidPlayer;

        //Getters
        std::map<int, Blob> getBlobs();

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;
		ofxCvGrayscaleImage 	originalBg;

		ofxCvShortImage		    floatBgImg;
		ofImage				    background;

		ofImage fileImage;

		float               fLearnRate;

		ofTrueTypeFont		verdana;
		ofTrueTypeFont		bigTitle;
		ofTrueTypeFont		info;

		int 				threshold;
		bool				bLearnBackground;
		bool                bDynamicBG;

		bool                newFrame;

        CPUImageFilter      processedImg;
        ofxCvColorImage		sourceImg;
        ofxCvColorImage		colorBg;


		void loadXMLSettings();
		void learnBackGround(ofxCvColorImage& img);

		string videoFileName;
		string  aamModelFileName;

        //XML Settings Vars
        ofxXmlSettings		XML;
        string				message;

        bool				bDrawOutlines;
        bool				bShowLabels;

        bool blur,erode, dilate, eqHist, highpass, amplify;

        bool                vJones;
        bool                aamTracking;

        ContourFinder       contourFinder;
        BlobTracker			tracker;

        int					MIN_BLOB_SIZE;
        int					MAX_BLOB_SIZE;

        //Communication
        TUIO				myTUIO;
        string				tmpLocalHost;
        int					tmpPort;
        int					tmpFlashPort;

        Calibration         calib;

        void                drawFingerOutlines();

        //Camshift / Mean-Shift variables
        CvBox2D             handBox;
        CvRect              handRect;
        CvConnectedComp     handComp;
        CvRect              handSelectionRect;
        CvPoint             handPoint;

        void                HandROIAdjust(int x , int y, IplImage* image);

        CvHistogram         *hist;
        int                 histDims;
        float*              hranges;
        ofxCvGrayscaleImage            histImg;
        IplImage*            h_plane;

        void                drawROIRect();

        IplImage**          planes;

        void                aamSearch();

        AAM_Pyramid         model;

        time_t      before, after;
        double      elapsed;

};

#endif
