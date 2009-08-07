#ifndef  _GETBACKGROUND_H
#define  _GETBACKGROUND_H

#include "useful.h"

IplImage* updateBackground(CvBGStatModel * bg_model, IplImage * tmp_frame);

void initBackgroundModel(CvBGStatModel ** , IplImage* , CvGaussBGStatModelParams* );

IplImage * getBinaryImage(IplImage * image);

#endif
