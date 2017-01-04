// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
using std::cout;
using std::endl;
#include "MaterialDlg.h"
#include "LightDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PngWrapper.h"
#include "iritSkel.h"



// For Status Bar access
#include "MainFrm.h"

#include "LinkedList.h"
#include <unordered_map>
#include <set>
extern CG_ModelList models;
extern double max;
extern int firstDraw;
extern double maxX, minX, maxY, minY, maxZ, minZ;
extern IPFreeformConvStateStruct CGSkelFFCState;

//Drawing Globals

double sesetivity = 1.0/15.0;

typedef std::tuple<double, COLORREF,Model*> Ztuple;
typedef std::tuple<CG_Point, COLORREF> hashTuple;
typedef std::tuple<CG_Point, vec4> PhongTuple;


std::vector<COLORREF>* vec_bitmap = NULL;
std::vector<Model*>* pixelOwner = NULL;
std::vector<Ztuple>* ZBuffer;
std::unordered_map<int, hashTuple> pixelHashY;
std::unordered_map<int, hashTuple> pixelHashX;

std::unordered_map<int, PhongTuple> pixelHashYPhong;
std::unordered_map<int, PhongTuple> pixelHashXPhong;
Model* currentObejct = NULL;
Model* changingObejct = NULL;

int currentZ;
vec4 currentPolyNormal = vec4(0, 0, 0);
vec4 globalNormal;
vec4 phongNormal;
double polyMaxX = INT64_MIN, polyMinX = INT64_MAX , polyMaxY = INT64_MIN, polyMinY = INT64_MAX;
int global_h;
int global_w;

//Mouse Globals
int mousePressed = 0;
int mouseX;
int mouseY;


int scale;

int cam = 0;
int single = 0;

int colorNotChange = 1;
COLORREF modelColor;
COLORREF backgroundColor = RGB(255, 255, 255);
COLORREF normalColor = RGB(0, 0, 255);
COLORREF globalCalculatedColor;

vec4 p1Normal;
vec4 p2Normal;
COLORREF globalP1Color;
COLORREF globalP2Color;

bool polygonNormals = false;
bool vertexNormals = false;
bool polyGiven = false;
bool polyCalculated = false;
bool vertexGiven = false;
bool vertexCalculated = false;
bool drawNormals = false;
bool BBox = false;
bool drawingBBox = false;
bool polyFill = false;
bool fillingPoly = false;
bool onFrame = true;

double F = - 50;

typedef struct{
	CG_Point point;
	CG_Point normal;
	COLORREF color;
}PointData;



// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))


/////////////////////////////////////////////////////////////////////////////
// CCG--WorkView

IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)
	//Added Functions
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_CAM_BUTTON, OnCamButton)
	ON_UPDATE_COMMAND_UI(ID_CAM_BUTTON, OnUpdateCamButton)
	ON_COMMAND(ID_OBJECT_BUTTON, OnObjectButton)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_BUTTON, OnUpdateObjectButton)
	ON_COMMAND(ID_COLOR_MODEL, OnModelColorUpdate)
	ON_COMMAND(ID_COLOR_BACKGROUND, OnBackgroundColorUpdate)
	ON_COMMAND(ID_COLOR_NORMALS, OnNormalColorUpdate)
	ON_COMMAND(ID_POLYGON_GIVEN, OnNormalPolygonGiven)
	ON_COMMAND(ID_POLYGON_CALCULATED, OnNormalPolygonCalculated)
	ON_UPDATE_COMMAND_UI(ID_POLYGON_GIVEN, OnNormalPolygonGivenCheck)
	ON_UPDATE_COMMAND_UI(ID_POLYGON_CALCULATED, OnNormalPolygonCalculatedCheck)
	ON_COMMAND(ID_VERTEX_GIVEN, OnNormalVertexGiven)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_GIVEN, OnNormalVertexGivenCheck)
	ON_COMMAND(ID_VIEW_BOUNDINGBOX, OnBoundingBox)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BOUNDINGBOX, OnBoundingBoxCheck)
	ON_COMMAND(ID_VERTEX_CALCULATED, OnNormalVertexCalculated)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_CALCULATED, OnNormalVertexCalculatedCheck)
	ON_COMMAND(ID_VIEW_SETTINGS, OnOpenSettings)
	ON_COMMAND(ID_SINGLE, OnChooseSingle)
	ON_UPDATE_COMMAND_UI(ID_SINGLE, OnChooseSingleCheck)
	ON_COMMAND(ID_EVERYTHING, OnChooseEverything)
	ON_UPDATE_COMMAND_UI(ID_SINGLE, OnChooseEverythingCheck)
	ON_COMMAND(ID_AXIS_XY, OnAxisXY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_XY, OnUpdateAxisXY)
	ON_COMMAND(ID_POLYFILL, OnPolyFill)
	ON_UPDATE_COMMAND_UI(ID_POLYFILL, OnPolyFillCheck)
	ON_COMMAND(ID_LIGHT_SHADING_PHONG, OnLightShadingPhong)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_PHONG, OnUpdateLightShadingPhong)
	ON_COMMAND(ID_LIGHT_SHADING_NOSHADING, OnLightNoShading)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_NOSHADING, OnUpdateLightNoShading)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
	GLUquadric *quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluCylinder(quad, radius, 0.0, height, 20, 20);
	gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

CCGWorkView::CCGWorkView()
{
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;

	m_nLightShading = ID_LIGHT_SHADING_FLAT;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;

	//init the first light to be enabled
	m_lights[LIGHT_ID_1].enabled = true;


	firstDraw = 1;

}

CCGWorkView::~CCGWorkView()
{
	if (pixelOwner != NULL)
		delete pixelOwner;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}



int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();

	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);

	if (NULL == m_pDC) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers

void CCGWorkView::resetTransformations(){
	CRect r;
	GetClientRect(&r);
	int w = r.Width();
	int h = r.Height();
	int mid_w = w / 2;
	int mid_h = h / 2;
	if (w > h)
		scale = mid_h;
	else
		scale = mid_w;

	global_h = h;
	global_w = w;

	/*if (firstDraw){
		camera.lookAt(vec4(0,0, 0), vec4(0,0, -1), vec4(0, -1, 0));
		m_scale = mat4::scale(1/max);
		m_translate = mat4::translate(vec4(0, 0, 0));
		//m_rotate = mat4::rotate(0, 0, 0);
	}*/
	double R = maxX, L = minX, T = maxY, B = minY;
	double F = minZ, N = maxZ;

	/*camera.lookAt(vec4((R + L) / 2, (T + B) / 2, N + 1, 1), vec4((R + L) / 2, (T + B) / 2, N,1), vec4(0, -1, 0));*/
	//camera.lookAt(vec4(0.5, 0.5, 50, 1), vec4(0, 0, 0, 1), vec4(0, -1, 0, 1));
	/*camera.lookAt(vec4(minX, maxY, 3, 1), vec4(minX,maxY, 0, 0), vec4(0, -1, 0, 1));
	*/
	/*camera.lookAt(vec4(0.5, 0.5, 0, 1.0), vec4(0.5, 0.5, -0.1, 1.0), vec4(0, -1, 0, 1.0));*/
	camera.lookAt(vec4((minX + maxX) / 2.0, (minY + maxY) / 2.0, maxZ,0), vec4((minX + maxX) / 2.0, (minY + maxY) / 2.0, -0.1, 1.0), vec4(0, -1, 0, 1.0));
	m_scale = mat4::scale(1.0);

	m_translate = mat4::translate(vec4(0, 0, 0));
}

