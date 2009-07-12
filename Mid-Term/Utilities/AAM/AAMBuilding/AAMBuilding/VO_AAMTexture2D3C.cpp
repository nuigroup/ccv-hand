/****************************************************************************
* File Name:        VO_AAMTexture2D3C.cpp                                   *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM texture model, extracted from 2D images with        *
*                   3 channels                                              *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/


#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "VO_AAMTexture2D3C.h"

using namespace std;

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator= overloading, similar to copy constructor

    @param      iTexture	The input texture

    @return     VO_AAMTexture2D3C&

*/
VO_AAMTexture2D3C& VO_AAMTexture2D3C::operator=(const VO_AAMTexture2D3C &iTexture)
{
    this->m_vTextures.resize( iTexture.GetSize () );

    // copy class data members
    this->m_vTextures       = iTexture.GetTextures();

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator= overloading, similar to copy constructor

    @param      value	assign all values in VO_AAMTexture2D3C to value

    @return     VO_AAMTexture2D3C&

*/
VO_AAMTexture2D3C& VO_AAMTexture2D3C::operator=(float value)
{
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        this->m_vTextures[i] = value;
    }

    return *this;
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator+ overloading, shift one AAM texture by value

    @param      value

    @return     VO_AAMTexture2D3C

*/
VO_AAMTexture2D3C VO_AAMTexture2D3C::operator+(float value)
{
	VO_AAMTexture2D3C res(*this);
	size_t len = this->m_vTextures.size ();

	for (unsigned int i = 0; i < len; i++)
	{
		res.m_vTextures[i] += value;
	}

	return res;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator+= overloading, add value to this AAM texture 

    @param      value

    @return     VO_AAMTexture2D3C&

*/
VO_AAMTexture2D3C& VO_AAMTexture2D3C::operator+=(float value)
{
	size_t len = this->m_vTextures.size ();

	for (unsigned int i = 0; i < len; i++)
	{
		this->m_vTextures[i] += value;
	}

	return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator+ overloading, add two AAM texture to one 

    @param      iTexture      the added AAM texture

    @return     VO_AAMTexture2D3C

*/
VO_AAMTexture2D3C VO_AAMTexture2D3C::operator+(const VO_AAMTexture2D3C &iTexture)
{
    VO_AAMTexture2D3C res(*this);
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        res.m_vTextures[i] += iTexture.m_vTextures[i];
    }

    return res;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator+= overloading, add the input AAM texture to this AAM texture

    @param      iTexture      the added AAM texture

    @return     VO_AAMTexture2D3C&

*/
VO_AAMTexture2D3C& VO_AAMTexture2D3C::operator+=(const VO_AAMTexture2D3C &iTexture)
{
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        this->m_vTextures[i] += iTexture.m_vTextures[i];
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator- overloading, shift one AAM texture by -value

    @param      value

    @return     VO_AAMTexture2D3C

*/
VO_AAMTexture2D3C VO_AAMTexture2D3C::operator-(float value)
{
    VO_AAMTexture2D3C res(*this);
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        res.m_vTextures[i] -= value;
    }

    return res;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator-= overloading, subtract value from this AAM texture 

    @param      value

    @return     VO_AAMTexture2D3C&

*/
VO_AAMTexture2D3C& VO_AAMTexture2D3C::operator-=(float value)
{
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        this->m_vTextures[i] -= value;
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator- overloading, subtract one texture from another

    @param      iTexture      the subtracted AAM texture

    @return     VO_AAMTexture2D3C

*/
VO_AAMTexture2D3C VO_AAMTexture2D3C::operator-(const VO_AAMTexture2D3C &iTexture)
{
    VO_AAMTexture2D3C res(*this);
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        res.m_vTextures[i] -= iTexture.m_vTextures[i];
    }

    return res;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator-= overloading, subtract the input AAM texture from this AAM texture

    @param      iTexture      the subtracted AAM texture

    @return     VO_AAMTexture2D3C&

*/
VO_AAMTexture2D3C& VO_AAMTexture2D3C::operator-=(const VO_AAMTexture2D3C &iTexture)
{
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        this->m_vTextures[i] -= iTexture.m_vTextures[i];
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator* overloading, scale one texture by value

    @param      value      scale size

    @return     VO_AAMTexture2D3C

*/
VO_AAMTexture2D3C VO_AAMTexture2D3C::operator*(float value)
{
    VO_AAMTexture2D3C res(*this);
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        res.m_vTextures[i] *= value;
    }

    return res;
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator-= overloading, scale this AAM texture by value

    @param      value      scale size

    @return     VO_AAMTexture2D3C&

*/
VO_AAMTexture2D3C& VO_AAMTexture2D3C::operator*=(float value)
{
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        this->m_vTextures[i] *= value;
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator* overloading, dot product of two AAM textures

    @param      iTexture      AAM texture to be dot producted

    @return     float       dot product

*/
float VO_AAMTexture2D3C::operator*(const VO_AAMTexture2D3C &iTexture)
{
    float result = 0.;
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        result += this->m_vTextures[i] * iTexture.m_vTextures[i];
    }

    return result;
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator/ overloading, scale a texture

    @param      value      1.0/value = scale size

    @return     VO_AAMTexture2D3C

*/
VO_AAMTexture2D3C VO_AAMTexture2D3C::operator/(float value)
{
    assert (value != 0);
    size_t len = this->m_vTextures.size ();

    VO_AAMTexture2D3C res(*this);

    for (unsigned int i = 0; i < len; i++)
    {
        res.m_vTextures[i] /= value;
    }

    return res;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      operator/= overloading, scale this texture with input float value

    @param      value      1.0/value = the scaled value

    @return     VO_AAMTexture2D3C&

*/
VO_AAMTexture2D3C& VO_AAMTexture2D3C::operator/=(float value)
{
    assert (value != 0);
    size_t len = this->m_vTextures.size ();

    for (unsigned int i = 0; i < len; i++)
    {
        this->m_vTextures[i] /= value;
    }

    return *this;
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      append a vector at the end of m_vTextures

    @param      v      the vector to be added

    @return     VO_AAMTexture2D3C&

*/
void VO_AAMTexture2D3C::Append(const vector<float> &v)
{
    size_t len = v.size ();
    for (unsigned int i = 0; i < len; i++)
    {
        this->m_vTextures.push_back (v[i]);
    }    
}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      scale 

    @param      s      scale this AAM texture by s

    @return     VO_AAMTexture2D3C

*/
void VO_AAMTexture2D3C::Scale( float s)
{
    for(unsigned int i = 0; i < this->m_vTextures.size(); i++)
    {
        this->m_vTextures[i] *= s;
    }
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      calculate 2-norm of AAM texture

    @return     2-norm of AAM texture

*/
float VO_AAMTexture2D3C::Norm2() const
{
    float norm = 0.0;
    size_t len = this->m_vTextures.size ();

    for(unsigned int i = 0; i < len; i++)
    {
        norm += pow( (float) this->m_vTextures[i], 2 );
    }
    norm = sqrt (norm);

    return (norm);

}

/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      Normalize this AAM texture

    @return     void

*/
void VO_AAMTexture2D3C::Normalize()
{
    float norm = this->Norm2();
    
    if(norm > FLT_EPSILON)
    {
        size_t len = this->m_vTextures.size ();

        for (unsigned int i = 0; i < len; i++)
        {
            this->m_vTextures[i] /= norm;
        }
    }
}


/**

    @author     JIA Pei

    @version    2006-04-13

    @brief      Standardize this AAM texture

    @float      2-order central moment

    @note       MEAN0NORM1 -- make the texture mean = 0; unit = 1; normalization (normal distribution)
                VARY01 -- more easily, just scale - between 0 and 1

*/
float VO_AAMTexture2D3C::Standardize(int mtd)
{
    switch (mtd)
    {
    case VARY01:
        {
            vector<float>::const_iterator templ_min = min_element(this->m_vTextures.begin(), this->m_vTextures.end() );
            vector<float>::const_iterator templ_max = max_element(this->m_vTextures.begin(), this->m_vTextures.end() );
            // Noted by JIA Pei. Don't use *iMin and *iMax directly!! Assign to a float first !! My god!!
            float templ_minimum = *templ_min;
            float templ_maximum = *templ_max;
            float templ_variance = templ_maximum - templ_minimum;

            if (fabs(templ_variance) > FLT_EPSILON)
            {
                for (unsigned int i = 0; i < this->m_vTextures.size(); i++)
                {
                    this->m_vTextures[i] = (this->m_vTextures[i] - templ_minimum) / templ_variance;
                }
            }

            float sqsum = this->Norm2();

            return sqrt(sqsum);
        }
        break;
    case MEAN0NORM1:
    default:
        {
            float mean = 0.;
            float sqsum = 0.;

            mean    = this->Sum();
            mean    /=  this->m_vTextures.size();

            for (unsigned int i = 0; i < this->m_vTextures.size(); i++)
            {
                this->m_vTextures[i]  -= mean;
                sqsum += this->m_vTextures[i] * this->m_vTextures[i];
            }

            if ( sqsum <= FLT_EPSILON )
            {
                cout << "The texture vector can't scaled to unit one" << endl;
                exit(1);
            }
            else
            {
                float temp = 1.0f/sqrt(sqsum);
                for (unsigned int i = 0; i < this->m_vTextures.size(); i++)
                {
                    this->m_vTextures[i] *= temp;
                }
            }

            return sqrt(sqsum);
        }
        break;

    }
}

/**

    @author	    JIA Pei

    @version	2006-09-02

    @brief		Sum up all values in texture

    @return	    float   texture summation

*/
float VO_AAMTexture2D3C::Sum() const
{
    float sum = std::accumulate(this->m_vTextures.begin(), this->m_vTextures.end(), (float)0.0 );
    return sum;
}



/**

    @author     JIA Pei

    @version    2008-03-04

    @brief      Format change - from CvMat* to VO_AAMTexture2D3C

*/
VO_AAMTexture2D3C VO_AAMTexture2D3C::VO_CvMat2AAMTexture(const CvMat* iTexture)
{
    unsigned int NbOfTextures = iTexture->cols;
    VO_AAMTexture2D3C resTexture;
    resTexture.SetSize(NbOfTextures);

    for(unsigned int i = 0; i < NbOfTextures; i++)
    {
        resTexture.SetATexture( i, CV_MAT_ELEM( *iTexture, float, 0, i) );
    }

    return resTexture;
}


/**

    @author     JIA Pei

    @version    2008-03-04

    @brief      Format change - from VO_AAMTexture2D3C to CvMat*

*/
void VO_AAMTexture2D3C::VO_AAMTexture2CvMat(CvMat* resTexture) const
{
    for (unsigned int i = 0; i < this->m_vTextures.size(); i++)
    {
        CV_MAT_ELEM( *resTexture, float, 0, i)      = this->m_vTextures[i];
    }
}

