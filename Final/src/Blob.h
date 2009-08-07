/************************************************************************
  			Blob.h

FUNCIONALITAT: Definici� de la classe CBlob
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificaci�, Autor, Data):

FUNCTIONALITY: Definition of the CBlob class and some helper classes to perform
			   some calculations on it
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/

#include "cxcore.h"
#include <functional>
#include <vector>
#include <algorithm>


#ifndef CBLOB_INSPECTA_INCLUDED
#define CBLOB_INSPECTA_INCLUDED
/**
	Classe que representa un blob, ent�s com un conjunt de pixels del
	mateix color contigus en una imatge binaritzada.

	Class to represent a blob, a group of connected pixels in a binary image
*/
class CBlob
{
public:
	//! Constructor est�ndard
	//! Standard constructor
	CBlob();
	//! Constructor de c�pia
	//! Copy constructor
	CBlob( const CBlob &src );
	//! Destructor est�ndard
	//! Standard Destructor
	~CBlob();

	//! Operador d'assignaci�
	//! Assigment operator
	CBlob& operator=(const CBlob &src );

	//! Indica si el blob est� buit ( no t� cap info associada )
	//! Shows if the blob has associated information
	bool IsEmpty() const
	{
		return (area == 0.0 && perimeter == 0.0 );
	};

	//! Neteja les cantonades del blob
	//! Clears the edges of the blob
	void ClearEdges();
	//! Copia les cantonades del blob a un altre (les afegeix al dest�)
	//! Adds the blob edges to another blob
	void CopyEdges( CBlob &destination ) const;
	//! Retorna el poligon convex del blob
	//! Calculates the convex hull of the blob
	bool GetConvexHull( CvSeq **dst ) const;
	//! Calcula l'elipse que s'adapta als v�rtexs del blob
	//! Fits an ellipse to the blob edges
	CvBox2D GetEllipse() const;

	//! Pinta l'interior d'un blob d'un color determinat
	//! Paints the blob in an image
	void FillBlob( IplImage *imatge, CvScalar color ) const;

	//! Funcions GET sobre els valors dels blobs
	//! Get functions

	inline int Label() const	{ return etiqueta; }
	inline int Parent() const	{ return parent; }
	inline double Area() const { return area; }
	inline double Perimeter() const { return perimeter; }
	inline int	  Exterior() const { return exterior; }
	inline int	  ExternPixels() const { return nombrePixelsExterns; }
	inline double Mean() const { return mean; }
	inline double StdDev() const { return stddev; }
	inline double MinX() const { return minx; }
	inline double MinY() const { return miny; }
	inline double MaxX() const { return maxx; }
	inline double MaxY() const { return maxy; }
	inline CvSeq *Edges() const { return edges; }
	inline double SumX() const { return sumx; }
	inline double SumY() const { return sumy; }
	inline double SumXX() const { return sumxx; }
	inline double SumYY() const { return sumyy; }
	inline double SumXY() const { return sumxy; }



	//! etiqueta del blob
	//! label of the blob
	int etiqueta;
	//! flag per indicar si es exterior o no
	//! true for extern blobs
	int exterior;
	//! nombre de pixels externs del perimetre
	//! number of extern pixels in the blob perimeter
	int nombrePixelsExterns;
	//! area del blob
	//! Blob area
	double area;
	//! perimetre del blob
	//! Blob perimeter
	double perimeter;

	//! etiqueta del blob pare
	//! label of the parent blob
	int parent;
	//! moments
	double sumx;
	double sumy;
	double sumxx;
	double sumyy;
	double sumxy;
	//! Bounding rect
	double minx;
	double maxx;
	double miny;
	double maxy;

	//! mitjana
	//! mean of the grey scale values of the blob pixels
	double mean;
	//! desviaci� standard
	//! standard deviation of the grey scale values of the blob pixels
	double stddev;

	//! �rea de mem�ria on es desaran els punts de contorn del blob
	//! storage which contains the edges of the blob
	CvMemStorage *m_storage;
	//!	Sequ�ncia de punts del contorn del blob
	//! Sequence with the edges of the blob
	CvSeq *edges;


private:
	//! Point datatype for plotting (FillBlob)
	typedef std::vector<CvPoint> vectorPunts;

	//! Helper class to compare two CvPoints (for sorting in FillBlob)
	struct comparaCvPoint : public std::binary_function<CvPoint, CvPoint, bool>
	{
		//! Definim que un punt �s menor com m�s amunt a la dreta estigui
		bool operator()(CvPoint a, CvPoint b)
		{
			if( a.y == b.y )
				return a.x < b.x;
			else
				return a.y < b.y;
		}
	};
};



/**************************************************************************
		Definici� de les classes per a fer operacions sobre els blobs

		Helper classes to perform operations on blobs
**************************************************************************/


//! Classe d'on derivarem totes les operacions sobre els blobs
//! Interface to derive all blob operations
class COperadorBlob
{
public:
	virtual double operator()(const CBlob &blob) const = 0;

	operator COperadorBlob*() const
	{
		return (COperadorBlob*)this;
	}
};

typedef COperadorBlob funcio_calculBlob;


//! Classe per calcular l'�rea d'un blob
//! Class to get the area of a blob
class CBlobGetArea : public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		return blob.Area();
	}
};

//! Classe per calcular el perimetre d'un blob
//! Class to get the perimeter of a blob
class CBlobGetPerimeter: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		return blob.Perimeter();
	}
};

