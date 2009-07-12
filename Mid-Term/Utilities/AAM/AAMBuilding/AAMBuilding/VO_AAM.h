/****************************************************************************
* File Name:        VO_AAM.h                                                *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     AAM Model, shape + texture                              *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/

#ifndef __VO_AAM__
#define __VO_AAM__


#include <vector>

#include "cv.h"
#include "highgui.h"


#include "VO_AAMTexture2D3C.h"
#include "VO_AAMWarpingPoint.h"
#include "VO_ASM.h"

using namespace std;

/****************************************************************************
* Class Name:       VO_AAM                                                  *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         AAM Model, shape + texture                              *
****************************************************************************/
class VO_AAM : public VO_ASM
{
protected:

    /** The texture samples before normalization, in the form of rgbrgbrgb... For IMM, 240*(30795*3=92385) */
    CvMat*						    m_CVMTextures;

    /** The normalized texture samples - mean 0, 2-norm=1. For IMM, 240*(30795*3=92385) */
    CvMat*						    m_CVMNormalizedTextures;

    /** Original normalized texture project to truncated space. For IMM, 240*116 */
    CvMat*						    m_CVMNormalizedTexturesProject2Truncated;

    /** Mean vector of m_CVMNormalizedTexturesProject2Truncated. For IMM, 1*116, should be 0 */
    CvMat*						    m_CVMTruancatedNormalizedTexturesMean;

    /** Standard deviation of m_CVMNormalizedTexturesProject2Truncated. For IMM, 1*116 */
    CvMat*						    m_CVMTruancatedNormalizedTexturesSD;

    /** Mean vector of m_CVMNormalizedTextures. For IMM, 1*(30795*3=92385),should be 0,2-norm!=1 used for cvCalcPCA*/
    CvMat*						    m_CVMNormalizedTexturesMean;

    /** Standard deviation of m_CVMNormalizedTextures. For IMM, 1*(30795*3=92385). Could be used as weights for AAM */
    CvMat*						    m_CVMNormalizedTexturesSD;

    /** Original Eigenvectors of m_CVMNormalizedTextures. For IMM, MIN(240,(30795*3=92385))*92385 = 240*92385 */
    CvMat*						    m_CVMNormalizedTexturesEigenVectors;

    /** Original Eigenvalues of m_CVMNormalizedTextures. For IMM, 1*MIN(240,(30795*3=92385)) = 1*92385 */
    CvMat*						    m_CVMNormalizedTexturesEigenValues;

    /** Truncated Eigenvectors of m_CVMNormalizedTextures. For IMM, 116*92385*/
    CvMat*						    m_CVMTruncatedNormalizedTexturesEigenVectors;

    /** Truncated Eigenvalues of m_CVMNormalizedTextures. For IMM, 1*116 */
    CvMat*						    m_CVMTruncatedNormalizedTexturesEigenValues;

    /** Texture extraction method - DIRECTTEXTURE, LAPLACETEXTURE, HARRISCORNERTEXTURE, HISTOGRAMEQUALIZED, GABOR, SEGMENTATION */
    unsigned int		            m_iTextureExtractionMethod;

    /** COLOR or Gray-level - 3 - COLOR; 1 - Gray-level */
    unsigned int				    m_iNbOfChannels;

    /** Number of pixels in template face convex hull, including convex hull. For IMM, 30795 */
    unsigned int				    m_iNbOfPixels;

    /** AAM texture vector length in format of rgbrgb....rgb. For IMM, 30795*3=92385 */
    unsigned int				    m_iNbOfTextures;

    /** Truncated AAM shape model vector length. For IMM: 116 */
    unsigned int				    m_iNbOfTruncatedTextures;

    /** Most possible texture eigens before PCA. For IMM: min (92385, 240) = 240 */
    unsigned int				    m_iNbOfEigenTexturesAtMost;

    /** Reference texture */
    VO_AAMTexture2D3C			    m_VOAAMReferenceTexture;

    /** The template texture average standard deviation : 11976.2 */
    float						    m_fAverageTextureStandardDeviation;

    /** Truncate Percentage for AAM texture PCA. Normally, 0.95 */
    float                           m_fTruncatedPercent_Texture;

    /** Size of Template Face */
    CvSize						    m_CVSizeOfTemplateFace;

    /** Template face image */
    IplImage*					    m_IplTemplateFace;

    /** Image of edges */
    IplImage*					    m_IplEdges;

