/****************************************************************************
* File Name:        VO_ASM.cpp                                              *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     ASM 2D shape model                                      *
* Used Library:     STL + OpenCV + Boost                                    *
****************************************************************************/


#include <fstream>
#include <set>

#include <boost/filesystem.hpp>

#include "VO_ASM.h"


using namespace std;

/** Default Constructor */
VO_ASM::VO_ASM()
{
    this->init();
}


/** Initialization */
void VO_ASM::init()
{
    this->m_CVMShapes										= NULL;
    this->m_CVMAlignedShapes								= NULL;
    this->m_CVMAlignedShapesProject2Truncated				= NULL;
    this->m_CVMTruancatedAlignedShapesMean					= NULL;
    this->m_CVMTruancatedAlignedShapesSD					= NULL;
    this->m_CVMAlignedShapesMean							= NULL;
    this->m_CVMAlignedShapesSD								= NULL;
    this->m_CVMAlignedShapesEigenVectors					= NULL;
    this->m_CVMAlignedShapesEigenValues						= NULL;
    this->m_CVMTruncatedAlignedShapesEigenVectors			= NULL;
    this->m_CVMTruncatedAlignedShapesEigenValues			= NULL;
    this->m_CVMSimilarityTransformMatrix					= NULL;
    this->m_CVMPoints										= NULL;
    this->m_CVMConvexHull									= NULL;
    this->m_iNbOfSamples									= 0;
    this->m_iNbOfPoints										= 0;
    this->m_iNbOfShapes										= 0;
    this->m_iNbOfTruncatedShapes							= 0;
    this->m_iNbOfEigenShapesAtMost							= 0;
    this->m_fAverageShapeSize                               = 0.0f;
    this->m_iNbOfPointsOnConvexHull							= 0;
    this->m_iNbOfEdges										= 0;
    this->m_iNbOfTriangles									= 0;
    this->m_fTruncatedPercent_Shape                         = 0.95f;

}


/** Destructor */
VO_ASM::~VO_ASM()
{
    if (this->m_CVMShapes)									cvReleaseMat (&this->m_CVMShapes);
    if (this->m_CVMAlignedShapes)							cvReleaseMat (&this->m_CVMAlignedShapes);
    if (this->m_CVMAlignedShapesProject2Truncated)			cvReleaseMat (&this->m_CVMAlignedShapesProject2Truncated);
    if (this->m_CVMTruancatedAlignedShapesMean)				cvReleaseMat (&this->m_CVMTruancatedAlignedShapesMean);
    if (this->m_CVMTruancatedAlignedShapesSD)				cvReleaseMat (&this->m_CVMTruancatedAlignedShapesSD);
    if (this->m_CVMAlignedShapesMean)						cvReleaseMat (&this->m_CVMAlignedShapesMean);
    if (this->m_CVMAlignedShapesSD)							cvReleaseMat (&this->m_CVMAlignedShapesSD);
    if (this->m_CVMAlignedShapesEigenVectors)				cvReleaseMat (&this->m_CVMAlignedShapesEigenVectors);
    if (this->m_CVMAlignedShapesEigenValues)				cvReleaseMat (&this->m_CVMAlignedShapesEigenValues);
    if (this->m_CVMTruncatedAlignedShapesEigenVectors)		cvReleaseMat (&this->m_CVMTruncatedAlignedShapesEigenVectors);
    if (this->m_CVMTruncatedAlignedShapesEigenValues)		cvReleaseMat (&this->m_CVMTruncatedAlignedShapesEigenValues);
    if (this->m_CVMSimilarityTransformMatrix)				cvReleaseMat (&this->m_CVMSimilarityTransformMatrix);
    if (this->m_CVMPoints)									cvReleaseMat (&this->m_CVMPoints);
    if (this->m_CVMConvexHull)								cvReleaseMat (&this->m_CVMConvexHull);
}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Align all shapes before PCA

@param	    vShapes         Input - all shapes before alignment

@param	    alignedShapes   Output - all shapes after alignment

@return     float           return average shape size of all shapes

@note       make sure all alignedShapes, meanshape as well, are of size "1" !!!

