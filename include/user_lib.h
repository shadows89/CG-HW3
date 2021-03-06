/******************************************************************************
* User_lib.h - Header file for the User Interaction library.		      *
* This header is also the interface header to the world.		      *
*******************************************************************************
* (C) Gershon Elber, Technion, Israel Institute of Technology                 *
*******************************************************************************
* Written by Gershon Elber, Mar. 90.					      *
******************************************************************************/

#ifndef USER_LIB_H
#define USER_LIB_H

#include "cagd_lib.h"
#include "geom_lib.h"
#include "iritprsr.h"

#define USER_HC_VEC_DRAW_SCALE	0.25

typedef enum {
    USER_ERR_WRONG_SRF,
    USER_ERR_MISSING_ATTRIB,
    USER_ERR_WRONG_ANGLE,
    USER_ERR_INVALID_PLANE,
    USER_ERR_RATIONAL_NO_SUPPORT,
    USER_ERR_NON_CRV_OBJ_IN_FONT,
    USER_ERR_NO_ADJ_INFO,
    USER_ERR_NO_NRML_INFO,
    USER_ERR_NO_CRVTR_INFO,
    USER_ERR_EXPCT_REG_TRIANG,
    USER_ERR_EXPCT_POLY_OBJ,
    USER_ERR_EXPCT_SRF_OBJ,
    USER_ERR_EXPCT_VRTX_NRMLS,
    USER_ERR_EXPCT_VRTX_UVS,
    USER_ERR_UNDEFINE_ERR,
    USER_ERR_WRONG_CTLPT_INDEX,
    USER_ERR_INVALID_SIZE,

    USER_ERR_NC_INVALID_PARAM,
    USER_ERR_NC_INVALID_INTER,
    USER_ERR_NC_NO_POLYLINES,
    USER_ERR_NC_MIX_CRVS_PLLNS
} UserFatalErrorType;

typedef enum {					/* Type of surface marching. */
    USER_SRF_MARCH_ISO_PARAM,
    USER_SRF_MARCH_ISOCLINES,
    USER_SRF_MARCH_ORTHOCLINES,
    USER_SRF_MARCH_PRIN_CRVTR
} UserSrfMarchType;

typedef struct UserFEKElementStruct {
    IrtRType k[2][2];			  /* (x, y) x (x, y) contributions. */
} UserFEKElementStruct;

typedef struct UserFECElementStruct {
    IrtRType c[2];			           /* (x, y) contributions. */
} UserFECElementStruct;

typedef struct UserFEInterIntervalStruct {
    struct UserFEInterIntervalStruct *Pnext;
    CagdRType T1Min, T1Max;		    /* Interval of overlap in Crv1. */
    CagdRType T2Min, T2Max;		    /* Interval of overlap in Crv2. */
    CagdRType Antipodal1, Antipodal2;  /* Locations of maximal penetration. */
    CagdVType ProjNrml;		    /* Direction to project penetration on. */
} UserFEInterIntervalStruct;

typedef void (*UserSetErrorFuncType)(UserFatalErrorType);
typedef int (*UserRegisterTestConverganceFuncType)(IrtRType CrntDist, int i);
typedef int (*UserCntrIsValidCntrPtFuncType)(const CagdSrfStruct *Srf,
					     CagdRType U,
					     CagdRType V);
typedef void (*UserHCEditDrawCtlPtFuncType)(int PtIndex,
					    int PtUniqueID,
					    IrtRType *Pos,
					    IrtRType *TanBack,
					    IrtRType *TanForward);

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Surface-Primitive Geometry (rays, points, etc.) interactions. */

VoidPtr IntrSrfHierarchyPreprocessSrf(const CagdSrfStruct *Srf,
				      IrtRType FineNess);
void IntrSrfHierarchyFreePreprocess(VoidPtr Handle);
CagdBType IntrSrfHierarchyTestRay(VoidPtr Handle,
				  CagdPType RayOrigin,
				  CagdVType RayDir,
				  CagdUVType InterUV);
