/*
 * Author: Thiago de Freitas Oliveira Araújo
 * Nuigroup Summer of Code 2009
 */

#include <stdio.h>
#include <cv.h>
#include <highgui.h>


void sum_rgb( IplImage* src, IplImage* dst ) {
  IplImage* r = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
  IplImage* g = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
  IplImage* b = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );


  IplImage* s = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
      
 
  cvSplit( src, r, g, b, NULL );
     

	cvZero(s);
	cvAcc(b,s);
	cvAcc(g,s);
	cvAcc(r,s);
	
	
	cvThreshold( s, s, 100, 100, CV_THRESH_TRUNC );
	cvConvertScale( s, dst, 1, 0 );
 
  cvReleaseImage( &r );
  cvReleaseImage( &g );   
  cvReleaseImage( &b );   
  cvReleaseImage( &s );
}

int main(int argc, char** argv)
{
  // Create a named window with a the name of the file.
 // cvNamedWindow( argv[1], 1 );
  //cvNamedWindow("Original", 1);
  //cvNamedWindow("X");
  //cvNamedWindow("Y");
  
  // Load the image from the given file name.
  IplImage* img = cvLoadImage( argv[1] );
  IplImage* src = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 1);
  cvCvtColor(img, src, CV_BGR2GRAY);
//  cvDilate(src,src); not necessary for tests with CvHoughLines for fingers
  //cvThreshold(src, src, 70,255, CV_THRESH_BINARY);

  printf ("\nDepth of the image %i,%i\n", src->depth, src->nChannels);
  IplImage* loucs = src;
  IplImage* dst = cvCreateImage( cvSize(loucs->width/2,loucs->height/2), loucs->depth, 1);
  printf("\nDepth of the created image %i, %i\n", loucs->depth, loucs->nChannels);
  //sum_rgb( src, dst);
  cvSmooth(src, loucs, CV_BILATERAL);
  cvPyrDown(loucs, dst);
  // Show the image in the named window
  //cvDilate(pyrdownagain, pyrdownagain);
  cvThreshold(dst,dst,60,255, CV_THRESH_BINARY);
  IplImage* firstX = cvCreateImage(cvSize(loucs->width, loucs->height), loucs->depth,1);
  IplImage* firstY = cvCreateImage(cvSize(loucs->width, loucs->height), loucs->depth,1);
  
//  cvThreshold(loucs, loucs, 60,255, CV_THRESH_BINARY);  not necessary for tests with fingers
  cvSobel(loucs, firstX, 0,1);
  cvSobel(loucs,firstY, 0,2);
  //cvDilate(firstX, firstX);
  //cvDilate(firstY,firstY);
  //cvShowImage( argv[1], src );
  //cvShowImage("Original", loucs);
  //cvShowImage("X", firstX);
  //cvShowImage("Y", firstY);
  cvSaveImage(argv[1], firstX);
  // Idle until the user hits the "Esc" key.
  //while( 1 ) { if( (cvWaitKey( 10 )&0x7f) == 27 ) break; }

  //cvDestroyWindow( argv[1] ); 
  //cvDestroyWindow("Original");
  //cvDestroyWindow("X");
  //cvDestroyWindow("Y");


  cvReleaseImage( &src );
  cvReleaseImage( &dst );
  cvReleaseImage(&loucs);
  cvReleaseImage(&firstX);
  cvReleaseImage(&firstY);
}