*/
float VO_ASM::VO_AlignAllShapes(const vector<VO_AAMShape2D>& vShapes, vector<VO_AAMShape2D>& alignedShapes)
{
    unsigned int numberOfSamples = vShapes.size();
    unsigned int numberOfPoints  = vShapes[0].GetSize();
    float averageShapeSize       = 0.0f;

    alignedShapes = vShapes;

    VO_AAMShape2D meanAlignedShape;
    meanAlignedShape.SetSize(numberOfPoints);

    for(unsigned int i = 0; i < numberOfSamples; i++)
    {
        averageShapeSize += alignedShapes[i].Normalize(); // move to origin and scale to unit size
    }
    averageShapeSize /= numberOfSamples;    // Why this is the average shape size without minor errors? 
    // Because later do-while loop doesn't change the normalized shape size

    VO_ASM::VO_CalcMeanShape( alignedShapes, meanAlignedShape );
    meanAlignedShape.SetHostImage("");

    // do a number of alignment iterations until the mean shape estimate is stable
    float diff, diff_max = FLT_EPSILON;
    int max_iter = EPOCH, iter = 1;
    VO_AAMShape2D tempMeanShape;
    float theta;

    CvMat * rot = cvCreateMat (1, numberOfSamples, CV_32FC1);

    do
    {
        // normalize and align all other shapes to the mean shape estimate
        for(unsigned int i=0;i < numberOfSamples;i++)
        {
            // align the i-th shape to the estimate of the mean shape
            alignedShapes[i].AlignTo( meanAlignedShape, &theta );
            CV_MAT_ELEM( *rot, float, 0, i )  = theta;  // record the rotation

            // re-scale to unit size to avoid the so-called 'shrinking effect'
            // i.e. the alignment error goes towards zero, when the shapes are downscaled
            alignedShapes[i].Scale( 1.0f/alignedShapes[i].GetShapeNorm () );
        }
        tempMeanShape = meanAlignedShape;

        // estimate the new mean shape
        VO_ASM::VO_CalcMeanShape( alignedShapes, meanAlignedShape );

        // if first iteration, adjust the orientation of the mean shape, so the rotation of the training set to the mean shape is 
        // -- on average -- zero or put more clearly: "make the meanshape have a mean orientation"
        if (iter==1)
        {
            CvScalar rotMean = cvAvg( rot );
            meanAlignedShape.Rotate ( (float) ( -rotMean.val[0] ) );
        }

        diff = (tempMeanShape-meanAlignedShape).GetShapeSize();

        ++iter;

    }while( fabs(diff)/meanAlignedShape.GetShapeSize() > diff_max && iter < max_iter );

    cvReleaseMat (&rot);

    return averageShapeSize;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Rescale the alignedshapes so that meanshape's 2-norm could be "1"

@param      meanAlignedShape    Input - meanalignedshape that all alignedshapes should rescale to

@param      alignedShapes       Input and Output - aligned shape and aligned rescaled shape

*/
void VO_ASM::VO_RescaleAllAlignedShapes(const VO_AAMShape2D& meanAlignedShape,
                                        vector<VO_AAMShape2D>& alignedShapes)
{
    unsigned int NbOfSamples = alignedShapes.size();

    // Explained by JIA Pei. 2008-03-04. scale back so that the mean shape size is equal to 1.0
    for(unsigned int i = 0; i < NbOfSamples; i++)
    {
        float ts = sqrt (alignedShapes[i]*meanAlignedShape);
        alignedShapes[i].Scale( 1.0f/ts );
    }
}



/**

@author     JIA Pei

@version    2008-03-24

@brief      Rescale the alignedShape to the already trained meanAligneShape

@param      meanAlignedShape    Input - meanalignedshape that all alignedshapes should rescale to

@param      alignedShape        Input and Output - aligned shape and aligned rescaled shape

*/
void VO_ASM::VO_RescaleOneAlignedShape(const VO_AAMShape2D& meanAlignedShape, VO_AAMShape2D& alignedShape)
{
    float ts = sqrt (alignedShape*meanAlignedShape);
    alignedShape.Scale( 1.0f/ts );
}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate mean shape

@param      vShapes     Input - all shapes

@param      meanShape   Output - mean shape

*/
void VO_ASM::VO_CalcMeanShape(const vector<VO_AAMShape2D>& vShapes, VO_AAMShape2D &meanShape)
{
    unsigned int numberOfSamples    = vShapes.size();
    unsigned int numberOfShapes     = vShapes[0].GetSize();
    meanShape.SetSize(numberOfShapes);
    meanShape = 0;

    for(unsigned int i = 0; i < numberOfSamples; i++)
    {
        meanShape += vShapes[i];
    }

    meanShape = meanShape / (float)numberOfSamples;

}




/**

@author     JIA Pei

@version    2008-03-04

@brief      Judge is point "pt" inside the convex hull "ch"

@param      pt              Input - the point

@param      ch              Input - convex hull

@param      includingHull	Input flag - whether including the boundary

@return     bool            including or excluding

*/
bool VO_ASM::VO_IsPointInConvexHull(CvPoint2D32f pt, const CvMat* ch, bool includingHull)
{
    if(!includingHull)
    {
        if (cvPointPolygonTest( ch, pt, 0 ) > 0.0 )
            return true;
    }
    else
    {
        if (cvPointPolygonTest( ch, pt, 0 ) >= 0.0)
            return true;
    }

    return false;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Judge whether edge indexed by (ind1+ind2) is already in the vector of "edges"

@param      edges           edge collection

@param      ind1            first index of the edge to be judged

@param      ind2	        second index of the edge to be judged

@return     bool            counted or not

*/
bool VO_ASM::VO_IsEdgeCounted(const vector<VO_AAMEdge>& edges, int ind1, int ind2)
{
    for (unsigned int i = 0; i < edges.size (); i++)
    {
        if ( ( (edges[i].GetIndex1() == ind1) && (edges[i].GetIndex2() == ind2) )
            || ( (edges[i].GetIndex1() == ind2) && (edges[i].GetIndex2() == ind1) ) )
        {
            return true;
        }
    }

    return false;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Judge whether triangle t is already in the vector of "triangles".

@param      triangles       triangle collection

@param      t               triangle to be judged

*/
bool VO_ASM::VO_IsTriangleCounted(const vector<VO_AAMTriangle2D> &triangles, const vector<int> t)
{
    set<int> tTriangle;
    set<int> sTriangle;

    for (unsigned int i = 0; i < triangles.size (); i ++)
    {
        // These two clear() are very important, cannot be displaced by empty().
        tTriangle.clear ();
        sTriangle.clear ();
        for (unsigned int j = 0; j < 3; j++ )
        {
            tTriangle.insert (triangles[i].GetVertexIndex(j) );
            sTriangle.insert (t[j]);
        }
        if (tTriangle == sTriangle)
        {
            return false;
        }
    }

    return true;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Build convex hull

@param      vertexes        Input - the point

@param      ch              Output - convex hull

@return     int             number of edges on the convex hull 

*/
int VO_ASM::VO_BuildConvexHull(const CvMat* vertexes, CvMat* ch)
{
    cvConvexHull2(vertexes, ch, CV_CLOCKWISE, 0 );

    return ch->cols;
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Build AAM Edges

@param      vertexes            Input - all vertexes/points composing the shape

@param      Subdiv              Input - sub division which is already computed beforehand

@param      outEdges	        Output - AAM edges

@return     int                 Number of edges

*/
int VO_ASM::VO_BuildEdges(const CvMat* vertexes, const CvSubdiv2D* Subdiv, vector<VO_AAMEdge>& outEdges)
{
    unsigned int NbOfPoints = vertexes->cols;
    CvSeqReader  reader;

    cvStartReadSeq( (CvSeq*)(Subdiv->edges), &reader, 0 );

    for( unsigned int i = 0; i < Subdiv->edges->total; i++ )
    {
        CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

        if( CV_IS_SET_ELEM( edge ))
        {
            CvPoint2D32f org;
            CvPoint2D32f dst;

            CvSubdiv2DPoint* org_pt = cvSubdiv2DEdgeOrg((CvSubdiv2DEdge)edge);
            CvSubdiv2DPoint* dst_pt = cvSubdiv2DEdgeDst((CvSubdiv2DEdge)edge);

            if( org_pt && dst_pt )
            {
                org = org_pt->pt;
                dst = dst_pt->pt;

                for (unsigned int j = 0; j < NbOfPoints; j++)
                {
                    // if the current edge convex (org points, not the dst point) is in our point list
                    if ( (fabs ( org.x - CV_MAT_ELEM( *vertexes, CvPoint2D32f, 0, j ).x ) < FLT_EPSILON )
                        && ( fabs ( org.y - CV_MAT_ELEM( *vertexes, CvPoint2D32f, 0, j ).y ) < FLT_EPSILON ) )
                    {
                        for (unsigned int k = 0; k < NbOfPoints; k++)
                        {
                            // With the above org point, we search around for the dst point(s), 
                            // which make org-dst an edge during cvSubdivDelaunay2DInsert()
                            if ( ( fabs (dst.x - CV_MAT_ELEM( *vertexes, CvPoint2D32f, 0, k ).x ) < FLT_EPSILON )
                                && ( fabs (dst.y - CV_MAT_ELEM( *vertexes, CvPoint2D32f, 0, k ).y ) < FLT_EPSILON ) )
                            {
                                // Already tested, this->m_vAAMEdge is definitely correct!
                                outEdges.push_back ( VO_AAMEdge(j,k) );
                            }
                        }
                    }
                }
            }
        }

        CV_NEXT_SEQ_ELEM( Subdiv->edges->elem_size, reader );
    }

    return outEdges.size();
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Build AAM Triangles

@param      edges               Input - to build triangles

@return     int                 Number of triangles

*/
int VO_ASM::VO_BuildTriangles(const CvMat* vertexes, const vector<VO_AAMEdge> &edges, vector<VO_AAMTriangle2D> &outTriangles)
{
    outTriangles.clear();

    unsigned int NbOfPoints = vertexes->cols;
    for (unsigned int i = 0; i < edges.size (); i++)
    {
        int ind1 = edges[i].GetIndex1();
        int ind2 = edges[i].GetIndex2();

        for (unsigned int j = 0; j < NbOfPoints; j++)
        {
            // At most, there are only 2 triangles that can be added 
            if( VO_ASM::VO_IsEdgeCounted(edges, ind1, j) && VO_ASM::VO_IsEdgeCounted(edges, ind2, j) )
            {
                vector<CvPoint2D32f> vVertexes;
                vector<int> iVertexes;
                vVertexes.resize (3);
                iVertexes.resize (3);
                vVertexes[0] = CV_MAT_ELEM( *vertexes, CvPoint2D32f, 0, ind1 );
                vVertexes[1] = CV_MAT_ELEM( *vertexes, CvPoint2D32f, 0, ind2 );
                vVertexes[2] = CV_MAT_ELEM( *vertexes, CvPoint2D32f, 0, j );
                iVertexes[0] = ind1;
                iVertexes[1] = ind2;
                iVertexes[2] = j;
                if (VO_ASM::VO_IsTriangleCounted(outTriangles, iVertexes) )
                {
                    outTriangles.push_back (VO_AAMTriangle2D(vVertexes, iVertexes));
                }
            }
        }
    }

    return outTriangles.size();
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculation triangulation mesh
For IMM 58 points dataset, 19 edges on the convex hull, 133 inner edges.
Totally, 133+19=152 edges
(133*2+19)/3=95 triangles

*/
void VO_ASM::VO_BuildTemplateMesh(const CvMat* vertexes, CvMat* ch, vector<VO_AAMEdge> & edges, vector<VO_AAMTriangle2D> & triangles )
{
    unsigned int NbOfPoints = vertexes->cols;

    // Build Convex Hull
    unsigned int NbOfEdgesOnConvexHull = VO_ASM::VO_BuildConvexHull(vertexes, ch);

    //////////////////////////////////////////////////////////////////////////
    // Build Delaunay Triangulation Sub Divisions
    // Later VO_BuildEdges need DTSubdiv information
    CvSubdiv2D* tempCVSubdiv = NULL;

    CvRect rect = cvBoundingRect( ch, 0 );

    CvMemStorage* DelaunayStorage = cvCreateMemStorage(0);

    // By JIA Pei, 2006-09-20. How to release this storage?
    tempCVSubdiv = cvCreateSubdivDelaunay2D( rect, DelaunayStorage );

    for( unsigned int i = 0; i < NbOfPoints; i++ )
    {
        cvSubdivDelaunay2DInsert( tempCVSubdiv, CV_MAT_ELEM( *vertexes, CvPoint2D32f, 0, i ));
    }

    //////////////////////////////////////////////////////////////////////////

    unsigned int NbOfEdges = VO_ASM::VO_BuildEdges(vertexes, tempCVSubdiv, edges);
    unsigned int NbOfTriangles = VO_ASM::VO_BuildTriangles (vertexes, edges, triangles);

    // How to release CvSubdiv2D* m_CVSubdiv is still a problem.
    if (tempCVSubdiv)	cvClearSubdivVoronoi2D( tempCVSubdiv );
    cvReleaseMemStorage( &DelaunayStorage );

}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Calculate Shape Boundary

@param      the shape

@return     bool

*/
CvRect VO_ASM::VO_CalcShapeBoundaryRect(const VO_AAMShape2D& iShape)
{
    CvRect rect;

    unsigned int NbOfPoints =  iShape.GetSize();
    CvMat* tempPoints = cvCreateMat (1, NbOfPoints, CV_32FC2);
    for (unsigned int i = 0; i < NbOfPoints; i++)
    {
        CV_MAT_ELEM( *tempPoints, CvPoint2D32f, 0, i ) = iShape.GetPoint(i).GetXY();
    }

    rect = cvBoundingRect( tempPoints, 1 );

    cvReleaseMat(&tempPoints);

    return rect;
}



/**

@author     JIA Pei

@version    2008-03-04

@brief      Is iShape inside img

@return     bool

*/
bool VO_ASM::VO_IsShapeInsideImage(const VO_AAMShape2D& iShape, const IplImage* img)
{
    CvRect rect = VO_ASM::VO_CalcShapeBoundaryRect(iShape);

    if ( (rect.x >= 0) && (rect.y >= 0) && ( (rect.x + rect.width) <= img->width )
        && ( (rect.y + rect.height) <= img->height ) )
    {
        return true;
    }
    else
        return false;
}




/**

@author     JIA Pei

@version    2008-03-04

@brief      Aligned shape to reference scale shape

@param      inShape         Input - aligned shape

@param      shapeSD         Input - shape standard deviation

@param      outShape        Output - output reference shape

*/
void VO_ASM::VO_AlignedShape2ReferenceScale(const VO_AAMShape2D& inShape, float shapeSD, VO_AAMShape2D& outShape)
{
    outShape = const_cast<VO_AAMShape2D&> (inShape) * shapeSD;

    outShape.Translate( -outShape.MinX(), -outShape.MinY() );
}


/**

@author     JIA Pei

@version    2008-03-04

@brief      Reference scale texture back to aligned one

@param      inShape         Input - reference shape

@param      shapeSD         Input - shape standard deviation

@param      outShape        Output - output aligned shape

*/
void VO_ASM::VO_ReferenceShapeBack2Aligned(const VO_AAMShape2D& inShape, float shapeSD, VO_AAMShape2D& outShape)
{
    outShape = inShape;

    // scale first
    outShape.Scale(1.0f/shapeSD );

    // centralize it
    float x, y;
    outShape.CenterOfGravity( x, y );
    outShape.Translate( -x, -y );

    // note: no rotation!!!
}



/**

@author     JIA Pei

@version    2008-03-20

@brief      shape parameters back project to aligned shape

@param      inP             Input - input shape parameters

@return     VO_AAMShape2D   - the back projected shape

*/
VO_AAMShape2D VO_ASM::VO_SParamBackProjectToAlignedShape(CvMat* inP)
{
    CvMat* oShape = cvCreateMat(1, this->m_iNbOfShapes, CV_32FC1);

    // P back to shape
    cvBackProjectPCA(inP, this->m_CVMAlignedShapesMean, this->m_CVMTruncatedAlignedShapesEigenVectors, oShape);
    VO_AAMShape2D res   =   VO_AAMShape2D::VO_CvMat2AAMShape2D(oShape);

    cvReleaseMat(&oShape);

    return res;
}



/**

@author     JIA Pei

@version    2008-03-04

@brief      build ASM

@param      vShapes     Input - all input shapes

@param      TPShape     Input - truncated percentage for shape model
*/
void VO_ASM::VO_BuildASM(const vector<VO_AAMShape2D> &vShapes, float TPShape)
{
    this->m_iNbOfSamples            = vShapes.size();
    this->m_iNbOfPoints             = vShapes[0].GetSize();
    this->m_iNbOfShapes             = this->m_iNbOfPoints * 2;
    this->m_fTruncatedPercent_Shape = TPShape;
    this->m_vAAMShapes              = vShapes;

    // Align all shapes
    this->m_fAverageShapeSize = VO_ASM::VO_AlignAllShapes(this->m_vAAMShapes, this->m_vAAMAlignedShapes);
    VO_ASM::VO_CalcMeanShape(this->m_vAAMAlignedShapes, this->m_VOAAMMeanAlignedShapePrior);
    VO_ASM::VO_RescaleAllAlignedShapes(this->m_VOAAMMeanAlignedShapePrior,this->m_vAAMAlignedShapes);
    // Calculate meanshape, now, bear in mind that all alignedshapes and the meanshape are all of size "1"
    VO_ASM::VO_CalcMeanShape(this->m_vAAMAlignedShapes, this->m_VOAAMMeanAlignedShapePost);     // actually, "1" already
    this->m_VOAAMMeanAlignedShapePost.Scale( 1.0f/this->m_VOAAMMeanAlignedShapePost.GetShapeNorm () );  // "1" again

    this->m_CVMShapes           = cvCreateMat(this->m_iNbOfSamples, this->m_iNbOfShapes, CV_32FC1);
    this->m_CVMAlignedShapes    = cvCreateMat(this->m_iNbOfSamples, this->m_iNbOfShapes, CV_32FC1);
    // VO_AAMShape2D2CvMat() is not available here. For a vector of VO_AAMShape2D
    for(unsigned int i=0; i < this->m_iNbOfSamples; i++)
    {
        for(unsigned int j = 0; j < this->m_iNbOfPoints; j++)
        {
            CV_MAT_ELEM( *this->m_CVMShapes, float, i, 2*j ) = this->m_vAAMShapes[i].GetPoint(j).GetXY().x;
            CV_MAT_ELEM( *this->m_CVMShapes, float, i, 2*j+1 ) = this->m_vAAMShapes[i].GetPoint(j).GetXY().y;

            CV_MAT_ELEM( *this->m_CVMAlignedShapes, float, i, 2*j ) = this->m_vAAMAlignedShapes[i].GetPoint(j).GetXY().x;
            CV_MAT_ELEM( *this->m_CVMAlignedShapes, float, i, 2*j+1 ) = this->m_vAAMAlignedShapes[i].GetPoint(j).GetXY().y;
        }
    }

    this->m_iNbOfEigenShapesAtMost = MIN(this->m_iNbOfSamples, this->m_iNbOfShapes);

    this->m_CVMAlignedShapesEigenValues     = cvCreateMat( 1, this->m_iNbOfEigenShapesAtMost, CV_32FC1 );
    this->m_CVMAlignedShapesEigenVectors    = cvCreateMat( this->m_iNbOfEigenShapesAtMost, this->m_iNbOfShapes, CV_32FC1 );

    // void cvCalcPCA( const CvArr* data, CvArr* avg, CvArr* eigenvalues, CvArr* eigenvectors, int flags );
    // According to OpenCV cvCalcPCA function description:
    // avg - The mean (average) vector, computed inside the function or provided by user.
    // Don't forget to specify m_CVMAlignedShapesMean before calculation
    this->m_CVMAlignedShapesMean = cvCreateMat( 1, this->m_iNbOfShapes, CV_32FC1 );
    this->m_CVMAlignedShapesSD   = cvCreateMat( 1, this->m_iNbOfShapes, CV_32FC1 );
    this->m_VOAAMMeanAlignedShapePost.VO_AAMShape2D2CvMat(this->m_CVMAlignedShapesMean);

    cvCalcPCA( this->m_CVMAlignedShapes, this->m_CVMAlignedShapesMean, this->m_CVMAlignedShapesEigenValues, 
        this->m_CVMAlignedShapesEigenVectors, CV_PCA_DATA_AS_ROW/* + CV_PCA_USE_AVG */);

    VO_Common::VO_CalcSamplesSDWithKnownMean(this->m_CVMAlignedShapes,this->m_CVMAlignedShapesMean, this->m_CVMAlignedShapesSD);

    CvScalar SumOfEigenValues = cvSum( this->m_CVMAlignedShapesEigenValues );

    float limit = (float) ( FLT_EPSILON * SumOfEigenValues.val[0] );

    unsigned int i = 0;
    for(i = 0; i < this->m_CVMAlignedShapesEigenValues->cols; i++)
    { 
        if ( CV_MAT_ELEM( *this->m_CVMAlignedShapesEigenValues, float, 0, i ) < limit) break;
    }

    // NonZero EigenValues
    unsigned int NbOfNonZeroEigenValues = i;

    for(i = NbOfNonZeroEigenValues; i < this->m_iNbOfEigenShapesAtMost; i++)
    {
        CV_MAT_ELEM( *this->m_CVMAlignedShapesEigenValues, float, 0, i ) = 0.0f;
    }

    SumOfEigenValues = cvSum( this->m_CVMAlignedShapesEigenValues );
    float ps = 0.0f;
    this->m_iNbOfTruncatedShapes = 0;

    for(unsigned int i = 0; i < NbOfNonZeroEigenValues; i++)
    {
        ps += CV_MAT_ELEM( *this->m_CVMAlignedShapesEigenValues, float, 0, i );
        ++this->m_iNbOfTruncatedShapes;
        if( ps/SumOfEigenValues.val[0] >= this->m_fTruncatedPercent_Shape) break;
    }

    this->m_CVMTruncatedAlignedShapesEigenValues    = cvCreateMat( 1, this->m_iNbOfTruncatedShapes, CV_32FC1);
    this->m_CVMTruncatedAlignedShapesEigenVectors   = cvCreateMat( this->m_iNbOfTruncatedShapes, this->m_iNbOfShapes, CV_32FC1);

    for(unsigned int i = 0; i < this->m_iNbOfTruncatedShapes; i++)
    {
        CV_MAT_ELEM( *this->m_CVMTruncatedAlignedShapesEigenValues, float, 0, i )
            = CV_MAT_ELEM( *this->m_CVMAlignedShapesEigenValues, float, 0, i );

        for (unsigned int j = 0; j < this->m_iNbOfShapes; j++)
        {
            CV_MAT_ELEM( *this->m_CVMTruncatedAlignedShapesEigenVectors, float, i, j )
                = CV_MAT_ELEM( *this->m_CVMAlignedShapesEigenVectors, float, i, j );
        }
    }

    this->m_CVMAlignedShapesProject2Truncated = cvCreateMat( this->m_iNbOfSamples, this->m_iNbOfTruncatedShapes, CV_32FC1);
    cvProjectPCA( this->m_CVMAlignedShapes, this->m_CVMAlignedShapesMean, this->m_CVMTruncatedAlignedShapesEigenVectors, 
        this->m_CVMAlignedShapesProject2Truncated );

    this->m_CVMTruancatedAlignedShapesMean  = cvCreateMat( 1, this->m_iNbOfTruncatedShapes, CV_32FC1);
    this->m_CVMTruancatedAlignedShapesSD    = cvCreateMat( 1, this->m_iNbOfTruncatedShapes, CV_32FC1);
    // This time, the mean value is not specified beforehand
    VO_Common::VO_CalcSamplesMeanNSD(this->m_CVMAlignedShapesProject2Truncated, this->m_CVMTruancatedAlignedShapesMean, this->m_CVMTruancatedAlignedShapesSD);

    // Calculate reference shape 
    VO_ASM::VO_AlignedShape2ReferenceScale(this->m_VOAAMMeanAlignedShapePost, this->m_fAverageShapeSize, this->m_VOAAMReferenceShape);

    // 1) Build template shape mesh
    this->m_CVMPoints = cvCreateMat (1, this->m_iNbOfPoints, CV_32FC2);
    this->m_CVMConvexHull = cvCreateMat (1, this->m_iNbOfPoints, CV_32FC2);

    for( unsigned int i = 0; i < this->m_iNbOfPoints; i++ )
    {
        CV_MAT_ELEM( *this->m_CVMPoints, CvPoint2D32f, 0, i ) = this->m_VOAAMReferenceShape.GetPoint(i).GetXY();
    }

    VO_ASM::VO_BuildTemplateMesh(this->m_CVMPoints, this->m_CVMConvexHull, this->m_vAAMEdge, this->m_vAAMTriangle2D );

    this->m_iNbOfPointsOnConvexHull     = this->m_CVMConvexHull->cols;
    this->m_iNbOfEdges                  = this->m_vAAMEdge.size();
    this->m_iNbOfTriangles              = this->m_vAAMTriangle2D.size();

    // 2) Calculate m_vAAMNormalizedTriangle2D - make sure the mesh is built first, so that we have m_iNbOfTriangles
    vector<CvPoint2D32f> tempVertexes;
    vector<int> iVertexes;
    tempVertexes.resize(3);
    iVertexes.resize(3);

    for (unsigned int i = 0; i < this->m_iNbOfTriangles; i++)
    {
        iVertexes[0] = this->m_vAAMTriangle2D[i].GetVertexIndex(0);
        iVertexes[1] = this->m_vAAMTriangle2D[i].GetVertexIndex(1);
        iVertexes[2] = this->m_vAAMTriangle2D[i].GetVertexIndex(2);
        tempVertexes[0].x = CV_MAT_ELEM( *this->m_CVMAlignedShapesMean, float, 0, 2*iVertexes[0] );
        tempVertexes[0].y = CV_MAT_ELEM( *this->m_CVMAlignedShapesMean, float, 0, 2*iVertexes[0] + 1);
        tempVertexes[1].x = CV_MAT_ELEM( *this->m_CVMAlignedShapesMean, float, 0, 2*iVertexes[1] );
        tempVertexes[1].y = CV_MAT_ELEM( *this->m_CVMAlignedShapesMean, float, 0, 2*iVertexes[1] + 1);
        tempVertexes[2].x = CV_MAT_ELEM( *this->m_CVMAlignedShapesMean, float, 0, 2*iVertexes[2] );
        tempVertexes[2].y = CV_MAT_ELEM( *this->m_CVMAlignedShapesMean, float, 0, 2*iVertexes[2] + 1);

        this->m_vAAMNormalizedTriangle2D.push_back (VO_AAMTriangle2D(tempVertexes, iVertexes));
    }


}



/**

@author     JIA Pei

@version    2008-03-18

@brief      Save ASM to a specified folder

@param      fn         Input - the folder that ASM to be saved to

*/
void VO_ASM::VO_Save(const string& fd)
{
    string fn = fd+"/ASM";
    if (!boost::filesystem::is_directory(fn) )
        boost::filesystem::create_directory( fn );

    fstream fp;
    string tempfn;

    // ASM
    tempfn = fn + "/ASM" + ".txt";
    fp.open(tempfn.c_str (), ios::out);

    // m_iNbOfSamples
    fp << "m_iNbOfSamples" << endl;
    fp << this->m_iNbOfSamples << endl;

    // m_iNbOfPoints
    fp << "m_iNbOfPoints" << endl;
    fp << this->m_iNbOfPoints << endl;

    // m_iNbOfShapes
    fp << "m_iNbOfShapes" << endl;
    fp << this->m_iNbOfShapes << endl;

    // m_iNbOfTruncatedShapes
    fp << "m_iNbOfTruncatedShapes" << endl;
    fp << this->m_iNbOfTruncatedShapes << endl;

    // m_iNbOfEigenShapesAtMost
    fp << "m_iNbOfEigenShapesAtMost" << endl;
    fp << this->m_iNbOfEigenShapesAtMost << endl;

    // m_fAverageShapeSize
    fp << "m_fAverageShapeSize" << endl;
    fp << this->m_fAverageShapeSize << endl;

    // m_iNbOfPointsOnConvexHull
    fp << "m_iNbOfPointsOnConvexHull" << endl;
    fp << this->m_iNbOfPointsOnConvexHull << endl;

    // m_iNbOfEdges
    fp << "m_iNbOfEdges" << endl;
    fp << this->m_iNbOfEdges << endl;

    // m_iNbOfTriangles
    fp << "m_iNbOfTriangles" << endl;
    fp << this->m_iNbOfTriangles << endl;

    // m_fTruncatedPercent_Shape
    fp << "m_fTruncatedPercent_Shape" << endl;
    fp << this->m_fTruncatedPercent_Shape << endl;

    fp.close();

    // m_CVMShapes
    tempfn = fn + "/m_CVMShapes" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMShapes" << endl;
    for (unsigned int i = 0; i < this->m_CVMShapes->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMShapes->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMShapes, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMAlignedShapes
    tempfn = fn + "/m_CVMAlignedShapes" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMAlignedShapes" << endl;
    for (unsigned int i = 0; i < this->m_CVMAlignedShapes->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMAlignedShapes->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMAlignedShapes, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMAlignedShapesProject2Truncated
    tempfn = fn + "/m_CVMAlignedShapesProject2Truncated" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMAlignedShapesProject2Truncated" << endl;
    for (unsigned int i = 0; i < this->m_CVMAlignedShapesProject2Truncated->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMAlignedShapesProject2Truncated->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMAlignedShapesProject2Truncated, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMTruancatedAlignedShapesMean
    tempfn = fn + "/m_CVMTruancatedAlignedShapesMean" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruancatedAlignedShapesMean" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruancatedAlignedShapesMean->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMTruancatedAlignedShapesMean, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMTruancatedAlignedShapesSD
    tempfn = fn + "/m_CVMTruancatedAlignedShapesSD" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruancatedAlignedShapesSD" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruancatedAlignedShapesSD->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMTruancatedAlignedShapesSD, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMAlignedShapesMean
    tempfn = fn + "/m_CVMAlignedShapesMean" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMAlignedShapesMean" << endl;
    for (unsigned int i = 0; i < this->m_CVMAlignedShapesMean->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMAlignedShapesMean, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMAlignedShapesSD
    tempfn = fn + "/m_CVMAlignedShapesSD" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMAlignedShapesSD" << endl;
    for (unsigned int i = 0; i < this->m_CVMAlignedShapesSD->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMAlignedShapesSD, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMAlignedShapesEigenVectors
    tempfn = fn + "/m_CVMAlignedShapesEigenVectors" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMAlignedShapesEigenVectors" << endl;
    for (unsigned int i = 0; i < this->m_CVMAlignedShapesEigenVectors->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMAlignedShapesEigenVectors->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMAlignedShapesEigenVectors, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMAlignedShapesEigenValues
    tempfn = fn + "/m_CVMAlignedShapesEigenValues" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMAlignedShapesEigenValues" << endl;
    for (unsigned int i = 0; i < this->m_CVMAlignedShapesEigenValues->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMAlignedShapesEigenValues, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMTruncatedAlignedShapesEigenVectors
    tempfn = fn + "/m_CVMTruncatedAlignedShapesEigenVectors" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruncatedAlignedShapesEigenVectors" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruncatedAlignedShapesEigenVectors->rows; i++)
    {
        for (unsigned int j = 0; j < this->m_CVMTruncatedAlignedShapesEigenVectors->cols; j++)
        {
            fp << CV_MAT_ELEM( *this->m_CVMTruncatedAlignedShapesEigenVectors, float, i, j) << " " ;
        }
        fp << endl;
    }
    fp.close();

    // m_CVMTruncatedAlignedShapesEigenValues
    tempfn = fn + "/m_CVMTruncatedAlignedShapesEigenValues" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMTruncatedAlignedShapesEigenValues" << endl;
    for (unsigned int i = 0; i < this->m_CVMTruncatedAlignedShapesEigenValues->cols; i++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMTruncatedAlignedShapesEigenValues, float, 0, i) << " " ;
    }
    fp << endl;
    fp.close();

    // m_CVMPoints
    tempfn = fn + "/m_CVMPoints" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMPoints" << endl;
    for (unsigned int j = 0; j < this->m_CVMPoints->cols; j++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMPoints, CvPoint2D32f, 0, j ).x << " " 
            << CV_MAT_ELEM( *this->m_CVMPoints, CvPoint2D32f, 0, j ).y << " " << endl;
    }
    fp.close();

    // m_CVMConvexHull
    tempfn = fn + "/m_CVMConvexHull" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_CVMConvexHull" << endl;
    for (unsigned int j = 0; j < this->m_CVMConvexHull->cols; j++)
    {
        fp << CV_MAT_ELEM( *this->m_CVMConvexHull, CvPoint2D32f, 0, j).x << " " 
            << CV_MAT_ELEM( *this->m_CVMConvexHull, CvPoint2D32f, 0, j).y << " " << endl;
    }
    fp.close();

    // m_VOAAMReferenceShape
    tempfn = fn + "/m_VOAAMReferenceShape" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_VOAAMReferenceShape" << endl;
    for (unsigned int i = 0; i < this->m_VOAAMReferenceShape.GetSize(); i++)
    {
        fp << this->m_VOAAMReferenceShape.GetPoint(i).GetXY().x << " ";
        fp << this->m_VOAAMReferenceShape.GetPoint(i).GetXY().y << " " << endl;
    }
    fp.close();

    // m_vAAMShapes
    tempfn = fn + "/m_vAAMShapes" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMShapes" << endl;
    for (unsigned int i = 0; i < this->m_vAAMShapes.size(); i++)
    {
        for (unsigned int j = 0; j < this->m_iNbOfPoints; j++)
        {
            fp << this->m_vAAMShapes[i].GetPoint(j).GetXY().x << " " 
                << this->m_vAAMShapes[i].GetPoint(j).GetXY().y << " "
                << this->m_vAAMShapes[i].GetPoint(j).GetPath() << " " 
                << this->m_vAAMShapes[i].GetPoint(j).GetType() << " " 
                << this->m_vAAMShapes[i].GetPoint(j).GetIndex() << " " 
                << this->m_vAAMShapes[i].GetPoint(j).GetFrom() << " "
                << this->m_vAAMShapes[i].GetPoint(j).GetTo() << " " << endl;
        }
        fp << endl;
    }
    fp.close();

    // m_vAAMAlignedShapes
    tempfn = fn + "/m_vAAMAlignedShapes" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMAlignedShapes" << endl;
    for (unsigned int i = 0; i < this->m_vAAMAlignedShapes.size(); i++)
    {
        for (unsigned int j = 0; j < this->m_iNbOfPoints; j++)
        {
            fp << this->m_vAAMAlignedShapes[i].GetPoint(j).GetXY().x << " ";
            fp << this->m_vAAMAlignedShapes[i].GetPoint(j).GetXY().y << " ";
        }
        fp << endl;
    }
    fp.close();

    // m_VOAAMMeanAlignedShapePrior
    tempfn = fn + "/m_VOAAMMeanAlignedShapePrior" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_VOAAMMeanAlignedShapePrior" << endl;
    for (unsigned int i = 0; i < this->m_iNbOfPoints; i++)
    {
        fp << this->m_VOAAMMeanAlignedShapePrior.GetPoint(i).GetXY().x << " ";
        fp << this->m_VOAAMMeanAlignedShapePrior.GetPoint(i).GetXY().y << " ";
    }
    fp << endl;
    fp.close();

    // m_VOAAMMeanAlignedShapePost
    tempfn = fn + "/m_VOAAMMeanAlignedShapePost" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_VOAAMMeanAlignedShapePost" << endl;
    for (unsigned int i = 0; i < this->m_iNbOfPoints; i++)
    {
        fp << this->m_VOAAMMeanAlignedShapePost.GetPoint(i).GetXY().x << " ";
        fp << this->m_VOAAMMeanAlignedShapePost.GetPoint(i).GetXY().y << " ";
    }
    fp << endl;
    fp.close();

    // m_vAAMEdge
    tempfn = fn + "/m_vAAMEdge" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMEdge" << endl;
    for (unsigned int i = 0; i < this->m_vAAMEdge.size(); i++)
    {
        fp << this->m_vAAMEdge[i].GetIndex1() << " ";
        fp << this->m_vAAMEdge[i].GetIndex2() << " " << endl;
    }
    fp.close();

    // m_vAAMTriangle2D
    tempfn = fn + "/m_vAAMTriangle2D" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMTriangle2D" << endl;
    for (unsigned int i = 0; i < this->m_vAAMTriangle2D.size (); i++)
    {
        fp  << this->m_vAAMTriangle2D[i].GetVertexIndex(0) << " " 
            << this->m_vAAMTriangle2D[i].GetVertex(0).x << " "
            << this->m_vAAMTriangle2D[i].GetVertex(0).y << " "
            << this->m_vAAMTriangle2D[i].GetVertexIndex(1) << " " 
            << this->m_vAAMTriangle2D[i].GetVertex(1).x << " "
            << this->m_vAAMTriangle2D[i].GetVertex(1).y << " "
            << this->m_vAAMTriangle2D[i].GetVertexIndex(2) << " "
            << this->m_vAAMTriangle2D[i].GetVertex(2).x << " "
            << this->m_vAAMTriangle2D[i].GetVertex(2).y << " "
            << this->m_vAAMTriangle2D[i].GetdD() << " " << endl; 
    }
    fp.close();

    // m_vAAMNormalizedTriangle2D
    tempfn = fn + "/m_vAAMNormalizedTriangle2D" + ".txt";
    fp.open(tempfn.c_str (), ios::out);
    fp << "m_vAAMNormalizedTriangle2D" << endl;
    for (unsigned int i = 0; i < this->m_vAAMNormalizedTriangle2D.size (); i++)
    {
        fp  << this->m_vAAMNormalizedTriangle2D[i].GetVertexIndex(0) << " " 
            << this->m_vAAMNormalizedTriangle2D[i].GetVertex(0).x << " "
            << this->m_vAAMNormalizedTriangle2D[i].GetVertex(0).y << " "
            << this->m_vAAMNormalizedTriangle2D[i].GetVertexIndex(1) << " " 
            << this->m_vAAMNormalizedTriangle2D[i].GetVertex(1).x << " "
            << this->m_vAAMNormalizedTriangle2D[i].GetVertex(1).y << " "
            << this->m_vAAMNormalizedTriangle2D[i].GetVertexIndex(2) << " "
            << this->m_vAAMNormalizedTriangle2D[i].GetVertex(2).x << " "
            << this->m_vAAMNormalizedTriangle2D[i].GetVertex(2).y << " "
            << this->m_vAAMNormalizedTriangle2D[i].GetdD() << " " << endl; 
    }
    fp.close();

}