CagdBType IntrSrfHierarchyTestPt(VoidPtr Handle,
				 CagdPType Pt,
				 CagdBType Nearest,
				 CagdUVType InterUV);

/* Surface-plane contouring. */

IPPolygonStruct *UserCntrSrfWithPlane(const CagdSrfStruct *Srf,
                                      IrtPlnType Plane,
                                      IrtRType FineNess);
IPPolygonStruct *UserCntrEvalToE3(const CagdSrfStruct *Srf,
				  IPPolygonStruct *Cntrs,
				  UserCntrIsValidCntrPtFuncType
				                             ValidCntrPtFunc);

/* Linear Bsplines vs polylines convertion. */

CagdCrvStruct *UserPolyline2LinBsplineCrv(const IPPolygonStruct *Poly,
					  CagdBType FilterDups);
CagdCrvStruct *UserPolylines2LinBsplineCrvs(const IPPolygonStruct *Polys,
					    CagdBType FilterDups);

/* Surface cone decomposition. */

IPObjectStruct *UserSrfVisibConeDecomp(const CagdSrfStruct *Srf,
				       CagdRType Resolution,
				       CagdRType ConeSize);
TrimSrfStruct *UserVisibilityClassify(const IPObjectStruct *SclrSrf,
				      TrimSrfStruct *TrimmedSrfs);
IPObjectStruct *UserViewingConeSrfDomains(const CagdSrfStruct *Srf,
					  const CagdSrfStruct *NSrf,
					  const IPPolygonStruct *ConeDirs,
					  CagdRType Resolution,
					  CagdRType ConeSize,
					  CagdRType Euclidean);
IPPolygonStruct *UserSrfTopoAspectGraph(CagdSrfStruct *PSrf,
					CagdRType Tolerance);

/* Surface marching. */

IPPolygonStruct *UserMarchOnSurface(UserSrfMarchType MarchType,
				    const CagdUVType UVOrig,
				    const CagdVType DirOrig,
				    const CagdSrfStruct *Srf,
				    const CagdSrfStruct *NSrf,
				    const CagdSrfStruct *DuSrf,
				    const CagdSrfStruct *DvSrf,
				    CagdRType Length,
				    CagdRType FineNess,
				    CagdBType ClosedInU,
				    CagdBType ClosedInV);
IPPolygonStruct *UserMarchOnPolygons(const IPObjectStruct *PObj,
				     UserSrfMarchType MarchType,
				     const IPPolygonStruct *PlHead,
				     IPVertexStruct *VHead,
				     CagdRType Length);

/* Curve/Surface visibility and accessibility. */

IPObjectStruct *UserCrvViewMap(const CagdCrvStruct *Crv,
			       const CagdCrvStruct *ViewCrv,
			       CagdRType SubTol,
			       CagdRType NumTol,
			       CagdBType TrimInvisible);
IPObjectStruct *UserCrvAngleMap(const CagdCrvStruct *Crv,
				CagdRType Tolerance,
				CagdRType Angle);
IPObjectStruct *UserCrvOMDiagExtreme(const CagdCrvStruct *Crv,
				     const IPObjectStruct *OM,
				     int DiagExtRes);

CagdCrvStruct *UserCrvVisibleRegions(const CagdCrvStruct *Crv,
				     const CagdRType *View,
				     CagdRType Tolerance);

TrimSrfStruct *UserMoldReliefAngle2Srf(const CagdSrfStruct *Srf,
				       const CagdVType VDir,
				       CagdRType Theta,
				       int MoreThanTheta,
				       CagdRType Tolerance);
CagdSrfStruct *UserMoldRuledRelief2Srf(const CagdSrfStruct *Srf,
				       const CagdVType VDir,
				       CagdRType Theta,
				       CagdRType Tolerance);

/* Minimal distance to polylines/gons. */

IrtRType UserMinDistLineBBox(const IrtPtType LinePos,
			     const IrtVecType LineDir,
			     IrtBboxType BBox);
