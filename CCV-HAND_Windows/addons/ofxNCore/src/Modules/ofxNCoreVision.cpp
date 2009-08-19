/*
*  ofxNCoreVision.cpp
*  NUI Group Community Core Vision
*
*  Created by NUI Group Dev Team A on 2/1/09.
*  Copyright 2009 NUI Group. All rights reserved.

New functionalities under development...

*  NUI Group Community Core Vision Hand Tracking
*  Google Summer of Code 2009
*  Student: Thiago de Freitas Oliveira Araújo
*  Mentor: Laurence Muller
* Web: http://nuigroup.com
*      http://ccv.nuigroup.com/
*      http://thiagodefreitas.wordpress.com
*      http://www.multigesture.net
*
*/

#include "ofxNCoreVision.h"
#include "../Controls/gui.h"

#define DEBUG 0



/******************************************************************************
* The setup function is run once to perform initializations in the application
*****************************************************************************/
void ofxNCoreVision::_setup(ofEventArgs &e)
{
    //set the title
    ofSetWindowTitle(" Community Core Vision - with beta Hand Tracking Module");

    //create filter
    if ( filter == NULL )
    {
        filter = new ProcessFilters();
    }

    //Load Settings from config.xml file
    loadXMLSettings();

    //removes the 'x' button on windows which causes a crash due to a GLUT bug
#ifdef TARGET_WIN32
    //Get rid of 'x' button
    HWND hwndConsole = FindWindowA(NULL, " Community Core Vision ");
    HMENU hMnu = ::GetSystemMenu(hwndConsole, FALSE);
    RemoveMenu(hMnu, SC_CLOSE, MF_BYCOMMAND);
#endif


    if (printfToFile)
    {
        /*****************************************************************************************************
        * LOGGING
        ******************************************************************************************************/
        /* alright first we need to get time and date so our logs can be ordered */
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        strftime (fileName,80,"../logs/log_%B_%d_%y_%H_%M_%S.txt",timeinfo);
        FILE *stream ;
        sprintf(fileName, ofToDataPath(fileName).c_str());
        if ((stream = freopen(fileName, "w", stdout)) == NULL) {}
        /******************************************************************************************************/
    }

    printf("printfToFile %i!\n", printfToFile);



    //Setup Window Properties
    ofSetWindowShape(winWidth,winHeight);
    ofSetVerticalSync(false);	            //Set vertical sync to false for better performance?

    printf("freedom?");

    //load camera/video
    initDevice();
    printf("freedom2?");

    //set framerate
    ofSetFrameRate(camRate * 1.3);			//This will be based on camera fps in the future

    if ((tmpl = cvLoadImage(tmplImageName.c_str(), 1)))
    {
        printf("Succesful at template loading\n");
    }

    if (tmpl_left = cvLoadImage(tmplLeftName.c_str(), 1))
    {
        printf("Succesful at template loading\n");
    }

    templateImgLeft = cvCreateImage( cvSize(camWidth - tmpl_left->width + 1 ,camHeight - tmpl_left->height + 1),32,1);

//    templateImgDraw.allocate(camWidth - tmpl->width + 1, camHeight - tmpl->height + 1);
//    blobsCheck.allocate(camWidth - tmpl->width + 1, camHeight - tmpl->height + 1);

    blobsCheck.allocate(camWidth, camHeight);
    camShiftImage.allocate(camWidth,camHeight);
    aamImage.allocate(camWidth,camHeight);
    /*****************************************************************************************************
    * Allocate images (needed for drawing/processing images)
    ******************************************************************************************************/
    processedImg.allocate(camWidth, camHeight); //main Image that'll be processed.
    processedImg.setUseTexture(false);			//We don't need to draw this so don't create a texture
    processedImgHand.allocate(camWidth, camHeight);
    processedImgHand.setUseTexture(false);
    sourceImg.allocate(camWidth, camHeight);    //Source Image
    sourceImg.setUseTexture(false);				//We don't need to draw this so don't create a texture
//    grayBg.allocate(camWidth,camHeight);
//    grayBg.setUseTexture(false);				//We don't need to draw this so don't create a texture
    processedImgColor.allocate(camWidth, camHeight);
    processedImgColor.setUseTexture(false);				//We don't need to draw this so don't create a texture
//    BgImgColor.allocate(camWidth, camHeight);
//    BgImgColor.setUseTexture(false);
    /******************************************************************************************************/

    bLearnBackground2 = false;

    //Fonts - Is there a way to dynamically change font size?
    verdana.loadFont("verdana.ttf", 8, true, true);	   //Font used for small images
    bigvideo.loadFont("verdana.ttf", 13, true, true);  //Font used for big images.
    bigTitle.loadFont("verdana.ttf", 14, true, true);
    info.loadFont("verdana.ttf", 6, true, true);

    //Static Images
    background.loadImage("images/background.jpg"); //Main (Temp?) Background

    printf("freedom3?");
    //GUI Controls
    controls = ofxGui::Instance(this);
    setupControls();

    printf("freedom4?");
	
	aamTracking = false;
	vJones = false;
	bTemplate = false;
	bCamshift = false;
	bKalman = false;
	bMalik = false;

    //Setup Calibration
    calib.setup(camWidth, camHeight, &tracker);

    //Allocate Filters
    filter->allocate( camWidth, camHeight );

    /*****************************************************************************************************
    * Startup Modes
    ******************************************************************************************************/
    //If Standalone Mode (not an addon)
    if (bStandaloneMode)
    {
        printf("Starting in standalone mode...\n\n");
        showConfiguration = true;
    }
    if (bMiniMode)
    {
        showConfiguration = true;
        bShowInterface = false;
        printf("Starting in Mini Mode...\n\n");
        ofSetWindowShape(190, 200); //minimized size
        filter->bMiniMode = bMiniMode;
    }
    else
    {
        bShowInterface = true;
        printf("Starting in full mode...\n\n");
    }
/*
#ifdef TARGET_WIN32
    //get rid of the console window
    FreeConsole();
#endif
*/
    printf("Community Core Vision is setup!\n\n");
}