void CCGWorkView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (0 >= cx || 0 >= cy) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth / (GLdouble)m_WindowHeight;

	global_w = cx;
	global_h = cy;
	//ZBuffer = std::vector<mytuple>((global_h + 1)*(global_w + 1));
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
	return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}





BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC)
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.

	//return CView::OnEraseBkgnd(pDC);
	return true;
}

COLORREF CCGWorkView::clacColor(int x, int y){
	
	int z = currentZ;
	int final_R, final_G, final_B;
	int R, G, B;
	
	vec4 unitNormal;
	if (m_nLightShading == ID_LIGHT_SHADING_FLAT){
		unitNormal = vec4::normalize(currentPolyNormal);
		R = GetRValue(modelColor);
		G = GetGValue(modelColor);
		B = GetBValue(modelColor);
	}
	else if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD){
		unitNormal = vec4::normalize(globalNormal);
		if (onFrame){
			R = GetRValue(modelColor);
			G = GetGValue(modelColor);
			B = GetBValue(modelColor);
		}
		else{
			R = GetRValue(globalCalculatedColor);
			G = GetGValue(globalCalculatedColor);
			B = GetBValue(globalCalculatedColor);
		}
	}
	else if (m_nLightShading == ID_LIGHT_SHADING_PHONG){
		unitNormal = vec4::normalize(phongNormal);
		R = GetRValue(modelColor);
		G = GetGValue(modelColor);
		B = GetBValue(modelColor);
	}

	final_R = m_ambientLight.colorR * m_lMaterialAmbient * R / 256;
	final_G = m_ambientLight.colorG * m_lMaterialAmbient * G / 256;
	final_B = m_ambientLight.colorB * m_lMaterialAmbient * B / 256;


	for (int i = 0; i < MAX_LIGHT; i++){
		if (m_lights[i].enabled){

			double IpR = m_lights[i].colorR;
			double IpG = m_lights[i].colorG;
			double IpB = m_lights[i].colorB;
			vec4 L;
			vec4 V = vec4::normalize(vec4(global_w-x,global_h-y,-z));
			double N_L;
			double R_V_n;
				
			switch (m_lights[i].type)
			{		
			case  LIGHT_TYPE_DIRECTIONAL:
				L = vec4::normalize(vec4(m_lights[i].dirX, m_lights[i].dirY, m_lights[i].dirZ));
				if (L.isZero())
					continue;
				N_L = unitNormal.dot(L);
				if (N_L < 0)
					continue;
				break;
			case LIGHT_TYPE_POINT:
				L = vec4::normalize(vec4(x - m_lights[i].posX, y - m_lights[i].posY, z - m_lights[i].posZ));
				if (L.isZero())
					continue;
				N_L = unitNormal.dot(L);
				if (N_L < 0)
					continue;
				break;
			case LIGHT_TYPE_SPOT:
				break;
			default:
				break;
			}

			vec4 vec_R = vec4::normalize((unitNormal* N_L - L) * 2);
			R_V_n = pow(vec_R.dot(V), m_nMaterialCosineFactor);

			final_R = (final_R + IpR * (R * m_lMaterialDiffuse * N_L + 256 * m_lMaterialSpecular * R_V_n)) / 256;
			final_B = (final_B + IpB * (B * m_lMaterialDiffuse * N_L + 256 * m_lMaterialSpecular * R_V_n)) / 256;
			final_G = (final_G + IpG * (G * m_lMaterialDiffuse * N_L + 256 * m_lMaterialSpecular * R_V_n)) / 256;
		}
	}

	if (final_R > 255)
		final_R = 255;
	if (final_R < 0)
		final_R = 0;
	
	if (final_G > 255)
		final_G = 255;
	if (final_G < 0)
		final_G = 0;

	if (final_B > 255)
		final_B = 255;
	if (final_B < 0)
		final_B = 0;
	
	
	return RGB(final_R, final_G, final_B);
}


void  CCGWorkView::plotPixel(int x, int y){
	if (x >= 0 && x <= global_w && y >= 0 && y <= global_h){
		if (std::get<0>((*ZBuffer)[x + y*global_w]) < currentZ){
			if (drawNormals)
				(*ZBuffer)[x + y*global_w] = Ztuple(currentZ, normalColor, currentObejct);
			else{
				if (polyFill){
					COLORREF calculatedColor;
					if (m_nLightShading == ID_LIGHT_SHADING_FLAT){
						calculatedColor = clacColor(x, y);
					}
					else if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD){
						calculatedColor = globalCalculatedColor;
					}
					else if (m_nLightShading == ID_LIGHT_SHADING_PHONG){
						calculatedColor = clacColor(x, y);
					}
					else {
						calculatedColor = modelColor;
					}
					(*ZBuffer)[x + y*global_w] = Ztuple(currentZ, calculatedColor, currentObejct);
				}
				else
					(*ZBuffer)[x + y*global_w] = Ztuple(currentZ, modelColor, currentObejct);
			}
			(*pixelOwner)[x + y*global_w] = currentObejct;
		}
	}
}

