/****************************************************************************
* File Name:        VO_ASM.h                                                *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     2D ASM                                                  *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/

#ifndef __VO_ASM__
#define __VO_ASM__


#include <vector>

#include "cv.h"
#include "highgui.h"


#include "VO_Common.h"
#include "VO_AAMEdge.h"
#include "VO_AAMShape2D.h"
#include "VO_AAMTriangle2D.h"

using namespace std;

/****************************************************************************
* Class Name:       VO_ASM                                                  *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         ASM Model                                               *
****************************************************************************/
class VO_ASM
{
protected:

    /** The shape samples before alignment, in the form of xyxyxyxy... For IMM, 240*116 */
    CvMat*						m_CVMShapes;

    /** Aligned shape samples, in the form of xyxyxy... For IMM, 240*116 */
    CvMat*						m_CVMAlignedShapes;

    /** Aligned shape project to truncated space. For IMM, 240*20 */
    CvMat*						m_CVMAlignedShapesProject2Truncated;

    /** Mean vector of m_CVMAlignedShapesProject2Truncated. For IMM, 1*20, should be 0 */
    CvMat*						m_CVMTruancatedAlignedShapesMean;

    /** Standard deviation of m_CVMAlignedShapesProject2Truncated. For IMM, 1*20 */
    CvMat*						m_CVMTruancatedAlignedShapesSD;

    /** Mean vector of m_CVMAlignedShapes. For IMM, 1*116, should be 0, 2-norm!=1 used for cvCalcPCA*/
    CvMat*						m_CVMAlignedShapesMean;

    /** Standard deviation of m_CVMAlignedShapes. For IMM, 1*116 */
    CvMat*						m_CVMAlignedShapesSD;

    /** Original Eigenvectors of m_CVMAlignedShapes. For IMM, MIN(240,116)*116 = 116*116 */
    CvMat*						m_CVMAlignedShapesEigenVectors;

    /** Original Eigenvalues of m_CVMAlignedShapes. For IMM, 1*MIN(240,116) = 1*116 */
    CvMat*						m_CVMAlignedShapesEigenValues;

    /** Truncated Eigenvectors of m_CVMAlignedShapes. For IMM, 20*116*/
    CvMat*						m_CVMTruncatedAlignedShapesEigenVectors;

    /** Truncated Eigenvalues of m_CVMAlignedShapes. For IMM, 1*20 */
    CvMat*						m_CVMTruncatedAlignedShapesEigenValues;

    /** Similarity transform matrix, refer to "AAM Revisited" P26-27. For IMM: 4*116*/
    CvMat*						m_CVMSimilarityTransformMatrix;

    /** Refer to m_VOAAMReferenceShape, m_CVMPoints starts from left top most pixel as origin. For IMM, 1*58 */
    CvMat*						m_CVMPoints;

    /** Convex Hull of the points and sub division of the plane. For IMM, 1*19 */
    CvMat*						m_CVMConvexHull;

    /** Number of training samples. For IMM: 240 */
    unsigned int				m_iNbOfSamples;

    /** Number of points to describe per shape. For IMM: 58 */
    unsigned int				m_iNbOfPoints;

    /** AAM shape vector length in format of xyxyxy....xy. For IMM: 58*2=116 */
    unsigned int				m_iNbOfShapes;

    /** Truncated AAM shape model vector length. For IMM: 20 */
    unsigned int				m_iNbOfTruncatedShapes;

    /** Most possible shape eigens before PCA. For IMM: min (116, 240) = 116 */
    unsigned int				m_iNbOfEigenShapesAtMost;

    /** Reference shape which is scaled back to the original size and translated to left top most = origin */
    VO_AAMShape2D				m_VOAAMReferenceShape;

    /** The reference shape average size : 582.425 */
    float						m_fAverageShapeSize;

    /** Number of points on m_CVMConvexHull. For IMM: 19 */
    unsigned int				m_iNbOfPointsOnConvexHull;

    /** Number of edges. For IMM: 152 */
    unsigned int				m_iNbOfEdges;

    /** Number of triangles. For IMM: 95 */
    unsigned int				m_iNbOfTriangles;

    /** Truncate Percentage for AAM shape PCA. Normally, 0.95 */
    float                       m_fTruncatedPercent_Shape;

    /** The same as m_CVMShapes, but in a vector format. For IMM, 240*(58*2) */
    vector<VO_AAMShape2D>		m_vAAMShapes;

    /** The same as m_CVMAlignedShapes, but in a vector format. For IMM, 240*(58*2) */
    vector<VO_AAMShape2D>		m_vAAMAlignedShapes;

