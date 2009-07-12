/****************************************************************************
* File Name:        VO_AAMEdge.h                                            *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM model edges                                         *
* Used Library:                                                             *
****************************************************************************/

#ifndef __VO_AAMEdge__
#define __VO_AAMEdge__

using namespace std;


/****************************************************************************
* Class Name:       VO_AAMEdge                                              *
* Author:           JIA Pei                                                 *
* Create Date:      2006-09-03                                              *
* Function:         AAM model edges                                         *
****************************************************************************/
class VO_AAMEdge
{
private:
    /** First vertex' index of this edge */
	unsigned int index1;

    /** Second vertex' index of this edge */
    unsigned int index2;

public:
    /** Default constructor to create a VO_AAMEdge object */
    VO_AAMEdge() {}

    /** Constructor to create a VO_AAMEdge object with two vertexes' indexes */
    VO_AAMEdge(int p1, int p2)
    {
        this->index1 = p1;
        this->index2 = p2;
    }
    
    /** Destructor */
    ~VO_AAMEdge() {}

    /** Default constructor to create a VO_AAMEdge object */
    bool operator==(const VO_AAMEdge& e) const
    {
        if ( ( this->index1 == e.GetIndex1() && this->index2 == e.GetIndex2() ) ||
            ( this->index1 == e.GetIndex2() && this->index2 == e.GetIndex1() ) )
            return true;
        else return false;
    }

    /** Get first index of the first edge vertex */
	unsigned int GetIndex1() const { return this->index1;}

    /** Get second index of the first edge vertex */
	unsigned int GetIndex2() const { return this->index2;}

    /** Set the first index of the first edge vertex */
	void SetIndex1(unsigned int idx1) { this->index1 = idx1;}

    /** Set the second index of the second edge vertex */
	void SetIndex2(unsigned int idx2) { this->index2 = idx2;}

};

#endif  // __VO_AAMEdge__