//! Classe que diu si un blob �s extern o no
//! Class to get the extern flag of a blob
class CBlobGetExterior: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		return blob.Exterior();
	}
};

//! Classe per calcular la mitjana de nivells de gris d'un blob
//! Class to get the mean grey level of a blob
class CBlobGetMean: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		return blob.Mean();
	}
};

//! Classe per calcular la desviaci� est�ndard dels nivells de gris d'un blob
//! Class to get the standard deviation of the grey level values of a blob
class CBlobGetStdDev: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		return blob.StdDev();
	}
};

//! Classe per calcular la compacitat d'un blob
//! Class to calculate the compactness of a blob
class CBlobGetCompactness: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular la longitud d'un blob
//! Class to calculate the length of a blob
class CBlobGetLength: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular l'amplada d'un blob
//! Class to calculate the breadth of a blob
class CBlobGetBreadth: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular el moment PQ del blob
//! Class to calculate the P,Q moment of a blob
class CBlobGetMoment: public COperadorBlob
{
public:
	//! Constructor est�ndard
	//! Standard constructor (gets the 00 moment)
	CBlobGetMoment()
	{
		m_p = m_q = 0;
	}
	//! Constructor: indiquem el moment p,q a calcular
	//! Constructor: gets the PQ moment
	CBlobGetMoment( int p, int q )
	{
		m_p = p;
		m_q = q;
	};
	double operator()(const CBlob &blob) const;

private:
	//! moment que volem calcular
	int m_p, m_q;
};

//! Classe per calcular el perimetre convex d'un blob
//! Class to calculate the convex hull perimeter of a blob
class CBlobGetHullPerimeter: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular la x minima en la y minima
//! Class to calculate the minimum x on the minimum y
class CBlobGetMinXatMinY: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular la y minima en la x maxima
//! Class to calculate the minimum y on the maximum x
class CBlobGetMinYatMaxX: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular la x maxima en la y maxima
//! Class to calculate the maximum x on the maximum y
class CBlobGetMaxXatMaxY: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular la y maxima en la x minima
//! Class to calculate the maximum y on the minimum y
class CBlobGetMaxYatMinX: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular l'elongacio d'un blob
//! Class to calculate the elongation of the blob
class CBlobGetElongation: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular la rugositat d'un blob
//! Class to calculate the roughness of the blob
class CBlobGetRoughness: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const;
};

//! Classe per calcular la dist�ncia entre el centre del blob i un punt donat
//! Class to calculate the euclidean distance between the center of a blob and a given point
class CBlobGetDistanceFromPoint: public COperadorBlob
{
public:
	//! Standard constructor (distance to point 0,0)
	CBlobGetDistanceFromPoint()
	{
		m_x = m_y = 0.0;
	}
	//! Constructor (distance to point x,y)
	CBlobGetDistanceFromPoint( const double x, const double y )
	{
		m_x = x;
		m_y = y;
	}

    double operator()(const CBlob &blob) const;

private:
	// coordenades del punt on volem calcular la dist�ncia
	double m_x, m_y;
};

//! Classe per calcular el nombre de pixels externs d'un blob
//! Class to get the number of extern pixels of a blob
class CBlobGetExternPixels: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		return blob.ExternPixels();
	}
};

//! Classe per calcular el ratio entre el perimetre i nombre pixels externs
//! valors propers a 0 indiquen que la majoria del blob �s intern
//! valors propers a 1 indiquen que la majoria del blob �s extern
//! Class to calculate the ratio between the perimeter and the number of extern pixels
class CBlobGetExternPerimeterRatio: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		if( blob.Perimeter() != 0 )
			return blob.ExternPixels() / blob.Perimeter();
		else
			return blob.ExternPixels();
	}
};

//! Classe per calcular el ratio entre l'�rea i nombre pixels externs
//! valors propers a 0 indiquen que la majoria del blob �s intern
//! valors propers a 1 indiquen que la majoria del blob �s extern
//! Class to calculate the ratio between the area and the number of extern pixels
class CBlobGetExternAreaRatio: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		if( blob.Area() != 0 )
			return blob.ExternPixels() / blob.Area();
		else
			return blob.ExternPixels();
	}
};

//! Classe per calcular el centre en el eix X d'un blob
//! Class to calculate the center in the X direction
class CBlobGetXCenter: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		return blob.MinX() + (( blob.MaxX() - blob.MinX() ) / 2.0);
	}
};

//! Classe per calcular el centre en el eix Y d'un blob
//! Class to calculate the center in the Y direction
class CBlobGetYCenter: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		return blob.MinY() + (( blob.MaxY() - blob.MinY() ) / 2.0);
	}
};

//! Classe per calcular la longitud de l'eix major d'un blob
//! Class to calculate the length of the major axis of the ellipse that fits the blob edges
class CBlobGetMajorAxisLength: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		CvBox2D elipse = blob.GetEllipse();

		return elipse.size.width;
	}
};

//! Classe per calcular la longitud de l'eix menor d'un blob
//! Class to calculate the length of the minor axis of the ellipse that fits the blob edges
class CBlobGetMinorAxisLength: public COperadorBlob
{
public:
    double operator()(const CBlob &blob) const
	{
		CvBox2D elipse = blob.GetEllipse();

		return elipse.size.height;
	}
};

#endif //CBLOB_INSPECTA_INCLUDED