/****************************************************************
*	Load/Save config.xml file Settings
****************************************************************/
void ofxNCoreVision::loadXMLSettings()
{
    // TODO: a seperate XML to map keyboard commands to action
    message = "Loading config.xml...";
    // Can this load via http?
    if ( XML.loadFile("config.xml"))
        message = "Settings Loaded!\n\n";
    else
        message = "No Settings Found...\n\n"; //FAIL

    //--------------------------------------------------------------
    //  START BINDING XML TO VARS
    //--------------------------------------------------------------
    winWidth					= XML.getValue("CONFIG:WINDOW:WIDTH", 950);
    winHeight					= XML.getValue("CONFIG:WINDOW:HEIGHT", 600);
    bcamera						= XML.getValue("CONFIG:CAMERA_0:USECAMERA", 1);
    deviceID					= XML.getValue("CONFIG:CAMERA_0:DEVICE", 0);
    camWidth					= XML.getValue("CONFIG:CAMERA_0:WIDTH", 320);
    camHeight					= XML.getValue("CONFIG:CAMERA_0:HEIGHT", 240);
    camRate						= XML.getValue("CONFIG:CAMERA_0:FRAMERATE", 0);
    videoFileName				= XML.getValue("CONFIG:VIDEO:FILENAME", "test_videos/RearDI.m4v");
    maxBlobs					= XML.getValue("CONFIG:BLOBS:MAXNUMBER", 20);
    bShowLabels					= XML.getValue("CONFIG:BOOLEAN:LABELS",0);
    bDrawOutlines				= XML.getValue("CONFIG:BOOLEAN:OUTLINES",0);
    filter->bLearnBakground		= XML.getValue("CONFIG:BOOLEAN:LEARNBG",0);
    filter->bVerticalMirror		= XML.getValue("CONFIG:BOOLEAN:VMIRROR",0);
    filter->bHorizontalMirror	= XML.getValue("CONFIG:BOOLEAN:HMIRROR",0);

    //Logging
    printfToFile				= XML.getValue("CONFIG:BOOLEAN:PRINTFTOFILE",0);

    //Filters
    filter->bTrackDark			= XML.getValue("CONFIG:BOOLEAN:TRACKDARK", 0);
    filter->bHighpass			= XML.getValue("CONFIG:BOOLEAN:HIGHPASS",1);
    filter->bAmplify			= XML.getValue("CONFIG:BOOLEAN:AMPLIFY", 1);
    filter->bSmooth				= XML.getValue("CONFIG:BOOLEAN:SMOOTH", 1);
    filter->bDynamicBG			= XML.getValue("CONFIG:BOOLEAN:DYNAMICBG", 1);
    //MODES
    bGPUMode					= XML.getValue("CONFIG:BOOLEAN:GPU", 0);
    bMiniMode                   = XML.getValue("CONFIG:BOOLEAN:MINIMODE",0);
    //CONTROLS
    tracker.MIN_MOVEMENT_THRESHOLD	= XML.getValue("CONFIG:INT:MINMOVEMENT",0);
    MIN_BLOB_SIZE				= XML.getValue("CONFIG:INT:MINBLOBSIZE",2);
    MAX_BLOB_SIZE				= XML.getValue("CONFIG:INT:MAXBLOBSIZE",100);
    backgroundLearnRate			= XML.getValue("CONFIG:INT:BGLEARNRATE", 0.01f);
    //Filter Settings
    filter->threshold			= XML.getValue("CONFIG:INT:THRESHOLD",0);
    filter->highpassBlur		= XML.getValue("CONFIG:INT:HIGHPASSBLUR",0);
    filter->highpassNoise		= XML.getValue("CONFIG:INT:HIGHPASSNOISE",0);
    filter->highpassAmp			= XML.getValue("CONFIG:INT:HIGHPASSAMP",0);
    filter->smooth				= XML.getValue("CONFIG:INT:SMOOTH",0);
    //NETWORK SETTINGS
    bTUIOMode					= XML.getValue("CONFIG:BOOLEAN:TUIO",0);
    myTUIO.bOSCMode				= XML.getValue("CONFIG:BOOLEAN:OSCMODE",1);
    myTUIO.bTCPMode				= XML.getValue("CONFIG:BOOLEAN:TCPMODE",1);
    myTUIO.bHeightWidth			= XML.getValue("CONFIG:BOOLEAN:HEIGHTWIDTH",0);
    tmpLocalHost				= XML.getValue("CONFIG:NETWORK:LOCALHOST", "localhost");
    tmpPort						= XML.getValue("CONFIG:NETWORK:TUIOPORT_OUT", 3333);
    tmpFlashPort				= XML.getValue("CONFIG:NETWORK:TUIOFLASHPORT_OUT", 3000);
    aamModelFileName			= XML.getValue("CONFIG:AAM:MODEL", "hand.amf");
    cascadeFileName			= XML.getValue("CONFIG:HAAR:CASCADE", "haarcascade.xml");
    myTUIO.bHandInfo			= XML.getValue("CONFIG:BOOLEAN:HANDINFO",0);
    tmplImageName			= XML.getValue("CONFIG:TEMPLATE:RIGHT","template_hand.bmp");
    tmplLeftName			= XML.getValue("CONFIG:TEMPLATE:LEFT","template_hand_left.bmp");
    myTUIO.setup(tmpLocalHost.c_str(), tmpPort, tmpFlashPort); //have to convert tmpLocalHost to a const char*
    //--------------------------------------------------------------
    //  END XML SETUP

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
    initBg  = true;
    stdDYcondens=0;
    CvSize axesCondens;

    predict = NULL;

    theta=0;
    /*---------*/
    /*
    Histogram creation
    */

    facedet.LoadCascade(cascadeFileName.c_str());

    histDims = 16;

    float hranges_arr[] = {0,180};

    hranges = hranges_arr;

    hist = cvCreateHist( 1, &histDims, CV_HIST_ARRAY, &hranges, 1 );

    histImg.allocate(camWidth,camHeight);
    cvZero(histImg.getCvImage() );

    h_plane = cvCreateImage(cvGetSize(histImg.getCvImage()), 8 , 1);
    planes = &h_plane;

    //Checking if lots of time is being consumed during the model load

//    time(&before);

    model.ReadModel(aamModelFileName);

    selected = false;

    Shape = model.__VJDetectShape;

//    time(&after);
//
//    elapsed = difftime(before, after);

}

void ofxNCoreVision::saveSettings()
{
    XML.setValue("CONFIG:CAMERA_0:USECAMERA", bcamera);
    XML.setValue("CONFIG:CAMERA_0:DEVICE", deviceID);
    XML.setValue("CONFIG:CAMERA_0:WIDTH", camWidth);
    XML.setValue("CONFIG:CAMERA_0:HEIGHT", camHeight);
    XML.setValue("CONFIG:CAMERA_0:FRAMERATE", camRate);
    XML.setValue("CONFIG:BOOLEAN:PRESSURE",bShowPressure);
    XML.setValue("CONFIG:BOOLEAN:LABELS",bShowLabels);
    XML.setValue("CONFIG:BOOLEAN:OUTLINES",bDrawOutlines);
    XML.setValue("CONFIG:BOOLEAN:LEARNBG", filter->bLearnBakground);
    XML.setValue("CONFIG:BOOLEAN:VMIRROR", filter->bVerticalMirror);
    XML.setValue("CONFIG:BOOLEAN:HMIRROR", filter->bHorizontalMirror);
    XML.setValue("CONFIG:BOOLEAN:PRINTFTOFILE", printfToFile);
    XML.setValue("CONFIG:BOOLEAN:TRACKDARK", filter->bTrackDark);
    XML.setValue("CONFIG:BOOLEAN:HIGHPASS", filter->bHighpass);
    XML.setValue("CONFIG:BOOLEAN:AMPLIFY", filter->bAmplify);
    XML.setValue("CONFIG:BOOLEAN:SMOOTH", filter->bSmooth);
    XML.setValue("CONFIG:BOOLEAN:DYNAMICBG", filter->bDynamicBG);
    XML.setValue("CONFIG:BOOLEAN:GPU", bGPUMode);
    XML.setValue("CONFIG:INT:MINMOVEMENT", tracker.MIN_MOVEMENT_THRESHOLD);
    XML.setValue("CONFIG:INT:MINBLOBSIZE", MIN_BLOB_SIZE);
    XML.setValue("CONFIG:INT:MAXBLOBSIZE", MAX_BLOB_SIZE);
    XML.setValue("CONFIG:INT:BGLEARNRATE", backgroundLearnRate);
    XML.setValue("CONFIG:INT:THRESHOLD", filter->threshold);
    XML.setValue("CONFIG:INT:HIGHPASSBLUR", filter->highpassBlur);
    XML.setValue("CONFIG:INT:HIGHPASSNOISE", filter->highpassNoise);
    XML.setValue("CONFIG:INT:HIGHPASSAMP", filter->highpassAmp);
    XML.setValue("CONFIG:INT:SMOOTH", filter->smooth);
    XML.setValue("CONFIG:BOOLEAN:MINIMODE", bMiniMode);
    XML.setValue("CONFIG:BOOLEAN:TUIO",bTUIOMode);
    XML.setValue("CONFIG:BOOLEAN:HEIGHTWIDTH", myTUIO.bHeightWidth);
    XML.setValue("CONFIG:BOOLEAN:OSCMODE", myTUIO.bOSCMode);
    XML.setValue("CONFIG:BOOLEAN:TCPMODE", myTUIO.bTCPMode);
//	XML.setValue("CONFIG:NETWORK:LOCALHOST", myTUIO.localHost);
//	XML.setValue("CONFIG:NETWORK:TUIO_PORT_OUT",myTUIO.TUIOPort);
    XML.saveFile("config.xml");
}

