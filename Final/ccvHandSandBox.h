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

#include <cv.h>

class ccvHandSandBox : public ofSimpleApp{

	public:

		void setup();
		void update();
		void draw();

		int camWidth;
		int camHeight;

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();

        ofVideoGrabber 		vidGrabber;
        ofVideoPlayer       vidPlayer;

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;
		ofxCvShortImage		    floatBgImg;
		ofImage				    background;

		float               fLearnRate;

		ofTrueTypeFont		verdana;

		int 				threshold;
		bool				bLearnBackground;
		bool                bDynamicBG;

		bool                newFrame;

        CPUImageFilter      processedImg;
        ofxCvColorImage		sourceImg;


		void loadXMLSettings();
		void learnBackGround(CPUImageFilter& img);

		string videoFileName;

        //XML Settings Vars
        ofxXmlSettings		XML;
        string				message;

        unsigned char* pixelsWcam;
        bool blur,erode, dilate, eqHist, highpass;

};

#endif
