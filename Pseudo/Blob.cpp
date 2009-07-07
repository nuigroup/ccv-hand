/**************************************************************
 * Google Summer of Code 2009
 * NuiGroup Mentoring Organization - http://nuigroup.com
 * Mentor: Laurence Muller
 * Student: Thiago de Freitas Oliveira Araújo
 * CCV-HAND Project
**************************************************************/

#include "Blob.h"
#include <cmath>
/***************************************************************************
Class Blob Source
This class implements the pseudo blob entity
****************************************************************************/

Blob::Blob(int x, int y)
{
    this->x = x;
    this->y = y;
}

Blob::~Blob()
{
}

float Blob::distance(Blob &b) {
	float a = (this->x - b.x);
	float h = (this->y - b.y);

	return sqrt(a*a + h*h);
}
