/**************************************************************
 * Google Summer of Code 2009
 * NuiGroup Mentoring Organization - http://nuigroup.com
 * Mentor: Laurence Muller
 * Student: Thiago de Freitas Oliveira Araújo
 * CCV-HAND Project
**************************************************************/
#include "Hand.h"
/***************************************************************************
Class Hand Source
This class implements the pseudo Hand entity
****************************************************************************/


Hand::Hand(int x , int y)
{
    this->xCentroid = x;
    this->yCentroid = y;

}

Hand::~Hand()
{

}

float Hand::getCentroidDistance()
{
}
