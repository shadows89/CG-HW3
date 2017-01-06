// CGWorkView.h : interface of the CCGWorkView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
#define AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "gl\gl.h"    // Include the standard CGWork  headers
#include "gl\glu.h"   // Add the utility library


#include "Light.h"
#include "mat4.hpp"
#include "Camera.hpp"
#include "Model.h"
#include "SensetivityDialog.h"
class CCGWorkView : public CView
{
protected: // create from serialization only
	CCGWorkView();
	DECLARE_DYNCREATE(CCGWorkView)

	// Attributes
public:
	CCGWorkDoc* GetDocument();

	// Operations
public:

private:
	int m_nAxis;				// Axis of Action, X Y or Z
	int m_nAction;				// Rotate, Translate, Scale
	int m_nView;				// Orthographic, perspective
	bool m_bIsPerspective;			// is the view perspective
	//mat4 m_prespective = mat4::prespective(1);
	mat4 m_scale = mat4::eye();
	mat4 m_translate = mat4::translate(vec4(1, 1, 1));
	mat4 m_rotate = mat4::rotate(0, 0, 0);
	mat4 m_pipeline = m_scale*m_translate*m_rotate;
	mat4 s_scale;
	Camera camera;

	SensetivityDialog settings;

	CString m_strItdFileName;		// file name of IRIT data

	int m_nLightShading;			// shading: Flat, Gouraud.

	double m_lMaterialAmbient;		// The Ambient in the scene
	double m_lMaterialDiffuse;		// The Diffuse in the scene
	double m_lMaterialSpecular;		// The Specular in the scene
	int m_nMaterialCosineFactor;		// The cosine factor for the specular

	void line(int x1, int y1, double z1, int x2, int y2, double z2);
	void line1(int x1, int y1, double z1, int x2, int y2, double z2);
	void line2(int x1, int y1, double z1, int x2, int y2, double z2);
	void line3(int x1, int y1, double z1, int x2, int y2, double z2);
	void line(vec4 p1, vec4 p2);
	void small_slope_negative(int x1, int y1, double z1, int x2, int y2, double z2);
	void small_slope_positive(int x1, int y1, double z1, int x2, int y2, double z2);
	void big_slope_positive(int x1, int y1, double z1, int x2, int y2, double z2);
	void big_slope_negative(int x1, int y1, double z1, int x2, int y2, double z2);
	void plotPixel(int x, int y);
	void updatePipeline();
	void resetTransformations();
	void drawBBox(Model* model);
	void hashPixel(int x, int y, int z);
	void hashPixelPhong(int x, int y, int z);
	bool isSilhouette(CG_Point p1, CG_Point p2);
	COLORREF clacColor(int x, int y);

	LightParams m_lights[MAX_LIGHT];	//configurable lights array
	LightParams m_ambientLight;		//ambient light (only RGB is used)



	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCGWorkView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CCGWorkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL InitializeCGWork();
	BOOL SetupViewingFrustum(void);
	BOOL SetupViewingOrthoConstAspect(void);

	virtual void RenderScene();


	HGLRC    m_hRC;			// holds the Rendering Context
	CDC*     m_pDC;			// holds the Device Context
	int m_WindowWidth;		// hold the windows width
	int m_WindowHeight;		// hold the windows height
	double m_AspectRatio;		// hold the fixed Aspect Ration

