/****************************************************************************
* File Name:        VO_AAMBasic.h                                           *
* Copyright (C):    2006, 2007, 2008 by JIA Pei                             *
* Version:          0.2                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-03-04                                              *
* Main Content:     Basic AAM Model, shape + texture concatenation          *
* Used Library:     STL + OpenCV                                            *
****************************************************************************/

#ifndef __VO_AAMBasic__
#define __VO_AAMBasic__


#include <vector>

#include "cv.h"
#include "highgui.h"

#include "VO_Common.h"
#include "VO_AAMEdge.h"
#include "VO_AAMTexture2D3C.h"
#include "VO_AAMWarpingPoint.h"
#include "VO_AAM.h"

using namespace std;

/****************************************************************************
* Class Name:       VO_AAMBasic                                             *
* Author:           JIA Pei                                                 *
* Create Date:      2008-03-04                                              *
* Function:         AAM Model, shape + texture                              *
****************************************************************************/
class VO_AAMBasic : public VO_AAM
{
protected:

    /** Shape weights, for scaling to texture scale. 20*20 */
    CvMat*						    m_CVMWeightsScaleShape2Texture;

    /** Concatenated samples. 240*136 */
    CvMat*						    m_CVMConcatenated;

    /** Original concatenated project to truncated space. For IMM, 240*18 */
    CvMat*						    m_CVMConcatenatedProject2Truncated;

    /** Mean vector of m_CVMConcatenatedProject2Truncated. For IMM, 1*18, should be 0 */
    CvMat*						    m_CVMTruancatedConcatenatedMean;

    /** Standard deviation of m_CVMConcatenatedProject2Truncated. For IMM, 1*18 */
    CvMat*						    m_CVMTruancatedConcatenatedSD;

    /** Mean vector of m_CVMConcatenated. For IMM, 1*136,should be 0,2-norm!=1 used for cvCalcPCA*/
    CvMat*						    m_CVMConcatenatedMean;

    /** Standard deviation of m_CVMNormalizedTextures. For IMM, 1*(30795*3=92385). Could be used as weights for AAM */
    CvMat*						    m_CVMConcatenatedSD;

    /** Original Eigenvectors of m_CVMConcatenated. For IMM, MIN(240,136)*136 = 136*136 */
    CvMat*						    m_CVMConcatenatedEigenVectors;

    /** Original Eigenvalues of m_CVMConcatenated. For IMM, 1*MIN(240,136) = 1*136 */
    CvMat*						    m_CVMConcatenatedEigenValues;

    /** Truncated Eigenvectors of m_CVMConcatenated. For IMM, 18*136 */
    CvMat*						    m_CVMTruncatedConcatenatedEigenVectors;

    /** Truncated Eigenvalues of m_CVMConcatenated. For IMM, 1*18 */
    CvMat*						    m_CVMTruncatedConcatenatedEigenValues;


    /** The shape part of the concatenated eigenvectors, 18*20 */
    CvMat*						    m_CVMPcs;

    /** The texture part of the concatenated eigenvectors, 18*116 */
    CvMat*						    m_CVMPcg;

    /** For shape, 116*18, refer to Cootes "Comparing Variations on the Active Appearance Model Algorithm" */
    CvMat*						    m_CVMQs;

    /** For texture, 92385*18, refer to Cootes "Comparing Variations on the Active Appearance Model Algorithm" */
    CvMat*						    m_CVMQg;

    /** For shape, 18*92385, Multivariate Linear Regression Matrix, refer to Stegmann's AAM-API Equation (7.4) */
    CvMat*						    m_CVMRc;

    /** For shape, 4*92385, Multivariate Linear Regression Matrix, refer to Stegmann's AAM-API Equation (7.4) */
    CvMat*						    m_CVMRt;

    /** Totally, n=m(4k+24)=240*(4*18+24)=23040 displacements. 4*18 */
    vector<vector<float> > 	        m_vCDisps;

    /** Totally, n=m(4k+24)=240*(4*18+24)=23040 displacements. 4*4, refer to AAP-API page 3 of 10 */
    vector<vector<float> > 	        m_vPoseDisps;