/************************************************
*				Init Device
************************************************/
//Init Device (camera/video)
void ofxNCoreVision::initDevice()
{

    //save/update log file
    if (printfToFile) if ((stream = freopen(fileName, "a", stdout)) == NULL) {}

    //Pick the Source - camera or video
    if (bcamera)
    {
        //check if a firefly, ps3 camera, or other is plugged in
#ifdef TARGET_WIN32
        /****PS3 - PS3 camera only****/
        if (ofxPS3::getDeviceCount() > 0 && PS3 == NULL)
        {
            PS3 = new ofxPS3();
            PS3->listDevices();
            PS3->initPS3(camWidth, camHeight, camRate);
            camWidth = PS3->getCamWidth();
            camHeight = PS3->getCamHeight();
            printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, PS3->getCamWidth(), PS3->getCamHeight());
            return;
        }
        /****ffmv - firefly camera only****/
        else if (ofxffmv::getDeviceCount() > 0 && ffmv == NULL)
        {
            ffmv = new ofxffmv();
            ffmv->listDevices();
            ffmv->initFFMV(camWidth,camHeight);
            printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, ffmv->getCamWidth(), ffmv->getCamHeight());
            camWidth = ffmv->getCamWidth();
            camHeight = ffmv->getCamHeight();
            return;
        }
        else if ( vidGrabber == NULL )
        {
            vidGrabber = new ofVideoGrabber();
            vidGrabber->listDevices();
            vidGrabber->setVerbose(true);
            vidGrabber->initGrabber(camWidth,camHeight);
            printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, vidGrabber->width, vidGrabber->height);
            camWidth = vidGrabber->width;
            camHeight = vidGrabber->height;
            return;
        }
        else if ( dsvl == NULL)
        {
            dsvl = new ofxDSVL();
            dsvl->initDSVL();
            printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, dsvl->getCamWidth(), dsvl->getCamHeight());
            camWidth = dsvl->getCamWidth();
            camHeight = dsvl->getCamHeight();
            return;
        }
#else
        if ( vidGrabber == NULL )
        {
            vidGrabber = new ofVideoGrabber();
            vidGrabber->listDevices();
            vidGrabber->setVerbose(true);
            vidGrabber->initGrabber(camWidth,camHeight);
            printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, vidGrabber->width, vidGrabber->height);
            camWidth = vidGrabber->width;
            camHeight = vidGrabber->height;
            return;
        }
#endif
    }
    else
    {
        if ( vidPlayer == NULL )
        {
            vidPlayer = new ofVideoPlayer();
            vidPlayer->loadMovie( videoFileName );
            vidPlayer->play();
            vidPlayer->setLoopState(OF_LOOP_NORMAL);
            printf("Video Mode\n\n");
            camHeight = vidPlayer->height;
            camWidth = vidPlayer->width;
            return;
        }
    }
}

