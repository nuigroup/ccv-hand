#include "kalman.h"
#define NUMBER_OF_MATRIX 6

CvKalman* initKalman(CvMat** indexMat, coord initCoord){

	struct matrixDesc MDSC[NUMBER_OF_MATRIX];

	float ValuesVect[100];

	MDSC[0].nRows= 4;
	MDSC[1].nRows= 4;
	MDSC[2].nRows= 2;
	MDSC[3].nRows= 4;
	MDSC[4].nRows= 2;
	MDSC[5].nRows= 4;

	MDSC[0].nCols= 4;
	MDSC[1].nCols= 1;
	MDSC[2].nCols= 4;
	MDSC[3].nCols= 4;
	MDSC[4].nCols= 2;
	MDSC[5].nCols= 4;

	for (int i=0;i< NUMBER_OF_MATRIX ;i++)
	{
		indexMat[i] = cvCreateMat( MDSC[i].nRows, MDSC[i].nCols, CV_32FC1 );
	}

	int DP = indexMat[0]->cols;
	int MP = indexMat[2]->rows;
	int CP = indexMat[1]->cols;

	CvKalman* kalman = cvCreateKalman(DP,MP,CP);

	int h = 0;

	for (int i=0;i< NUMBER_OF_MATRIX;i++){
		for (int j=0;j<indexMat[i]->rows;j++){
			for (int l=0;l<indexMat[i]->cols;l++){
				indexMat[i]->data.fl[j*indexMat[i]->cols+l] = ValuesVect[h];
				h++;
			}
		}

	}

	float A[] = {1,0,1,0, 0,1,0,1, 0,0,1,0, 0,0,0,1};
	float Bu[] = { 0, 0, 0, 0};
	float H[] = {1,0,0,0, 0,1,0,0 };
	float Q[] = {0.001,0,0,0, 0,0.001,0,0, 0,0,0.001,0, 0,0,0,0.001};
	float R[] = {0.2845,0.0045, 0.0045,0.0455 };
	float P[] = { 1000,0,0,0, 0,1000,0,0, 0,0,1000,0, 0,0,0,1000};


	for( int i=0; i < DP*DP; i++ )
		kalman->transition_matrix->data.fl[i]= A[i];

	for( int i=0; i < DP*1; i++ )
		kalman->control_matrix->data.fl[i] = Bu[i];

	for( int i=0; i < DP*MP; i++)
		kalman->measurement_matrix->data.fl[i] = H[i];

	for( int i=0; i < DP*DP; i++)
		kalman->process_noise_cov->data.fl[i] = Q[i];

	for( int i=0; i < MP*MP; i++)
		kalman->measurement_noise_cov->data.fl[i] = R[i];

	for( int i=0; i < DP*DP; i++)
		kalman->error_cov_pre->data.fl[i] = P[i];

	float a[] = { initCoord.cX, initCoord.cY, 0, 0};

	CvMat Ma=cvMat(1, DP, CV_32FC1, a);
	copyMat(&Ma, kalman->state_post);
	return kalman;
}

void copyMat (CvMat* source, CvMat* dest){
	int i,j;
	for (i=0; i<source->rows; i++)
		for (j=0; j<source->cols;j++)
			dest->data.fl[i*source->cols+j]=source->data.fl[i*source->cols+j];

}

float* updateKalman(CvKalman * kalman, coord coord){

	int Meanx, Meany;
	CvMat* measurement = cvCreateMat(2,1, CV_32FC1 );
	Meanx = (int) coord.cX;
	Meany =  (int) coord.cY;
	cvmSet(measurement,0,0,Meanx);
	cvmSet(measurement,1,0,Meany);
	CvMat* u = cvCreateMat(1,1, CV_32FC1 );
	u->data.fl[0]=1;

	const CvMat* predict = cvKalmanPredict(kalman,u);
	const CvMat* correct= cvKalmanCorrect(kalman, measurement);

	return correct->data.fl;

}

float* updateKalman(CvKalman * kalman){

	CvMat* u = cvCreateMat(1,1, CV_32FC1 );
	u->data.fl[0]=1;

	const CvMat* predict = cvKalmanPredict(kalman,u);

	return predict->data.fl;

}
