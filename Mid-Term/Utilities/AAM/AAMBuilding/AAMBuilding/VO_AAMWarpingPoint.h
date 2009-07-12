/****************************************************************************
* File Name:        VO_AAMWarpingPoint.h                                    *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM model warping information -                         * 
*                   pixel texture relationship                              *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/

#ifndef __VO_AAMWarpingPoint__
#define __VO_AAMWarpingPoint__

#include <vector>

#include "cv.h"
#include "highgui.h"

#include "VO_AAMTriangle2D.h"

using namespace std;


/****************************************************************************
* Class Name:       VO_AAMWarpingPoint                                      *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         AAM model warping information -                         * 
*                   pixel texture relationship                              *
****************************************************************************/
class VO_AAMWarpingPoint
{
private:

    /** Warping point coordinates */
    CvPoint2D32f				m_CVPosition;

    /** Which triangle is this point in, in the AAM model triangle vector? */
    unsigned int				m_iTriangleIndex;

    /** What's the position in the AAM model template face point vector? */
    unsigned int				m_iPointIndex;

    /** The triangle that this warping point belong to */
    VO_AAMTriangle2D			m_oAAMTriangle2d;

    /** The Jacobian - 1st part , refer to http://www.visionopen.com/cv/aam.php */
    vector<float>				m_Jacobian_One;

    /** The Jacobian Matrix in terms of shape parameters 2*20 */
    vector<vector<float> >		m_JacobianMatrix;

    /** The Jacobian Matrix in terms of global shape normalizing transform */
    vector<vector<float> >		m_JacobianMatrix4GlobalShapeNorm;

    /** Gradients in 3 channels (blue, green, red) for this warping point */
    vector< vector<float> >	    m_Gradients;

    // Steepest Descent Images 3 (blue, green, red) * 20 (p parameters).
    vector< vector<float> >	    m_SteepestDescentImages;

    // Steepest Descent Images for Global Shape Transform 3 (blue, green, red) * 4.
    vector< vector<float> >	    m_SteepestDescentImages4GlobalShapeNorm;

public:
	/** Default constructor to create a VO_AAMWarpingPoint object */
	VO_AAMWarpingPoint()
    {
        this->m_CVPosition                              = cvPoint2D32f(0.0, 0.0);
        this->m_iTriangleIndex                          = 0;
        this->m_iPointIndex                             = 0;
        //this->m_oAAMTriangle2d;
        this->m_Jacobian_One.clear();
        this->m_JacobianMatrix.clear();
        this->m_JacobianMatrix4GlobalShapeNorm.clear();
        this->m_Gradients.clear();
        this->m_SteepestDescentImages.clear();
        this->m_SteepestDescentImages4GlobalShapeNorm.clear();
    }

    /** Copy constructor */
	VO_AAMWarpingPoint( const VO_AAMWarpingPoint &t )
	{
		this->m_CVPosition                              = t.GetPosition();
		this->m_iTriangleIndex                          = t.GetTriangleIndex();
		this->m_iPointIndex                             = t.GetPointIndex();
		this->m_oAAMTriangle2d                          = t.GetAAMTriangle2d();
		this->CalcJacobianOne ();
		this->m_JacobianMatrix                          = t.GetJacobianMatrix();
		this->m_JacobianMatrix4GlobalShapeNorm          = t.GetJacobianMatrix4GlobalShapeNorm();
		this->m_Gradients                               = t.GetGradients();
		this->m_SteepestDescentImages                   = t.GetSteepestDescentImages();
		this->m_SteepestDescentImages4GlobalShapeNorm   = t.GetSteepestDescentImages4GlobalShapeNorm();
	}

    /** Destructor */
	~VO_AAMWarpingPoint() {}

    /** Calculate Jacobian first part for later Jacobian matrix calculation */
    void						CalcJacobianOne();

    /** Calculate Jacobian matrix for later Steepest Descent Image Calculation */
    void						CalcJacobianMatrix(const CvMat* TruncatedAlignedShapesEigenVectors);

    /** Calculate Jacobian matrix for global shape normalization for later Steepest Descent Image Calculation */
    void						CalcJacobianMatrix4GlobalShapeNorm(const CvMat* SimilarityTransformMatrix);

    /** Calculate Steepest Descent Image */
    void						CalcSteepestDescentImages(unsigned int inChannels);