/******************************************************************************
* The update function runs continuously. Use it to update states and variables
*****************************************************************************/
void ofxNCoreVision::_update(ofEventArgs &e)
{
	printf("update!\n");
    //save/update log file
    if (printfToFile) if ((stream = freopen(fileName, "a", stdout)) == NULL) {}

	printf("update2\n");
    if (exited) return;
	printf("update3\n");
    bNewFrame = false;
	
	printf("Before grabbing\n");

    if (bcamera) //if camera
    {
		#ifdef TARGET_WIN32
        if (PS3!=NULL)//ps3 camera
        {
			printf("ps3, freedom (?)\n");
            bNewFrame = PS3->isFrameNew();
			printf("ps3, freedom 2(?)\n");
        }
        else if (ffmv!=NULL)
        {
			printf("ffmv, freedom (?)\n");
            ffmv->grabFrame();
            bNewFrame = true;
			printf("ffmv, freedom 1(?)\n");
        }
        else if (vidGrabber !=NULL)
        {
			printf("VidGrab, freedom (?)\n");
            vidGrabber->grabFrame();
            bNewFrame = vidGrabber->isFrameNew();
			printf("VidGrab, freedom 2(?)\n");
        }
        else if (dsvl !=NULL)
        {
			printf("dsvl, freedom 1(?)\n");
            bNewFrame = dsvl->isFrameNew();
			printf("dsvl, freedom 2(?)\n");
        }
#else
		printf("Camera none, freedom 1(?)\n");
        vidGrabber->grabFrame();
        bNewFrame = vidGrabber->isFrameNew();
		printf("Camera none, freedom 2(?)\n");
#endif
    }
    else //if video
    {
		printf("VidPlayer, freedom 1(?)\n");
        vidPlayer->idleMovie();
        bNewFrame = vidPlayer->isFrameNew();
		printf("VidPlayer, freedom 2(?)\n");
    }

    //if no new frame, return
    if (!bNewFrame)
    {
		printf("No new frame, freedom (?)\n");
        return;
    }
    else//else process camera frame
    {
        ofBackground(0, 0, 0);

        // Calculate FPS of Camera
        frames++;
        float time = ofGetElapsedTimeMillis();
        if (time > (lastFPSlog + 1000))
        {
            fps = frames;
            frames = 0;
            lastFPSlog = time;
        }//End calculation

        float beforeTime = ofGetElapsedTimeMillis();

        if (bGPUMode)
        {
			printf("Frame ok-GPU1!");
            grabFrameToGPU(filter->gpuSourceTex);
			printf("Frame ok-GPU!");
            filter->applyGPUFilters();
            contourFinder.findContours(filter->gpuReadBackImageGS,  (MIN_BLOB_SIZE * 2) + 1, ((camWidth * camHeight) * .4) * (MAX_BLOB_SIZE * .001), maxBlobs, false);
        }
        else
        {
            printf("Frame ok!");
            grabFrameToCPU();
            printf("Frame not fault!");
            /*Hand Tracking
            */
            h_plane = processedImg.getCvImage();
            planes = &h_plane;

            learnBackGround(sourceImg);

            processedImg = sourceImg;

            processedImgHand = sourceImg;
            //      cvSub(processedImg.getCvImage(), grayBg.getCvImage(), processedImg.getCvImage());
            cvSub(processedImgColor.getCvImage(), filter->BgImgColor.getCvImage(), processedImgColor.getCvImage());

            filter->applyCPUFiltersHand(processedImgHand);

            #if DEBUG
                            cvSaveImage("subtractionHand.bmp", processedImgHand.getCvImage());
#endif
            handContourFinder.findContours(processedImgHand, (MIN_BLOB_SIZE * 2) + 1, ((camWidth * camHeight) * .4) * (MAX_BLOB_SIZE * .001), 5, false);




#if DEBUG
            printf("Got problems here(?)\n");
#endif
            cvCalcHist( planes, hist, 0, 0 ); // Compute histogram

#if DEBUG
            printf("Ok!\n");
#endif
            cvCalcBackProject( planes, histImg.getCvImage(), hist );

            /********************/

            /*******************************************************
            Gaussian Background Model
            *******************************************************/

            if (initBg)
            {
                initBackgroundModel(&bkgdMdl,sourceImg.getCvImage(), &paramMoG);
                initBg  = false;
            }

            else
            {
#if DEBUG
                printf("AfterBackground initialization:\n");
#endif

                blobsCheck = sourceImg;
                camShiftImage = sourceImg;
                aamImage = sourceImg;

                //cvSub(blobsCheck.getCvImage(), grayBg.getCvImage(), blobsCheck.getCvImage());
#if DEBUG
                printf("Copied Image\n");
#endif

                binaryForeground = updateBackground(bkgdMdl,blobsCheck.getCvImage());
#if DEBUG
                printf("Updated background\n");
#endif
                blobsVector = getBlobs2(blobsCheck.getCvImage(),binaryForeground);
#if DEBUG
                printf("BLobs Vector\n");
#endif
                blobs_total = blobsVector.GetNumBlobs();
#if DEBUG
                printf("Blobs Total:\n");
#endif

            #if DEBUG
                            cvSaveImage("blobsCheck.bmp", blobsCheck.getCvImage());
#endif

                /*
                            if (blur)blobsCheck.blur( 3 );
                            if (dilate)blobsCheck.dilate( );
                            if (erode)blobsCheck.erode( );
                            if (eqHist)blobsCheck.equalizeHist();
                            if (highpass)blobsCheck.highpass(12,3);
                            if (amplify)blobsCheck.amplify(blobsCheck, 300);
                */
#if DEBUG
                printf("Before Selection:\n");
#endif

                if ( blobs_total > 0 )
                {
                    if (selected == false)
                    {

#if DEBUG
                        printf("How many: %d\n", blobs_total);
#endif
                        //drawInitialBlobs(blobsCheck.getCvImage(), blobsVector);

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

                        /*******************************************************
                        Kalman Tracking Usage
                        *******************************************************/

                        if (bKalman)
                        {
                            drawBlob2(blobsCheck.getCvImage(), coordReal, 255,255,255);

                            printf("Before Kalman> %lf, %lf, %lf, %lf,%d, %d, %d, %d \n", coordPredict.cX, coordPredict.cY, coordReal.cX, coordReal.cY, coordReal.MaxX, coordReal.MaxY, coordReal.MinX, coordReal.MinY);

                            predict = updateKalman(kalman,coordReal);


                            coordPredict.set (coordReal.MaxX, coordReal.MinX, coordReal.MaxY, coordReal.MinY);

                            coordPredict.set ((int)predict[0], (int)predict[1]);
                            printf("After Kalman> %lf, %lf, %lf, %lf,%d, %d, %d, %d \n", coordPredict.cX, coordPredict.cY, coordReal.cX, coordReal.cY, coordReal.MaxX, coordReal.MaxY, coordReal.MinX, coordReal.MinY);

                            muX = sqrt(kalman->error_cov_pre->data.fl[0]);
                            muY = sqrt(kalman->error_cov_pre->data.fl[5]);

                            axes = cvSize( muX , muY );

                            printf("Ellipse Kalman\n");
                            cvEllipse( blobsCheck.getCvImage(), cvPoint(coordPredict.cX,coordPredict.cY), axes, theta, 0, 360, CV_RGB(255,255,0),1);
                            cvLine( blobsCheck.getCvImage(),  cvPoint(coordPredict.cX,coordPredict.cY), cvPoint(coordPredict.cX,coordPredict.cY), CV_RGB(255,0, 0), 4, 8, 0 );
                            cvLine( blobsCheck.getCvImage(),  cvPoint(coordPredict.cX,coordPredict.cY), cvPoint(coordReal.cX,coordReal.cY), CV_RGB(255,0, 0), 1, 8, 0 );


                            //distance from real
                            resultDistance = sqrt((double)(coordReal.cX - coordPredict.cX)*(coordReal.cX - coordPredict.cX) + (coordReal.cY - coordPredict.cY)*(coordReal.cY - coordPredict.cY));
                            meanKalmanDistance+=resultDistance;
                        }
                        /*******************************************************
                        End Kalman Tracking Usage
                        *******************************************************/
                    }
                }
            }

            /****************************************************
            End of the Gaussian Background Model
            ***************************************************/

            filter->applyCPUFilters( processedImg );
            contourFinder.findContours(processedImg,  (MIN_BLOB_SIZE * 2) + 1, ((camWidth * camHeight) * .4) * (MAX_BLOB_SIZE * .001), maxBlobs, false);

            /*****************************
            Begin of other Tracking Methods
            *********************************/

            /******************************************
            Template Matching using the CCV contourFinder
            **********************************************/

            if (bTemplate)
            {
                printf("Template Matching nBlobs!: %d\n", contourFinder.nBlobs);
                if (contourFinder.nBlobs > 0)
                {
                    for (int i =0; i < contourFinder.nBlobs; i++)
                    {
                        if ( contourFinder.blobs[i].area > 500)
                        {


                            /************************************
                            Template Matching
                            ***************************************/
                            // templateImg = cvCreateImage(cvSize(processedImgColor.getCvImage()->width - tmpl->width + 1 , processedImgColor.getCvImage()->height - tmpl->height + 1),32, 1);

                            win = cvRect(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 150,150);
//                        /* make sure that the search window is still within the frame */
                            if (win.x < 0)
                                win.x = 0;
                            if (win.y < 0)
                                win.y = 0;
                            if (win.x + win.width > processedImgColor.getCvImage()->width)
                                win.x = processedImgColor.getCvImage()->width - win.width;
                            if (win.y + win.height > processedImgColor.getCvImage()->height)
                                win.y = processedImgColor.getCvImage()->height - win.height;

#if DEBUG
                            cvSaveImage("sourceTemplate.bmp", processedImgColor.getCvImage());
#endif

                            cvSetImageROI(processedImgColor.getCvImage(),win);


                            templateImg = cvCreateImage(cvSize(win.width - tmpl->width + 1 , win.height - tmpl->height + 1),32, 1);

                            printf("Widths: %d %d %d, Heights: %d, %d, %d\n", win.width, tmpl->width, templateImg->width, win.height, tmpl->height, templateImg->height);
                            cvMatchTemplate(processedImgColor.getCvImage(), tmpl, templateImg, 1);

							printf("Match template 1 (freedom)\n");

                            cvResetImageROI(processedImgColor.getCvImage());
                            //cvReleaseImage(&templateImg);
	
                            cvNormalize(templateImg, templateImg, 1, 0, CV_MINMAX);
                            cvPow(templateImg,templateImg,5);


                            cvMinMaxLoc(templateImg, &minF, &maxF, &templateRightMin, &templateRightMax);


                            printf("Maximum: %lf, Minimum %lf\n", maxF, minF);

#if DEBUG
                            cvSaveImage("done.bmp", templateImg);
#endif
                            cvSetImageROI(processedImgColor.getCvImage(),win);
							
							printf("Match template 2 (freedom)\n");
                            templateImgLeft = cvCreateImage(cvSize(win.width - tmpl_left->width + 1 , win.height - tmpl_left->height + 1),32, 1);


                            cvMatchTemplate(processedImgColor.getCvImage(), tmpl_left, templateImgLeft, 5);
							printf("Match template 2 after (freedom)\n");
                            cvResetImageROI(processedImgColor.getCvImage());
                            //cvReleaseImage(&templateImgLeft);
                            cvNormalize(templateImgLeft, templateImgLeft, 1, 0, CV_MINMAX);

                            cvMinMaxLoc(templateImgLeft, &minF_left, &maxF_left, &templateLeftMin, &templateLeftMax);


                            if (maxF > 0)
                            {
                                templateRightMax.x += win.x;
                                templateRightMax.y += win.y;
                                contourFinder.blobs[i].xHand = templateRightMax.x;
                                contourFinder.blobs[i].yHand = templateRightMax.y;
                            }
                            else
                            {
                                if (maxF_left > 0)
                                {
                                    templateLeftMax.x += win.x;
                                    templateLeftMax.y += win.y;
                                    contourFinder.blobs[i].xHand = templateLeftMax.x;
                                    contourFinder.blobs[i].yHand = templateLeftMax.y;
                                }
                                else
                                {
                                    contourFinder.blobs[i].xHand = 0;
                                    contourFinder.blobs[i].yHand = 0;
                                }
                            }

                            printf("Maximum: %lf, Minimum %lf\n", maxF_left, minF_left);
                            cvPow(templateImgLeft, templateImgLeft, 5);

#if DEBUG
                            cvSaveImage("done_left.bmp", templateImgLeft);
                            cvSaveImage("source.bmp", processedImgColor.getCvImage());
#endif

//                        templateImgDraw.resize(templateImg->width, templateImg->height);
//
//                        cvConvertImage(templateImg, templateImgDraw.getCvImage());
                        }

                        /******************************************
                        Model fitting with the CCV contourFinder
                        **********************************************/

//                        //flag = model.InitShapeFromDetBox(Shape,blobsCheck.getCvImage(),facedet);
//                        Shape[0].x = contourFinder.blobs[i].centroid.x;
//                        Shape[0].y = contourFinder.blobs[i].centroid.y;
//                        Shape[1].x = contourFinder.blobs[i].centroid.x + contourFinder.blobs[i].boundingRect.width;
//                        Shape[1].y = contourFinder.blobs[i].centroid.x + contourFinder.blobs[i].boundingRect.height;
//
//
//                        flag = true;
//                        /*
//                        modelIC.InitParams(blobsCheck.getCvImage());
//                        */
//                        if (flag == false)
//                        {
//                            printf("False model fitting\n");
//                        }
//                        else
//                        {
//
//                            model.Fit(blobsCheck.getCvImage(), Shape, 30, true);
//                            //printf("Pyramid OK\n");
//                            /*
//                            modelIC.Fit(blobsCheck.getCvImage(), Shape, 30, false);
//                            printf ("Inverse Compositional OK\n");
//                            */
//                            model.Draw(blobsCheck.getCvImage(), Shape, 2);
//                            /*
//                            modelIC.Draw(blobsCheck.getCvImage(), Shape, 1);
//                            */
//                        }
                    }
                }
            }
//--------------------------------------------------------------------------------------------
            /**********************************************

            Camshift Tracking
            *******************************************/
            //Working on ROI-Camshift
            if (bCamshift)
            {
                printf("Before Camshift\n");
                printf("COord Real x: %lf, Coord Real y: %lf\n", coordReal.cX, coordReal.cY);
#ifdef TARGET_WIN32
				if ( coordReal.cX <= 0 && coordReal.cY <= 0)
			   {
				   coordReal.cX = 1.0;
				   coordReal.cY = 1.0;
			   }
#endif
               HandROIAdjust(coordReal.cX,coordReal.cY, processedImg.getCvImage());
			   printf("COord Real x: %lf, Coord Real y: %lf\n", coordReal.cX, coordReal.cY);
			   
                cvCamShift(processedImg.getCvImage(), handSelectionRect, cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ), &handComp, &handBox);
                handSelectionRect = handComp.rect;

                cvEllipseBox( camShiftImage.getCvImage(), handBox, CV_RGB(255,255,255), 5, CV_AA, 0 );

                /***********************************************
                End of the Camshift Tracking
                ***********************************************/
            }

            /**************************************************
            Start of AAM methods for better Shape control and entities association
            ****************************************************/

            if (aamTracking)
            {
                printf("AAM fitting(be careful.. :P)\n");

                if (vJones)
                {

					printf("if vJones (freedom)!\n");

                    flag = model.InitShapeFromDetBox(Shape,blobsCheck.getCvImage(),facedet);

                        if (flag == false)
                        {
                            printf("False model fitting\n");
                        }
                        else
                        {

                            model.Fit(aamImage.getCvImage(), Shape, 30, false);
                            //printf("Pyramid OK\n");
                            /*
                            modelIC.Fit(blobsCheck.getCvImage(), Shape, 30, false);
                            printf ("Inverse Compositional OK\n");
                            */
                            model.Draw(aamImage.getCvImage(), Shape, 2);
                            /*
                            modelIC.Draw(blobsCheck.getCvImage(), Shape, 1);
                            */
                        }

                }
                else
                {
                /****************************************************************
                AAM-Fitting with Template Matching
                ****************************************************************/
                if (bTemplate)
                {
                    printf("Template Matching + AAM fitting \n");
                    if ( contourFinder.nBlobs > 5)
                    {
                        for (int i =0; i < contourFinder.nBlobs; i++)
                        {
                            if (contourFinder.blobs[i].xHand > 0)
                            {

                                Shape[0].x = (contourFinder.blobs[i].xHand)*2.0;
                                Shape[0].y = (contourFinder.blobs[i].yHand)*2.0;
                                Shape[1].x = Shape[0].x + 2.0*150;
                                Shape[1].y = Shape[0].y + 2.0*150;
                                Shape.ScaleXY((-Shape[0].x+Shape[1].x)/100, (-Shape[0].y+Shape[1].y)/100);
                                Shape.Translate((Shape[0].x+Shape[1].x)/2., (Shape[0].y+Shape[1].y)/2. );


                                flag = true;
                                /*
                                modelIC.InitParams(blobsCheck.getCvImage());
                                */
                                if (flag == false)
                                {
                                    printf("False model fitting\n");
                                }
                                else
                                {

                                    model.Fit(aamImage.getCvImage(), Shape, 30, false);
                                    printf("Pyramid OK\n");
                                    /*
                                    modelIC.Fit(blobsCheck.getCvImage(), Shape, 30, false);
                                    printf ("Inverse Compositional OK\n");
                                    */
                                    model.Draw(aamImage.getCvImage(), Shape, 2);
                                    /*
                                    modelIC.Draw(blobsCheck.getCvImage(), Shape, 1);
                                    */
                                }
                            }
                        }
                    }
                    } //end of Template If
                else
                {
                    /****************************************************************
                    AAM-Fitting with Kalman
                    ****************************************************************/
                    if (bKalman)
                    {
                        printf("Kalman Tracking + AAM fitting \n");
//                        printf("Kalman + fitting \n");
//                        if( contourFinder.nBlobs > 5)
//                        {
//                        Shape[0].x = coordReal.MinX;
//                        Shape[0].y = coordReal.MinY;
//                        Shape[1].x = coordReal.lX;
//                        Shape[1].y = coordReal.lY;
//
//
//                        flag = true;
//                        /*
//                        modelIC.InitParams(blobsCheck.getCvImage());
//                        */
//                        if (flag == false)
//                        {
//                            printf("False model fitting\n");
//                        }
//                        else
//                        {
//
//                            model.Fit(blobsCheck.getCvImage(), Shape, 30, true);
//                            printf("Pyramid OK\n");
//                            /*
//                            modelIC.Fit(blobsCheck.getCvImage(), Shape, 30, false);
//                            printf ("Inverse Compositional OK\n");
//                            */
//                            model.Draw(blobsCheck.getCvImage(), Shape, 2);
//                            /*
//                            modelIC.Draw(blobsCheck.getCvImage(), Shape, 1);
//                            */
//                        }
//                        }
                    }

                    /****************************************************************
                    End of AAM-Fitting with Kalman
                    ****************************************************************/

                    else
                    {

                        /*---------------------------------------------------------*/

                        /****************************************************************
                        //AAM-Fitting with Camshift
                        //****************************************************************/
                        if (bCamshift)
                        {
                            printf("Camshift Tracking + AAM fitting \n");
//                        if( contourFinder.nBlobs > 5)
//                        {
//                        Shape[0].x = handSelectionRect.x;
//                        Shape[0].y = handSelectionRect.y;
//                        Shape[1].x = handSelectionRect.x + handSelectionRect.width/2;
//                        Shape[1].y = handSelectionRect.x + handSelectionRect.height/2;
//
//
//                        flag = true;
//                        /*
//                        modelIC.InitParams(blobsCheck.getCvImage());
//                        */
//                        if (flag == false)
//                        {
//                            printf("False model fitting\n");
//                        }
//                        else
//                        {
//
//                            model.Fit(blobsCheck.getCvImage(), Shape, 30, true);
//                            //printf("Pyramid OK\n");
//                            /*
//                            modelIC.Fit(blobsCheck.getCvImage(), Shape, 30, false);
//                            printf ("Inverse Compositional OK\n");
//                            */
//                            model.Draw(blobsCheck.getCvImage(), Shape, 2);
//                            /*
//                            modelIC.Draw(blobsCheck.getCvImage(), Shape, 1);
//                            */
//                        }
//                        }
                        }

                        /****************************************************************
                        ENd of AAM-Fitting with Camshift
                        ****************************************************************/

                    }
                    printf("AAM fitting.. (ok!) \n");
                }

                }
            }

            else
            {


        /******************************************
            Viola and Jones - Hand Detection
            **********************************************/

            if (vJones)
            {
				printf("if vJones2 (freedom)!\n");

                facedet.DetectFace2(aamImage.getCvImage());

            }

            /******************************************
            End of Viola and Jones - Hand Detection
            **********************************************/

            }


            /****************************************************************
            End of AAM-Fitting
            ****************************************************************/

            /*---------------------------------------------------------*/
            /*******************************************************************
            Malik's Finger Detection Algorithm
            ********************************************************************/
			if(	bMalik)
			{
            printf("Before array: Ok!\n");

            nHands = handContourFinder.nBlobs;

            printf("Nhands: %d\n", nHands);
//
            for (int i = 0; i < nHands; i++)
            {
                /*
                if (handContourFinder.nBlobs > 0)
                {
                myHand.myBlob = handContourFinder.blobs
                fingerFinder.findmyFinger(myHand);
                printf("How many fingers: %d\n",myHand.nFingers);
                }
                */
                hands[i].myBlob = handContourFinder.blobs[i];
                fingerFinder.findFingers(hands[i]);
            }

            printf("Before energy steps: Ok!");

            //---------------------------------------------
            for (int i = 0; i < MAX_N_TRACKED_FINGERS; i++)
            {
                myFinger[i].energy *= 0.905f;
                myFinger[i].bFoundMeThisFrame = false;
            }


            for (int i = 0; i < nHands; i++)
            {
                int nFingers = hands[i].nFingers;
                printf("NFingers: %d\n", nFingers);
                for (int j = 0; j < nFingers; j++)
                {


                    bool bFound = false;
                    int  smallestIndex = -1;
                    float smallestDist = 10000;

                    for (int k = 0; k < MAX_N_TRACKED_FINGERS; k++)
                    {

                        if (myFinger[k].energy < 0.01 || myFinger[k].bFoundMeThisFrame) continue;	// skip non energized persistant faces.


                        float dx = myFinger[k].pos.x - hands[i].fingerPos[j].x;
                        float dy = myFinger[k].pos.y - hands[i].fingerPos[j].y;
                        float len = sqrt((dx*dx) + (dy*dy));

                        if (len < smallestDist)
                        {
                            smallestDist 		= len;
                            smallestIndex		= k;
                        }
                    }



                    if (smallestDist < 80)
                    {
                        myFinger[smallestIndex].energy += 0.2f;
                        myFinger[smallestIndex].energy = MIN(myFinger[smallestIndex].energy, 1);
                        myFinger[smallestIndex].bFoundMeThisFrame = true;
                        myFinger[smallestIndex].pos.x = hands[i].fingerPos[j].x;
                        myFinger[smallestIndex].pos.y = hands[i].fingerPos[j].y;
                        myFinger[smallestIndex].birthday = ofGetElapsedTimef();
                        bFound = true;
                    }

                    if (!bFound)
                    {

                        int  smallestIndex = -1;
                        float smallestEnergy = 100000000	;
                        // ok find the earliest, of first non energized persistant face:
                        for (int k = 0; k < MAX_N_TRACKED_FINGERS; k++)
                        {
                            if (myFinger[k].bFoundMeThisFrame) continue;
                            if (myFinger[k].birthday < smallestEnergy)
                            {
                                smallestEnergy 	= myFinger[k].birthday;
                                smallestIndex 	= k;
                            }
                        }

                        if (smallestIndex != -1)
                        {
                            myFinger[smallestIndex].pos.x = hands[i].fingerPos[j].x;
                            myFinger[smallestIndex].pos.y = hands[i].fingerPos[j].y;
                            myFinger[smallestIndex].energy = 0.2f;
                            myFinger[smallestIndex].bFoundMeThisFrame = true;
                            myFinger[smallestIndex].birthday = ofGetElapsedTimef();
                        }
                    }


                }
            }


            for (int i = 0; i < MAX_N_TRACKED_FINGERS; i++)
            {
                if (myFinger[i].bFoundMeThisFrame == false)
                {
                    myFinger[i].energy = 0;
                }
            }
}
            /*******************************************************************
            End of Malik's Finger Detection Algorithm
            ********************************************************************/
        }


        /*****************************
        End of other Tracking Methods
        *********************************/

//Track found contours/blobss
        tracker.track(&contourFinder);
//get DSP time
        differenceTime = ofGetElapsedTimeMillis() - beforeTime;

//Dynamic Background subtraction LearRate
        if (filter->bDynamicBG)
        {
            filter->fLearnRate = backgroundLearnRate * .0001; //If there are no blobs, add the background faster.
            if (contourFinder.nBlobs > 0) //If there ARE blobs, add the background slower.
            {
                filter->fLearnRate = backgroundLearnRate * .0001;
            }
        }//End Background Learning rate

        if (bTUIOMode)
        {
            //Start sending OSC
            myTUIO.sendTUIO(&getBlobs());
        }
    }
}