void CCGWorkView::line(CG_Point p1, CG_Point p2){
	if (polyFill){
		if (m_bIsPerspective){
			if (m_nLightShading == ID_LIGHT_SHADING_FLAT)
				line1(p1[0] / p1[3], p1[1] / p1[3], p1[2] / p1[3], p2[0] / p2[3], p2[1] / p2[3], p2[2] / p2[3]);
			if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD)
				line2(p1[0] / p1[3], p1[1] / p1[3], p1[2] / p1[3], p2[0] / p2[3], p2[1] / p2[3], p2[2] / p2[3]);
			if (m_nLightShading == ID_LIGHT_SHADING_PHONG)
				line1(p1[0] / p1[3], p1[1] / p1[3], p1[2] / p1[3], p2[0] / p2[3], p2[1] / p2[3], p2[2] / p2[3]);
			if (m_nLightShading == ID_LIGHT_SHADING_NOSHADING)
				line2(p1[0] / p1[3], p1[1] / p1[3], p1[2] / p1[3], p2[0] / p2[3], p2[1] / p2[3], p2[2] / p2[3]);
		}
		//line(p1[0] / p1[3], p1[1] / p1[3], p1[2] / p1[3], p2[0] / p2[3], p2[1] / p2[3], p2[2] / p2[3] );
		else
		{
			if (m_nLightShading == ID_LIGHT_SHADING_FLAT)
				line1(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
			if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD)
				line2(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
			if (m_nLightShading == ID_LIGHT_SHADING_PHONG)
				line3(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
			if (m_nLightShading == ID_LIGHT_SHADING_NOSHADING)
				line1(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
		}
	}
	else{
		if (m_bIsPerspective)
			line(p1[0] / p1[3], p1[1] / p1[3], p1[2] / p1[3], p2[0] / p2[3], p2[1] / p2[3], p2[2] / p2[3]);
		else
			line(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
	}
		//line(p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
}

//TODO
void CCGWorkView::hashPixelPhong(int x, int y, int z){
	if (pixelHashXPhong.count(x) == 0){
		PhongTuple newPhongTuple(CG_Point(x, y, z, 1), phongNormal);
		pixelHashXPhong.insert({ x, newPhongTuple });
	}
	else{
		fillingPoly = true;
		
		PhongTuple tmp = pixelHashXPhong[x];
		p1Normal = std::get<1>(tmp);
		p2Normal = phongNormal;
		line(std::get<0>(tmp), CG_Point(x, y, z, 1));
		fillingPoly = false;

	}

	if (pixelHashYPhong.count(y) == 0){
		PhongTuple newPhongTuple(CG_Point(x, y, z, 1), phongNormal);
		pixelHashYPhong.insert({ y, newPhongTuple });
	}
	else{
		fillingPoly = true;

		PhongTuple tmp = pixelHashYPhong[y];
		p1Normal = std::get<1>(tmp);
		p2Normal = phongNormal;
		line(std::get<0>(tmp), CG_Point(x, y, z, 1));
		fillingPoly = false;

	}
}

void CCGWorkView::hashPixel(int x, int y, int z){
	if (pixelHashX.count(x) == 0){
		hashTuple newHashTuple(CG_Point(x, y, z, 1), globalCalculatedColor);
			pixelHashX.insert({ x, newHashTuple });
	}
	else{
		fillingPoly = true;
		hashTuple tmp = pixelHashX[x];
		globalP2Color = std::get<1>(tmp);
		globalP1Color = globalCalculatedColor;
		line(std::get<0>(tmp), CG_Point(x, y, z, 1));
		fillingPoly = false;
	}

	if (pixelHashY.count(y) == 0){
		hashTuple newHashTuple(CG_Point(x, y, z, 1), globalCalculatedColor);
		pixelHashY.insert({ y, newHashTuple });
	}
	else{
		fillingPoly = true;
		hashTuple tmp = pixelHashY[y];
		globalP2Color = std::get<1>(tmp);
		globalP1Color = globalCalculatedColor;
		line(std::get<0>(tmp), CG_Point(x, y, z, 1));
		fillingPoly = false;

	}
}

void updateGlobalColor(COLORREF p1Color, COLORREF p2Color,int x,int y,int x1,int y1,int z1,double dist){
	
	if (dist == 0)
		globalCalculatedColor = p1Color;
	else{
		double a = sqrt(pow(x - x1, 2) + pow(y - y1, 2) + pow(currentZ - z1 ,2)) / dist;
		int R = GetRValue(p2Color)*a + GetRValue(p1Color)*(1 - a);
		int G = GetGValue(p2Color)*a + GetGValue(p1Color)*(1 - a);
		int B = GetBValue(p2Color)*a + GetBValue(p1Color)*(1 - a);
		globalCalculatedColor = RGB(R, G, B);
	}
}

void updateGlobaPhongNormal(vec4 normal_1, vec4 normal_2, int x, int y, int x1, int y1,int z1, double dist){

	if (dist == 0)
		phongNormal = normal_1;
	else{
		double a = sqrt(pow(x - x1, 2) + pow(y - y1, 2) + pow(currentZ - z1, 2)) / dist;
		phongNormal = normal_2 * a + normal_1 *(1 - a);
	}
}


//Phong shading
void CCGWorkView::line3(int x1, int y1, double z1, int x2, int y2, double z2){
	int i, x, y, z, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
	vec4 normal_1;
	vec4 normal_2;

	double dist = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));

	normal_1 = p1Normal;
	normal_2 = p2Normal;

	x = x1;
	y = y1;
	z = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			currentZ = z;
			updateGlobaPhongNormal(normal_1, normal_2, x, y, x1, y1, z1, dist);
			plotPixel(x, y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixelPhong(x, y, z);
			}
			if (err_1 > 0) {
				y += y_inc;
				err_1 -= dx2;
			}
			if (err_2 > 0) {
				z += z_inc;
				err_2 -= dx2;
			}
			err_1 += dy2;
			err_2 += dz2;
			x += x_inc;
		}
	}
	else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			currentZ = z;
			updateGlobaPhongNormal(normal_1, normal_2, x, y, x1, y1, z1, dist);
			plotPixel(x, y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixelPhong(x, y, z);
			}
			if (err_1 > 0) {
				x += x_inc;
				err_1 -= dy2;
			}
			if (err_2 > 0) {
				z += z_inc;
				err_2 -= dy2;
			}
			err_1 += dx2;
			err_2 += dz2;
			y += y_inc;
		}
	}
	else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			currentZ = z;
			updateGlobaPhongNormal(normal_1, normal_2, x, y, x1, y1, z1, dist);
			plotPixel(x, y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixelPhong(x, y, z);
			}
			if (err_1 > 0) {
				y += y_inc;
				err_1 -= dz2;
			}
			if (err_2 > 0) {
				x += x_inc;
				err_2 -= dz2;
			}
			err_1 += dy2;
			err_2 += dx2;
			z += z_inc;
		}
	}
	currentZ = z;
	updateGlobaPhongNormal(normal_1, normal_2, x, y, x1, y1, z1, dist);
	plotPixel(x, y);
	if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
		hashPixel(x, y, z);
	}
}

//Gauroud shading
void CCGWorkView::line2(int x1, int y1, double z1, int x2, int y2, double z2){
	int i, x, y, z, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;

	COLORREF p1Color;
	COLORREF p2Color;

	double dist = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));

	if (onFrame){
		globalNormal = p1Normal;
		p1Color = clacColor(x1, y1);
		globalNormal = p2Normal;
		p2Color = clacColor(x2, y2);
	}
	else{
		p1Color = globalP1Color;
		p2Color = globalP2Color;
	}

	x = x1;
	y = y1;
	z = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			currentZ = z;
			updateGlobalColor(p1Color, p2Color, x, y, x1, y1, z1, dist);
			plotPixel(x, y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixel(x, y, z);
			}
			if (err_1 > 0) {
				y += y_inc;
				err_1 -= dx2;
			}
			if (err_2 > 0) {
				z += z_inc;
				err_2 -= dx2;
			}
			err_1 += dy2;
			err_2 += dz2;
			x += x_inc;
		}
	}
	else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			currentZ = z;
			updateGlobalColor(p1Color, p2Color, x, y, x1, y1, z1, dist);
			plotPixel(x, y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixel(x, y, z);
			}
			if (err_1 > 0) {
				x += x_inc;
				err_1 -= dy2;
			}
			if (err_2 > 0) {
				z += z_inc;
				err_2 -= dy2;
			}
			err_1 += dx2;
			err_2 += dz2;
			y += y_inc;
		}
	}
	else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			currentZ = z;
			updateGlobalColor(p1Color, p2Color, x, y, x1, y1, z1, dist);
			plotPixel(x, y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixel(x, y, z);
			}
			if (err_1 > 0) {
				y += y_inc;
				err_1 -= dz2;
			}
			if (err_2 > 0) {
				x += x_inc;
				err_2 -= dz2;
			}
			err_1 += dy2;
			err_2 += dx2;
			z += z_inc;
		}
	}
	currentZ = z;
	updateGlobalColor(p1Color, p2Color, x, y, x1, y1, z1, dist);
	plotPixel(x, y);
	if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
		hashPixel(x, y, z);
	}
}