    /** Image of convex hull */
    IplImage*					    m_IplConvexHull;

    /** If the out boundary edges compose a concave hull, instead of a convex hull */
    IplImage*					    m_IplConcave;

    /** How many triangles totally */
    vector<IplImage*>			    m_vIplTriangles;

    /** All loaded images for AAM texture model building, texture extraction */
    vector<IplImage*>			    m_vImages;

    /** The same as m_CVMTextures, but in a vector format. For IMM, 240*(30795*3=92385) */
    vector<VO_AAMTexture2D3C>	    m_vAAMTextures;

    /** The same as m_CVMNormalizedTextures, but in a vector format. For IMM, 240*(30795*3=92385) */
    vector<VO_AAMTexture2D3C>	    m_vAAMNormalizedTextures;

    /** Different from m_CVMNormalizedTexturesMean, which hasn't been standardized */
    VO_AAMTexture2D3C			    m_VOAAMMeanNormalizedTexturePrior;

    /** The same as m_CVMNormalizedTexturesMean, but in a vector format. For IMM, 1*(30795*3=92385) */
    VO_AAMTexture2D3C			    m_VOAAMMeanNormalizedTexturePost;

    /** Unnormalized point warping information. For IMM, 30795 */
    vector<VO_AAMWarpingPoint>		m_vPointWarpInfo;

    /** Normalized point warping information. For IMM, 30795 */
    vector<VO_AAMWarpingPoint>		m_vNormalizedPointWarpInfo;

    /** Initialization */
    void                            init();

public:

    /** Default constructor to create a VO_AAM object */
    VO_AAM();

    /** Destructor */
    virtual ~VO_AAM();

    /**Calculate point warping information */
    static unsigned int             VO_CalcPointWarpingInfo(const vector <VO_AAMTriangle2D>& triangles, const CvMat* ch, vector<VO_AAMWarpingPoint>& warpInfo);

    /** Load a texture vector from the image "img", in terms of "iShape", with a texture building method */
    static bool					    VO_LoadOneTextureFromShape(const VO_AAMShape2D& iShape, const IplImage* img, const vector <VO_AAMTriangle2D>& triangles,const vector<VO_AAMWarpingPoint>& warpInfo, VO_AAMTexture2D3C& oTexture, int tbm = DIRECTTEXTURE);

    /** Load all textures from the training images, with a texture building method */
    static bool				        VO_LoadTexturesFromShapes(const vector<VO_AAMShape2D>& iShapes, const vector<IplImage*> imgs, const vector <VO_AAMTriangle2D>& triangles, const vector<VO_AAMWarpingPoint>& warpInfo, vector<VO_AAMTexture2D3C>& oTextures, int tbm = DIRECTTEXTURE);

    /** Normalize all textures */
    static float				    VO_NormalizeAllTextures(const vector<VO_AAMTexture2D3C>& vTextures, vector<VO_AAMTexture2D3C>& normalizedTextures);

    /** Rescale all normalized textures */
    static void 				    VO_RescaleAllNormalizedTextures(const VO_AAMTexture2D3C& meanNormalizeTexture, vector<VO_AAMTexture2D3C>& normalizedTextures);

    /** Rescale one normalized texture */
    static void 				    VO_RescaleOneNormalizedTexture(const VO_AAMTexture2D3C& meanNormalizeTexture, VO_AAMTexture2D3C& normalizedTexture);

    /** Returns the mean shape of all textures */
    static void					    VO_CalcMeanTexture(const vector<VO_AAMTexture2D3C>& vTextures, VO_AAMTexture2D3C &meanTexture); 
    /** Is the current point pt in trained template face, template face's boundary might be a concave hull, instead of convex hull */
    static bool 				    VO_IsPointInTemplateFace(const vector <VO_AAMTriangle2D>& triangles, const CvPoint2D32f& pt);

    /** Put one texture to the template face. Just for display! */
    static void					    VO_PutOneTextureToTemplateShape(const VO_AAMTexture2D3C &texture, const vector <VO_AAMTriangle2D>& triangles, IplImage* img);

    /** Warp form one shape to another */
    static void                     VO_WarpFromOneShapeToAnother(const VO_AAMShape2D& iShape, const VO_AAMShape2D& oShape, const vector <VO_AAMTriangle2D>& triangles, const IplImage* Iimg, IplImage* Oimg);

