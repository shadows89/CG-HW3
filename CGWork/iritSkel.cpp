#include "StdAfx.h"
#include "iritSkel.h"


CG_ModelList models;
double max = 0;
double maxX = INT64_MIN, maxY = INT64_MIN, maxZ = INT64_MIN;
double minX = INT64_MAX, minY = INT64_MAX, minZ = INT64_MAX;
int firstDraw = 0;


/*****************************************************************************
* Skeleton for an interface to a parser to read IRIT data files.			 *
******************************************************************************
* (C) Gershon Elber, Technion, Israel Institute of Technology                *
******************************************************************************
* Written by:  Gershon Elber				Ver 1.0, Feb 2002				 *
* Minimal changes made by Amit Mano			November 2008					 *
******************************************************************************/

IPFreeformConvStateStruct CGSkelFFCState = {
	FALSE,          /* Talkative */
	FALSE,          /* DumpObjsAsPolylines */
	TRUE,           /* DrawFFGeom */
	FALSE,          /* DrawFFMesh */
	{ 10, 10, 10 }, /* 10 isocurves peru/v/w direction. */
	100,            /* 100 point samples along a curve. */
	SYMB_CRV_APPROX_UNIFORM,  /* CrvApproxMethod */
	FALSE,   /* ShowIntrnal */
	FALSE,   /* CubicCrvsAprox */
	20,      /* Polygonal FineNess */
	FALSE,   /* ComputeUV */
	TRUE,    /* ComputeNrml */
	FALSE,   /* FourPerFlat */
	0,       /* OptimalPolygons */
	FALSE,   /* BBoxGrid */
	TRUE,    /* LinearOnePolyFlag */
	FALSE
};

//CGSkelProcessIritDataFiles(argv + 1, argc - 1);


/*****************************************************************************
* DESCRIPTION:                                                               *
* Main module of skeleton - Read command line and do what is needed...	     *
*                                                                            *
* PARAMETERS:                                                                *
*   FileNames:  Files to open and read, as a vector of strings.              *
*   NumFiles:   Length of the FileNames vector.								 *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelProcessIritDataFiles(CString &FileNames, int NumFiles)
{
	IPObjectStruct *PObjects;
	IrtHmgnMatType CrntViewMat;

	/* Get the data files: */
	IPSetFlattenObjects(FALSE);
	CStringA CStr(FileNames);
	if ((PObjects = IPGetDataFiles((const char* const *)&CStr, 1/*NumFiles*/, TRUE, FALSE)) == NULL)
		return false;
	PObjects = IPResolveInstances(PObjects);

	if (IPWasPrspMat)
		MatMultTwo4by4(CrntViewMat, IPViewMat, IPPrspMat);
	else
		IRIT_GEN_COPY(CrntViewMat, IPViewMat, sizeof(IrtHmgnMatType));

	/* Here some useful parameters to play with in tesselating freeforms: */
	CGSkelFFCState.FineNess = 20;   /* Res. of tesselation, larger is finer. */
	CGSkelFFCState.ComputeUV = TRUE;   /* Wants UV coordinates for textures. */
	CGSkelFFCState.FourPerFlat = TRUE;/* 4 poly per ~flat patch, 2 otherwise.*/
	CGSkelFFCState.LinearOnePolyFlag = TRUE;    /* Linear srf gen. one poly. */

	models.clear();
	firstDraw = 0;
	max = 0;
	maxX = INT64_MIN; maxY = INT64_MIN; maxZ = INT64_MIN;
	minX = INT64_MAX; minY = INT64_MAX; minZ = INT64_MAX;
	/* Traverse ALL the parsed data, recursively. */
	IPTraverseObjListHierarchy(PObjects, CrntViewMat,
		CGSkelDumpOneTraversedObject);
	firstDraw = 1;
	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Call back function of IPTraverseObjListHierarchy. Called on every non    *