//Flat shading or NO SHADING
void CCGWorkView::line1(int x1, int y1, double z1, int x2, int y2, double z2){
	int i, x,y,z,dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;

	x = x1;
	y = y1;
	z = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			currentZ = z;
			plotPixel(x, y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixel(x, y, z);
			}
			if (err_1 > 0) {
				y += y_inc;
				err_1 -= dx2;
			}
			if (err_2 > 0) {
				z += z_inc;
				err_2 -= dx2;
			}
			err_1 += dy2;
			err_2 += dz2;
			x += x_inc;
		}
	}
	else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			currentZ = z;
			plotPixel(x,y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixel(x, y, z);
			}
			if (err_1 > 0) {
				x += x_inc;
				err_1 -= dy2;
			}
			if (err_2 > 0) {
				z += z_inc;
				err_2 -= dy2;
			}
			err_1 += dx2;
			err_2 += dz2;
			y += y_inc;
		}
	}
	else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			currentZ = z;
			plotPixel(x,y);
			if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
				hashPixel(x, y, z);
			}
			if (err_1 > 0) {
				y += y_inc;
				err_1 -= dz2;
			}
			if (err_2 > 0) {
				x += x_inc;
				err_2 -= dz2;
			}
			err_1 += dy2;
			err_2 += dx2;
			z += z_inc;
		}
	}
	currentZ = z;
	plotPixel(x, y);
	if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
		hashPixel(x, y, z);
	}
	
	/*int dx, dy;
	dx = x2 - x1;
	dy = y2 - y1;

	//big or small slope
	if (abs(dy) > abs(dx)){
		if (x1 < x2){
			if (y1 < y2){
				big_slope_negative(x1, y1,z1, x2, y2,z2);
			}
			else{
				big_slope_positive(x1, y1,z1, x2, y2,z2);
			}
		}
		else{
			if (y1 < y2){
				big_slope_positive(x2, y2,z2, x1, y1,z1);
			}
			else{
				big_slope_negative(x2, y2,z2, x1, y1,z1);
			}
		}
	}
	else{
		if (x1 < x2){
			if (y1 < y2){
				small_slope_negative(x1, y1,z1 ,x2, y2,z2);
			}
			else{
				small_slope_positive(x1, y1,z1, x2, y2,z2);
			}
		}
		else{
			if (y1 < y2){
				small_slope_positive(x2, y2,z1, x1, y1,z1);
			}
			else{
				small_slope_negative(x2, y2,z2, x1, y1,z1);
			}
		}
	}*/
}

//No fill
void CCGWorkView::line(int x1, int y1, double z1, int x2, int y2, double z2){
	int i, x, y, z, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;

	x = x1;
	y = y1;
	z = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;
	x_inc = (dx < 0) ? -1 : 1;
	l = abs(dx);
	y_inc = (dy < 0) ? -1 : 1;
	m = abs(dy);
	z_inc = (dz < 0) ? -1 : 1;
	n = abs(dz);
	dx2 = l << 1;
	dy2 = m << 1;
	dz2 = n << 1;

	if ((l >= m) && (l >= n)) {
		err_1 = dy2 - l;
		err_2 = dz2 - l;
		for (i = 0; i < l; i++) {
			currentZ = z;
			plotPixel(x, y);
			if (err_1 > 0) {
				y += y_inc;
				err_1 -= dx2;
			}
			if (err_2 > 0) {
				z += z_inc;
				err_2 -= dx2;
			}
			err_1 += dy2;
			err_2 += dz2;
			x += x_inc;
		}
	}
	else if ((m >= l) && (m >= n)) {
		err_1 = dx2 - m;
		err_2 = dz2 - m;
		for (i = 0; i < m; i++) {
			currentZ = z;
			plotPixel(x, y);
			if (err_1 > 0) {
				x += x_inc;
				err_1 -= dy2;
			}
			if (err_2 > 0) {
				z += z_inc;
				err_2 -= dy2;
			}
			err_1 += dx2;
			err_2 += dz2;
			y += y_inc;
		}
	}
	else {
		err_1 = dy2 - n;
		err_2 = dx2 - n;
		for (i = 0; i < n; i++) {
			currentZ = z;
			plotPixel(x, y);
			if (err_1 > 0) {
				y += y_inc;
				err_1 -= dz2;
			}
			if (err_2 > 0) {
				x += x_inc;
				err_2 -= dz2;
			}
			err_1 += dy2;
			err_2 += dx2;
			z += z_inc;
		}
	}
	currentZ = z;
	plotPixel(x, y);
}

