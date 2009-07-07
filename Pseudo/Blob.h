/**************************************************************
 * Google Summer of Code 2009
 * NuiGroup Mentoring Organization - http://nuigroup.com
 * Mentor: Laurence Muller
 * Student: Thiago de Freitas Oliveira Araújo
 * CCV-HAND Project
**************************************************************/
#ifndef __BLOB_H__
#define __BLOB_H__

/***************************************************************************
Class Blob Header
This class implements the pseudo blob entity
****************************************************************************/
#include <string>

using namespace std;

class Blob
{
    private:

        int x;
        int y;

    public:
/***************************************************************************
Constructor
****************************************************************************/
        Blob(int x=0, int y=0);
        virtual ~Blob();

/***************************************************************************
vars
****************************************************************************/
        string id;

/***************************************************************************
Methods
****************************************************************************/
	int getX() {
			return this->x;
		}

		void setX(int x) {
			this->x = x;
		}

		int getY() {
			return this->y;
		}

		void setY(int y) {
			this->y = y;
		}

		float distance(Blob &b);

};
#endif // __BLOB_H__