    /** Stegmann: Gradient Matrix 92385*18 */
    CvMat*						    m_CVMCParamGradientMatrix;

    /** Stegmann: Pose Gradient Matrix 92385*4 we may ignore this */
    CvMat*						    m_CVMPoseGradientMatrix;

    /** Number of concatenated = m_iNbOfTruncatedShapes + m_iNbOfTruncatedTextures. For IMM: 20+116=136 */
    unsigned int				    m_iNbOfConcatenated;

    /** Truncated AAM concatenated model vector length. For IMM: 18 */
    unsigned int				    m_iNbOfTruncatedConcatenated;
    
    /** Most possible concatenated eigens before PCA. For IMM: min (136, 240) = 136 */
    unsigned int				    m_iNbOfEigenConcatenatedAtMost;

    /** Truncate Percentage for AAM concatenated PCA. Normally, 0.95 */
    float                           m_fTruncatedPercent_Concatenated;

    /** The same as m_CVMConcatenated, but in a vector format. For IMM, 240*136 */
    vector<vector<float> >		    m_vAAMConcatenated;

    /** The same as m_CVMConcatenatedMean, but in a vector format. For IMM, 1*136. should equal to 0 */
    vector<float>				    m_vAAMMeanConcatenated;

    /** Initialization */
    void                            init();

public:

    /** Default constructor to create a VO_AAMBasic object */
    VO_AAMBasic();

    /** Destructor */
    ~VO_AAMBasic();

    //void                        VO_ShapeTexture2Concatenated( VO_AAMShape2D iShape, VO_AAMTexture2D3C iTexture, vector<float>& b );



    /** Off-line build prediction matrix for fitting. */
    /** Please refer to http://www2.imm.dtu.dk/~aam/main/node16.html; revised from AAM-API */
    void						    VO_BuildRegressionMatrices();

    /** Carry out C displacement experiments */
    void						    VO_DoCParamExperiments();

    /** Carry out pose displacement experiments */
    void						    VO_DoPoseExperiments();

    /** Carry out multi variate linear regression experiments */
    void						    VO_DoRegression();



    /** Build gradient matrices */
    void						    VO_BuildGradientMatrices();

    /** Build gradient matrices in terms of C parameters */
    void						    VO_EstCParamGradientMatrix(CvMat* oCParamGM);

    /** Build gradient matrices in terms of pose */
    void						    VO_EstPoseGradientMatrix(CvMat* oPoseGM);

    /** Concatenated parameters back projected to shape parameters and texture parameters */
    void				            VO_CParamBackProjectToSParamTParam(const CvMat* inC, CvMat* outS, CvMat* outT);

    /** Concatenated parameters back projected to aligned shape */
    VO_AAMShape2D   	            VO_CParamBackProjectToAlignedShape(const CvMat* inC);

    /** Concatenated parameters back projected to normalized texture */
    VO_AAMTexture2D3C               VO_CParamBackProjectToNormalizedTexture(const CvMat* inC);

    /** Build displacement sets */
    void					        VO_CreateDisplacementSets();

    /** Build displacement sets for C parameters */
    static vector<vector<float> >	VO_CalcCParamDisplacementVectors(const vector<float>& vStdDisp, const CvMat* cVectors);

    /** Build displacement sets for Pose parameters */
    static vector<vector<float> >	VO_CalcPoseDisplacementVectors( const vector<float> &vScaleDisp, const vector<float> &vRotDisp, const vector<float> &vXDisp, const vector<float> &vYDisp, int method = LINEARIZEMAP);

    /** Change from pose parameters to displacement vector */
    static void					    VO_Pose2DisplacementVector( const vector<float>& vPose, vector<float> &vDisp, int method = LINEARIZEMAP);

    /** Change from displacement vector to pose parameters */
    static void					    VO_DisplacementVector2Pose( const vector<float> &vDisp, vector<float>& vPose, int method = LINEARIZEMAP);

    /** Load all concatenated data from both aligned shapes and normalized textures */
    void				            VO_LoadConcatenated();