/*
void CCGWorkView::small_slope_negative(int x1, int y1, double z1, int x2, int y2, double z2){
	int x, y, dx, dy , d, delta_e, delta_ne;
	double z,dz ,delta_z;
	x = x1;
	y = y1;
	z = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;
	d = 2 * dy - dx;
	delta_e = 2 * dy;
	delta_ne = 2 * (dy - dx);
	delta_z = dz / abs(dx);
	currentZ = z;
	plotPixel(x, y);
	while (x < x2){
		if (d < 0){
			d = d + delta_e;
			x++;
		}
		else {
			d = d + delta_ne;
			x++;
			y++;
		}
		z = z + delta_z;
		currentZ = z;
		plotPixel(x, y);
		if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
			if (pixelHashX.count(x) == 0){
				pixelHashX.insert({ x, CG_Point(x, y, z, 1) });
			}
			else{
				fillingPoly = true;
				CG_Point tmp = pixelHashX[x];
				line(tmp, CG_Point(x, y, z, 1));
				fillingPoly = false;
			}
			if (pixelHashY.count(y) == 0){
				pixelHashY.insert({ y, CG_Point(x, y, z, 1) });
			}
			else{
				fillingPoly = true;
				CG_Point tmp = pixelHashY[y];
				line(tmp, CG_Point(x, y, z, 1));
				fillingPoly = false;
			}
		}
	}
}

void CCGWorkView::small_slope_positive(int x1, int y1, double z1, int x2, int y2, double z2){
	int x, y, dx, dy, d, delta_e, delta_ne;
	double z, dz, delta_z;
	x = x1;
	y = y1;
	z = z1;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);
	dz = z1 - z2;
	d = 2 * dy - dx;
	delta_e = 2 * dy;
	delta_ne = 2 * (dy - dx);
	delta_z = dz / abs(dx);
	currentZ = z;
	plotPixel(x, y);
	while (x < x2){
		if (d < 0){
			d = d + delta_e;
			x++;
		}
		else {
			d = d + delta_ne;
			x++;
			y--;
		}
		z += delta_z;
		currentZ = z;
		plotPixel(x, y);
		if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
			if (pixelHashX.count(x) == 0){
				pixelHashX.insert({ x, CG_Point(x, y, z, 1) });
			}
			else{
				fillingPoly = true;
				CG_Point tmp = pixelHashX[x];
				line(tmp, CG_Point(x, y, z, 1));
				fillingPoly = false;
			}
			if (pixelHashY.count(y) == 0){
				pixelHashY.insert({ y, CG_Point(x, y, z, 1) });
			}
			else{
				fillingPoly = true;
				CG_Point tmp = pixelHashY[y];
				line(tmp, CG_Point(x, y, z, 1));
				fillingPoly = false;
			}
		}
	}
}


void CCGWorkView::big_slope_negative(int x1, int y1, double z1, int x2, int y2, double z2){
	int x, y, dx, dy, d, delta_e, delta_ne;
	double z, dz, delta_z;
	x = x1;
	y = y1;
	z = z1;
	dx = x2 - x1;
	dy = y2 - y1;
	dz = z1 - z2;
	d = 2 * dx - dy;
	delta_e = 2 * dx;
	delta_ne = 2 * (dx - dy);
	delta_z = dz / abs(dy);
	currentZ = z;
	plotPixel(x, y);
	while (y < y2){
		if (d < 0){
			d = d + delta_e;
			y++;
		}
		else {
			d = d + delta_ne;
			x++;
			y++;
		}
		z += delta_z;
		currentZ = z;
		plotPixel(x, y);
		if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
			if (pixelHashY.count(y) == 0){
				pixelHashY.insert({ y, CG_Point(x, y, z, 1) });
			}
			else{
				fillingPoly = true;
				CG_Point tmp = pixelHashY[y];
				line(tmp, CG_Point(x, y, z, 1));
				fillingPoly = false;
			}
			if (pixelHashX.count(x) == 0){
				pixelHashX.insert({ x, CG_Point(x, y, z, 1) });
			}
			else{
				fillingPoly = true;
				CG_Point tmp = pixelHashX[x];
				line(tmp, CG_Point(x, y, z, 1));
				fillingPoly = false;
			}
		}
	}
}

void CCGWorkView::big_slope_positive(int x1, int y1, double z1, int x2, int y2, double z2){
	int x, y, dx, dy, d, delta_e, delta_ne;
	double z, dz, delta_z;
	x = x1;
	y = y1;
	z = z1;
	dx = abs(x2 - x1);
	dy = abs(y2 - y1);
	dz = z1 - z2;
	d = 2 * dx - dy;
	delta_e = 2 * dx;
	delta_ne = 2 * (dx - dy);
	delta_z = dz / abs(dy);
	currentZ = z;
	plotPixel(x, y);
	while (y > y2){
		if (d < 0){
			d = d + delta_e;
			y--;
		}
		else {
			d = d + delta_ne;
			x++;
			y--;
		}
		z += delta_z;
		currentZ = z;
		plotPixel(x, y);
		if (!fillingPoly && polyFill && !drawNormals && !drawingBBox){
			if (pixelHashY.count(y) == 0){
				pixelHashY.insert({ y, CG_Point(x, y, z, 1) });
			}
			else{
				fillingPoly = true;
				CG_Point tmp = pixelHashY[y];
				line(tmp, CG_Point(x, y, z, 1));
				fillingPoly = false;
			}
			if (pixelHashX.count(x) == 0){
				pixelHashX.insert({ x, CG_Point(x, y, z, 1) });
			}
			else{
				fillingPoly = true;
				CG_Point tmp = pixelHashX[x];
				line(tmp, CG_Point(x, y, z, 1));
				fillingPoly = false;
			}
		}
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////

void CCGWorkView::updatePipeline(){
	//mat4 I = mat4::eye();
	//mat4 scaleMat = mat4::scale(scale);
	//mat4 rotate = mat4::rotate(xTheta,yTheta, zTheta);
	//mat4 translation = mat4::translate(vec4(mid_w, mid_h, 1));

	//m_scale = m_scale * (1/max);
	//double R = maxs[0] + 2 * (abs(maxs[0])), L = mins[0] - 2 * (abs(mins[0])), T = maxs[1] + 2 * (abs(maxs[1])), B = mins[1] - 2 * (abs(mins[1]));
	//double F = mins[2] - 2 * (abs(mins[2])), N = maxs[2];
	//camera.lookAt(vec4(global_w / 2, global_h / 2, 50, 1), vec4(global_w / 2, global_h / 2, 0, 1), vec4(0, -1, 0, 1));


	mat4 invCamera = camera.transformation().inverse();
	//mat4 invCamera = mat4::eye();
	/*double R = mm[0], L =mi[0], T = mm[1], B = mi[1];*/
	double R = max(max*2,10.0), L = -R, T = 1.0 / m_AspectRatio*R, B = -T;
	//double R = global_w, L = 0, T = global_h, B = 0;
	double N = -0.01;
	//F = minZ-50;
	double normalizedCubeFactor = 2.0;
	mat4 s1 = mat4::scale(vec4(normalizedCubeFactor / (R - L), normalizedCubeFactor / (T - B), normalizedCubeFactor / (N - F), 1));
	mat4 t1 = mat4::translate(vec4(-(L + R) / normalizedCubeFactor, -(T + B) / normalizedCubeFactor, -(F + N) / normalizedCubeFactor, 1));

	mat4 s2 = mat4::scale(vec4(global_w / normalizedCubeFactor, global_h / normalizedCubeFactor, 1000, 1));
	mat4 t2 = mat4::translate(vec4((global_w - 1) / normalizedCubeFactor, (global_h - 1) / normalizedCubeFactor, 0, 1));
	mat4 l = t2*s2;
	mat4 r = s1*t1;
	mat4 projection = l*r;


	camera.setProjection(projection);



	if (m_bIsPerspective){

		//mat4 p = mat4::eye();

		//mat4 p = mat4::eye();
		//p[0][0] = N;
		//p[1][1] = (N);
		//p[2][2] = (N) + (F);
		//p[2][3] = -(N*F);
		//p[3][2] = 1;
		//p[3][3] = 0;
		mat4 p = mat4::eye();
		//p[3][3] = 0;
		//p[2][2] = (abs(F))/(abs(F)-abs(N));
		//p[2][3] = - (abs(N*F)) / (abs(F) - abs(N));
		//p[3][2] = 1/(abs(F));

		p[3][2] = 1 / (abs(F));
		//p[3][3]=0;
		m_pipeline = invCamera *m_translate*m_scale*m_rotate;
		m_pipeline = p*m_pipeline;
		m_pipeline = projection*m_pipeline;
		//m_pipeline =projection*p *invCamera *m_translate*m_scale*m_rotate;//*camera.transformation().inverse()*m_translate*m_scale*m_rotate;
	}
	else{

		m_pipeline = projection *invCamera*m_translate*m_scale*m_rotate;
	}
}

void CCGWorkView::drawBBox(Model* model){
	CG_Point p1(model->minX, model->minY, model->minZ, 1.0);
	CG_Point p2(model->minX, model->minY, model->maxZ, 1.0);
	CG_Point p3(model->minX, model->maxY, model->minZ, 1.0);
	CG_Point p4(model->minX, model->maxY, model->maxZ, 1.0);
	CG_Point p5(model->maxX, model->minY, model->minZ, 1.0);
	CG_Point p6(model->maxX, model->minY, model->maxZ, 1.0);
	CG_Point p7(model->maxX, model->maxY, model->minZ, 1.0);
	CG_Point p8(model->maxX, model->maxY, model->maxZ, 1.0);

	p1 = m_pipeline*model->position*(p1);
	p2 = m_pipeline*model->position*(p2);
	p3 = m_pipeline*model->position*(p3);
	p4 = m_pipeline*model->position*(p4);
	p5 = m_pipeline*model->position*(p5);
	p6 = m_pipeline*model->position*(p6);
	p7 = m_pipeline*model->position*(p7);
	p8 = m_pipeline*model->position*(p8);

	line(p1, p5);
	line(p1, p3);
	line(p1, p2);
	line(p3, p7);
	line(p3, p4);
	line(p4, p2);
	line(p4, p8);
	line(p2, p6);
	line(p6, p8);
	line(p6, p5);
	line(p8, p7);
	line(p5, p7);

}