/************************************************
*				Input Device Stuff
************************************************/
//get pixels from camera
void ofxNCoreVision::getPixels()
{

#ifdef TARGET_WIN32
    if (PS3!=NULL)
    {
        sourceImg.setFromPixels(PS3->getPixels(), camWidth, camHeight);
        //convert to grayscale
        processedImg = sourceImg;
    }
    else if (ffmv != NULL)
    {
        processedImg.setFromPixels(ffmv->fcImage[ffmv->getDeviceID()].pData, camWidth, camHeight);
    }
    else if (vidGrabber != NULL )
    {
        sourceImg.setFromPixels(vidGrabber->getPixels(), camWidth, camHeight);
        //convert to grayscale
        processedImg = sourceImg;
    }
    else if (dsvl!=NULL)
    {
        if (dsvl->getNumByes() != 1) //if not grayscale
        {
            sourceImg.setFromPixels(dsvl->getPixels(), camWidth, camHeight);
            //convert to grayscale
            processedImg = sourceImg;
        }
        else //if grayscale
        {
            processedImg.setFromPixels(dsvl->getPixels(), camWidth, camHeight);
        }
    }
#endif
}


//Grab frame from CPU
void ofxNCoreVision::grabFrameToCPU()
{
    //Set sourceImg as new camera/video frame
    if (bcamera)
    {
#ifdef TARGET_WIN32
        getPixels();
#else
        sourceImg.setFromPixels(vidGrabber->getPixels(), camWidth, camHeight);
        //convert to grayscale
        processedImg = sourceImg;
        processedImgColor = sourceImg;
#endif
    }
    else
    {
        sourceImg.setFromPixels(vidPlayer->getPixels(), camWidth, camHeight);
        //convert to grayscale
        processedImgColor = sourceImg;
        processedImg = sourceImg;
    }
}

