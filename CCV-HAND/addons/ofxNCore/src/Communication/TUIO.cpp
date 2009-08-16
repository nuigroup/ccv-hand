/*
*  TUIO.h
*
*
*  Created on 2/2/09.
*  Copyright 2009 NUI Group. All rights reserved.
*
** Modified by: Thiago de Freitas Oliveira Araújo
*  NUI Group Community Core Vision Hand Tracking
*  Google Summer of Code 2009
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

#include "TUIO.h"

TUIO::TUIO() {
}

TUIO::~TUIO() {

	// this could be useful for whenever we get rid of an object
}

void TUIO::setup(const char* host, int port, int flashport) {

	localHost = host;
	TUIOPort = port;
	TUIOFlashPort = flashport;
	frameseq = 0;

	//FOR TCP
	bIsConnected = m_tcpServer.setup(TUIOFlashPort);
	//FOR OSC
	TUIOSocket.setup(localHost, TUIOPort);
}

void TUIO::sendTUIO(std::map<int, Blob> * blobs)
{
	frameseq += 1;

	//if sending OSC (not TCP)
	if(bOSCMode){

		ofxOscBundle b;

		if(blobs->size() == 0)
		{
			//Sends alive message - saying 'Hey, there's no alive blobs'
			ofxOscMessage alive;
			alive.setAddress("/tuio/2Dcur");
			alive.addStringArg("alive");

			//Send fseq message
			ofxOscMessage fseq;
			fseq.setAddress( "/tuio/2Dcur" );
			fseq.addStringArg( "fseq" );
			fseq.addIntArg(frameseq);

			b.addMessage( alive ); //add message to bundle
			b.addMessage( fseq ); //add message to bundle
			TUIOSocket.sendBundle( b ); //send bundle
		}
		else //actually send the blobs
		{
			map<int, Blob>::iterator this_blob;
			for(this_blob = blobs->begin(); this_blob != blobs->end(); this_blob++)
			{
				//Set Message
				ofxOscMessage set;
				set.setAddress("/tuio/2Dcur");
				set.addStringArg("set");
				set.addIntArg(this_blob->second.id); //id
				set.addFloatArg(this_blob->second.centroid.x);  // x
				set.addFloatArg(this_blob->second.centroid.y); // y
				set.addFloatArg(this_blob->second.D.x); //dX
				set.addFloatArg(this_blob->second.D.y); //dY
				set.addFloatArg(this_blob->second.maccel); //m
				if(bHandInfo)
				{
				set.addFloatArg(this_blob->second.handID);
				set.addFloatArg(this_blob->second.xHand);
				set.addFloatArg(this_blob->second.yHand);
				}
				if(bHeightWidth){
					set.addFloatArg(this_blob->second.boundingRect.width); // wd
					set.addFloatArg(this_blob->second.boundingRect.height);// ht
				}
				b.addMessage( set ); //add message to bundle
			}

			//Send alive message of all alive IDs
			ofxOscMessage alive;
			alive.setAddress("/tuio/2Dcur");
			alive.addStringArg("alive");

			std::map<int, Blob>::iterator this_blobID;
			for(this_blobID = blobs->begin(); this_blobID != blobs->end(); this_blobID++)
			{
				alive.addIntArg(this_blobID->second.id); //Get list of ALL active IDs
			}

			//Send fseq message
			ofxOscMessage fseq;
			fseq.setAddress( "/tuio/2Dcur" );
			fseq.addStringArg( "fseq" );
			fseq.addIntArg(frameseq);

			b.addMessage( alive ); //add message to bundle
			b.addMessage( fseq ); //add message to bundle

			TUIOSocket.sendBundle( b ); //send bundle
		}

	}else if(bTCPMode) //else, if TCP (flash) mode
	{
		if(blobs->size() == 0){

			m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\""+ofToString(TUIOPort)+"\" TIME=\""+ofToString(ofGetElapsedTimef())+"\">" +
							 "<MESSAGE NAME=\"/tuio/2Dcur\">"+
							 "<ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>"+
							 "</MESSAGE>"+
							 "<MESSAGE NAME=\"/tuio/2Dcur\">"+
							 "<ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/>"+
							 "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(frameseq)+"\"/>" +
							 "</MESSAGE>"+
							 "</OSCPACKET>");
		}
		else
		{
			string setBlobsMsg;

			map<int, Blob>::iterator this_blob;
			for(this_blob = blobs->begin(); this_blob != blobs->end(); this_blob++)
			{
				//if sending height and width
				if(bHandInfo){
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blob->second.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.maccel)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.boundingRect.width)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.boundingRect.height)+"\"/>"+
					// To handle hand info
					//HAND ID
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.handID)+"\"/>"+
					//xHand
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.xHand )+"\"/>"+
					//yHand
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.yHand)+"\"/>"+
					"</MESSAGE>";
				}
				if(bHeightWidth){
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blob->second.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.maccel)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.boundingRect.width)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.boundingRect.height)+"\"/>"+
					"</MESSAGE>";
				}
				else{
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blob->second.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.maccel)+"\"/>"+
					"</MESSAGE>";
				}
			}

			string aliveBeginMsg = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>";

			string aliveBlobsMsg;
			std::map<int, Blob>::iterator this_blobID;
			for(this_blobID = blobs->begin(); this_blobID != blobs->end(); this_blobID++)
			{
				aliveBlobsMsg += "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blobID->second.id)+"\"/>";
			}

			string aliveEndMsg = "</MESSAGE>";

			string fseq = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(frameseq)+"\"/></MESSAGE>";

			m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\""+ofToString(TUIOPort)+"\" TIME=\""+ofToString(ofGetElapsedTimef())+"\">" +
									setBlobsMsg + aliveBeginMsg + aliveBlobsMsg + aliveEndMsg + fseq + "</OSCPACKET>");
		}
	}
}
