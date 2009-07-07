#include "testApp.h"
#include <cmath>
#include <cstring>
#include <iostream>

using namespace std;


//--------------------------------------------------------------
void testApp::setup(){	 
	ofBackground(0,0,0);
	ofSetVerticalSync(true);	
	
	angle = 0.0;
	xcool = 0.0;
	blobito = false;
}

//--------------------------------------------------------------
void testApp::update(){
	
	angle += 0.5;
	xcool += 0.8;
}

//--------------------------------------------------------------
void testApp::draw()
{
	
	float distance;
	ofSetCircleResolution(44);
	ofEnableAlphaBlending();
	//Hand Palm Centroid
	ofSetWindowTitle("CCV PSEUDO HAND TRACKING");
	ofSetColor(0xFFFFFF);
	ofEllipse(400, 300, 200, 180);
	ofSetColor(0xFF6559);
	ofLine(400,290, 400,310);
	ofLine(390,300, 410,300);

	ofDrawBitmapString("Hand 1", 420,320);
	
	//Fingertips
	glPushMatrix();

           // glTranslatef(xcool,4,2);
			ofSetColor(0xFFFFFF);
			ofCircle(190,120,12);
			//glRotatef(angle, 0, 0, 1);
	glPopMatrix();

	ofSetColor(0xFF6559);
	ofLine(190,110, 190,130);
	ofLine(180,120, 200,120);
	ofLine(190,120, 400,300);
	ofDrawBitmapString("Id 1", 210,140);
	float x = 400-190;
	float y = 300-120;
	distance = sqrt(pow(x,2)+pow(y,2));
	string dist = "D = "+ ofToString(distance, 2);
	ofDrawBitmapString(dist, 250,180);
	ofDrawBitmapString("Id 10", 950,510);
	
	ofSetColor(0xFFFFFF);
	ofCircle(320,50,12);
	ofSetColor(0xFF6559);
	ofLine(320,40, 320,60);
	ofLine(310,50, 330,50);
	ofLine(320,50, 400,300);
	ofDrawBitmapString("Id 2", 340,70);
	
	ofSetColor(0xFFFFFF);
	ofCircle(420,30,12);
	ofSetColor(0xFF6559);
	ofLine(420,20, 420,40);
	ofLine(410,30, 430,30);
	ofLine(420,30, 400,300);
	ofDrawBitmapString("Id 3", 440,50);
	
	ofSetColor(0xFFFFFF);
	ofCircle(510,75,12);
	ofSetColor(0xFF6559);
	ofLine(510,65, 510,85);
	ofLine(500,75, 520,75);
	ofLine(510,75, 400,300);
	ofDrawBitmapString("Id 4", 530,95);
	
	ofSetColor(0xFFFFFF);
	ofCircle(570,300,12);
	ofSetColor(0xFF6559);
	ofLine(570,290, 570,310);
	ofLine(560,300, 580,300);
	ofLine(570,300, 400,300);
	ofDrawBitmapString("Id 5", 590,320);

	//Hand Palm Centroid
	ofSetColor(0xFFFFFF);
	ofEllipse(800, 600, 200, 180);
	ofSetColor(0xFF6559);
	ofLine(800,590, 800,610);
	ofLine(790,600, 810,600);
	ofDrawBitmapString("Hand 2", 820,620);

	ofSetColor(0xFFFFFF);
	ofCircle(530,600,12);
	ofSetColor(0xFF6559);
	ofLine(530,590, 530,610);
	ofLine(520,600, 540,600);
	ofLine(530,600, 800,600);
	ofDrawBitmapString("Id 6", 550,620);

	ofSetColor(0xFFFFFF);
	ofCircle(650,420,12);
	ofSetColor(0xFF6559);
	ofLine(650,410, 650,430);
	ofLine(640,420, 660,420);
	ofLine(650,420, 800,600);
	ofDrawBitmapString("Id 7", 670,430);

	ofSetColor(0xFFFFFF);
	ofCircle(740,380,12);
	ofSetColor(0xFF6559);
	ofLine(740,370, 740,390);
	ofLine(730,380, 750,380);
	ofLine(740,380, 800,600);
	ofDrawBitmapString("Id 8", 760,400);

	ofSetColor(0xFFFFFF);
	ofCircle(860,407,12);
	ofSetColor(0xFF6559);
	ofLine(860,397, 860,417);
	ofLine(850,407, 870,407);
	ofLine(860,407, 800,600);
	ofDrawBitmapString("Id 9", 880,427);

	ofSetColor(0xFFFFFF);
	ofCircle(930,490,12);
	ofSetColor(0xFF6559);
	ofLine(930,480, 930,500);
	ofLine(920,490, 940,490);
	ofLine(930,490, 800,600);
	ofDrawBitmapString("Id 10", 950,510);

	ofSetColor(0xffffff);
    ofDrawBitmapString( "1: Connecting Lines  2: Show blobs  3: Show distances\n\n",20,20);	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{ 
	 switch( key )
    {
        case '1': 
			blobito = !blobito;
			break;
        case '2': 
			break;
        case '3': 
			break;
        case 'p':
			break;
        case 'P':
            break;
	 }
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
	//so we don't update the control point anymore
}