//Grab frame from GPU
void ofxNCoreVision::grabFrameToGPU(GLuint target)
{
    //grab the frame to a raw openGL texture
    if (bcamera)
    {
        glEnable(GL_TEXTURE_2D);
        //glPixelStorei(1);
        glBindTexture(GL_TEXTURE_2D, target);

#ifdef TARGET_WIN32
        if (PS3!=NULL)
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, PS3->getPixels());
        }
        else if (vidGrabber!=NULL)
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, vidGrabber->getPixels());
        }
        else if (dsvl!=NULL)
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, dsvl->getPixels());
        }
#else
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, vidGrabber->getPixels());
#endif
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D,0);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, target);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, vidPlayer->getPixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D,0);
    }
}


/******************************************************************************
* The draw function paints the textures onto the screen. It runs after update.
*****************************************************************************/
void ofxNCoreVision::_draw(ofEventArgs &e)
{
    if (exited) return;

    if (showConfiguration)
    {
        //if calibration
        if (bCalibration)
        {
            //Don't draw main interface
            calib.passInContourFinder(contourFinder.nBlobs, contourFinder.blobs);
            calib.doCalibration();
        }
        //if mini mode
        else if (bMiniMode)
        {
            drawMiniMode();
        }
        //if full mode
        else if (bShowInterface)
        {
            drawFullMode();
            if (bDrawOutlines || bShowLabels) drawFingerOutlines();
        }
        //draw gui controls
        if (!bCalibration && !bMiniMode)
        {
            controls->draw();
        }
    }
}

