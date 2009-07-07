//
// Example 8-3. Finding and drawing contours on an input image
//
//

// Modified for testing contours and the colorspace simultaneously
//Thiago - GSOC 2009
/* *************** License:**************************
   Oct. 3, 2008
   Right to use this code in any way you want without warrenty, support or any guarentee of it working.

   BOOK: It would be nice if you cited it:
   Learning OpenCV: Computer Vision with the OpenCV Library
     by Gary Bradski and Adrian Kaehler
     Published by O'Reilly Media, October 3, 2008

   AVAILABLE AT:
     http://www.amazon.com/Learning-OpenCV-Computer-Vision-Library/dp/0596516134
     Or: http://oreilly.com/catalog/9780596516130/
     ISBN-10: 0596516134 or: ISBN-13: 978-0596516130

   OTHER OPENCV SITES:
   * The source code is on sourceforge at:
     http://sourceforge.net/projects/opencvlibrary/
   * The OpenCV wiki page (As of Oct 1, 2008 this is down for changing over servers, but should come back):
     http://opencvlibrary.sourceforge.net/
   * An active user group is at:
     http://tech.groups.yahoo.com/group/OpenCV/
   * The minutes of weekly OpenCV development meetings are at:
     http://pr.willowgarage.com/wiki/OpenCV
   ************************************************** *///
//


#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <iostream>

//Some defines we left out of the book
#define CVX_RED		CV_RGB(0xff,0x00,0x00)
#define CVX_GREEN	CV_RGB(0x00,0xff,0x00)
#define CVX_BLUE	CV_RGB(0x00,0x00,0xff)

using namespace std;

//  Example 8-3. Finding and drawing contours on an input image
int main(int argc, char* argv[]) {

  cvNamedWindow( argv[0], 1 );
  cvNamedWindow("teste",1);
  cvNamedWindow("Subtraction",1);

  IplImage* img_8uc1 = NULL;
  IplImage* img_testebgu = NULL;
  cout << "MessagePAssed: " << argv[1] << ", " << argv[2] << endl;

  //Changed this a little for safer image loading and help if not
  if( argc != 3  || !(img_8uc1 = cvLoadImage( argv[1], CV_LOAD_IMAGE_GRAYSCALE )) || !(img_testebgu = cvLoadImage( argv[2], CV_LOAD_IMAGE_GRAYSCALE )) ){
  printf("\nExample 8_3 Drawing Contours\nCall is:\n./ch8_ex8_3 image\n\n");
  return -1;}

  IplImage* img_mine = cvCreateImage( cvGetSize(img_8uc1), 8, 1 );
  IplImage* img_edge = cvCreateImage( cvGetSize(img_8uc1), 8, 1 );
  IplImage* img_8uc3 = cvCreateImage( cvGetSize(img_8uc1), 8, 3 );
  IplImage* imagem_testebg = cvCreateImage(cvGetSize(img_8uc1), 8, 1);
  CvMemStorage* storage = cvCreateMemStorage();
  CvSeq* first_contour = NULL;
  cvSub(img_8uc1, img_testebgu, imagem_testebg);
  cvSmooth(imagem_testebg, imagem_testebg, CV_MEDIAN, 3, 3,0,0);
  cvSmooth(imagem_testebg, img_mine, CV_GAUSSIAN, 3, 3,1);
  cvThreshold( img_mine, img_edge, 40, 200, CV_THRESH_BINARY );
  cvShowImage("teste", img_edge);
  cvShowImage("Subtraction", imagem_testebg);

  int Nc = cvFindContours(
     img_edge,
     storage,
     &first_contour,
     sizeof(CvContour),
     CV_RETR_LIST // Try all four values and see what happens
  );
  int n=0,k;
  printf("\n\nHit any key to draw the next contour, ESC to quit\n\n");
  printf( "Total Contours Detected: %d\n", Nc );
  for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) {
     cvCvtColor( imagem_testebg, img_8uc3, CV_GRAY2BGR );
     cvDrawContours(
        img_8uc3,
        c,
        CVX_RED,  //Yarg, these are defined above, but not in the book.  Oops
        CVX_BLUE,
        0,        // Try different values of max_level, and see what happens
        2,
        8
     );
     printf("Contour #%d\n", n );
     cvShowImage( argv[0], img_8uc3 );
     printf(" %d elements:\n", c->total );
     for( int i=0; i<c->total; ++i ) {
     CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
        printf("    (%d,%d)\n", p->x, p->y );
     }
     if((k = cvWaitKey()&0x7F) == 27)
       break;
     n++;
  }
  printf("Finished all contours. Hit key to finish\n");
  cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );
  cvShowImage( argv[0], img_8uc3 );
  cvWaitKey(0);
  cvDestroyWindow( argv[0] );
  cvDestroyWindow("teste");
  cvDestroyWindow("Subtraction");
  cvReleaseImage( &img_8uc1 );
  cvReleaseImage( &img_8uc3 );
  cvReleaseImage( &img_edge );
  cvReleaseImage(&img_mine);
  cvReleaseImage(&imagem_testebg);
  cvReleaseImage(&img_testebgu);

  return 0;
}
