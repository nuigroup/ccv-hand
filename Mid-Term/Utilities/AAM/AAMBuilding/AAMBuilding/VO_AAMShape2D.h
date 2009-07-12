/****************************************************************************
* File Name:        VO_AAMShape2D.h                                         *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM 2D shape model                                      *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/

#ifndef __VO_AAMShape2D__
#define __VO_AAMShape2D__

#include <vector>
#include <string>
#include <cmath>

#include "cv.h"
#include "highgui.h"

#include "VO_Common.h"
#include "VO_AAMPoint2D.h"
//#include "vo_aampoint3d.h"


#ifndef M_PI // Some math.h don't include this.
//#define M_PI 3.14159265358979323846264338327950288
#define M_PI 3.14159265
#endif


using namespace std;


/****************************************************************************
* Class Name:       VO_AAMShape2D                                           *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         AAM 2D shape model                                      *
****************************************************************************/
class VO_AAMShape2D
{
private:
    /** point vector in AAM shape model */
    vector<VO_AAMPoint2D>       m_vPoint;

    /** Optional 'host image' filename including full path */
    string                      m_sHostImage;

public:
	/** Default constructor to create a VO_AAMShape2D object */
    VO_AAMShape2D()
    {
        this->m_vPoint.clear();
        this->m_sHostImage = "";
    }

    /** Copy constructor */
    VO_AAMShape2D( const VO_AAMShape2D &v );

    /** Destructor */
    ~VO_AAMShape2D() {}

    // operators
    VO_AAMShape2D&              operator=(const VO_AAMShape2D &iShape);
    VO_AAMShape2D&              operator=(float value);
    VO_AAMShape2D               operator+(float value);
    VO_AAMShape2D&              operator+=(float value);
    VO_AAMShape2D               operator+(const VO_AAMShape2D &iShape);
    VO_AAMShape2D&              operator+=(const VO_AAMShape2D &iShape);
    VO_AAMShape2D               operator-(float value);
    VO_AAMShape2D&              operator-=(float value);
    VO_AAMShape2D               operator-(const VO_AAMShape2D &iShape);
    VO_AAMShape2D&              operator-=(const VO_AAMShape2D &iShape);
    VO_AAMShape2D               operator*(float value);
    VO_AAMShape2D&              operator*=(float value);
    float                       operator*(const VO_AAMShape2D &iShape);
    VO_AAMShape2D               operator/(float value);
    VO_AAMShape2D&              operator/=(float value);

    void                        CopyData( const VO_AAMShape2D &iShape );

    /** Clear all data members */
    void                        Clear() {this->SetSize (0); this->m_sHostImage.clear ();}

    /**

        @author     JIA Pei

        @version    2006-09-02

        @brief		Calculates the Center Of Gravity of the shape

        @param      x   output parameter	X Center Of Gravity output

        @param      y	output parameter    Y Center Of Gravity output

        @return     Nothing.

    */
    template<class   T>
    void                CenterOfGravity( T &x, T &y )
    {
        T xSum, ySum;

        xSum = ySum = 0.0;

        for(unsigned int i=0; i<this->m_vPoint.size(); ++i)
        {
            xSum += this->m_vPoint[i].GetXY().x;
            ySum += this->m_vPoint[i].GetXY().y;
        }

        x = (T) ( xSum/this->m_vPoint.size() );
        y = (T) ( ySum/this->m_vPoint.size() );
    }

    // Transformations
    void                        Translate( float x, float y );
    void                        Scale( float s);
    void                        ScaleXY( float sx, float sy);
    void                        Rotate( float theta);
    float                       Normalize();
    float                       GetShapeSize() const;
    float                       GetShapeNorm() const;
    float                       GetRotation( const VO_AAMShape2D &ref ) const;

    // Align the shapes with respect to position, scale and orientation.
    void                        AlignTo( const VO_AAMShape2D &ref, float *pTheta = NULL );
    void                        AlignTransformation( const VO_AAMShape2D &ref, float &scale, float &theta, CvPoint2D32f &t );
    void                        ProcrustesAnalysis( const VO_AAMShape2D &ref, float &norm, float &theta, CvPoint2D32f &t );
    void                        InverseProcrustesAnalysis( float norm, float theta, CvPoint2D32f t );

    float                       MinX()	const;
    float                       MinY()	const;
    float                       MaxX()	const;
    float                       MaxY()	const;

    /** read all annotations for .asf files */
    static void                 ReadASF( const string &filename, VO_AAMShape2D& oAAMShape);

    /** read all annotations for .pts files */
    static void                 ReadPTS( const string &filename, VO_AAMShape2D& oAAMShape);

    /** Format change - from VO_AAMTriangle2D to CvMat* */
    void                        VO_AAMShape2D2CvMat(CvMat* res) const;

    /** Format change - from CvMat* to VO_AAMTriangle2D */
    static VO_AAMShape2D        VO_CvMat2AAMShape2D(const CvMat* iShape);

    /** Get shape width */
    float                       GetWidth() const { return this->MaxX() - this->MinX(); }

    /** Get shape height */
    float                       GetHeight() const { return this->MaxY() - this->MinY(); }

    /** Get shape size */
	size_t        		        GetSize() const {return this->m_vPoint.size();}

    /** Get host image route */
    string						GetHostImage() const {return this->m_sHostImage;}

    /** Get the point vector */
    vector<VO_AAMPoint2D>		GetPoints() const {return this->m_vPoint;}

    /** Get one point at index idx */
    VO_AAMPoint2D				GetPoint(int idx = 0) const {return this->m_vPoint[idx]; }

    /** Set shape size */
	void						SetSize(size_t length) {this->m_vPoint.resize(length);}

    /** Set host image route */
	void						SetHostImage(const string &iHostImage){this->m_sHostImage = iHostImage;}

    /** Set points */
	void						SetPoints(const vector<VO_AAMPoint2D>& iPoints) {this->m_vPoint = iPoints;}

    /** Set a point at position idx */
	void						SetPoint(const VO_AAMPoint2D& iPoint, int idx = 0) {this->m_vPoint[idx] = iPoint;}

    /** Insert a point at end of the point vector */
	void						InsertPoint(const VO_AAMPoint2D& iPoint) {this->m_vPoint.push_back(iPoint);}



};

#endif  // __VO_AAMShape2D__

