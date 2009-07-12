/****************************************************************************
* File Name:        VO_IO.h                                                 *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     IO for reading images and annotations files             *
* Used Library:     STL + OpenCV + Boost                                    *
****************************************************************************/


#ifndef  __VO_IO__
#define  __VO_IO__

#include <vector>
#include <iostream>               // for std::cout

#include <boost/filesystem.hpp>   // includes all needed Boost.Filesystem declarations

#include "cv.h"

#include "VO_AAMShape2D.h"

using namespace std;
using namespace boost::filesystem;


/****************************************************************************
* Class Name:       VO_IO                                                   *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         IO for reading images and annotations files             *
****************************************************************************/
class VO_IO
{
public:
    // scan all files with the "file_extension" under "dir_path" directory and sort them
    static bool       		ScanNSortDirectory(const path &dir_path, const string &file_extension, vector<string>& files);

    // scan all annotation files
    static vector<string>	ScanNSortAnnotationInDirectory(const string &dir_path);

    // scan all image files
    static vector<string>	ScanNSortImagesInDirectory(const string &dir_path);

};

#endif  // __VO_IO__