    /** Different from m_CVMAlignedShapesMean, which hasn't been standardized */
    VO_AAMShape2D				m_VOAAMMeanAlignedShapePrior;

    /** The same as m_CVMAlignedShapesMean, but in a vector format. For IMM, 1*(58*2). should equal to 0 */
    VO_AAMShape2D				m_VOAAMMeanAlignedShapePost;

    /** Edges in the template shape, only contain the index pairs. For IMM,  152 */
    vector<VO_AAMEdge>			m_vAAMEdge;

    /** Unnormalized triangles in the template shape. For IMM, 95 */
    vector<VO_AAMTriangle2D>	m_vAAMTriangle2D;

    /** Normalized triangles in the template shape - just replace the vertexes in m_vAAMTriangle2D by corresponding vertexes of m_VOAAMMeanAlignedShapePost. For IMM, 95 */
    vector<VO_AAMTriangle2D>	m_vAAMNormalizedTriangle2D;

    /** Initialization */
    void                        init();

public:
    /** Default constructor to create a VO_ASM object */
    VO_ASM();

    /** Destructor */
    virtual ~VO_ASM();

    /** Align all shapes */
    static float				VO_AlignAllShapes(const vector<VO_AAMShape2D>& vShapes, vector<VO_AAMShape2D>& alignedShapes);

    /** Rescale all aligned shapes */
    static void 				VO_RescaleAllAlignedShapes(const VO_AAMShape2D& meanAlignedShape, vector<VO_AAMShape2D>& alignedShapes);

    /** Rescale one aligned shape */
    static void 				VO_RescaleOneAlignedShape(const VO_AAMShape2D& meanAlignedShape, VO_AAMShape2D& alignedShape);

    /** Returns the mean shape of all shapes */
    static void					VO_CalcMeanShape(const vector<VO_AAMShape2D>& vShapes, VO_AAMShape2D &meanShape); 

    /** Judge whether the point "pt" is in convex hull "ch" */
    static bool                 VO_IsPointInConvexHull(CvPoint2D32f pt, const CvMat* ch, bool includingHull);

    /** Judge whether edge indexed by (ind1+ind2) is already in the vector of "edges" */
    static bool                 VO_IsEdgeCounted(const vector<VO_AAMEdge> &edges, int ind1, int ind2);

    /** Judge whether triangle t is already in the vector of "triangles" */
    static bool                 VO_IsTriangleCounted(const vector<VO_AAMTriangle2D> &triangles, const vector<int> t);

    /** Build convex hull */
    static int                  VO_BuildConvexHull(const CvMat* vertexes, CvMat* ch);

    /** Build AAM Edges */
    static int                  VO_BuildEdges(const CvMat* vertexes, const CvSubdiv2D* Subdiv, vector<VO_AAMEdge>& outEdges);

    /** Build AAM triangles */
    static int                  VO_BuildTriangles(const CvMat* vertexes, const vector<VO_AAMEdge> &edges, vector<VO_AAMTriangle2D> &outTriangles);

    /** Build Delaunay triangulation mesh */
    static void                 VO_BuildTemplateMesh(const CvMat* vertexes, CvMat* ch, vector<VO_AAMEdge> & edges, vector<VO_AAMTriangle2D> & triangles );

    /** Get shape boundary */
    static CvRect               VO_CalcShapeBoundaryRect(const VO_AAMShape2D& iShape);

    /** Judge whether the shape is inside an image */
    static bool                 VO_IsShapeInsideImage(const VO_AAMShape2D& iShape, const IplImage* img);

    /** Change the aligned shape inShape to the reference one outShape */
    static void                 VO_AlignedShape2ReferenceScale(const VO_AAMShape2D& inShape, float shapeSD, VO_AAMShape2D& outShape);

    /** Reference shape inShape back to aligned outShape */
    static void                 VO_ReferenceShapeBack2Aligned(const VO_AAMShape2D& inShape, float shapeSD,VO_AAMShape2D& outShape);

    /** Shape parameters back projected to shape parameters*/
    VO_AAMShape2D   	        VO_SParamBackProjectToAlignedShape(CvMat* inP);

    /** Build ASM */
    void						VO_BuildASM(const vector<VO_AAMShape2D> &vShapes, float TPShape = 0.95f);

    /** Save ASM, to a specified folder */
    void                        VO_Save(const string& fd);

    /** Load Parameters for fitting */
    void                        VO_LoadParamters4Fitting(const string& fd);