	// Generated message map functions
protected:
	//{{AFX_MSG(CCGWorkView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileLoad();
	afx_msg void OnViewOrthographic();
	afx_msg void OnUpdateViewOrthographic(CCmdUI* pCmdUI);
	afx_msg void OnViewPerspective();
	afx_msg void OnUpdateViewPerspective(CCmdUI* pCmdUI);
	afx_msg void OnActionRotate();
	afx_msg void OnUpdateActionRotate(CCmdUI* pCmdUI);
	afx_msg void OnActionScale();
	afx_msg void OnUpdateActionScale(CCmdUI* pCmdUI);
	afx_msg void OnActionTranslate();
	afx_msg void OnUpdateActionTranslate(CCmdUI* pCmdUI);
	afx_msg void OnAxisX();
	afx_msg void OnUpdateAxisX(CCmdUI* pCmdUI);
	afx_msg void OnAxisY();
	afx_msg void OnUpdateAxisY(CCmdUI* pCmdUI);
	afx_msg void OnAxisZ();
	afx_msg void OnUpdateAxisZ(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingFlat();
	afx_msg void OnUpdateLightShadingFlat(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingGouraud();
	afx_msg void OnUpdateLightShadingGouraud(CCmdUI* pCmdUI);
	afx_msg void OnLightConstants();
	
	//added functions
	afx_msg void OnCamButton();
	afx_msg void OnUpdateCamButton(CCmdUI* pCmdUI);
	afx_msg void OnObjectButton();
	afx_msg void OnUpdateObjectButton(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnModelColorUpdate();
	afx_msg void OnBackgroundColorUpdate();
	afx_msg void OnNormalPolygonGiven();
	afx_msg void OnNormalVertexGiven();
	afx_msg void OnNormalVertexCalculated();
	afx_msg void OnNormalPolygonGivenCheck(CCmdUI* pCmdUI);
	afx_msg void OnNormalVertexGivenCheck(CCmdUI* pCmdUI);
	afx_msg void OnNormalVertexCalculatedCheck(CCmdUI* pCmdUI);
	afx_msg void OnNormalPolygonCalculated();
	afx_msg void OnNormalPolygonCalculatedCheck(CCmdUI* pCmdUI);
	afx_msg void OnBoundingBox();
	afx_msg void OnBoundingBoxCheck(CCmdUI* pCmdUI);
	afx_msg void OnNormalColorUpdate();
	afx_msg void OnOpenSettings();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChooseEverything();
	afx_msg void OnChooseEverythingCheck(CCmdUI* pCmdUI);
	afx_msg void OnChooseSingle();
	afx_msg void OnChooseSingleCheck(CCmdUI* pCmdUI);
	afx_msg void OnAxisXY();
	afx_msg void OnUpdateAxisXY(CCmdUI* pCmdUI);
	afx_msg void OnPolyFill();
	afx_msg void OnPolyFillCheck(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingPhong();
	afx_msg void OnUpdateLightShadingPhong(CCmdUI* pCmdUI);
	afx_msg void OnLightNoShading();
	afx_msg void OnUpdateLightNoShading(CCmdUI* pCmdUI);
	afx_msg void OnBackgroundImageLoad();
	afx_msg void OnBackgroundImageReset();
	afx_msg void OnBackgroundImageViewNormal();
	afx_msg void OnUpdateBackgroundImageViewNormal(CCmdUI* pCmdUI);
	afx_msg void OnBackgroundImageViewStrech();
	afx_msg void OnUpdateBackgroundImageViewStrech(CCmdUI* pCmdUI);
	afx_msg void OnBackgroundImageViewRepeat();
	afx_msg void OnUpdateBackgroundImageViewRepeat(CCmdUI* pCmdUI);
	
	afx_msg void OnNormalInverse();
	afx_msg void OnCreatePicture();
	afx_msg void OnBackfaceCulling();
	afx_msg void OnUpdateBackfaceCulling(CCmdUI* pCmdUI);
	afx_msg void OnMaterialConstants();
	afx_msg void OnSilhouette();
	afx_msg void OnUpdateSilhouette(CCmdUI* pCmdUI);
	afx_msg void OnRenderOptions();

	afx_msg void OnChoosePolygonNormal();
	afx_msg void OnUpdateChoosePolygonNormal(CCmdUI* pCmdUI);
	afx_msg void OnChooseGivenPlygonNormal();
	afx_msg void OnCUpdatehooseGivenPolygonNormal(CCmdUI* pCmdUI);
	afx_msg void OnChooseVertexNormal();
	afx_msg void OnUpdateChooseVertexNormal(CCmdUI* pCmdUI);
	afx_msg void OnChooseGivenVertexNormal();
	afx_msg void OnUpdateChooseGivenVertexNormal(CCmdUI* pCmdUI);
	afx_msg void OnSilhoutteColor();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CGWorkView.cpp
inline CCGWorkDoc* CCGWorkView::GetDocument()
{
	return (CCGWorkDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