* list object found in hierarchy.                                            *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Non list object to handle.                                   *
*   Mat:        Transformation matrix to apply to this object.               *
*                                                                            *
* RETURN VALUE:                                                              *
*   void									                                 *
*****************************************************************************/
void CGSkelDumpOneTraversedObject(IPObjectStruct *PObj, IrtHmgnMatType Mat)
{
	IPObjectStruct *PObjs;

	if (IP_IS_FFGEOM_OBJ(PObj))
		PObjs = IPConvertFreeForm(PObj, &CGSkelFFCState);
	else
		PObjs = PObj;

	for (PObj = PObjs; PObj != NULL; PObj = PObj->Pnext)
		if (!CGSkelStoreData(PObj))
			exit(1);
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Prints the data from given geometry object.								 *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Object to print.                                             *
*   Indent:     Column of indentation.                                       *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelStoreData(IPObjectStruct *PObj)
{
	int i;
	const char *Str;
	double _RGB[3], Transp;
	IPPolygonStruct *PPolygon;
	IPVertexStruct *PVertex;
	IPAttributeStruct *Attrs = AttrTraceAttributes(PObj->Attr, PObj->Attr);
	_RGB[0] = 0;
	_RGB[1] = 0;
	_RGB[2] = 0;
	if (PObj->ObjType != IP_OBJ_POLY) {
		AfxMessageBox(_T("Non polygonal object detected and ignored"));
		return true;
	}

	/* You can use IP_IS_POLYGON_OBJ(PObj) and IP_IS_POINTLIST_OBJ(PObj)
	to identify the type of the object*/

	if (CGSkelGetObjectColor(PObj, _RGB))
	{
		/* color code */
	}
	if (CGSkelGetObjectTransp(PObj, &Transp))
	{
		/* transparency code */
	}
	if ((Str = CGSkelGetObjectTexture(PObj)) != NULL)
	{
		/* volumetric texture code */
	}
	if ((Str = CGSkelGetObjectPTexture(PObj)) != NULL)
	{
		/* parametric texture code */
	}
	if (Attrs != NULL)
	{
		printf("[OBJECT\n");
		while (Attrs) {
			/* attributes code */
			Attrs = AttrTraceAttributes(Attrs, NULL);
		}
	}
	Model* model = new Model(new CG_PolygonList, new CG_NormalList, new CG_NormalList, RGB(255 * _RGB[2], 255 * _RGB[1], 255 * _RGB[0]));
	for (PPolygon = PObj->U.Pl; PPolygon != NULL; PPolygon = PPolygon->Pnext)
	{
		if (PPolygon->PVertex == NULL) {
			AfxMessageBox(_T("Dump: Attemp to dump empty polygon"));
			delete model;
			return false;
		}

		/* Count number of vertices. */
		for (PVertex = PPolygon->PVertex->Pnext, i = 1;
			PVertex != PPolygon->PVertex && PVertex != NULL;
			PVertex = PVertex->Pnext, i++);
		/* use if(IP_HAS_PLANE_POLY(PPolygon)) to know whether a normal is defined for the polygon
		access the normal by the first 3 components of PPolygon->Plane */
		PVertex = PPolygon->PVertex;
		CG_Polygon* poly = new CG_Polygon;

		do {			     /* Assume at least one edge in polygon! */
			/* code handeling all vertex/normal/texture coords */
			/* use if(IP_HAS_NORMAL_VRTX(PVertex)) to know whether a normal is defined for the vertex
			access the vertex coords by PVertex->Coord
			access the vertex normal by PVertex->Normal */


			double x = PVertex->Coord[0];
			double y = PVertex->Coord[1];
			double z = PVertex->Coord[2];
			CG_Point* p = new CG_Point(x, y, z, 1);
			CG_Point* v = new CG_Point(x, y, z, 1);
			if (abs(x) > max)
				max = abs(x);
			if (abs(y) > max)
				max = abs(y);
			/*if (abs(z) > max)
				max = abs(z);*/
			if (x > maxX)
				maxX = x;
			if (y > maxY)
				maxY = y;
			if (z > maxZ)
				maxZ = z;
			if (x < minX)
				minX = x;
			if (y < minY)
				minY = y;
			if (z < minZ)
				minZ = z;
			if (x > model->maxX)
				model->maxX = x;
			if (y > model->maxY)
				model->maxY = y;
			if (z > model->maxZ)
				model->maxZ = z;
			if (x < model->minX)
				model->minX = x;
			if (y < model->minY)
				model->minY = y;
			if (z < model->minZ)
				model->minZ = z;

			poly->add(p);
			model->vertices->add(v);
			if (IP_HAS_NORMAL_VRTX(PVertex) != 0){
				model->vertexNormals->add(new vec4(PVertex->Normal[0], PVertex->Normal[1], PVertex->Normal[2]));
				//model->VertexNormalHash.insert({ *p, vec4(PVertex->Normal[0], PVertex->Normal[1], PVertex->Normal[2]) });
			}
			PVertex = PVertex->Pnext;
		} while (PVertex != PPolygon->PVertex && PVertex != NULL);
		if (IP_HAS_PLANE_POLY(PPolygon))
			model->polygonNormals->add(new vec4(PPolygon->Plane[0], PPolygon->Plane[1], PPolygon->Plane[2]));
		
		model->polygons->add(poly);
		/* Close the polygon. */
	}
	models.add(model);
	model->calculateNormals();
	/* Close the object. */

	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the color of an object.                                          *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its color.                                         *
*   RGB:    as 3 floats in the domain [0, 1].                                *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has color, FALSE otherwise.                       *
*****************************************************************************/
int CGSkelGetObjectColor(IPObjectStruct *PObj, double RGB[3])
{
	static int TransColorTable[][4] = {
		{ /* BLACK	*/   0, 0, 0, 0 },
		{ /* BLUE	*/   1, 0, 0, 255 },
		{ /* GREEN	*/   2, 0, 255, 0 },
		{ /* CYAN	*/   3, 0, 255, 255 },
		{ /* RED	*/   4, 255, 0, 0 },
		{ /* MAGENTA 	*/   5, 255, 0, 255 },
		{ /* BROWN	*/   6, 50, 0, 0 },
		{ /* LIGHTGRAY	*/   7, 127, 127, 127 },
		{ /* DARKGRAY	*/   8, 63, 63, 63 },
		{ /* LIGHTBLUE	*/   9, 0, 0, 255 },
		{ /* LIGHTGREEN	*/   10, 0, 255, 0 },
		{ /* LIGHTCYAN	*/   11, 0, 255, 255 },
		{ /* LIGHTRED	*/   12, 255, 0, 0 },
		{ /* LIGHTMAGENTA */ 13, 255, 0, 255 },
		{ /* YELLOW	*/   14, 255, 255, 0 },
		{ /* WHITE	*/   15, 255, 255, 255 },
		{ /* BROWN	*/   20, 50, 0, 0 },
		{ /* DARKGRAY	*/   56, 63, 63, 63 },
		{ /* LIGHTBLUE	*/   57, 0, 0, 255 },
		{ /* LIGHTGREEN	*/   58, 0, 255, 0 },
		{ /* LIGHTCYAN	*/   59, 0, 255, 255 },
		{ /* LIGHTRED	*/   60, 255, 0, 0 },
		{ /* LIGHTMAGENTA */ 61, 255, 0, 255 },
		{ /* YELLOW	*/   62, 255, 255, 0 },
		{ /* WHITE	*/   63, 255, 255, 255 },
		{ -1, 0, 0, 0 }
	};
	int i, j, Color, RGBIColor[3];

	if (AttrGetObjectRGBColor(PObj,
		&RGBIColor[0], &RGBIColor[1], &RGBIColor[2])) {
		for (i = 0; i < 3; i++)
			RGB[i] = RGBIColor[i] / 255.0;

		return TRUE;
	}
	else if ((Color = AttrGetObjectColor(PObj)) != IP_ATTR_NO_COLOR) {
		for (i = 0; TransColorTable[i][0] >= 0; i++) {
			if (TransColorTable[i][0] == Color) {
				for (j = 0; j < 3; j++)
					RGB[j] = TransColorTable[i][j + 1] / 255.0;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the volumetric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of volumetric texture map to apply, NULL if none.        *
*****************************************************************************/
const char *CGSkelGetObjectTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "texture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the parametric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its parametric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of parametric texture map to apply, NULL if none.        *
*****************************************************************************/
const char *CGSkelGetObjectPTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "ptexture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the transparency level of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*   Transp: Transparency level between zero and one.                         *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has transparency, FALSE otherwise.                *
*****************************************************************************/
int CGSkelGetObjectTransp(IPObjectStruct *PObj, double *Transp)
{
	*Transp = AttrGetObjectRealAttrib(PObj, "transp");

	return !IP_ATTR_IS_BAD_REAL(*Transp);
}