    /** Calculate Steepest Descent Image Calculation for global shape normalization */
    void						CalcSteepestDescentImages4GlobalShapeNorm(unsigned int inChannels);




    /** Get this warping point coordinates */
    CvPoint2D32f    		    GetPosition() const {return this->m_CVPosition;}

    /** Get this warping point triangle index in AAM model triangle vector */
    int    		                GetTriangleIndex() const {return this->m_iTriangleIndex;}

    /** Get this warping point index in AAM model point vector */
    int    		                GetPointIndex() const {return this->m_iPointIndex;}

    /** Get the triangle that this warping point belong to */
    VO_AAMTriangle2D    		GetAAMTriangle2d() const {return this->m_oAAMTriangle2d;}

    /** Get Jacobian matrix of this warping point */
    vector<vector<float> >      GetJacobianMatrix() const { return this->m_JacobianMatrix;}

    /** Get Jacobian matrix for global shape normalization of this warping point */
    vector<vector<float> >      GetJacobianMatrix4GlobalShapeNorm() const { return this->m_JacobianMatrix4GlobalShapeNorm;}

    /** Get gradients for this warping point */
    vector< vector<float> >	    GetGradients() const {return this->m_Gradients;}

    /** Get steepest descent image for this warping point */
    vector< vector<float> >	    GetSteepestDescentImages() const {return this->m_SteepestDescentImages;}

    /** Get steepest descent image for global shape normalization for this warping point */
    vector< vector<float> >	    GetSteepestDescentImages4GlobalShapeNorm() const { return this->m_SteepestDescentImages4GlobalShapeNorm;}


    /** Set this warping point coordinates */
    void    		            SetPosition(CvPoint2D32f iPoint) {this->m_CVPosition = iPoint;}

    /** Set this warping point triangle index in AAM model triangle vector */
    void    		            SetTriangleIndex(unsigned int iTriangleIndex) {this->m_iTriangleIndex = iTriangleIndex;}

    /** Set this warping point index in AAM model point vector */
    void    		            SetPointIndex(unsigned int iPointIndex) {this->m_iPointIndex = iPointIndex;}

    /** Set 3 vertexes of the triangle that this point belong to, with input in format of a vector of points */
	void						SetAAMTriangle2d(const vector<CvPoint2D32f>& inVertexes, const vector<int> &iIndexes)
    {
        assert(inVertexes.size() == 3);
        this->m_oAAMTriangle2d = VO_AAMTriangle2D(inVertexes, iIndexes);
    }

    /** Set 3 vertexes of the triangle that this point belong to, with input in format of a row or CvMat* */
	void						SetAAMTriangle2d(const CvMat* inVertexes, const vector<int> &iIndexes)
    {
        assert(inVertexes->cols == 3);
        this->m_oAAMTriangle2d = VO_AAMTriangle2D(inVertexes, iIndexes);
    }

    /** Set the triangle that this point belong to */
	void						SetAAMTriangle2d(const VO_AAMTriangle2D& inAAMTriangle2d)
    {
        this->m_oAAMTriangle2d = inAAMTriangle2d;
    }

    /** Set Jacobian matrix for this warping point */
    void	                    SetJacobianMatrix(vector<vector<float> > & iJM) { this->m_JacobianMatrix = iJM;}

    /** Set Jacobian matrix for global shape normalization for this warping point */
    void	                    SetJacobianMatrix4GlobalShapeNorm(vector< vector<float> > & iJM4GlobalShapeNorm) { this->m_JacobianMatrix4GlobalShapeNorm = iJM4GlobalShapeNorm;}

    /** Set gradients for this warping point */
    void                        SetGradients(vector< vector<float> > & iGradients) { this->m_Gradients = iGradients;}

    /** Set steepest descent image for this warping point */
    void	                    SetSteepestDescentImages(vector< vector<float> > & iSDI) { this->m_SteepestDescentImages = iSDI;}

    /** Set steepest descent image for global shape normalization for this warping point */
    void	                    SetSteepestDescentImages4GlobalShapeNorm(vector< vector<float> > & iSDI4GlobalShapeNorm) { this->m_SteepestDescentImages4GlobalShapeNorm = iSDI4GlobalShapeNorm;}

};

#endif		// __VO_AAMWarpingPoint__