void CCGWorkView::OnDraw(CDC* pDC)
{
	CCGWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	//CG_PolygonList& polygons = MyPoly::GetInstance();
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);

	CRect r;
	GetClientRect(&r);
	int w = r.Width();
	int h = r.Height();

	if (firstDraw){
		resetTransformations();
		m_rotate = mat4::rotate(0, 0, 0);
		firstDraw = 0;
	}

	CCGWorkView::updatePipeline();

	if (pixelOwner != NULL)
		delete pixelOwner;

	

	vec_bitmap = new std::vector<COLORREF>((w + 1)*(h + 1), backgroundColor);
	pixelOwner = new std::vector<Model*>((w + 1)*(h + 1), NULL);
	ZBuffer = new std::vector<Ztuple>((w + 1)*(h + 1), Ztuple(INT64_MIN, backgroundColor, NULL));
	if (models.getSize() != 0){
		CG_Point* p1;
		CG_Point* p2;
		CG_Point* tmp;
		vec4 vp = vec4::normalize(camera.transformation()[2]);
		for (Model* model = models.first(); model != NULL; model = models.next()){
			currentObejct = model;
			if(colorNotChange)
				modelColor = model->color;
			CG_NormalList* polynormal = model->calculatedPolygonNormals;
			
			/*if (model->polygons->size != model->calculatedPolygonNormals->size){
				polynormal = model->polygonNormals;
			}*/
			CG_Point* dir = polynormal->first();
			CG_Point* mid = model->polygonMids->first();
			
			/*p1Normal = m_translate*m_rotate*model->position*(*model->calculatedVertexNormals->first());
			p2Normal = m_translate*m_rotate*model->position*(*model->calculatedVertexNormals->next());

			/*if (model->polygons->size != model->calculatedPolygonNormals->size){
				polynormal = model->polygonNormals;
			}*/


			
			for (CG_Polygon* polygon = model->polygons->first(); polygon != NULL; polygon = model->polygons->next()){
				
				pixelHashX.clear();
				pixelHashY.clear();

				pixelHashXPhong.clear();
				pixelHashYPhong.clear();
			
				vec4 currentMid = m_translate*m_rotate*model->position*(*mid);
				currentPolyNormal = m_translate*m_rotate*model->position*(*dir);
				
				
				if (m_bIsPerspective){
					vp = camera.eye() - currentMid;
				}
				
				if (vp.dot(currentMid - currentPolyNormal) > 0){
					dir = polynormal->next();
					mid = model->polygonMids->next();
					continue;
				}

				p1 = polygon->first();
				p2 = polygon->next();
				
				p1Normal =m_translate*m_rotate*model->position*model->calculatedVertexNormalHash[p1->toString()];
				p2Normal = m_translate*m_rotate*model->position*model->calculatedVertexNormalHash[p2->toString()];

				while (true){
					vec4 p1Offset = m_pipeline*model->position*(*p1);
					vec4 p2Offset = m_pipeline*model->position*(*p2);
					
					line(p1Offset, p2Offset);
					tmp = p2;
					p2 = polygon->next();
					p1 = tmp;
									
					if (p2 == NULL){
						p2 = polygon->first();
						break;
					}

					p1Normal = p2Normal;
					p2Normal = m_translate*m_rotate*model->position*model->calculatedVertexNormalHash[p2->toString()];
				}

				vec4 p1Offset = m_pipeline*model->position*(*p1);
				vec4 p2Offset = m_pipeline*model->position*(*p2);

				//p1Normal = p2Normal;
				p2Normal = m_translate*m_rotate*model->position*model->calculatedVertexNormalHash[p2->toString()];
				
				line(p1Offset, p2Offset);

				
				if (BBox){
					drawingBBox = true;
					drawBBox(model);
					drawingBBox = false;
				}
				dir = polynormal->next();
				mid = model->polygonMids->next();

			}
			if (polygonNormals){
				drawNormals = TRUE;
				CG_NormalList* tmp = NULL;
				if (polyGiven)
					tmp = model->polygonNormals;
				else
					tmp = model->calculatedPolygonNormals;
				for (CG_Point* dir = tmp->first(),*mid = model->polygonMids->first(); dir != NULL && mid != NULL;
					dir = tmp->next(), mid = model->polygonMids->next()){
					vec4 normal = (*mid + *dir);
					normal[3] = 1;
					normal = m_pipeline*model->position*normal;
					vec4 screenPoint = m_pipeline*model->position*(*mid);
					line(screenPoint, normal);
				}
				drawNormals = FALSE;
			}
			if (vertexNormals){
				drawNormals = TRUE;
				CG_NormalList* tmp;
				if (vertexGiven)
					tmp = model->vertexNormals;
				else
					tmp = model->calculatedVertexNormals;
				for (CG_Point* point = model->vertices->first(),*direction = tmp->first();
							point != NULL && direction != NULL;
							point = model->vertices->next(), direction =tmp->next()){
					vec4 normal = (*point + *direction);
					normal[3] = 1;
					normal = m_pipeline*model->position*normal;
					vec4 screenPoint = m_pipeline*model->position*(*point);
					vec4 sp2 = screenPoint + vec4::normalize(normal) * 10;
					line(screenPoint, normal);
				}
				drawNormals = FALSE;
			}
		}
	}
	for (int x = 0; x <= w; x++){
		for (int y = 0; y <= h; y++){
			(*vec_bitmap)[x + y*w] = std::get<1>((*ZBuffer)[x + y*w]);
		}
	}
	CBitmap bitmap;
	std::vector<COLORREF> tmp(*vec_bitmap);
	bitmap.CreateBitmap(w, h, 1, 32, &tmp[0]);
	dcMem.SelectObject(&bitmap);
	pDC->TransparentBlt(0, 0, w, h, &dcMem, 0, 0, w, h, RGB(100, 100, 100));
	bitmap.DeleteObject();
	dcMem.DeleteDC();
	delete vec_bitmap;
	delete ZBuffer;

}


/*void CCGWorkView::scanPolyFill(){
	fillingPoly = true;
	for (int j = polyMinY; j <= polyMaxY; j++){
		int countOnPixels = 0;
		for (int i = polyMinX; i <= polyMaxX; i++){
			/*if ((*currentPolyMap)[i + j*global_w])
				countOnPixels++;
		}
		if (countOnPixels > 1){
			bool inPoly = false;
			for (int i = polyMinX; i <= polyMaxX; i++){
				/*if ((*currentPolyMap)[i + j*global_w])
					inPoly = !inPoly;
				if (inPoly){
					currentZ = (currentPolyNormal[3] - currentPolyNormal[0] * i - currentPolyNormal[1] * j) / currentPolyNormal[2];
					plotPixel(i, j);
				}
			}
		}
	}
	fillingPoly = false;
}*/

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy()
{
	CView::OnDestroy();

	// delete the DC
	if (m_pDC) {
		delete m_pDC;
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

void CCGWorkView::RenderScene() {
	// do nothing. This is supposed to be overriden...

	return;
}


void CCGWorkView::OnFileLoad()
{
	TCHAR szFilters[] = _T("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK) {
		m_strItdFileName = dlg.GetPathName();		// Full path and filename
		PngWrapper p;
		CGSkelProcessIritDataFiles(m_strItdFileName, 1);
		// Open the file and read it.
		// Your code here...

		Invalidate();	// force a WM_PAINT for drawing.
	}

}





// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic()
{
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	Invalidate();		// redraw using the new view.
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective()
{
	m_nView = ID_VIEW_PERSPECTIVE;
	m_bIsPerspective = true;
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}




// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate()
{
	m_nAction = ID_ACTION_ROTATE;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate()
{
	m_nAction = ID_ACTION_TRANSLATE;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale()
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}




// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX()
{
	m_nAxis = ID_AXIS_X;
	//m_rotate = mat4::rotateX(45);

}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_X);
}


void CCGWorkView::OnAxisY()
{
	m_nAxis = ID_AXIS_Y;
	//m_rotate = mat4::rotateY(45);

}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Y);
}


