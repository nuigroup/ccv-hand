/*
 * Author: Thiago de Freitas Oliveira Araújo
 * Nuigroup Summer of Code 2009
 */

#include <highgui.h>
#include <cv.h>
#include <stdio.h>
#include <iostream>

using namespace std;

int main( int argc, char** argv )
{

  char nome[50];
  IplImage* img = cvLoadImage( argv[1] );
  IplImage* src = cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U, 1);
  cvCvtColor(img, src,CV_BGR2GRAY);
  cvDilate(src,src);
  sprintf(nome,"%s",argv[1]);
  cout << nome << endl;

  cvSmooth(img, img, CV_BILATERAL);
  IplImage* dst = cvCreateImage(cvSize(src->width/2,src->height/2),src->depth, 1);
  cvPyrDown(src, dst);
  IplImage* dst2 = cvCreateImage(cvSize(dst->width/2,dst->height/2),dst->depth, 1);
  cvPyrDown(dst, dst2);

  cvSaveImage(nome, dst2);

  //cvWaitKey(0);
  cvReleaseImage( &img );
  cvReleaseImage(&src);
  cvReleaseImage(&dst);

  cvDestroyWindow("Bilateral");
}