    /** Gets and Sets */
    CvMat*                      GetShapes() const {return this->m_CVMShapes;}
    CvMat*                      GetAlignedShapes() const {return this->m_CVMAlignedShapes;}
    CvMat*                      GetAlignedShapesProject2Truncated() const {return this->m_CVMAlignedShapesProject2Truncated;}
    CvMat*                      GetTruancatedAlignedShapesMean() const {return this->m_CVMTruancatedAlignedShapesMean;}
    CvMat*                      GetTruancatedAlignedShapesSD() const {return this->m_CVMTruancatedAlignedShapesSD;}
    CvMat*                      GetAlignedShapesMean() const {return this->m_CVMAlignedShapesMean;}
    CvMat*                      GetAlignedShapesSD() const {return this->m_CVMAlignedShapesSD;}
    CvMat*                      GetAlignedShapesEigenVectors() const {return this->m_CVMAlignedShapesEigenVectors;}
    CvMat*                      GetAlignedShapesEigenValues() const {return this->m_CVMAlignedShapesEigenValues;}
    CvMat*                      GetTruncatedAlignedShapesEigenVectors() const {return this->m_CVMTruncatedAlignedShapesEigenVectors;}
    CvMat*                      GetTruncatedAlignedShapesEigenValues() const {return this->m_CVMTruncatedAlignedShapesEigenValues;}
    CvMat*                      GetSimilarityTransformMatrix() const {return this->m_CVMSimilarityTransformMatrix;}
    CvMat*                      GetPoints() const {return this->m_CVMPoints;}
    CvMat*                      GetConvexHull() const {return this->m_CVMConvexHull;}
    unsigned int                GetNbOfSamples() const {return this->m_iNbOfSamples;}
    unsigned int                GetNbOfPoints() const {return this->m_iNbOfPoints;}
    unsigned int                GetNbOfShapes() const {return this->m_iNbOfShapes;}
    unsigned int                GetNbOfTruncatedShapes() const {return this->m_iNbOfTruncatedShapes;}
    unsigned int                GetNbOfEigenShapesAtMost() const {return this->m_iNbOfEigenShapesAtMost;}
    VO_AAMShape2D               GetAAMReferenceShape() const {return this->m_VOAAMReferenceShape;}
    float                       GetAverageShapeSize() const {return this->m_fAverageShapeSize;}
    unsigned int                GetNbOfPointsOnConvexHull() const {return this->m_iNbOfPointsOnConvexHull;}
    unsigned int                GetNbOfEdges() const {return this->m_iNbOfEdges;}
    unsigned int                GetNbOfTriangles() const {return this->m_iNbOfTriangles;}
    float                       GetTruncatedPercent_Shape() const {return this->m_fTruncatedPercent_Shape;}
    vector<VO_AAMShape2D>       GetAAMShapes() const {return this->m_vAAMShapes;}
    vector<VO_AAMShape2D>       GetAAMAlignedShapes() const {return this->m_vAAMAlignedShapes;}
    VO_AAMShape2D               GetAAMMeanAlignedShapePrior() const {return this->m_VOAAMMeanAlignedShapePrior;}
    VO_AAMShape2D               GetAAMMeanAlignedShapePost() const {return this->m_VOAAMMeanAlignedShapePost;}
    vector<VO_AAMEdge>          GetAAMEdge() const {return this->m_vAAMEdge;}
    vector<VO_AAMTriangle2D>    GetAAMTriangle2D() const {return this->m_vAAMTriangle2D;}
    vector<VO_AAMTriangle2D>    GetAAMNormalizedTriangle2D() const {return this->m_vAAMNormalizedTriangle2D;}