    /** Warp form one shape to another */
    static vector<float>		    VO_CalcSubPixelTexture(float x, float y, const IplImage* image);

    /** Get an intensity vector of size 1 or 3 (rgb), at point (x,y) in image */
    static vector<float>		    VO_CalcPixelRGB(int x, int y, const IplImage* image);

    /** Change the normalized texture inTexture to the reference one outTexture */
    static void                     VO_NormalizedTexture2ReferenceScale(const VO_AAMTexture2D3C& inTexture, float textureSD, VO_AAMTexture2D3C& outTexture);

    /** Normalize inTexture to outTexture */
    static void                     VO_ReferenceTextureBack2Normalize(const VO_AAMTexture2D3C& inTexture, float textureSD, VO_AAMTexture2D3C& outTexture);

    /** Put edges on template face */
    static void                     VO_PutEdgesOnTemplateFace(const vector<VO_AAMEdge>& edges, const VO_AAMShape2D& templateShape, const IplImage* iImg, IplImage* oImg);

    /** Put convex hull on template face */
    static void                     VO_PutConvexHullOnTemplateFace(const CvMat* ch, const IplImage* iImg, IplImage* oImg);

    /** Put concave pixels on template face */
    static void                     VO_PutConcavePixelsOnTemplateFace(const CvMat* ch /* convex hull*/, const vector <VO_AAMTriangle2D>& triangles, const IplImage* iImg, IplImage* oImg);

    /** Put every single triangle onto template face */
    static void                     VO_PutTrianglesOnTemplateFace(const vector <VO_AAMTriangle2D>& triangles, const IplImage* iImg, vector<IplImage*>& oImgs);

    /** Texture parameters back projected to texture parameters */
    VO_AAMTexture2D3C	            VO_TParamBackProjectToNormalizedTexture(CvMat* inT);

    /** Build AAM */
    void						    VO_BuildAAM(const vector<VO_AAMShape2D> &vShapes, const vector<IplImage*> &imgs, int channel = COLORCHANNELS, int tbm = DIRECTTEXTURE, float TPShape = 0.95f, float TPTexture = 0.95f);

    /** Save AAM, to a specified folder */
    void                            VO_Save(const string& fd);

    /** Load Parameters for fitting */
    void                            VO_LoadParamters4Fitting(const string& fd);

    /** Gets and Sets */
    CvMat*                          GetTextures() const {return this->m_CVMTextures;}
    CvMat*                          GetNormalizedTextures() const {return this->m_CVMNormalizedTextures;}
    CvMat*                          GetNormalizedTexturesProject2Truncated()const {return this->m_CVMNormalizedTexturesProject2Truncated;}
    CvMat*                          GetTruancatedNormalizedTexturesMean() const {return this->m_CVMTruancatedNormalizedTexturesMean;}
    CvMat*                          GetTruancatedNormalizedTexturesSD() const {return this->m_CVMTruancatedNormalizedTexturesSD;}
    CvMat*                          GetNormalizedTexturesMean() const {return this->m_CVMNormalizedTexturesMean;}
    CvMat*                          GetNormalizedTexturesSD() const {return this->m_CVMNormalizedTexturesSD;}
    CvMat*                          GetNormalizedTexturesEigenVectors() const {return this->m_CVMNormalizedTexturesEigenVectors;}
    CvMat*                          GetNormalizedTexturesEigenValues() const {return this->m_CVMNormalizedTexturesEigenValues;}
    CvMat*                          GetTruncatedNormalizedTexturesEigenVectors() const 
                                    {return this->m_CVMTruncatedNormalizedTexturesEigenVectors;}
    CvMat*                          GetTruncatedNormalizedTexturesEigenValues() const 
                                    {return this->m_CVMTruncatedNormalizedTexturesEigenValues;}
    unsigned int                    GetTextureExtractionMethod() const {return this->m_iTextureExtractionMethod;}
    unsigned int                    GetNbOfChannels() const {return this->m_iNbOfChannels;}
    unsigned int                    GetNbOfPixels() const {return this->m_iNbOfPixels;}
    unsigned int                    GetNbOfTextures() const {return this->m_iNbOfTextures;}
    unsigned int                    GetNbOfTruncatedTextures() const {return this->m_iNbOfTruncatedTextures;}
    unsigned int                    GetNbOfEigenTexturesAtMost() const {return this->m_iNbOfEigenTexturesAtMost;}
    VO_AAMTexture2D3C               GetAAMReferenceTexture() const {return this->m_VOAAMReferenceTexture;}
    float                           GetAverageTextureStandardDeviation() const {return this->m_fAverageTextureStandardDeviation;}
    float                           GetTruncatedPercent_Texture() const {return this->m_fTruncatedPercent_Texture;}
    CvSize                          GetSizeOfTemplateFace() const {return this->m_CVSizeOfTemplateFace;}
    IplImage*                       GetIplTemplateFace() const {return this->m_IplTemplateFace;}
    IplImage*                       GetIplEdges() const {return this->m_IplEdges;}
    IplImage*                       GetIplConvexHull() const {return this->m_IplConvexHull;}
    IplImage*                       GetIplConcave() const {return this->m_IplConcave;}
    vector<IplImage*>               GetIplTriangles() const {return this->m_vIplTriangles;}
    vector<IplImage*>               GetIplImages() const {return this->m_vImages;}
    vector<VO_AAMTexture2D3C>       GetAAMTextures() const {return this->m_vAAMTextures;}
    vector<VO_AAMTexture2D3C>       GetAAMNormalizedTextures() const {return this->m_vAAMNormalizedTextures;}
    VO_AAMTexture2D3C               GetAAMMeanNormalizedTexturePrior() const {return this->m_VOAAMMeanNormalizedTexturePrior;}
    VO_AAMTexture2D3C               GetAAMMeanNormalizedTexturePost() const {return this->m_VOAAMMeanNormalizedTexturePost;}
    vector<VO_AAMWarpingPoint>      GetAAMPointWarpInfo() const {return this->m_vPointWarpInfo;}
    vector<VO_AAMWarpingPoint>      GetAAMNormalizedPointWarpInfo() const {return this->m_vNormalizedPointWarpInfo;}