    /** Build Basic AAM model */
    void						    VO_BuildAAMBasic(const vector<VO_AAMShape2D> &vShapes, const vector<IplImage*> &imgs, int channel = COLORCHANNELS, int tbm = DIRECTTEXTURE, float TPShape = 0.95f, float TPTexture = 0.95f, float TPConcatenated = 0.95f);

    /** Save AAMBasic, to a specified folder */
    void                            VO_Save(const string& fd);

    /** Load Parameters for fitting */
    void                            VO_LoadParamters4Fitting(const string& fd);

    /** Gets and Sets */
    CvMat*                          GetWeightsScaleShape2Texture() const {return this->m_CVMWeightsScaleShape2Texture;}
    CvMat*                          GetConcatenated() const {return this->m_CVMConcatenated;}
    CvMat*                          GetConcatenatedProject2Truncated()const {return this->m_CVMConcatenatedProject2Truncated;}
    CvMat*                          GetTruancatedConcatenatedMean() const {return this->m_CVMTruancatedConcatenatedMean;}
    CvMat*                          GetTruancatedConcatenatedSD() const {return this->m_CVMTruancatedConcatenatedSD;}
    CvMat*                          GetConcatenatedMean() const {return this->m_CVMConcatenatedMean;}
    CvMat*                          GetConcatenatedSD() const {return this->m_CVMConcatenatedSD;}
    CvMat*                          GetConcatenatedEigenVectors() const {return this->m_CVMConcatenatedEigenVectors;}
    CvMat*                          GetConcatenatedEigenValues() const {return this->m_CVMConcatenatedEigenValues;}
    CvMat*                          GetTruncatedConcatenatedEigenVectors() const {return this->m_CVMTruncatedConcatenatedEigenVectors;}
    CvMat*                          GetTruncatedConcatenatedEigenValues() const {return this->m_CVMTruncatedConcatenatedEigenValues;}
    CvMat*                          GetPcs() const {return this->m_CVMPcs;}
    CvMat*                          GetPcg() const {return this->m_CVMPcg;}
    CvMat*                          GetQs() const {return this->m_CVMQs;}
    CvMat*                          GetQg() const {return this->m_CVMQg;}
    CvMat*                          GetRc() const {return this->m_CVMRc;}
    CvMat*                          GetRt() const {return this->m_CVMRt;}
    vector<vector<float> >          GetCDisps() const {return this->m_vCDisps;}
    vector<vector<float> >          GetPoseDisps() const {return this->m_vPoseDisps;}
    CvMat*                          GetCParamGradientMatrix() const {return this->m_CVMCParamGradientMatrix;}
    CvMat*                          GetPoseGradientMatrix() const {return this->m_CVMPoseGradientMatrix;}
    unsigned int                    GetNbOfConcatenated() const {return this->m_iNbOfConcatenated;}
    unsigned int                    GetNbOfTruncatedConcatenated() const {return this->m_iNbOfTruncatedConcatenated;}
    unsigned int                    GetNbOfEigenConcatenatedAtMost() const {return this->m_iNbOfEigenConcatenatedAtMost;}
    float                           GetTruncatedPercent_Concatenated() const {return this->m_fTruncatedPercent_Concatenated;}
    vector<vector<float> >          GetAAMConcatenated() const {return this->m_vAAMConcatenated;}
    vector<float>                   GetAAMMeanConcatenated() const {return this->m_vAAMMeanConcatenated;}