void CCGWorkView::OnAxisZ()
{
	m_nAxis = ID_AXIS_Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Z);
	//m_rotate = mat4::rotateZ( 45);

}




// OPTIONS HANDLERS ///////////////////////////////////////////




// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat()
{
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}


void CCGWorkView::OnLightShadingGouraud()
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

void CCGWorkView::OnLightShadingPhong()
{
	m_nLightShading = ID_LIGHT_SHADING_PHONG;
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingPhong(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_PHONG);
}

void CCGWorkView::OnLightNoShading()
{
	m_nLightShading = ID_LIGHT_SHADING_NOSHADING;
	Invalidate();
}

void CCGWorkView::OnUpdateLightNoShading(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_NOSHADING);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants()
{
	CLightDialog dlg;

	for (int id = LIGHT_ID_1; id<MAX_LIGHT; id++)
	{
		dlg.SetDialogData((LightID)id, m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT, m_ambientLight);

	if (dlg.DoModal() == IDOK)
	{
		for (int id = LIGHT_ID_1; id<MAX_LIGHT; id++)
		{
			m_lights[id] = dlg.GetDialogData((LightID)id);
		}
		m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);
	}
	Invalidate();
}

void CCGWorkView::OnLButtonDown(UINT nFlags, CPoint point){
	mousePressed = 1;
}

void CCGWorkView::OnChooseEverything(){
	single = 0;
}

void CCGWorkView::OnChooseSingle(){
	single = 1;
}

void CCGWorkView::OnChooseSingleCheck(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(single);
}

void CCGWorkView::OnChooseEverythingCheck(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(single == 0);
}

void CCGWorkView::OnRButtonDown(UINT nFlags, CPoint point){
	changingObejct = (*pixelOwner)[point.x + point.y * global_w];
}

void CCGWorkView::OnLButtonUp(UINT nFlags, CPoint point){
	mousePressed = 0;
}

void CCGWorkView::OnMouseMove(UINT nFlags, CPoint point){
	if (mousePressed){
		double deltaX = (mouseX - point.x)*sesetivity;
		double deltaY = (mouseY - point.y)*sesetivity;
		if (cam){
			switch (m_nAction){
			case ID_ACTION_TRANSLATE:
				switch (m_nAxis){
				case ID_AXIS_X:
					camera.transformation().updateTranslate(vec4(-deltaX/5, 0, 0));
					camera.updateEye(mat4::translate(vec4(-deltaX / 5, 0, 0))*camera.eye());
					break;
				case ID_AXIS_Y:
					camera.transformation().updateTranslate(vec4(0, -deltaX / 5, 0));
					camera.updateEye(mat4::translate(vec4(0,-deltaX / 5, 0))*camera.eye());
					break;
				case ID_AXIS_Z:
					camera.transformation().updateTranslate(vec4(0, 0, -deltaX / 5));
					camera.updateEye(mat4::translate(vec4(0,0,-deltaX / 5))*camera.eye());
					break;
				case ID_AXIS_XY:
					camera.transformation().updateTranslate(vec4(-deltaX / 5, -deltaY / 5, 0));
					camera.updateEye(mat4::translate(vec4(-deltaX / 5, -deltaY / 5, 0))*camera.eye());
					break;
				}
				break;
			case ID_ACTION_ROTATE:
				switch (m_nAxis){
				case ID_AXIS_X:
					camera.setTransformation(camera.transformation() * mat4::rotateX(deltaX / 5));
					camera.updateEye(mat4::translate(vec4(-deltaX / 5, -deltaY / 5, 0))*camera.eye());
					break;
				case ID_AXIS_Y:
					camera.setTransformation(camera.transformation() * mat4::rotateY(deltaX / 5));
					break;
				case ID_AXIS_Z:
					camera.setTransformation(camera.transformation() * mat4::rotateZ(deltaX / 5));
					break;
				case ID_AXIS_XY:
					camera.setTransformation(camera.transformation() * mat4::rotateX(deltaX / 5)*mat4::rotateY(deltaY/5));
					break;
				}
				break;
			case ID_ACTION_SCALE:
				switch (m_nAxis){
				case ID_AXIS_X:
					camera.transformation().updateScale(vec4(deltaX / 50, 0, 0));

					break;
				case ID_AXIS_Y:
					camera.transformation().updateScale(vec4(0, deltaX / 50 , 0));

					break;
				case ID_AXIS_Z:
					camera.transformation().updateScale(vec4(0, 0, deltaX / 50));

					break;
				case ID_AXIS_XY:
					camera.transformation().updateScale(vec4(deltaX / 50, deltaY / 50, 0));
					break;
				}
				break;
			}
		}
		else{
			if (single && changingObejct != NULL){
				switch (m_nAction){
				case ID_ACTION_TRANSLATE:
					switch (m_nAxis){
					case ID_AXIS_X:
						//maxX -= delta;
						//minX -= delta;
						//mins[0] -= delta;
						//maxs[0] -= delta;
						changingObejct->position.updateTranslate(vec4(deltaX / 5, 0, 0));
						break;
					case ID_AXIS_Y:
						//mins[1] -= delta;
						//maxs[1] -= delta;
						changingObejct->position.updateTranslate(vec4(0, deltaX / 5, 0));
						break;
					case ID_AXIS_Z:
						//maxs[2] -= delta;
						//mins[2] -= delta;
						changingObejct->position.updateTranslate(vec4(0, 0, deltaX / 5));
						break;
					case ID_AXIS_XY:
						changingObejct->position.updateTranslate(vec4(deltaX / 5, deltaY / 5, 0));
						break;
					}
					break;
				case ID_ACTION_ROTATE:
					switch (m_nAxis){
					case ID_AXIS_X:
						changingObejct->position = changingObejct->position * mat4::rotateX(deltaX / 5);
						break;
					case ID_AXIS_Y:
						changingObejct->position = changingObejct->position * mat4::rotateY(deltaX / 5);
						break;
					case ID_AXIS_Z:
						changingObejct->position = changingObejct->position * mat4::rotateZ(deltaX / 5);
						break;
					case ID_AXIS_XY:
						changingObejct->position = changingObejct->position * mat4::rotateX(deltaX / 5) * mat4::rotateY(deltaY / 5);
						break;
					}
					break;
				case ID_ACTION_SCALE:
					switch (m_nAxis){
					case ID_AXIS_X:
						changingObejct->maxX *= deltaX;
						changingObejct->minY *= deltaX;
						changingObejct->position.updateScale(vec4(deltaX / 50, 0, 0));
						break;
					case ID_AXIS_Y:
						changingObejct->maxY *= deltaX;
						changingObejct->minY *= deltaX;
						changingObejct->position.updateScale(vec4(0, deltaX / 50, 0));
						break;
					case ID_AXIS_Z:
						changingObejct->maxZ *= deltaX;
						changingObejct->minZ *= deltaX;
						changingObejct->position.updateScale(vec4(0, 0, deltaX / 50));
						break;
					case ID_AXIS_XY:
						changingObejct->maxX *= deltaX;
						changingObejct->minX *= deltaX;
						changingObejct->maxY *= deltaY;
						changingObejct->minY *= deltaY;
						changingObejct->position.updateScale(vec4(deltaX / 50, deltaY / 50, 0));
						break;
					}
					break;
				}
			}
			else{
				switch (m_nAction){
				case ID_ACTION_TRANSLATE:
					switch (m_nAxis){
					case ID_AXIS_X:
						//maxX -= delta;
						//minX -= delta;
						//mins[0] -= delta;
						//maxs[0] -= delta;
						m_translate.updateTranslate(vec4(deltaX / 5, 0, 0));
						break;
					case ID_AXIS_Y:
						//mins[1] -= delta;
						//maxs[1] -= delta;
						m_translate.updateTranslate(vec4(0, deltaX / 5, 0));
						break;
					case ID_AXIS_Z:
						//maxs[2] -= delta;
						//mins[2] -= delta;
						m_translate.updateTranslate(vec4(0, 0, deltaX / 5));
						break;
					case ID_AXIS_XY:
						//maxs[2] -= delta;
						//mins[2] -= delta;
						m_translate.updateTranslate(vec4(deltaX / 5, deltaY / 5, 0));
						break;
					}
					break;
				case ID_ACTION_ROTATE:
					switch (m_nAxis){
					case ID_AXIS_X:
						m_rotate = m_rotate * mat4::rotateX(deltaX / 5);
						break;
					case ID_AXIS_Y:
						m_rotate = m_rotate * mat4::rotateY(deltaX / 5);
						break;
					case ID_AXIS_Z:
						m_rotate = m_rotate * mat4::rotateZ(deltaX / 5);
						break;
					case ID_AXIS_XY:
						m_rotate = m_rotate * mat4::rotateX(deltaX / 5)*mat4::rotateY(deltaY / 5);
						break;
					}
					break;
				case ID_ACTION_SCALE:
					switch (m_nAxis){
					case ID_AXIS_X:
						maxX *= deltaX;
						minY *= deltaX;
						m_scale.updateScale(vec4(deltaX / 50, 0, 0));
						break;
					case ID_AXIS_Y:
						maxY *= deltaX;
						minY *= deltaX;
						m_scale.updateScale(vec4(0, deltaX / 50, 0));
						break;
					case ID_AXIS_Z:
						maxZ *= deltaX;
						minZ *= deltaX;
						m_scale.updateScale(vec4(0, 0, deltaX / 50));
						break;
					case ID_AXIS_XY:
						maxX *= deltaX;
						minX *= deltaX;
						maxY *= deltaY;
						minY *= deltaY;
						m_scale.updateScale(vec4(deltaX / 50, deltaY / 50, 0));
						break;
					}
					break;
				}

			}
		}
		Invalidate();
	}
	mouseX = point.x;
	mouseY = point.y;
}