    void                            SetTextures(const CvMat* inTextures) {this->m_CVMTextures = cvCloneMat(inTextures);}
    void                            SetNormalizedTextures(const CvMat* inNormalizedTextures)
                                    {this->m_CVMNormalizedTextures = cvCloneMat(inNormalizedTextures);}
    void                            SettNormalizedTexturesProject2Truncated(const CvMat* inNormalizedTexturesProject2Truncated) 
                                    {this->m_CVMNormalizedTexturesProject2Truncated = cvCloneMat(inNormalizedTexturesProject2Truncated);}
    void                            SetTruancatedNormalizedTexturesMean(const CvMat* inTruancatedNormalizedTexturesMean) 
                                    {this->m_CVMTruancatedNormalizedTexturesMean = cvCloneMat(inTruancatedNormalizedTexturesMean);}
    void                            SetTruancatedNormalizedTexturesSD(const CvMat* inTruancatedNormalizedTexturesSD) 
                                    {this->m_CVMTruancatedNormalizedTexturesSD = cvCloneMat(inTruancatedNormalizedTexturesSD);}
    void                            SetNormalizedTexturesMean(const CvMat* inNormalizedTexturesMean) 
                                    {this->m_CVMNormalizedTexturesMean = cvCloneMat(inNormalizedTexturesMean);}
    void                            SetNormalizedTexturesSD(const CvMat* inNormalizedTexturesSD) 
                                    {this->m_CVMNormalizedTexturesSD = cvCloneMat(inNormalizedTexturesSD);}
    void                            SetNormalizedTexturesEigenVectors(const CvMat* inNormalizedTexturesEigenVectors) 
                                    {this->m_CVMNormalizedTexturesEigenVectors = cvCloneMat(inNormalizedTexturesEigenVectors);}
    void                            SetNormalizedTexturesEigenValues(const CvMat* inNormalizedTexturesEigenValues) 
                                    {this->m_CVMNormalizedTexturesEigenValues = cvCloneMat(inNormalizedTexturesEigenValues);}
    void                            SetTruncatedNormalizedTexturesEigenVectors(const CvMat* inTruncatedNormalizedTexturesEigenVectors)
                                    {this->m_CVMTruncatedNormalizedTexturesEigenVectors = cvCloneMat(inTruncatedNormalizedTexturesEigenVectors);}
    void                            SetTruncatedNormalizedTexturesEigenValues(const CvMat* inTruncatedNormalizedTexturesEigenValues)
                                    {this->m_CVMTruncatedNormalizedTexturesEigenValues = cvCloneMat(inTruncatedNormalizedTexturesEigenValues);}
    void                            SetTextureExtractionMethod(unsigned int tem) {this->m_iTextureExtractionMethod = tem;}
    void                            SetNbOfNbOfChannels(unsigned int inNbOfChannels) {this->m_iNbOfChannels = inNbOfChannels;}
    void                            SetNbOfPixels(unsigned int inNbOfPixels) {this->m_iNbOfPixels = inNbOfPixels;}
    void                            SetNbOfTextures(unsigned int inNbOfTextures) {this->m_iNbOfTextures = inNbOfTextures;}
    void                            SetNbOfTruncatedTextures(unsigned int inNbOfTruncatedTextures)
                                    {this->m_iNbOfTruncatedTextures = inNbOfTruncatedTextures;}
    void                            SetNbOfEigenTexturesAtMost(unsigned int inNbOfEigenTexturesAtMost)
                                    {this->m_iNbOfEigenTexturesAtMost = inNbOfEigenTexturesAtMost;}
    void                            SetAAMReferenceTexture(const VO_AAMTexture2D3C& inAAMReferenceTexture)
                                    {this->m_VOAAMReferenceTexture = inAAMReferenceTexture;}
    void                            SetAverageTextureStandardDeviation(float inAverageTextureStandardDeviation) 
                                    {this->m_fAverageTextureStandardDeviation = inAverageTextureStandardDeviation;}
    void                            SetTruncatedPercent_Texture(float inTP) {this->m_fTruncatedPercent_Texture = inTP;}
    void                            SetSizeOfTemplateFace(const CvSize& inSize) {this->m_CVSizeOfTemplateFace = inSize;}
    void                            SetIplTemplateFace(const IplImage* inIplTF) 
                                    {this->m_IplTemplateFace = cvCloneImage(inIplTF);}
    void                            SetIplEdges(const IplImage* inIplEdges) 
                                    {this->m_IplEdges = cvCloneImage(inIplEdges);}
    void                            SetIplConvexHull(const IplImage* inIplCH) 
                                    {this->m_IplConvexHull = cvCloneImage(inIplCH);}
    void                            SetIplConcave(const IplImage* inIplConcave) 
                                    {this->m_IplConcave = cvCloneImage(inIplConcave);}
    void                            SetIplTriangles(const vector<IplImage*>& inIplTriangles) 
                                    {
                                        unsigned int NbOfSamples = inIplTriangles.size();
                                        this->m_vIplTriangles.resize(NbOfSamples);
                                        for (unsigned int i = 0; i < NbOfSamples; i++)
                                        {
                                            this->m_vIplTriangles[i] = cvCloneImage(inIplTriangles[i]); 
                                        }
                                    }
    void                            SetIplImages(const vector<IplImage*>& inIplImages)
                                    {
                                        unsigned int NbOfSamples = inIplImages.size();
                                        this->m_vIplTriangles.resize(NbOfSamples);
                                        for (unsigned int i = 0; i < NbOfSamples; i++)
                                        {
                                            this->m_vImages[i] = cvCloneImage(inIplImages[i]); 
                                        }
                                    }
    void                            SetAAMTextures(const vector<VO_AAMTexture2D3C>& inAAMTextures) {this->m_vAAMTextures = inAAMTextures;}
    void                            SetAAMNormalizedTextures(const vector<VO_AAMTexture2D3C>& inAAMNormalizedTextures)
                                    {this->m_vAAMNormalizedTextures = inAAMNormalizedTextures;}
    void                            SetAAMMeanNormalizedTexturePrior(const VO_AAMTexture2D3C& inAAMMeanNormalizedTexture) 
                                    {this->m_VOAAMMeanNormalizedTexturePrior = inAAMMeanNormalizedTexture;}
    void                            SetAAMMeanNormalizedTexturePost(const VO_AAMTexture2D3C& inAAMMeanNormalizedTexture) 
                                    {this->m_VOAAMMeanNormalizedTexturePost = inAAMMeanNormalizedTexture;}
    void                            SetAAMPointWarpInfo(const vector<VO_AAMWarpingPoint>& inPointWarpInfo) 
                                    {this->m_vPointWarpInfo = inPointWarpInfo;}
    void                            SetAAMNormalizedPointWarpInfo(const vector<VO_AAMWarpingPoint>& inAAMNormalizedPointWarpInfo)
                                    {this->m_vNormalizedPointWarpInfo = inAAMNormalizedPointWarpInfo;}

};


#endif // __VO_AAM__