IrtRType UserMinDistLinePolygonList(const IrtPtType LinePos,
				    const IrtVecType LineDir,
				    IPPolygonStruct *Pls,
				    IPPolygonStruct **MinPl,
				    IrtPtType MinPt,
				    IrtRType *HitDepth,
				    IrtRType *IndexFrac);
IrtRType UserMinDistLinePolylineList(const IrtPtType LinePos,
				     const IrtVecType LineDir,
				     IPPolygonStruct *Pls,
				     int PolyClosed,
				     IPPolygonStruct **MinPl,
				     IrtPtType MinPt,
				     IrtRType *IndexFrac);
IrtRType UserMinDistPointPolylineList(const IrtPtType Pt,
				      IPPolygonStruct *Pls,
				      IPPolygonStruct **MinPl,
				      IPVertexStruct **MinV,
				      int *Index);

/* Surface surface intersection. */

int UserSrfSrfInter(const CagdSrfStruct *Srf1,
		    const CagdSrfStruct *Srf2,
		    int Euclidean,
		    CagdRType Eps,
		    int AlignSrfs,
		    CagdCrvStruct **Crvs1,
		    CagdCrvStruct **Crvs2);

/* Jacobian of trivariates and zero set. */

IPObjectStruct *UserTVZeroJacobian(const TrivTVStruct *Tv,
				   CagdBType Euclidean,
				   int SkipRate,
				   const CagdRType Fineness[3]);
IPObjectStruct *UserTrivarZeros(const TrivTVStruct *Tv,
				const TrivTVStruct *TvEuclidean,
				int SkipRate,
				const CagdRType Fineness[3]);

/* Z direction collision. */
IrtRType UserTwoObjMaxZRelMotion(IPObjectStruct *PObj1,
				 IPObjectStruct *PObj2,
				 IrtRType FineNess,
				 int NumIters);

/* Geometry registration. */

int UserRegisterTestConvergance(IrtRType Dist, int i);
IrtRType UserRegisterTwoPointSets(int n1,
				  IrtPtType *PtsSet1,
				  int n2,
				  IrtPtType *PtsSet2,
				  IrtRType AlphaConverge,
				  IrtRType Tolerance,
				  UserRegisterTestConverganceFuncType
				      RegisterTestConvergance,
				  IrtHmgnMatType RegMat);
IrtRType UserRegisterPointSetSrf(int n,
				 IrtPtType *PtsSet,
				 const CagdSrfStruct *Srf,
				 IrtRType AlphaConverge,
				 IrtRType Tolerance,
				 UserRegisterTestConverganceFuncType
				                    RegisterTestConvergance,
				 IrtHmgnMatType RegMat);

/* Bump mapping. */

IPObjectStruct *UserDDMPolysOverSrf(const CagdSrfStruct *Srf,
				    const IPObjectStruct *Texture,
				    IrtRType UDup,
				    IrtRType VDup,
				    int LclUV);
IPObjectStruct *UserDDMPolysOverPolys(IPObjectStruct *PlSrf,
				      const IPObjectStruct *Texture,
				      IrtRType UDup,
				      IrtRType VDup,
				      int LclUV);


/* Freeform kernels. */

IPObjectStruct *UserSrfKernel(CagdSrfStruct *Srf,
			      CagdRType Tolerance,
			      int SkipRate);

/* Freeform umbilical. */

MvarPtStruct *UserSrfUmbilicalPts(const CagdSrfStruct *Srf,
				  CagdRType SubTol,
				  CagdRType NumTol);

/* Curvatre analysis. */

IPObjectStruct *UserCrvCrvtrByOneCtlPt(const CagdCrvStruct *Crv,
				       int CtlPtIdx,
				       CagdRType Min,
				       CagdRType Max,
				       CagdRType SubdivTol,
				       CagdRType NumerTol,
				       int Operation);

/* Image scaling by bivariate spline surface. */

