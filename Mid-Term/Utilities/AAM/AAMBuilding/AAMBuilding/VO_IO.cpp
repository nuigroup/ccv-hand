/****************************************************************************
* File Name:        VO_IO.cpp                                               *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     IO for reading images and annotations files             *
* Used Library:     STL + OpenCV + Boost                                    *
****************************************************************************/


#include <sstream>
#include <string>
#include <iostream>
#include <fstream>

#include "VO_IO.h"

using namespace std;
using namespace boost::filesystem;


/** local (static) compare function for the qsort() call */
static int str_compare( const void *arg1, const void *arg2 )
{
    return strcmp( ( *(string*)arg1).c_str (), ( *(string*)arg2).c_str () );
}


/**

    @author     JIA Pei

    @version    2007-08-08

    @brief      Scan a directory and sort all files with a specified extension

    @param      dir_path            input parameter, path to read from

    @param      file_extension      input parameter, file extension to search for

    @param      files               output parameter, file names found without any path

    @return     bool                fail to scan or not

    @note		http://www.boost.org/libs/filesystem/doc/index.htm

*/
bool VO_IO::ScanNSortDirectory(const path &dir_path, const string &file_extension, vector<string>& files)
{
    if ( !exists( dir_path ) ) 
    {
        return false;
    }
    	
    directory_iterator end_itr;

    for ( directory_iterator itr(dir_path); itr != end_itr; ++itr )
    {
        if ( is_directory(itr->status()) )
        {
            VO_IO::ScanNSortDirectory(itr->path(), file_extension, files);
        }
        else if ( extension( itr->path() ) == file_extension )
        {
            files.push_back( itr->path().string() );
        }
    }

    return true;
}


/**

    @author     JIA Pei

    @version    2007-08-08

    @brief      Scan a directory and sorts all file in format of .pts or .asf

    @param      dir_path            input parameter, path to read from

    @return     vector<string>      A vector of all scanned file names

*/
vector<string> VO_IO::ScanNSortAnnotationInDirectory(const string &dir_path)
{
    vector<string> annotationALL;
    vector< vector<string> > annotations;
    annotations.resize(2);				// 2 types, pts, asf
    for(unsigned int i = 0; i < annotations.size(); i++)
    {
        annotations[i].resize(0);
    }

    path tempp(dir_path, native );

    VO_IO::ScanNSortDirectory( tempp, ".pts", annotations[0]);
    VO_IO::ScanNSortDirectory( tempp, ".asf", annotations[1]);
	
    for(unsigned int i = 0; i < annotations.size(); i++)
    {
        for(unsigned int j = 0; j < annotations[i].size(); j++)
        {
            annotationALL.push_back(annotations[i][j]);
        }
    }

	// sort the filenames
    if(annotationALL.size() > 0)
    {
        qsort( (void *)&(annotationALL[0]), (size_t)annotationALL.size(), sizeof(string), str_compare );
    }

    return annotationALL;
}

/**

    @author     JIA Pei

    @version    2007-08-08

    @brief      Scan a directory and sort all files in format of .jpg .bmp .ppm .pgm .png .gif .tif

    @param      dir_path            input parameter, path to read from

    @return     vector<string>      A vector of all scanned file names

*/
vector<string> VO_IO::ScanNSortImagesInDirectory(const string &dir_path)
{
    vector<string> imgALL;
    vector< vector<string> > imgs;
    imgs.resize(7);
    for(unsigned int i = 0; i < imgs.size(); i++)
    {
        imgs[i].resize(0);
    }

    path tempp(dir_path, native );

    VO_IO::ScanNSortDirectory( tempp, ".jpg", imgs[0]);
    VO_IO::ScanNSortDirectory( tempp, ".bmp", imgs[1]);
    VO_IO::ScanNSortDirectory( tempp, ".ppm", imgs[2]);
    VO_IO::ScanNSortDirectory( tempp, ".pgm", imgs[3]);
    VO_IO::ScanNSortDirectory( tempp, ".png", imgs[4]);
    VO_IO::ScanNSortDirectory( tempp, ".gif", imgs[5]);
    VO_IO::ScanNSortDirectory( tempp, ".tif", imgs[6]);
	
    for(unsigned int i = 0; i < imgs.size(); i++)
    {
        for(unsigned int j = 0; j < imgs[i].size(); j++)
        {
            imgALL.push_back(imgs[i][j]);
        }
    }

	// sort the filenames
    if(imgALL.size() > 0)
    {
        qsort( (void *)&(imgALL[0]), (size_t)imgALL.size(), sizeof(string), str_compare );
    }

    return imgALL;
}



