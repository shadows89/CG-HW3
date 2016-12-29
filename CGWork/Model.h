#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "LinkedList.h"
#include "mat4.hpp"


typedef vec4 CG_Point;
typedef LinkedList<CG_Point*> CG_Polygon;
typedef LinkedList<CG_Point*> CG_NormalList;
typedef LinkedList<CG_Polygon*> CG_PolygonList;

class Model{
public:
	CG_PolygonList* polygons;
	CG_NormalList* polygonNormals;
	CG_NormalList* vertexNormals;
	CG_NormalList* calculatedPolygonNormals;
	CG_NormalList* calculatedVertexNormals;
	CG_Polygon* vertices;
	CG_Polygon* polygonMids;
	COLORREF color;

	mat4 position;

	double maxX = INT64_MIN, maxY = INT64_MIN, maxZ = INT64_MIN;
	double minX = INT64_MAX, minY = INT64_MAX, minZ = INT64_MAX;

	Model(CG_PolygonList* polygons,CG_NormalList* polygonNormals,CG_NormalList* vertexNormals,COLORREF color){
		this->polygons = polygons;
		this->polygonNormals = polygonNormals;
		this->vertexNormals = vertexNormals;
		this->color = color;
		position = mat4::eye();
		calculatedPolygonNormals = new CG_NormalList;
		calculatedVertexNormals = new CG_NormalList;
		vertices = new CG_Polygon;
		polygonMids = new CG_Polygon;
	}

	~Model(){
		delete calculatedPolygonNormals;
		delete calculatedVertexNormals;
		delete polygons;
		delete polygonNormals;
		delete vertexNormals;
		delete vertices;
		delete polygonMids;
	}

	void calculateNormals(){
		calculatePolygonNormals();
		calculateVertexNormals();
	}



	void calculatePolygonNormals(){
		for (CG_Polygon* polygon = polygons->first(); polygon != NULL; polygon = polygons->next()){
			CG_Point* mid =  new vec4(0, 0, 0);
			for (CG_Point* point = polygon->first(); point != NULL; point = polygon->next())
				(*mid) = (*mid) + (*point);
			(*mid) = (*mid) *(1.0/(polygon->getSize()));
			polygonMids->add(mid);
		}

		for (CG_Polygon* polygon = polygons->first(); polygon != NULL; polygon = polygons->next()){
			CG_Point* p1, *p2, *p3;
			p1 = polygon->first();
			p2 = polygon->next();
			p3 = polygon->next();
			if (p1 == NULL || p2 == NULL || p3 == NULL)
				continue;
			else{
				vec4 tmp1 = *p1 - *p2;
				vec4 tmp2 = *p2 - *p3;
				vec4 cross = vec4::cross(tmp1, tmp2);
				if (cross.isZero()){
					while (p3 != NULL){
						p1 = p2;
						p2 = p3;
						p3 = polygon->next();
						if (p3 == NULL)
							break;
						vec4 tmp1 = *p1 - *p2;
						vec4 tmp2 = *p2 - *p3;
						vec4 cross = vec4::cross(tmp1, tmp2);
						if (!cross.isZero()){
							calculatedPolygonNormals->add(new vec4(cross));
							break;
						}
					}
				}
				else{
					calculatedPolygonNormals->add(new vec4(cross));
				}
			}
		}
	}

	void calculateVertexNormals(){
		for (CG_Point* vertex = vertices->first(); vertex != NULL; vertex = vertices->next()){
			vec4* normal = new vec4(0, 0, 0);
			CG_Point* polyNormal = calculatedPolygonNormals->first();
			int count = 0;
			for (CG_Polygon* polygon = polygons->first(); polygon != NULL && polyNormal != NULL; polygon = polygons->next()){
				if (pointInPolygon(vertex, polygon)){
					*normal = *normal + *polyNormal;
					count++;
				}
				polyNormal = calculatedPolygonNormals->next();
			}
			*normal = (*normal) * (1.0 / count);
			calculatedVertexNormals->add(normal);
		}
	}

	static bool pointInPolygon(vec4* point, CG_Polygon* polygon){
		for (CG_Point* p = polygon->first(); p != NULL; p = polygon->next()){
			if ((*point - *p).isZero())
				return TRUE;
		}
		return FALSE;
	}
};


typedef  LinkedList<Model*> CG_ModelList;

#endif