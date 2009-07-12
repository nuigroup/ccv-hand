/****************************************************************************
* File Name:        VO_AAMPoint2D.h                                         *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM point 2D structure                                  *
* Used Library:     OpenCV                                                  *
****************************************************************************/

#ifndef __VO_AAMPoint2D__
#define __VO_AAMPoint2D__

using namespace std;

#include "cv.h"
#include "highgui.h"

/****************************************************************************
* Class Name:       VO_AAMPoint2D                                           *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         AAM point 2D structure                                  *
****************************************************************************/
class VO_AAMPoint2D
{
private:
    /** Which path is this point in, refer to IMM face database */
    unsigned int        m_iPath;

    /** Which type does this point belong to */
    unsigned int        m_iType;

    /** Coordinates of this point  */
    CvPoint2D32f        m_CVxy;

    /** Index of this point, in the AAM model vertex sequence */
    unsigned int        m_iIndex;

    /** In the path, which point does this point connected from? */
    unsigned int        m_iFrom;

    /** In the path, which point does this point connected to? */
    unsigned int        m_iTo;

public:
    /** Default constructor to create a VO_AAMPoint2D object */
	VO_AAMPoint2D()
    {
        this->m_iPath   = 0;
        this->m_iType   = 0;
        this->m_CVxy    = cvPoint2D32f(0.0, 0.0);;
        this->m_iIndex  = 0;
        this->m_iFrom   = 0;
        this->m_iTo     = 0;
    }

    /** Copy constructor */
    VO_AAMPoint2D( const VO_AAMPoint2D &inAAMPoint2d )
    {
        this->m_iPath   = inAAMPoint2d.GetPath();
        this->m_iType   = inAAMPoint2d.GetType();
        this->m_CVxy    = inAAMPoint2d.GetXY();
        this->m_iIndex  = inAAMPoint2d.GetIndex();
        this->m_iFrom   = inAAMPoint2d.GetFrom();
        this->m_iTo     = inAAMPoint2d.GetTo();
    }

    /** Destructor */
    ~VO_AAMPoint2D() {}

    /** operator= overloading, similar to copy constructor */
    VO_AAMPoint2D&  operator=(const VO_AAMPoint2D &inAAMPoint2d)
    {
        this->m_iPath   = inAAMPoint2d.GetPath();
        this->m_iType   = inAAMPoint2d.GetType();
        this->m_CVxy    = inAAMPoint2d.GetXY();
        this->m_iIndex  = inAAMPoint2d.GetIndex();
        this->m_iFrom   = inAAMPoint2d.GetFrom();
        this->m_iTo     = inAAMPoint2d.GetTo();
        return *this;
    }

	/** Get path which this point is on */
	unsigned int        GetPath() const { return this->m_iPath;}

    /** Get point type */
	unsigned int        GetType() const { return this->m_iType;}

    /** Get point coordinates as CvPoint2D32f */
	CvPoint2D32f        GetXY() const { return this->m_CVxy;}

    /** Get point coordinates CvPoint */
    CvPoint             GetXYInt() const { return cvPointFrom32f( this->m_CVxy ); } 

    /** Get point index, in the AAM model vertex sequence */
	unsigned int        GetIndex() const { return this->m_iIndex;}

    /** Get the point index in the path, which this point connects from */
	unsigned int        GetFrom() const { return this->m_iFrom;}

    /** Get the point index in the path, which this point connects to */
	unsigned int        GetTo() const { return this->m_iTo;}

    /** Set path the point go through */
	void                SetPath(unsigned int iPath) { this->m_iPath = iPath;}

    /** Set point type */
	void                SetType(unsigned int iType) { this->m_iType = iType;}

    /** Set point coordinates */
	void                SetXY(CvPoint2D32f iCVxy) { this->m_CVxy = iCVxy;}  

    /** Set point index, in the AAM model vertex sequence  */
	void                SetIndex(unsigned int iIndex) { this->m_iIndex = iIndex;}

    /** Set the point index, which this point connected from in path */
	void                SetFrom(unsigned int iFrom) { this->m_iFrom = iFrom;}

    /** Set the point index, which this point connected to  in path */
	void                SetTo(unsigned int iTo) { this->m_iTo = iTo;}
};

#endif      // __VO_AAMPoint2D__

