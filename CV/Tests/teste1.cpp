/*
 * Author: Thiago de Freitas Oliveira Araújo
 * Nuigroup Summer of Code 2009
 * Color Space Visualization
 */

#include <highgui.h>
#include <cv.h>

int main(int argc, char** argv)
{

	IplImage* img = cvLoadImage(argv[1]);
	IplImage* hls_im = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage* hsv_im = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage* ycc_im = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage* gray_im = cvCreateImage(cvGetSize(img), 8, 1);

	cvNamedWindow("HSV", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Normal", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("HLS", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("YCC", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("GRAY", CV_WINDOW_AUTOSIZE);

	cvCvtColor(img, hls_im, CV_RGB2HLS);
	cvCvtColor(img, hsv_im,CV_RGB2HSV);
	cvCvtColor(img, ycc_im, CV_RGB2YCrCb);
	cvCvtColor(img, gray_im, CV_RGB2GRAY);

	cvShowImage("HLS", hls_im);
	cvShowImage("HSV", hsv_im);
	cvShowImage("YCC", hsv_im);
	cvShowImage("GRAY", gray_im);
	cvShowImage("Normal", img);

	cvWaitKey(0);

	cvReleaseImage(&img);
	cvReleaseImage(&gray_im);
	cvReleaseImage(&hls_im);
	cvReleaseImage(&hsv_im);
	cvReleaseImage(&ycc_im);

	cvDestroyWindow("HLS");
	cvDestroyWindow("YCC");
	cvDestroyWindow("HSV");
	cvDestroyWindow("GRAY");
	cvDestroyWindow("Normal");
}
