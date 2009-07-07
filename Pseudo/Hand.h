/**************************************************************
 * Google Summer of Code 2009
 * NuiGroup Mentoring Organization - http://nuigroup.com
 * Mentor: Laurence Muller
 * Student: Thiago de Freitas Oliveira Araújo
 * CCV-HAND Project
**************************************************************/
#ifndef __HAND_H__
#define __HAND_H__

/***************************************************************************
Class Hand Header
This class implements the pseudo Hand entity
****************************************************************************/
#include "Blob.h"

class Hand
{
    private:

        float xCentroid;
        float yCentroid;

    public:
/***************************************************************************
Constructor
****************************************************************************/
    Hand(int x, int y);
	virtual ~Hand();

/***************************************************************************
vars
****************************************************************************/
	string id;

/***************************************************************************
Methods
****************************************************************************/
	float getCentroidDistance();

};


#endif // __HAND_H__
