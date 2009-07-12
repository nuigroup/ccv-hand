/****************************************************************************
* File Name:        VO_AAMShape2D.cpp                                       *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM 2D shape model                                      *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "VO_AAMShape2D.h"

using namespace std;


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      Copy constructor

    @param      iShape	The input shape

    @return     N/A

*/
VO_AAMShape2D::VO_AAMShape2D( const VO_AAMShape2D &iShape )
{
    this->CopyData (iShape);
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator= overloading, similar to copy constructor

    @param      iShape	The input shape

    @return     VO_AAMShape2D&

*/
VO_AAMShape2D& VO_AAMShape2D::operator=(const VO_AAMShape2D &iShape)
{
    this->CopyData (iShape);
    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator= overloading, similar to copy constructor

    @param      value	assign all values in VO_AAMShape2D to value

    @return     VO_AAMShape2D&

*/
VO_AAMShape2D& VO_AAMShape2D::operator=(float value)
{
    if (this->m_vPoint.size())
    {
        CvPoint2D32f tempCvPoint2D32f;
        for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
        {
            tempCvPoint2D32f.x = value;
            tempCvPoint2D32f.y = value;
			this->m_vPoint[i].SetXY(tempCvPoint2D32f);
        }
    }

    return *this;  
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator+ overloading, shift one AAM shape by value

    @param      value

    @return     VO_AAMShape2D

*/
VO_AAMShape2D VO_AAMShape2D::operator+(float value)
{    
    VO_AAMShape2D res(*this);
    CvPoint2D32f tempCvPoint2D32f;
    VO_AAMPoint2D tempAAMPoint2d;

    for (unsigned int i = 0; i < res.GetPoints().size(); i++)
    {
        tempCvPoint2D32f.x = res.GetPoint(i).GetXY().x + value;
        tempCvPoint2D32f.y = res.GetPoint(i).GetXY().y + value;
        tempAAMPoint2d.SetXY(tempCvPoint2D32f);
        res.SetPoint(tempAAMPoint2d, i);
    }

    return res;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator+= overloading, add value to this AAM shape 

    @param      value

    @return     VO_AAMShape2D&

*/
VO_AAMShape2D& VO_AAMShape2D::operator+=(float value)
{
    CvPoint2D32f tempCvPoint2D32f;

    for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
        tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x + value;
        tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y + value;
        this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator+ overloading, add two AAM shape to one 

    @param      iShape      the added AAM shape

    @return     VO_AAMShape2D

*/
VO_AAMShape2D VO_AAMShape2D::operator+(const VO_AAMShape2D &iShape)
{    
    VO_AAMShape2D res(*this);
    CvPoint2D32f tempCvPoint2D32f;
    VO_AAMPoint2D tempAAMPoint2d;

    for (unsigned int i = 0; i < res.GetPoints().size(); i++)
    {
        tempCvPoint2D32f.x = res.GetPoint(i).GetXY().x + iShape.GetPoint(i).GetXY().x;
        tempCvPoint2D32f.y = res.GetPoint(i).GetXY().y + iShape.GetPoint(i).GetXY().y;
        tempAAMPoint2d.SetXY(tempCvPoint2D32f);
        res.SetPoint(tempAAMPoint2d, i);
    }

    return res;
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator+= overloading, add the input AAM shape to this AAM shape

    @param      iShape      the added AAM shape

    @return     VO_AAMShape2D&

*/
VO_AAMShape2D& VO_AAMShape2D::operator+=(const VO_AAMShape2D &iShape)
{
    CvPoint2D32f tempCvPoint2D32f;

    for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
        tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x + iShape.GetPoint(i).GetXY().x;
        tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y + iShape.GetPoint(i).GetXY().y;
		this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator- overloading, shift one AAM shape by -value

    @param      value

    @return     VO_AAMShape2D

*/
VO_AAMShape2D VO_AAMShape2D::operator-(float value)
{    
    VO_AAMShape2D res(*this);
    CvPoint2D32f tempCvPoint2D32f;
    VO_AAMPoint2D tempAAMPoint2d;

    for (unsigned int i = 0; i < res.GetPoints().size(); i++)
    {
        tempCvPoint2D32f.x = res.GetPoint(i).GetXY().x - value;
        tempCvPoint2D32f.y = res.GetPoint(i).GetXY().y - value;
        tempAAMPoint2d.SetXY(tempCvPoint2D32f);
        res.SetPoint(tempAAMPoint2d, i);
    }

    return res;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator-= overloading, subtract value from this AAM shape 

    @param      value

    @return     VO_AAMShape2D&

*/
VO_AAMShape2D& VO_AAMShape2D::operator-=(float value)
{
    CvPoint2D32f tempCvPoint2D32f;

    for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
        tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x - value;
        tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y - value;
        this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator- overloading, subtract one shape from another

    @param      iShape      the subtracted AAM shape

    @return     VO_AAMShape2D

*/
VO_AAMShape2D VO_AAMShape2D::operator-(const VO_AAMShape2D &iShape)
{
    VO_AAMShape2D res(*this);
    CvPoint2D32f tempCvPoint2D32f;
    VO_AAMPoint2D tempAAMPoint2d;

    for (unsigned int i = 0; i < res.GetPoints().size(); i++)
    {
        tempCvPoint2D32f.x = res.GetPoint(i).GetXY().x - iShape.GetPoint(i).GetXY().x;
        tempCvPoint2D32f.y = res.GetPoint(i).GetXY().y - iShape.GetPoint(i).GetXY().y;
        tempAAMPoint2d.SetXY(tempCvPoint2D32f);
        res.SetPoint(tempAAMPoint2d, i);
    }

    return res;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator-= overloading, subtract the input AAM shape from this AAM shape

    @param      iShape      the subtracted AAM shape

    @return     VO_AAMShape2D&

*/
VO_AAMShape2D& VO_AAMShape2D::operator-=(const VO_AAMShape2D &iShape)
{
	CvPoint2D32f tempCvPoint2D32f;

    for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
        tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x - iShape.GetPoint(i).GetXY().x;
        tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y - iShape.GetPoint(i).GetXY().y;
		this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }

    return *this;
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator* overloading, scale a shape with input float value

    @param      value      scale size

    @return     VO_AAMShape2D

*/
VO_AAMShape2D VO_AAMShape2D::operator*(float value)
{
    VO_AAMShape2D res(*this);
    CvPoint2D32f tempCvPoint2D32f;
    VO_AAMPoint2D tempAAMPoint2d;

    for (unsigned int i = 0; i < res.GetPoints().size(); i++)
    {
        tempCvPoint2D32f.x = res.GetPoint(i).GetXY().x * value;
        tempCvPoint2D32f.y = res.GetPoint(i).GetXY().y * value;
        tempAAMPoint2d.SetXY(tempCvPoint2D32f);
        res.SetPoint(tempAAMPoint2d, i);
    }

    return res;
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator*= overloading, scale this shape with input float value

    @param      iShape      the subtracted AAM shape

    @return     VO_AAMShape2D&

*/
VO_AAMShape2D& VO_AAMShape2D::operator*=(float value)
{
	CvPoint2D32f tempCvPoint2D32f;

    for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
		tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x * value;
		tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y * value;
        this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator* overloading, dot product of two AAM shapes

    @param      iShape      AAM shape to be dot producted

    @return     float       dot product

*/
float VO_AAMShape2D::operator*(const VO_AAMShape2D &iShape)
{
    float result = 0.;

    for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
        result += this->m_vPoint[i].GetXY().x * iShape.GetPoint(i).GetXY().x;
        result += this->m_vPoint[i].GetXY().y * iShape.GetPoint(i).GetXY().y;
    }

    return result;
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator/ overloading, scale a shape

    @param      value      1.0/value = scale size

    @return     VO_AAMShape2D

*/
VO_AAMShape2D VO_AAMShape2D::operator/(float value)
{
    assert (value != 0);
    
    VO_AAMShape2D res(*this);
    CvPoint2D32f tempCvPoint2D32f;
    VO_AAMPoint2D tempAAMPoint2d;

    for (unsigned int i = 0; i < res.GetPoints().size(); i++)
    {
        tempCvPoint2D32f.x = res.GetPoint(i).GetXY().x / value;
        tempCvPoint2D32f.y = res.GetPoint(i).GetXY().y / value;
        tempAAMPoint2d.SetXY(tempCvPoint2D32f);
        res.SetPoint(tempAAMPoint2d, i);
    }

    return res;
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator/= overloading, scale this shape with input float value

    @param      value      1.0/value = the scaled value

    @return     VO_AAMShape2D&

*/
VO_AAMShape2D& VO_AAMShape2D::operator/=(float value)
{
    assert (value != 0);

	CvPoint2D32f tempCvPoint2D32f;

    for(unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {		
		tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x / value;
		tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y / value;
        this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      copy all data from the input VO_AAMShape2D to this VO_AAMShape2D

    @param      iShape      1.0/value = the scaled value

    @return     void

*/
void VO_AAMShape2D::CopyData( const VO_AAMShape2D& iShape )
{
    // copy vector data (i.e. the point coordinates)
    this->m_vPoint.resize( iShape.GetSize() );
    this->m_vPoint                  = iShape.GetPoints();
    this->m_sHostImage              = iShape.GetHostImage();
}



/**

    @author     JIA Pei

    @version    2006-09-02

    @brief      Translate this shape.

    @param      x	X-translation.

    @param      y	Y-translation.

    @return     Nothing.

*/
void VO_AAMShape2D::Translate( float x, float y )
{
	CvPoint2D32f tempCvPoint2D32f;
    for(unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {		
		tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x + x;
		tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y + y;
        this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }
}



/**

    @author     JIA Pei

    @version    2006-09-02

    @brief		Scale this shape.

    @param      s			Scale factor

    @return     void

*/
void VO_AAMShape2D::Scale( float s)
{
	CvPoint2D32f tempCvPoint2D32f;
    for(unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {		
		tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x * s;
		tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y * s;
        this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }
}


/**

    @author     JIA Pei

    @version    2006-09-02

    @brief		Scale this shape

    @param      sx			Scale factor in X direction

    @param      sy			Scale factor in Y direction

    @return     void

*/
void VO_AAMShape2D::ScaleXY( float sx, float sy)
{
	CvPoint2D32f tempCvPoint2D32f;
    for(unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
		tempCvPoint2D32f.x = this->m_vPoint[i].GetXY().x * sx;
		tempCvPoint2D32f.y = this->m_vPoint[i].GetXY().y * sy;
        this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }
}



/**

    @author     JIA Pei

    @version    2006-09-02

    @brief		Rotates the shape 'theta' radians

    @param      theta		Rotation angle in radians

    @return     Nothing.

*/
void VO_AAMShape2D::Rotate( float theta)
{
    float x, y;

    // set up rotation matrix
    float c00 =  cos( theta );
    float c01 = -sin( theta );
    float c10 =  sin( theta );
    float c11 =  cos( theta );

    for(unsigned int i=0;i<this->m_vPoint.size();i++)
    {
		CvPoint2D32f tempCvPoint2D32f;
        x = this->m_vPoint[i].GetXY().x;
        y = this->m_vPoint[i].GetXY().y;
		tempCvPoint2D32f.x =  c00*x+c01*y;
		tempCvPoint2D32f.y =  c10*x+c11*y;
        this->m_vPoint[i].SetXY(tempCvPoint2D32f);
    }
}



/**

    @author	    JIA Pei

    @version	2006-09-02

    @brief		Normalize the shape by translating to its Center Of Gravity
                scaling by the reciprocal of the 2-norm

    @return	    The 2-norm of the shape after translating to origin
*/

float VO_AAMShape2D::Normalize()
{
    float x,y;

    this->CenterOfGravity( x, y );

    this->Translate( -x, -y );

    float norm = this->GetShapeNorm();

    this->Scale( 1.0f/norm );

    return norm;
}



/**

    @author   JIA Pei

    @version  2006-04-13

    @brief    Returns the 2-norm of this centralized shape

    @return   The 2-norm of the shape after translating to origin

*/
float VO_AAMShape2D::GetShapeSize() const
{
    VO_AAMShape2D tmp(*this);

    return tmp.Normalize();
}


/**

    @author     JIA Pei

    @version    2006-09-02

    @brief      Get the norm of the shape

    @return     float

*/
float VO_AAMShape2D::GetShapeNorm() const
{
    float norm = 0.0;

    // Normalize the vector to unit length, using the 2-norm.
    for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
        norm += this->m_vPoint[i].GetXY().x * this->m_vPoint[i].GetXY().x;
        norm += this->m_vPoint[i].GetXY().y * this->m_vPoint[i].GetXY().y;
    }

    norm = sqrt(norm);

    return norm;
}



/**

    @author	    JIA Pei

    @version	2006-09-02

    @brief		Returns the rotation between ref and this (in radians).
                Get the rotation between two shapes by minimizing the sum of squared point distances, 
                as described by Goodall (and Bookstein) using Singular Value Decomposition (SVD).

                Note that both shapes must be normalized with respect to scale and position beforehand.
                This could be done by using VO_AAMShape2D::Normalize(). 

    @return	    The estimated angle, theta, between the two shapes.

*/
float VO_AAMShape2D::GetRotation( const VO_AAMShape2D &ref ) const
{
    assert( ref.m_vPoint.size() == this->m_vPoint.size() );

    CvMat* mRef            =   cvCreateMat ( (int)this->m_vPoint.size(), 2, CV_32FC1 );
    CvMat* mRefTranspose   =   cvCreateMat ( 2, (int)this->m_vPoint.size(), CV_32FC1 );
    CvMat* mS              =   cvCreateMat ( (int)this->m_vPoint.size(), 2, CV_32FC1 );
    CvMat* res             =   cvCreateMat ( 2, 2, CV_32FC1 );

    // get data as OpenCV matrices ( nbPoints x 2 columns )
    for(unsigned int i=0; i < this->m_vPoint.size(); i++)
    {
        CV_MAT_ELEM( *mRef, float, i, 0 ) = ref.GetPoint(i).GetXY().x;
        CV_MAT_ELEM( *mRef, float, i, 1 ) = ref.GetPoint(i).GetXY().y;

        CV_MAT_ELEM( *mS, float, i, 0 ) = this->m_vPoint[i].GetXY().x;
        CV_MAT_ELEM( *mS, float, i, 1 ) = this->m_vPoint[i].GetXY().y;
    }

    // Now, calculate the rotation

    cvTranspose( mRef, mRefTranspose );

    cvMatMul( mRefTranspose, mS, res );

    CvMat* S   =   cvCreateMat (2, 1, CV_32FC1);
    CvMat* UT  =   cvCreateMat (2, 2, CV_32FC1);
    CvMat* V   =   cvCreateMat (2, 2, CV_32FC1);

    // SVD Definition in OpenCV
    // A = U*W*V^T = U^T*W*V
    // cvSVD( CvArr* A, CvArr* W, CvArr* U=NULL, CvArr* V=NULL, int flags=0 );
    cvSVD( res, S, UT, V, CV_SVD_U_T );

    cvMatMul( V, UT, res );

    // res now holds a normal 2x2 rotation matrix
    float angle;
    float cos_theta = CV_MAT_ELEM( *res, float, 0, 0 );
    float sin_theta = CV_MAT_ELEM( *res, float, 1, 0 );

    // cos_theta should be less than 1.0; but here, cos_theta is calculated by matrix computation, 
    // in stead of by acos() function, so cos_theta might have some special values like cos_theta >= 1.0.
    if ( ( fabs(1.0-cos_theta) < FLT_EPSILON ) || cos_theta >= 1.0 )
    {
        // cos_theta = 1  =>  shapes are already aligned
        angle = 0;
        return angle;
    }
    if ( fabs(cos_theta) < FLT_EPSILON )
    {
        // cos_theta = 0  =>  90 degrees rotation
        return (float)(M_PI/2.0);
    }
    // cos_theta should be bigger than -1.0; but here, cos_theta is calculated by  matrix computation,
    // in stead of by acos() function, so cos_theta might have some special values like cos_theta <= -1.0.
    if ( ( fabs(1.0+cos_theta) < FLT_EPSILON ) || cos_theta <= -1.0 )
    {
        // cos_theta=-1  =>  180 degrees rotation
        angle = (float)M_PI;
    }
    else
    {
        // get the rotation in radians
        float a_cos = acos( cos_theta );
        float a_sin = asin( sin_theta );

        // http://en.wikipedia.org/wiki/Trigonometric_function
        if (a_sin<0)
        {		
            // lower half of the unit circle
            angle = -a_cos;
        }
        else
        {
            // upper half of the unit circle
            angle = a_cos;
        }
    }

    cvReleaseMat(&mRef);
    cvReleaseMat(&mRefTranspose);
    cvReleaseMat(&mS);
    cvReleaseMat(&res);

    cvReleaseMat(&S);
    cvReleaseMat(&UT);
    cvReleaseMat(&V);

    return angle;
}




/**

    @author	    JIA Pei

    @version	2006-09-02

    @brief		Aligns this to 'ref' with respect to pose.

    @param	    ref			The reference shape.

    @param	    pTheta		Optional pointer to return the rotation carried out on this shape

    @return	    none.
*/
void VO_AAMShape2D::AlignTo( const VO_AAMShape2D &ref, float *pTheta )
{
    // make a copy of 'ref'
    VO_AAMShape2D refCpy( ref );
    float x,y;

    // translate, move this and refCpy to origin
    this->CenterOfGravity( x, y );
    this->Translate( -x, -y );
    refCpy.CenterOfGravity( x, y );
    refCpy.Translate( -x, -y ); // remember now, (x, y) are for ref (refCpy) !!

    // scale, using the 2-norm
    float this_size = this->GetShapeNorm();
    float ref_size = refCpy.GetShapeNorm();
    this->Scale( ref_size/this_size );

    // align rotation between this and refCpy
    float theta;
    theta = -this->GetRotation( refCpy );
    this->Rotate( theta );

    if (pTheta)
    {
        *pTheta = -theta;
    }

    // translate this to ref origin
    this->Translate( x, y );

}


/**

    @author	    JIA Pei

    @version	2006-10-06

    @brief		Returns the transformation that aligns this to 'ref'

    @param	    ref	    input parameter - The reference shape

    @param	    scale	output parameter - scale value

    @param	    theta	output parameter - rotation angle theta

    @param	    t	    output parameter - translation in both X, Y directions

    @return	    void

*/
void VO_AAMShape2D::AlignTransformation( const VO_AAMShape2D &ref, float &scale, float &theta, CvPoint2D32f &t )
{
    VO_AAMShape2D refCpy( ref );
    VO_AAMShape2D thisCpy( *this );		// a must, we don't want to change "this" right now
    float x1,y1, x2, y2;

    // move thisCpy and refCpy to origin
    thisCpy.CenterOfGravity( x1, y1 );
    thisCpy.Translate( -x1, -y1 );
    refCpy.CenterOfGravity( x2, y2 );
    refCpy.Translate( -x2, -y2 );
    t.x = x2 - x1;
    t.y = y2 - y1;

    // normalize scale, using the 2-norm
	float this_size = thisCpy.GetShapeNorm();
	float ref_size = refCpy.GetShapeNorm();
    scale = ref_size/this_size;
    thisCpy.Scale( scale );	

    // align rotation between thisCpy and refCpy
	theta = -thisCpy.GetRotation( refCpy );	
}


/**

    @author	    JIA Pei

    @version	2008-02-25

    @brief		Proscrustes Analysis

    @param	    ref	    input parameter - The reference shape

    @param	    scale	output parameter - scale value

    @param	    theta	output parameter - rotation angle theta

    @param	    t	    output parameter - translation in both X, Y directions

    @return	    void

    @note       http://en.wikipedia.org/wiki/Procrustes_analysis

*/
void VO_AAMShape2D::ProcrustesAnalysis( const VO_AAMShape2D &ref, float &norm, float &theta, CvPoint2D32f &t )
{
    // move this to origin
    this->CenterOfGravity( t.x, t.y );
    this->Translate( -t.x, -t.y );

    // 2-norm = 1
    norm = this->GetShapeNorm();
    this->Scale( 1.0f/norm );

    // align rotation between this and ref
    theta = -this->GetRotation( ref );
    this->Rotate( theta );
}

/**

    @author	    JIA Pei

    @version	2008-02-25

    @brief	    Inverse Proscrustes Analysis

    @param	    scale	input parameter - scale value

    @param	    theta	input parameter - rotation angle theta

    @param	    t	    input parameter - translation in both X, Y directions

    @return	    void

    @note       http://en.wikipedia.org/wiki/Procrustes_analysis

*/
void VO_AAMShape2D::InverseProcrustesAnalysis( float scale, float theta, CvPoint2D32f t )
{
    // 2-norm back
    this->Scale(scale);

    // rotation back
    this->Rotate(theta);
    
    // COG back
    this->Translate(t.x, t.y);
}


/**

    @author	    JIA Pei

    @version	2006-09-02

    @brief		Calculate min value in X direction

    @return	    min value in X direction

*/
float VO_AAMShape2D::MinX() const
{	
    float val, min = FLT_MAX_10_EXP;

    for(unsigned int i=0;i<this->m_vPoint.size();i++)
    {	
        val = this->m_vPoint[i].GetXY().x;
        min = val<min ? val : min;
    }
    return min;
}

/**

    @author	    JIA Pei

    @version	2006-09-02

    @brief		Calculate min value in Y direction

    @return	    min value in Y direction

*/
float VO_AAMShape2D::MinY() const
{	
    float val, min = FLT_MAX_10_EXP;

    for(unsigned int i=0;i<this->m_vPoint.size();i++)
    {	
        val = this->m_vPoint[i].GetXY().y;
        min = val<min ? val : min;
    }
    return min;
}

/**

    @author	    JIA Pei

    @version	2006-09-02

    @brief		Calculate max value in X direction

    @return	    max value in X direction

*/
float VO_AAMShape2D::MaxX() const
{	
    float val, max = FLT_MIN_10_EXP ;

    for(unsigned int i=0;i<this->m_vPoint.size();i++)
    {
        val = this->m_vPoint[i].GetXY().x;
        max = val>max ? val : max;
    }
    return max;
}


/**

    @author	    JIA Pei

    @version	2006-09-02

    @brief		Calculate max value in Y direction

    @return	    max value in Y direction

*/
float VO_AAMShape2D::MaxY() const
{	
    float val, max = FLT_MIN_10_EXP ;

    for(unsigned int i=0;i<this->m_vPoint.size();i++)
    {
        val = this->m_vPoint[i].GetXY().y;
        max = val>max ? val : max;
    }
    return max;
}



/**

    @author     JIA Pei

    @version    2008-03-04

    @brief      Format change - from CvMat* to VO_AAMTriangle2D

*/
VO_AAMShape2D VO_AAMShape2D::VO_CvMat2AAMShape2D(const CvMat* iShape)
{
    unsigned int NbOfPoints = iShape->cols/2;
    VO_AAMShape2D resShape;
    resShape.SetSize(NbOfPoints);
    CvPoint2D32f tempCvPoint2D32f;
    VO_AAMPoint2D tempPoint2d;
    for(unsigned int i = 0; i < NbOfPoints; i++)
    {
        tempCvPoint2D32f.x = CV_MAT_ELEM( *iShape, float, 0, 2*i);
        tempCvPoint2D32f.y = CV_MAT_ELEM( *iShape, float, 0, 2*i+1);
        tempPoint2d.SetXY(tempCvPoint2D32f);
        resShape.SetPoint(tempPoint2d, i);
    }

    return resShape;
}


/**

    @author     JIA Pei

    @version    2008-03-04

    @brief      Format change - from VO_AAMTriangle2D to CvMat*

*/
void VO_AAMShape2D::VO_AAMShape2D2CvMat(CvMat* res) const
{
    for (unsigned int i = 0; i < this->m_vPoint.size(); i++)
    {
        CV_MAT_ELEM( *res, float, 0 , 2 * i) = this->m_vPoint[i].GetXY().x;
        CV_MAT_ELEM( *res, float, 0 , 2 * i + 1) = this->m_vPoint[i].GetXY().y;
    }
}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Read a file and obtain all annotation data in VO_AAMShape2D

@param      filename        input parameter, which .asf annotation file to read

@param      VO_AAMShape2D   output parameter, save annotation data to AAM shape data structure

*/
void VO_AAMShape2D::ReadASF( const string &filename, VO_AAMShape2D& oAAMShape )
{
    fstream fp;
    fp.open(filename.c_str (), ios::in);

    stringstream ss;
    string temp;

    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );

    unsigned int NbOfPoints = atoi(temp.c_str ());

    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );
    getline( fp, temp );

    VO_AAMPoint2D temppoint;
    CvPoint2D32f tp;
    int tempInt;

    for (unsigned int i = 0; i < NbOfPoints; i++)
    {
        getline(fp, temp, ' ');
        ss << temp;
        ss >> tempInt;
        ss.clear();
        temppoint.SetPath( tempInt );
        getline(fp, temp, ' ');
        ss << temp;
        ss >> tempInt;
        temppoint.SetType( tempInt );
        ss.clear();
        getline(fp, temp, ' ');
        // In DTU IMM , x means rows from left to right
        ss << temp;
        ss >> tp.x;
        ss.clear();
        getline(fp, temp, ' ');
        // In DTU IMM , y means cols from top to bottom
        ss << temp;
        ss >> tp.y;
        ss.clear();
        temppoint.SetXY(tp);
        getline(fp, temp, ' ');
        ss << temp;
        ss >> tempInt;
        temppoint.SetIndex( tempInt );
        ss.clear();
        getline(fp, temp, ' ');
        ss << temp;
        ss >> tempInt;
        temppoint.SetFrom( tempInt );
        ss.clear();
        getline(fp, temp);
        ss << temp;
        ss >> tempInt;
        temppoint.SetTo( tempInt );
        ss.clear();
        // In sum, topleft is (0,0), right bottom is (640,480)
        oAAMShape.InsertPoint(temppoint);
    }

    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);
    getline(fp, temp);

    oAAMShape.SetHostImage( temp );

    fp.close ();

}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Read a file and obtain all annotation data in VO_AAMShape2D

@param      filename        input parameter, which .pts annotation file to read

@param      VO_AAMShape2D   output parameter, save annotation data to AAM shape data structure

*/
void VO_AAMShape2D::ReadPTS( const string &filename, VO_AAMShape2D& oAAMShape)
{
    fstream fp;
    fp.open(filename.c_str (), ios::in);

    string temp;
    stringstream ss;
    string::size_type loc;

    do
    {
        getline(fp, temp );
        loc = temp.find("n_points: 68", 0);
    }while ( loc == string::npos && !fp.eof( ) );
    unsigned int NbOfPoints = 68;
    getline(fp, temp);

    VO_AAMPoint2D temppoint;
    CvPoint2D32f tp;

    for (unsigned int i = 0; i < NbOfPoints; i++)
    {		
        getline(fp, temp, ' ');
        // x refers to a row from left to right
        ss << temp;
        ss >> tp.x;
        ss.clear();
        getline(fp, temp);        
        // x refers to a row from left to right
        ss << temp;
        ss >> tp.y;
        ss.clear();
        // In sum, topleft is (0,0), right bottom is (720,576)
        temppoint.SetXY(tp);
        oAAMShape.InsertPoint(temppoint);
    }

    fp.close ();
}

