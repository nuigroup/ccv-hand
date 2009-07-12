/****************************************************************************
* File Name:        VO_AAMTexture2D3C.h                                     *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM texture model                                       *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/

#ifndef __VO_AAMTexture2D3C__
#define __VO_AAMTexture2D3C__

#include <vector>
#include <string>

#include "cv.h"
#include "highgui.h"

#include "VO_Common.h"

using namespace std;


/****************************************************************************
* Class Name:       VO_AAMTexture2D3C                                       *
* Author:           JIA Pei                                                 *
* Create Date:      2006-09-01                                              *
* Function:         AAM texture model                                       *
****************************************************************************/
class VO_AAMTexture2D3C
{
private:
    /** texture vector in AAM texture model */
	vector<float>          m_vTextures;        // 92385

public:
    /** Default constructor to create a VO_AAMTexture2D3C object */
    VO_AAMTexture2D3C() { this->m_vTextures.clear(); }

    /** Copy constructor */
    VO_AAMTexture2D3C( const VO_AAMTexture2D3C &t ){this->m_vTextures = t.GetTextures(); }

    /** Constructor to create a VO_AAMTexture2D3C object with a vector of float values */
	VO_AAMTexture2D3C( const vector<float> &iTexture ){this->m_vTextures = iTexture; }

    /** Destructor */
    ~VO_AAMTexture2D3C() {}

    // operators
    VO_AAMTexture2D3C&          operator=(const VO_AAMTexture2D3C &iTexture);
    VO_AAMTexture2D3C&          operator=(float value);
	VO_AAMTexture2D3C           operator+(float value);
	VO_AAMTexture2D3C&          operator+=(float value);
    VO_AAMTexture2D3C           operator+(const VO_AAMTexture2D3C &iTexture);
    VO_AAMTexture2D3C&          operator+=(const VO_AAMTexture2D3C &iTexture);
    VO_AAMTexture2D3C           operator-(float value);
    VO_AAMTexture2D3C&          operator-=(float value);
    VO_AAMTexture2D3C           operator-(const VO_AAMTexture2D3C &iTexture);
    VO_AAMTexture2D3C&          operator-=(const VO_AAMTexture2D3C &iTexture);
    VO_AAMTexture2D3C           operator*(float value);
    VO_AAMTexture2D3C&          operator*=(float value);
    float                       operator*(const VO_AAMTexture2D3C &iTexture);
    VO_AAMTexture2D3C           operator/(float value);
    VO_AAMTexture2D3C&          operator/=(float value);


    void                        Append(const vector<float> &v);
    void                        Scale( float s);
    float                       Norm2() const;
    void                        Normalize();
	float					    Standardize(int mtd = MEAN0NORM1);
	float					    Sum() const;

    void                        Vector2CvMat(CvMat* res) const;

    /** Format change - from VO_AAMTexture2D3C to CvMat* */
    void                        VO_AAMTexture2CvMat(CvMat* resTexture) const;

    /** Format change - from CvMat* to VO_AAMTexture2D3C */
    static VO_AAMTexture2D3C    VO_CvMat2AAMTexture(const CvMat* iTexture);

    /** Get texture size */    
    size_t                      GetSize() const {return this->m_vTextures.size ();}

    /** Get the texture value at texture vector position i */
    float                       GetATexture(int i) const	{ return this->m_vTextures[i];}
    
    /** Get the texture vector */
    vector<float>               GetTextures() const { return this->m_vTextures;}

    /** Set texture size */
    void					    SetSize(int length) {this->m_vTextures.resize(length);}

    /** Set the texture value at texture vector position i */
    void                        SetATexture(int i, float value) { this->m_vTextures[i] = value;}
	
    /** Set the texture vector with input in a row in vector format */
    void                        SetTextures(const vector<float> &iTextures) { this->m_vTextures = iTextures;}

    /** Set the texture vector with input in a row in CvMat* format */
    void SetTextures(const CvMat* iTextures)
    {
        this->m_vTextures.resize(iTextures->cols);
        for(int i = 0; i < iTextures->cols; i++)
        {
            this->m_vTextures[i] = CV_MAT_ELEM( *iTextures, float, 0, i );
        }
    }
};

#endif  // __vo_aamtexture2d3c__