    void                            SetWeightsScaleShape2Texture(const CvMat* inWeightsScaleShape2Texture) 
                                    {this->m_CVMWeightsScaleShape2Texture = cvCloneMat(inWeightsScaleShape2Texture);}
    void                            SetConcatenated(const CvMat* inConcatenated) {this->m_CVMConcatenated = cvCloneMat(inConcatenated);}
    void                            SetConcatenatedProject2Truncated(const CvMat* inConcatenatedProject2Truncated) 
                                    {this->m_CVMConcatenatedProject2Truncated = cvCloneMat(inConcatenatedProject2Truncated);}
    void                            SetTruancatedConcatenatedMean(const CvMat* inTruancatedConcatenatedMean) 
                                    {this->m_CVMTruancatedConcatenatedMean = cvCloneMat(inTruancatedConcatenatedMean);}
    void                            SetTruancatedConcatenatedSD(const CvMat* inTruancatedConcatenatedSD) 
                                    {this->m_CVMTruancatedConcatenatedSD = cvCloneMat(inTruancatedConcatenatedSD);}
    void                            SetConcatenatedMean(const CvMat* inConcatenatedMean) {this->m_CVMConcatenatedMean = cvCloneMat(inConcatenatedMean);}
    void                            SetConcatenatedSD(const CvMat* inConcatenatedSD) {this->m_CVMConcatenatedSD = cvCloneMat(inConcatenatedSD);}
    void                            SetConcatenatedEigenVectors(const CvMat* inConcatenatedEigenVectors) 
                                    {this->m_CVMConcatenatedEigenVectors = cvCloneMat(inConcatenatedEigenVectors);}
    void                            SetConcatenatedEigenValues(const CvMat* inConcatenatedEigenValues) 
                                    {this->m_CVMConcatenatedEigenValues = cvCloneMat(inConcatenatedEigenValues);}
    void                            SetTruncatedConcatenatedEigenVectors(const CvMat* inTruncatedConcatenatedEigenVectors)
                                    {this->m_CVMTruncatedConcatenatedEigenVectors = cvCloneMat(inTruncatedConcatenatedEigenVectors);}
    void                            SetTruncatedConcatenatedEigenValues(const CvMat* inTruncatedConcatenatedEigenValues)
                                    {this->m_CVMTruncatedConcatenatedEigenValues = cvCloneMat(inTruncatedConcatenatedEigenValues);}
    void                            SetPcs(const CvMat* inPcs) {this->m_CVMPcs = cvCloneMat(inPcs);}
    void                            SetPcg(const CvMat* inPcg) {this->m_CVMPcg = cvCloneMat(inPcg);}
    void                            SetQs(const CvMat* inQs) {this->m_CVMQs = cvCloneMat(inQs);}
    void                            SetQg(const CvMat* inQg) {this->m_CVMQg = cvCloneMat(inQg);}
    void                            SetRc(const CvMat* inRc) {this->m_CVMRc = cvCloneMat(inRc);}
    void                            SetRt(const CvMat* inRt) {this->m_CVMRt = cvCloneMat(inRt);}
    void                            SetCDisps(const vector<vector<float> >& inCDisps) {this->m_vCDisps = inCDisps;}
    void                            SetPoseDisps(const vector<vector<float> >& inPoseDisps) {this->m_vPoseDisps = inPoseDisps;}
    void                            SetCParamGradientMatrix(const CvMat* inCParamGradientMatrix) 
                                    {this->m_CVMCParamGradientMatrix = cvCloneMat(inCParamGradientMatrix);}
    void                            SetPoseParamGradientMatrix(const CvMat* inPoseGradientMatrix) 
                                    {this->m_CVMPoseGradientMatrix = cvCloneMat(inPoseGradientMatrix);}
    void                            SetNbOfConcatenated(unsigned int inNbOfConcatenated) {this->m_iNbOfConcatenated = inNbOfConcatenated;}
    void                            SetNbOfTruncatedConcatenated(unsigned int inNbOfTruncatedConcatenated) 
                                    {this->m_iNbOfTruncatedConcatenated = inNbOfTruncatedConcatenated;}
    void                            SetNbOfEigenConcatenatedAtMost(unsigned int inNbOfEigenConcatenatedAtMost) 
                                    {this->m_iNbOfEigenConcatenatedAtMost = inNbOfEigenConcatenatedAtMost;}
    void                            SetTruncatedPercent_Concatenated(float inTruncatedPercent_Concatenated) 
                                    {this->m_fTruncatedPercent_Concatenated = inTruncatedPercent_Concatenated;}
    void                            SetAAMConcatenated(const vector<vector<float> >& inAAMConcatenated) {this->m_vAAMConcatenated = inAAMConcatenated;}
    void                            SetAAMMeanConcatenated(const vector<float>& inAAMMeanConcatenated)
                                    {this->m_vAAMMeanConcatenated = inAAMMeanConcatenated;}

 };


#endif // __VO_AAMBasic__

