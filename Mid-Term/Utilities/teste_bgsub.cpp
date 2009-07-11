/*
 * Author: Thiago de Freitas Oliveira Araújo
 * Nuigroup Summer of Code 2009
 * Background subtract for training model with Falcon's video
 */

#include <highgui.h>
#include <cv.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	IplImage* orig = NULL;
	IplImage* img_tmp = NULL;

	cvNamedWindow("Original",1);
	cvNamedWindow("Other",1);
	cvNamedWindow("Subtracted",1);

	orig = cvLoadImage("t1_0000.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	cout << "MessagePAssed: " << argv[1] << ", " << argv[2] << endl;

	IplImage* sub = cvCreateImage( cvGetSize(orig), 8, 1 );


	img_tmp = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);

	cvSmooth(orig, orig, CV_MEDIAN, 3, 3,0,0);
	cvSmooth(orig, orig, CV_GAUSSIAN, 3, 3,0,0);

	cvAbsDiff(orig, img_tmp, sub);

	cvShowImage("Original", orig);
	cvShowImage("Other", img_tmp);
	cvShowImage("Subtracted", sub);


	cvSaveImage(argv[1], sub);

	cvReleaseImage(&orig);
	cvReleaseImage(&sub);
	cvReleaseImage(&img_tmp);

	cvDestroyAllWindows();
}