IrtImgPixelStruct *IrtImgScaleImage(IrtImgPixelStruct *InImage,
				    int InMaxX,
				    int InMaxY,
				    int InAlpha,
				    int OutMaxX,
				    int OutMaxY,
				    int Order);

/* Warping of text using composition with surfaces. */

IPObjectStruct *UserWarpTextOnSurface(CagdSrfStruct *Srf,
				      const char *Txt,
				      IrtRType HSpace,
				      IrtRType VBase,
				      IrtRType VTop,
				      IrtRType Ligatures);

/* User inteface to construct piecewise planar cubic Hermite crvs. */

VoidPtr UserHCEditInit(CagdRType StartX, CagdRType StartY, CagdBType Periodic);
VoidPtr UserHCEditFromCurve(const CagdCrvStruct *Crv, CagdRType Tol);
int UserHCEditIsPeriodic(VoidPtr HC);
void UserHCEditSetPeriodic(VoidPtr HC, CagdBType Periodic);
CagdBType UserHCEditGetCtlPtCont(VoidPtr HC, int Index);
void UserHCEditSetCtlPtCont(VoidPtr HC, int Index, CagdBType Cont);
void UserHCEditSetDrawCtlptFunc(VoidPtr HC,
				UserHCEditDrawCtlPtFuncType CtlPtDrawFunc);
void UserHCEditDelete(VoidPtr HC);
VoidPtr UserHCEditCopy(VoidPtr HC);

int UserHCEditTranslate(VoidPtr HC, CagdRType Dx, CagdRType Dy);
int UserHCEditCreateAppendCtlpt(VoidPtr HC,
				CagdRType x,
				CagdRType y,
				int MouseMode);
int UserHCEditCreateDone(VoidPtr HC, CagdRType StartX, CagdRType StartY);
int UserHCEditInsertCtlpt(VoidPtr HC, CagdRType x, CagdRType y, CagdRType t);
int UserHCEditDeleteCtlpt(VoidPtr HC, CagdRType x, CagdRType y);
int UserHCEditUpdateCtl(VoidPtr HC,
			int CtlIndex,
			CagdBType IsPosition,
			CagdRType NewX,
			CagdRType NewY);
int UserHCEditMoveCtl(VoidPtr HC,
		      CagdRType OldX,
		      CagdRType OldY,
		      CagdRType NewX,
		      CagdRType NewY,
		      int MouseMode,
		      CagdRType *MinDist);
int UserHCEditMoveCtlPt(VoidPtr HC, 
			CagdRType OldX,
			CagdRType OldY,
			CagdRType NewX,
			CagdRType NewY,
			int MouseMode);
int UserHCEditMoveCtlTan(VoidPtr HC,
			 CagdRType OldX,
			 CagdRType OldY,
			 CagdRType NewX,
			 CagdRType NewY,
			 int MouseMode);
int UserHCEditIsNearCrv(VoidPtr HC,
			CagdRType x,
			CagdRType y,
			CagdRType *t,
			CagdRType Eps,
			int NormalizeZeroOne);
int UserHCEditIsNearCtlPt(VoidPtr HC,
			  CagdRType *x,
			  CagdRType *y,
			  int *Index,
			  int *UniqueID,
			  CagdRType Eps);
int UserHCEditIsNearCtlTan(VoidPtr HC,
			   CagdRType *x,
			   CagdRType *y,
			   int *Index,
			   int *UniqueID,
			   CagdBType *Forward,
			   CagdRType Eps);
CagdCrvStruct *UserHCEditGetCrvRepresentation(VoidPtr HC, int ArcLen);
int UserHCEditGetCtlPtTan(VoidPtr HC, int Index, CagdPType Pos, CagdPType Tan);
int UserHCEditGetNumCtlPt(VoidPtr HC);
int UserHCEditDrawCtlpts(VoidPtr HC, int DrawTans);
int UserHCEditMatTrans(VoidPtr HC, IrtHmgnMatType Mat);
int UserHCEditTransform(VoidPtr HC, CagdRType *Dir, CagdRType Scl);
int UserHCEditRelativeTranslate(VoidPtr HC, CagdRType *Dir);
int UserHCEditEvalDefTans(VoidPtr HC, int Index);

