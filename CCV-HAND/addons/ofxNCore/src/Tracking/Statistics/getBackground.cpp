#include "getBackground.h"

void initBackgroundModel(CvBGStatModel ** bgmodel, IplImage* tmp_frame, CvGaussBGStatModelParams* paramMoG){

	paramMoG->win_size = 200;
	paramMoG->n_gauss = 5;
	paramMoG->bg_threshold = 0.9;
	paramMoG->std_threshold = 2.5;
	paramMoG->minArea = 600.f;
	paramMoG->weight_init = 0.07;
	paramMoG->variance_init = 50;
	*bgmodel = cvCreateGaussianBGModel(tmp_frame, paramMoG);

}

IplImage* updateBackground(CvBGStatModel *bg_model, IplImage * tmp_frame){

	cvUpdateBGStatModel(tmp_frame, bg_model);

	return bg_model->foreground;
}

IplImage * getBinaryImage(IplImage * image){


	IplImage* img = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
	cvCvtColor(image, img, CV_RGB2GRAY);
	IplImage* binImg = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
	cvThreshold(img,binImg,100,255,CV_THRESH_BINARY);
	cvReleaseImage(&img);
	return binImg;

}