void ofxNCoreVision::drawFullMode()
{

    ofSetColor(0xFFFFFF);
    //Draw Background Image
    background.draw(0, 0);
    //Draw arrows
    ofSetColor(187, 200, 203);
    ofFill();
    ofTriangle(680, 420, 680, 460, 700, 440);
    ofTriangle(70, 420, 70, 460, 50, 440);
    ofSetColor(255, 255, 0);

    ofSetColor(0xFFFFFF);
    //Draw Image Filters To Screen
    if (bGPUMode) filter->drawGPU();
    else filter->draw();

    ofSetColor(0x000000);
    if (bShowPressure)
    {
        bigvideo.drawString("Pressure Map", 140, 20);
    }
    else
    {
        bigvideo.drawString("Source Image", 140, 20);
    }
    bigvideo.drawString("Tracked Image", 475, 20);

    //draw link to tbeta website
    ofSetColor(79, 79, 79);
    ofFill();
    ofRect(721, 586, 228, 14);
    ofSetColor(0xFFFFFF);
    ofDrawBitmapString("|  ~  |ccv.nuigroup.com", 725, 596);

    /************************
    Hand-Tracking strings
    ***************************/


    info.drawString("GOOGLE SUMMER OF CODE 2009", 725, 470);
    ofSetColor(0xFFFFFF);
    bigTitle.drawString("CCV HAND TRACKING", 725, 490);

    info.drawString("Student: Thiago de Freitas Oliveira Araujo", 725, 500);
    info.drawString("Mentor: Laurence Muller", 725, 520);


    info.drawString("Follow the development at:", 725, 540);
    ofSetColor(0xFFFFFF);
    info.drawString("thiagodefreitas.wordpress.com", 740, 550);

    sourceImg.draw(160, 610, 160, 120);
    info.drawString("Template Matching Tracking", 170, 745);

    camShiftImage.draw(340, 610, 160, 120);
    info.drawString("Camshift Tracking", 370, 745);

    blobsCheck.draw(520, 610, 160, 120);
    info.drawString("Kalman Tracking/Gaussian Blobs", 530, 745);

    aamImage.draw(700, 610, 160, 120);
    info.drawString("AAM-Fitting", 755, 745);


    /***********************************
    Drawing Malik Fingers
    ****************************/
    printf("Showing fingers\n");
    for (int j=0; j < 5;j++)
    {
        if(hands[j].nFingers > 0)
        {
        for (int i = 0; i < hands[j].nFingers; i++)
        {
            ofCircle((hands[j].fingerPos[i].x*MAIN_WINDOW_WIDTH)/camWidth + 40,(hands[j].fingerPos[i].y*MAIN_WINDOW_HEIGHT)/camHeight + 30, 5);
        }
    }
    }
    printf("Showed fingers\n");


    /*******************************************
    End Malik Draw

    **********************************************/
    /*****************************
    End of Hand-Tracking draw strings
    ******************************/

//Display Application information in bottom right
    string str = "Calc. Time [ms]:\n     ";
    str+= ofToString(differenceTime, 0)+"\n\n";

    if (bcamera)
    {
        string str2 = "Camera [Res]:\n     ";
        str2+= ofToString(camWidth, 0) + " x " + ofToString(camWidth, 0)  + "\n\n";
        string str4 = "Camera [fps]:\n     ";
        str4+= ofToString(fps, 0)+"\n\n";
        ofSetColor(0xFFFFFF);
        info.drawString(str + str2 + str4, 860, 630);
    }
    else
    {
        string str2 = "Video [Res]:\n     ";
        str2+= ofToString(vidPlayer->width, 0) + " x " + ofToString(vidPlayer->height, 0)  + "\n\n";
        string str4 = "Video [fps]:\n     ";
        str4+= ofToString(fps, 0)+"\n\n";
        ofSetColor(0xFFFFFF);
        info.drawString(str + str2 + str4, 860, 630);
    }

    if (bTUIOMode)
    {
        //Draw Port and IP to screen
        ofSetColor(0xffffff);
        char buf[256];
        if (myTUIO.bOSCMode)
            sprintf(buf, "Sending OSC messages to:\nHost: %s\nPort: %i", myTUIO.localHost, myTUIO.TUIOPort);
        else
        {
            if (myTUIO.bIsConnected)
                sprintf(buf, "Sending TCP messages to:\nPort: %i", myTUIO.TUIOFlashPort);
            else
                sprintf(buf, "Could not bind or send TCP to:\nPort: %i", myTUIO.TUIOFlashPort);
        }

        info.drawString(buf, 740, 420);
    }

    ofSetColor(0xFF0000);
    verdana.drawString("Press spacebar to toggle fast mode", 730, 578);
}

void ofxNCoreVision::drawMiniMode()
{
    //black background
    ofSetColor(0,0,0);
    ofRect(0,0,ofGetWidth(), ofGetHeight());
    //draw outlines
    if (bDrawOutlines)
    {
        for (int i=0; i<contourFinder.nBlobs; i++)
        {
            contourFinder.blobs[i].drawContours(0,0, camWidth, camHeight+175, ofGetWidth(), ofGetHeight());
        }
    }

    //draw grey rectagles for text information
    ofSetColor(128,128,128);
    ofFill();
    ofRect(0,ofGetHeight() - 83, ofGetWidth(), 20);
    ofRect(0,ofGetHeight() - 62, ofGetWidth(), 20);
    ofRect(0,ofGetHeight() - 41, ofGetWidth(), 20);
    ofRect(0,ofGetHeight() - 20, ofGetWidth(), 20);

    //draw text
    ofSetColor(250,250,250);
    verdana.drawString("Calc. Time  [ms]:        " + ofToString(differenceTime,0),10, ofGetHeight() - 70 );
    if (bcamera)
    {
        verdana.drawString("Camera [fps]:            " + ofToString(fps,0),10, ofGetHeight() - 50 );
    }
    else
    {
        verdana.drawString("Video [fps]:              " + ofToString(fps,0),10, ofGetHeight() - 50 );
    }
    verdana.drawString("Blob Count:               " + ofToString(contourFinder.nBlobs,0),10, ofGetHeight() - 29 );
    verdana.drawString("Sending TUIO:  " ,10, ofGetHeight() - 9 );

    //draw green tuio circle
    if ((myTUIO.bIsConnected || myTUIO.bOSCMode) && bTUIOMode)//green = connected
        ofSetColor(0x00FF00);
    else
        ofSetColor(0xFF0000); //red = not connected
    ofFill();
    ofCircle(ofGetWidth() - 17 , ofGetHeight() - 10, 5);
    ofNoFill();
}