    void                        SetShapes(const CvMat* inShapes) {this->m_CVMShapes = cvCloneMat(inShapes);}
    void                        SetAlignedShapes(const CvMat* inAlignedShapes)
                                {this->m_CVMAlignedShapes = cvCloneMat(inAlignedShapes);}
    void                        SetAlignedShapesProject2Truncated(const CvMat* inAlignedShapesProject2Truncated) 
                                {this->m_CVMAlignedShapesProject2Truncated = cvCloneMat(inAlignedShapesProject2Truncated);}
    void                        SetTruancatedAlignedShapesMean(const CvMat* inTruancatedAlignedShapesMean) 
                                {this->m_CVMTruancatedAlignedShapesMean = cvCloneMat(inTruancatedAlignedShapesMean);}
    void                        SetTruancatedAlignedShapesSD(const CvMat* inTruancatedAlignedShapesSD) 
                                {this->m_CVMTruancatedAlignedShapesSD = cvCloneMat(inTruancatedAlignedShapesSD);}
    void                        SetAlignedShapesMean(const CvMat* inAlignedShapesMean) 
                                {this->m_CVMAlignedShapesMean = cvCloneMat(inAlignedShapesMean);}
    void                        SetAlignedShapesSD(const CvMat* inAlignedShapesSD) 
                                {this->m_CVMAlignedShapesSD = cvCloneMat(inAlignedShapesSD);}
    void                        SetAlignedShapesEigenVectors(const CvMat* inAlignedShapesEigenVectors) 
                                {this->m_CVMAlignedShapesEigenVectors = cvCloneMat(inAlignedShapesEigenVectors);}
    void                        SetAlignedShapesEigenValues(const CvMat* inAlignedShapesEigenValues) 
                                {this->m_CVMAlignedShapesEigenValues = cvCloneMat(inAlignedShapesEigenValues);}
    void                        SetTruncatedAlignedShapesEigenVectors(const CvMat* inTruncatedAlignedShapesEigenVectors)
                                {this->m_CVMTruncatedAlignedShapesEigenVectors = cvCloneMat(inTruncatedAlignedShapesEigenVectors);}
    void                        SetTruncatedAlignedShapesEigenValues(const CvMat* inTruncatedAlignedShapesEigenValues)
                                {this->m_CVMTruncatedAlignedShapesEigenValues = cvCloneMat(inTruncatedAlignedShapesEigenValues);}
    void                        SetSimilarityTransformMatrix(const CvMat* inSimilarityTransformMatrix)
                                {this->m_CVMSimilarityTransformMatrix = cvCloneMat(inSimilarityTransformMatrix);}
    void                        SetPoints(const CvMat* inPoints) {this->m_CVMPoints = cvCloneMat(inPoints);}
    void                        SetConvexHull(const CvMat* inConvexHull) {this->m_CVMConvexHull = cvCloneMat(inConvexHull);}
    void                        SetNbOfSamples(unsigned int inNbOfSamples) {this->m_iNbOfSamples = inNbOfSamples;}
    void                        SetNbOfPoints(unsigned int inNbOfPoints) {this->m_iNbOfPoints = inNbOfPoints;}
    void                        SetNbOfShapes(unsigned int inNbOfShapes) {this->m_iNbOfShapes = inNbOfShapes;}
    void                        SetNbOfTruncatedShapes(unsigned int inNbOfTruncatedShapes) 
                                {this->m_iNbOfTruncatedShapes = inNbOfTruncatedShapes;}
    void                        SetNbOfEigenShapesAtMost(unsigned int inNbOfEigenShapesAtMost)
                                {this->m_iNbOfEigenShapesAtMost = inNbOfEigenShapesAtMost;}
    void                        SetAAMReferenceShape(const VO_AAMShape2D& inAAMReferenceShape)
                                {this->m_VOAAMReferenceShape = inAAMReferenceShape;}
    void                        SetAverageShapeSize(float inAverageShapeSize) {this->m_fAverageShapeSize = inAverageShapeSize;}
    void                        SetNbOfPointsOnConvexHull(unsigned int inNbOfPointsOnConvexHull)
                                {this->m_iNbOfPointsOnConvexHull = inNbOfPointsOnConvexHull;}
    void                        SetNbOfEdges(unsigned int inNbOfEdges) {this->m_iNbOfEdges = inNbOfEdges;}
    void                        SetNbOfTriangles(unsigned int inNbOfTriangles) {this->m_iNbOfTriangles = inNbOfTriangles;}
    void                        SetTruncatedPercent_Shape(float inTP) {this->m_fTruncatedPercent_Shape = inTP;}
    void                        SetAAMShapes(const vector<VO_AAMShape2D>& inAAMShapes) {this->m_vAAMShapes = inAAMShapes;}
    void                        SetAAMAlignedShapes(const vector<VO_AAMShape2D>& inAAMAlignedShapes)
                                {this->m_vAAMAlignedShapes = inAAMAlignedShapes;}
    void                        SetAAMMeanAlignedShapePrior(const VO_AAMShape2D& inAAMMeanAlignedShape) 
                                {this->m_VOAAMMeanAlignedShapePrior = inAAMMeanAlignedShape;}
    void                        SetAAMMeanAlignedShapePost(const VO_AAMShape2D& inAAMMeanAlignedShape) 
                                {this->m_VOAAMMeanAlignedShapePost = inAAMMeanAlignedShape;}
    void                        SetAAMEdge(const vector<VO_AAMEdge>& inAAMEdge) {this->m_vAAMEdge = inAAMEdge;}
    void                        SetAAMTriangle2D(const vector<VO_AAMTriangle2D>& inAAMTriangle2D) 
                                {this->m_vAAMTriangle2D = inAAMTriangle2D;}
    void                        SetAAMNormalizedTriangle2D(const vector<VO_AAMTriangle2D>& inAAMNormalizedTriangle2D)
                                {this->m_vAAMNormalizedTriangle2D = inAAMNormalizedTriangle2D;}

};

#endif // __VO_ASM__