/* Functions to create machining NC tool path. */

IPObjectStruct *UserNCContourToolPath(const IPObjectStruct *PObj,
				      IrtRType Offset,
				      IrtRType ZBaseLevel,
				      IrtRType Tolerance,
				      IPNCGCodeUnitType Units);
IPObjectStruct *UserNCPocketToolPath(const IPObjectStruct *PObj,
				     IrtRType ToolRadius,
				     IrtRType RoughOffset,
				     IrtRType TPathSpace,
				     IrtRType TPathJoin,
				     IPNCGCodeUnitType Units,
				     int TrimSelfInters);

/* Functions related to finite elements' evaluations. */

UserFEKElementStruct *UserFEKBuildMat(CagdSrfStruct *Srf,
				      int IntegRes,
				      IrtRType E,
				      IrtRType Nu,
				      int *Size);
UserFEKElementStruct *UserFEKBuildMat2(CagdPType *Points,
				       int ULength,
				       int VLength,
				       int UOrder,
				       int VOrder,
				       CagdEndConditionType EndCond,
				       int IntegRes,
				       IrtRType E,
				       IrtRType Nu,
				       int *Size);
CagdBType UserFEPointInsideSrf(CagdSrfStruct *Srf, CagdPType Pt);
UserFEInterIntervalStruct *UserFEGetInterInterval(CagdCrvStruct *Crv1,
						  CagdSrfStruct *Srf1,
						  CagdCrvStruct *Crv2,
						  CagdSrfStruct *Srf2);
UserFECElementStruct *UserFEBuildC1Mat(CagdCrvStruct *Crv1,
				       CagdSrfStruct *Srf1,
				       CagdCrvStruct *Crv2,
				       CagdSrfStruct *Srf2,
				       int IntegRes);
UserFECElementStruct *UserFEBuildC1Mat2(CagdPType *Crv1Pts,
					int Crv1Length,
					int Crv1Order,
					CagdPType *Srf1Pts,
					int Srf1ULength,
					int Srf1VLength,
					int Srf1UOrder,
					int Srf1VOrder,
					CagdPType *Crv2Pts,
					int Crv2Length,
					int Crv2Order,
					CagdPType *Srf2Pts,
					int Srf2ULength,
					int Srf2VLength,
					int Srf2UOrder,
					int Srf2VOrder,
					CagdEndConditionType EndCond,
					int IntegRes);
UserFECElementStruct *UserFEBuildC2Mat(CagdCrvStruct *Crv1,
				       CagdSrfStruct *Srf1,
				       CagdCrvStruct *Crv2,
				       CagdSrfStruct *Srf2,
				       int IntegRes);
UserFECElementStruct *UserFEBuildC2Mat2(CagdPType *Crv1Pts,
					int Crv1Length,
					int Crv1Order,
					CagdPType *Srf1Pts,
					int Srf1ULength,
					int Srf1VLength,
					int Srf1UOrder,
					int Srf1VOrder,
					CagdPType *Crv2Pts,
					int Crv2Length,
					int Crv2Order,
					CagdPType *Srf2Pts,
					int Srf2ULength,
					int Srf2VLength,
					int Srf2UOrder,
					int Srf2VOrder,
					CagdEndConditionType EndCond,
					int IntegRes);
IrtRType UserFEEvalRHSC(UserFECElementStruct *C,
			CagdCrvStruct *Crv1,
			CagdCrvStruct *Crv2);

/* Error handling. */

UserSetErrorFuncType UserSetFatalErrorFunc(UserSetErrorFuncType ErrorFunc);
const char *UserDescribeError(UserFatalErrorType ErrorNum);
void UserFatalError(UserFatalErrorType ErrID);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif /* USER_LIB_H */
