#include "extractBlob.h"



coord extractBlob(CBlobResult blobs, coord selectedCoord){

	coord coordinate;
	CBlob Blob;

	if ( blobs.GetNumBlobs()==0 ) {
		coordinate.flag=false;
		return coordinate;
	}
	else {

		Blob = getNearestBlob( blobs, selectedCoord);

		coordinate.set( (int) Blob.MaxX(), (int) Blob.MinX(), (int) Blob.MaxY(), (int) Blob.MinY());

		return coordinate;
	}

}

void drawInitialBlobs(IplImage * tmp_frame, CBlobResult blobs){

	coord drawCoord;

	for (int i=0; i<blobs.GetNumBlobs();i++){

		drawCoord.set( (int) blobs.GetBlob(i).MaxX(), (int) blobs.GetBlob(i).MinX(), (int) blobs.GetBlob(i).MaxY(), (int) blobs.GetBlob(i).MinY());

		drawBlob2(tmp_frame, drawCoord, 255, 255, 255);
	}
}

CBlob getNearestBlob(CBlobResult blobs, coord coordinate){

	int tot = blobs.GetNumBlobs();
	CBlob Blob;
	float distance[10];
	float minimum;

	coord tempCoord;

	for (int i=0; i<tot; i++){
		Blob = blobs.GetBlob(i);
		tempCoord.set( (int) Blob.MaxX(), (int) Blob.MinX(), (int) Blob.MaxY(), (int) Blob.MinY());
		distance[i] = sqrt((double)(tempCoord.cX - coordinate.cX)*(tempCoord.cX - coordinate.cX) + (tempCoord.cY - coordinate.cY)*(tempCoord.cY - coordinate.cY));
	}

	int minDistanceId=0;


	for (int j=0; j<tot; j++){
		minimum = min( distance[j], distance[minDistanceId]);
		if ( distance[j] == minimum ) minDistanceId = j;
		}


	Blob = blobs.GetBlob( minDistanceId );

	return Blob;

}

CBlobResult getBlobs2(IplImage* tmp_frame, IplImage* binFore){

	CBlobResult blobs;


	blobs = CBlobResult( binFore, NULL, 200, true );



	blobs.Filter( blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, 40);


	blobs.Filter( blobs, B_INCLUDE, CBlobGetArea(), B_LESS, (tmp_frame->height)*(tmp_frame->width)*1.5);
	blobs.Filter( blobs, B_INCLUDE, CBlobGetPerimeter(), B_LESS, (tmp_frame->height)+(tmp_frame->width)*2*1.5);


	return blobs;
}