void ofxNCoreVision::drawFingerOutlines()
{
    //Find the blobs for drawing
    for (int i=0; i<contourFinder.nBlobs; i++)

    {
//	    char debugNumber[1024];
//	    sprintf(debugNumber, "quant: %i", contourFinder.nBlobs);
//        verdana.drawString(debugNumber, 365, contourFinder.blobs[i].boundingRect.height/2 + 185);

        if (bDrawOutlines)
        {
            /*
             if( contourFinder.blobs[i].area > 1000)
             {*/
            //Draw contours (outlines) on the source image
            contourFinder.blobs[i].drawContours(40, 30, camWidth, camHeight, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
            /*}*/
        }
        if (bShowLabels) //Show ID label;
        {
            /*
            if( contourFinder.blobs[i].area > 1000)
             {*/
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
            verdana.drawString(idStr, xpos + 365, ypos + contourFinder.blobs[i].boundingRect.height/2 + 45);

//			cvEllipseBox(sourceImg, contourFinder.track_box, CV_RGB(255,0,0), 3, CV_AA, 0);
            /*}*/
            if (contourFinder.blobs[i].xHand > 0)
            {
                ofSetColor(255,255,255);
                printf("%d , %d\n", contourFinder.blobs[i].xHand, contourFinder.blobs[i].yHand);
                ofCircle((contourFinder.blobs[i].xHand*160)/camWidth + 160, (contourFinder.blobs[i].yHand*120)/camHeight + 610, 10);
                contourFinder.blobs[i].xHand = 0;
                contourFinder.blobs[i].yHand = 0;
                printf("Entered Hand Drawing Function\n");
            }

        }
    }
    ofSetColor(0xFFFFFF);
}

/*****************************************************************************
* KEY EVENTS
*****************************************************************************/
void ofxNCoreVision::_keyPressed(ofKeyEventArgs &e)
{
    // detect escape key
    if (e.key==0x1b)
    {
        exited=true;
    }

    if (showConfiguration)
    {
        switch (e.key)
        {
        case 'a':
            filter->threshold++;
            controls->update(appPtr->trackedPanel_threshold, kofxGui_Set_Int, &appPtr->filter->threshold, sizeof(int));
            break;
        case 'z':
            filter->threshold--;
            controls->update(appPtr->trackedPanel_threshold, kofxGui_Set_Int, &appPtr->filter->threshold, sizeof(int));
            break;
        case 'b':
            filter->bLearnBakground = true;
            break;
        case 'o':
            bDrawOutlines ? bDrawOutlines = false : bDrawOutlines = true;
            controls->update(appPtr->trackedPanel_outlines, kofxGui_Set_Bool, &appPtr->bDrawOutlines, sizeof(bool));
            break;
        case 'h':
            filter->bHorizontalMirror ? filter->bHorizontalMirror = false : filter->bHorizontalMirror = true;
            controls->update(appPtr->propertiesPanel_flipH, kofxGui_Set_Bool, &appPtr->filter->bHorizontalMirror, sizeof(bool));
            break;
        case 'j':
            filter->bVerticalMirror ? filter->bVerticalMirror = false : filter->bVerticalMirror = true;
            controls->update(appPtr->propertiesPanel_flipV, kofxGui_Set_Bool, &appPtr->filter->bVerticalMirror, sizeof(bool));
            break;
        case 't':
            myTUIO.bOSCMode = !myTUIO.bOSCMode;
            myTUIO.bTCPMode = false;
            bTUIOMode = myTUIO.bOSCMode;
            controls->update(appPtr->optionPanel_tuio_tcp, kofxGui_Set_Bool, &appPtr->myTUIO.bTCPMode, sizeof(bool));
            controls->update(appPtr->optionPanel_tuio_osc, kofxGui_Set_Bool, &appPtr->myTUIO.bOSCMode, sizeof(bool));
            //clear blobs
//			myTUIO.blobs.clear();
            break;
        case 'f':
            myTUIO.bOSCMode = false;
            myTUIO.bTCPMode = !myTUIO.bTCPMode;
            bTUIOMode = myTUIO.bTCPMode;
            controls->update(appPtr->optionPanel_tuio_tcp, kofxGui_Set_Bool, &appPtr->myTUIO.bTCPMode, sizeof(bool));
            controls->update(appPtr->optionPanel_tuio_osc, kofxGui_Set_Bool, &appPtr->myTUIO.bOSCMode, sizeof(bool));
            //clear blobs
//			myTUIO.blobs.clear();
            break;
        case 'g':
            bGPUMode ? bGPUMode = false : bGPUMode = true;
            controls->update(appPtr->gpuPanel_use, kofxGui_Set_Bool, &appPtr->bGPUMode, sizeof(bool));
            filter->bLearnBakground = true;
            break;
        case 'v':
            if (bcamera && vidGrabber != NULL)
                vidGrabber->videoSettings();
            break;
        case 'l':
            bShowLabels ? bShowLabels = false : bShowLabels = true;
            controls->update(appPtr->trackedPanel_ids, kofxGui_Set_Bool, &appPtr->bShowLabels, sizeof(bool));
            break;
        case 'p':
            bShowPressure ? bShowPressure = false : bShowPressure = true;
            break;
        case ' ':
            if (bMiniMode && !bCalibration) // NEED TO ADD HERE ONLY GO MINI MODE IF NOT CALIBRATING
            {
                bMiniMode = false;
                bShowInterface = true;
                filter->bMiniMode = bMiniMode;
                ofSetWindowShape(950,600); //default size
            }
            else if (!bCalibration)
            {
                bMiniMode = true;
                bShowInterface = false;
                filter->bMiniMode = bMiniMode;
                ofSetWindowShape(190,200); //minimized size
            }
            break;
        case 'x': //Exit Calibrating
            if (bCalibration)
            {
                bShowInterface = true;
                bCalibration = false;
                calib.calibrating = false;
                tracker.isCalibrating = false;
                if (bFullscreen == true) ofToggleFullscreen();
                bFullscreen = false;
            }
            break;
        }
    }
}

void ofxNCoreVision::_keyReleased(ofKeyEventArgs &e)
{
    if (showConfiguration)
    {
        if ( e.key == 'c' && !bCalibration)
        {
            bShowInterface = false;
            //Enter/Exit Calibration
            bCalibration = true;
            calib.calibrating = true;
            tracker.isCalibrating = true;
            if (bFullscreen == false) ofToggleFullscreen();
            bFullscreen = true;
        }
    }
    if ( e.key == '~' || e.key == '`' && !bMiniMode && !bCalibration) showConfiguration = !showConfiguration;
}

/*****************************************************************************
*	MOUSE EVENTS
*****************************************************************************/
void ofxNCoreVision::_mouseDragged(ofMouseEventArgs &e)
{
    if (showConfiguration)
        controls->mouseDragged(e.x, e.y, e.button); //guilistener
}

void ofxNCoreVision::_mousePressed(ofMouseEventArgs &e)
{
    if (showConfiguration)
    {
        controls->mousePressed(e.x, e.y, e.button); //guilistener
        if (e.x > 722 && e.y > 586)
        {
            ofLaunchBrowser("http://ccv.nuigroup.com");
        }
        if (e.x > 720 && e.y > 560)
        {
            ofLaunchBrowser("http://thiagodefreitas.wordpress.com");
        }
    }
}

void ofxNCoreVision::_mouseReleased(ofMouseEventArgs &e)
{
    if (showConfiguration)
        controls->mouseReleased(e.x, e.y, 0); //guilistener
}

/*****************************************************************************
* Getters
*****************************************************************************/

std::map<int, Blob> ofxNCoreVision::getBlobs()
{

    return tracker.getTrackedBlobs();
}

/****************************************************************************
Hand ROI adjust for Camshift
*****************************************************************************/
void ofxNCoreVision::HandROIAdjust(int x, int y, IplImage* image)
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

void ofxNCoreVision::learnBackGround(ofxCvColorImage& img)
{
    //Capture full background
    if (bLearnBackground2 == true)
    {
        filter->BgImgColor = img;
        filter->BgImgColor.flagImageChanged();
//        bLearnBackground2 = false;
    }
}

/*****************************************************************************
* ON EXIT
*****************************************************************************/
void ofxNCoreVision::_exit(ofEventArgs &e)
{
    //save/update log file
    if ((stream = freopen(fileName, "a", stdout)) == NULL) {}

    saveSettings();

#ifdef TARGET_WIN32
    if (PS3!=NULL) delete PS3;
    if (ffmv!=NULL) delete ffmv;
    if (dsvl!=NULL) delete dsvl;
#endif

    if (vidGrabber!=NULL) delete vidGrabber;
    if (vidPlayer !=NULL) delete vidPlayer;
    // -------------------------------- SAVE STATE ON EXIT

    printf("Vision module has exited!\n");
}