void CCGWorkView::OnCamButton(){
	cam = 1;
}

void CCGWorkView::OnObjectButton(){
	cam = 0;
}

void CCGWorkView::OnUpdateCamButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cam == 1);
}

void CCGWorkView::OnUpdateObjectButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(cam == 0);
}

void CCGWorkView::OnModelColorUpdate(){
	CColorDialog color;
	color.DoModal();
	COLORREF tmpColor = color.GetColor();
	modelColor = RGB(GetBValue(tmpColor), GetGValue(tmpColor), GetRValue(tmpColor));
	colorNotChange = 0;
	Invalidate();
}

void CCGWorkView::OnBackgroundColorUpdate(){
	CColorDialog color;
	color.DoModal();
	COLORREF tmpColor = color.GetColor();
	backgroundColor = RGB(GetBValue(tmpColor), GetGValue(tmpColor), GetRValue(tmpColor));
	Invalidate();
}

void CCGWorkView::OnNormalColorUpdate(){
	CColorDialog color;
	color.DoModal();
	COLORREF tmpColor = color.GetColor();
	normalColor = RGB(GetBValue(tmpColor), GetGValue(tmpColor), GetRValue(tmpColor));
	Invalidate();
}

void CCGWorkView::OnNormalPolygonGiven(){
	if (polygonNormals){
		if (polyGiven){
			polyGiven = false;
			polygonNormals = false;
		}
		else{
			polyCalculated = false;
			polyGiven = true;
		}
	}
	else{
		polyGiven = true;
		polygonNormals = true;
	}Invalidate();
}

void CCGWorkView::OnNormalPolygonCalculated(){
	if (polygonNormals){
		if (polyCalculated){
			polyCalculated = false;
			polygonNormals = false;
		}
		else{
			polyGiven = false;
			polyCalculated = true;
		}
	}
	else{
		polyCalculated = true;
		polygonNormals = true;
	}
	Invalidate();
}

void CCGWorkView::OnNormalPolygonCalculatedCheck(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(polyCalculated);
}


void CCGWorkView::OnNormalPolygonGivenCheck(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(polyGiven);
}

void CCGWorkView::OnNormalVertexGiven(){
	if (vertexNormals){
		if (vertexGiven){
			vertexGiven = false;
			vertexNormals = false;
		}
		else{
			vertexCalculated = false;
			vertexGiven = true;
		}
	}
	else{
		vertexGiven = true;
		vertexNormals = true;
	}
	Invalidate();
}

void CCGWorkView::OnNormalVertexCalculated(){
	if (vertexNormals){
		if (vertexCalculated){
			vertexCalculated = false;
			vertexNormals = false;
		}
		else{
			vertexCalculated = true;
			vertexGiven = false;
		}
	}
	else{
		vertexCalculated = true;
		vertexNormals = true;
	}
	Invalidate();
}

void CCGWorkView::OnNormalVertexGivenCheck(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(vertexGiven);
}

void CCGWorkView::OnNormalVertexCalculatedCheck(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(vertexCalculated);
}

void CCGWorkView::OnBoundingBox(){
	BBox = !BBox;
	Invalidate();
}

void CCGWorkView::OnBoundingBoxCheck(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(BBox);
}

void CCGWorkView::OnOpenSettings(){
	settings.DoModal();
	F = -settings.d;
	sesetivity = settings.sensetivity / 500.0;
	CGSkelFFCState.FineNess = settings.fineness;
	Invalidate();
}

void CCGWorkView::OnAxisXY(){
	m_nAxis = ID_AXIS_XY;
}

void CCGWorkView::OnUpdateAxisXY(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_XY);
}

void CCGWorkView::OnPolyFill(){
	polyFill = !polyFill;
	Invalidate();
}

void CCGWorkView::OnPolyFillCheck(CCmdUI* pCmdUI){
	pCmdUI->SetCheck(polyFill);
}