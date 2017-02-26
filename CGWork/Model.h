#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <unordered_map>
#include "LinkedList.h"
#include "mat4.hpp"



typedef vec4 CG_Point;
typedef LinkedList<CG_Point*> CG_Polygon;
typedef LinkedList<CG_Point*> CG_NormalList;
typedef LinkedList<CG_Polygon*> CG_PolygonList;


typedef std::unordered_map<std::string, vec4 > VertexNormalHash;
typedef std::unordered_map<std::string, CG_NormalList* > EdgePloygonHash;


class Model{
public:
	CG_PolygonList* polygons;
	CG_NormalList* polygonNormals;
	CG_NormalList* vertexNormals;
	CG_NormalList* calculatedPolygonNormals;
	CG_NormalList* calculatedVertexNormals;
	CG_Polygon* vertices;
	CG_Polygon* polygonMids;
	VertexNormalHash givenVertexNormalHash;
	VertexNormalHash calculatedVertexNormalHash;

	CG_NormalList* uvVertexList;

	EdgePloygonHash edgeGivenPolygonNormalHash;
	EdgePloygonHash edgeCalculatedPolygonNormalHash;
	COLORREF color;

	mat4 position;

	double maxX = INT64_MIN, maxY = INT64_MIN, maxZ = INT64_MIN;
	double minX = INT64_MAX, minY = INT64_MAX, minZ = INT64_MAX;

	double maxUV = INT64_MIN;

	Model(CG_PolygonList* polygons,CG_NormalList* polygonNormals,CG_NormalList* vertexNormals,COLORREF color){
		this->polygons = polygons;
		this->polygonNormals = polygonNormals;
		this->vertexNormals = vertexNormals;
		this->color = color;
		position = mat4::eye();
		calculatedPolygonNormals = new CG_NormalList;
		calculatedVertexNormals = new CG_NormalList;
		uvVertexList = new CG_NormalList;
		vertices = new CG_Polygon;
		polygonMids = new CG_Polygon;
	}

	~Model(){
		for (EdgePloygonHash::iterator iter = edgeCalculatedPolygonNormalHash.begin();
					iter != edgeCalculatedPolygonNormalHash.end(); iter++){
			delete std::get<1>(*iter);
		}
		for (EdgePloygonHash::iterator iter = edgeGivenPolygonNormalHash.begin();
					iter != edgeGivenPolygonNormalHash.end(); iter++){
			delete std::get<1>(*iter);
		}
		delete uvVertexList;
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
		updateEdgePolygonHash();
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
				vec4 cross = vec4::normalize(vec4::cross(tmp1, tmp2));
				if (cross.isZero()){
					while (p3 != NULL){
						p1 = p2;
						p2 = p3;
						p3 = polygon->next();
						if (p3 == NULL)
							break;
						vec4 tmp1 = *p1 - *p2;
						vec4 tmp2 = *p2 - *p3;
						vec4 cross = vec4::normalize(vec4::cross(tmp1, tmp2));
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
			*normal = vec4::normalize((*normal) * (1.0 / count));
			calculatedVertexNormals->add(normal);
			calculatedVertexNormalHash.insert({ (*vertex).toString(), *normal });
		}
	}

	static bool pointInPolygon(vec4* point, CG_Polygon* polygon){
		for (CG_Point* p = polygon->first(); p != NULL; p = polygon->next()){
			if ((*point - *p).isZero())
				return TRUE;
		}
		return FALSE;
	}

	void updateEdgePolygonHash(){
		vec4* calculatedPolyNormal = calculatedPolygonNormals->first();
		vec4* givenPolyNormal = polygonNormals->first();
		for (CG_Polygon* polygon = polygons->first(); polygon != NULL; polygon = polygons->next()){
			CG_Point* p1 = polygon->first();
			CG_Point* p2 = polygon->next();
			std::string edge;
			vec4* temp;
			while (true){
				//edge = (*p1).toString() + (*p2).toString();
				temp = new vec4((*calculatedPolyNormal)[0], (*calculatedPolyNormal)[1], (*calculatedPolyNormal)[2]);
				updateEdgePolygonHashAux((*p1), (*p2), temp, true);
				temp = new vec4((*givenPolyNormal)[0], (*givenPolyNormal)[1], (*givenPolyNormal)[2]);
				updateEdgePolygonHashAux((*p1), (*p2), temp, false); p1 = p2;
				p2 = polygon->next();
				if (p2 == NULL){
					p2 = polygon->first();
					//edge = (*p1).toString() + (*p2).toString();
					temp = new vec4((*calculatedPolyNormal)[0], (*calculatedPolyNormal)[1], (*calculatedPolyNormal)[2]);
					updateEdgePolygonHashAux((*p1), (*p2), temp, true);
					temp = new vec4((*givenPolyNormal)[0], (*givenPolyNormal)[1], (*givenPolyNormal)[2]);
					updateEdgePolygonHashAux((*p1), (*p2), temp, false);
					calculatedPolyNormal = calculatedPolygonNormals->next();
					givenPolyNormal = polygonNormals->next();
					break;
				}
			}
		}
	}

	void updateEdgePolygonHashAux(CG_Point p1, CG_Point p2, vec4* polyNormal, bool updateByCalculated){
		std::string edge1 = p1.toString() + p2.toString(), edge2 = p2.toString() + p1.toString();
		if (updateByCalculated){
			if (edgeCalculatedPolygonNormalHash.count(edge1) == 0){
				if (edgeCalculatedPolygonNormalHash.count(edge2) == 0){
					CG_NormalList* newNormalList = new CG_NormalList();
					newNormalList->add(polyNormal);
					edgeCalculatedPolygonNormalHash.insert({ edge1, newNormalList });
				}
				else{
					CG_NormalList* normalList = edgeCalculatedPolygonNormalHash[edge2];
					normalList->add(polyNormal);
				}
			}
			else{
				CG_NormalList* normalList = edgeCalculatedPolygonNormalHash[edge1];
				normalList->add(polyNormal);
			}
		}
		else{
			if (edgeGivenPolygonNormalHash.count(edge1) == 0){
				if (edgeGivenPolygonNormalHash.count(edge2) == 0){
					CG_NormalList* newNormalList = new CG_NormalList();
					newNormalList->add(polyNormal);
					edgeGivenPolygonNormalHash.insert({ edge1, newNormalList });
				}
				else{
					CG_NormalList* normalList = edgeGivenPolygonNormalHash[edge2];
					normalList->add(polyNormal);
				}
			}
			else{
				CG_NormalList* normalList = edgeGivenPolygonNormalHash[edge1];
				normalList->add(polyNormal);
			}
		}
		/*else{
			if (edgeGivenPolygonNormalHash.count(edge) == 0){
				CG_NormalList* newNormalList = new CG_NormalList();
				newNormalList->add(polyNormal);
				edgeGivenPolygonNormalHash.insert({ edge, newNormalList });
			}
			else{
				CG_NormalList* normalList = edgeGivenPolygonNormalHash[edge];
				normalList->add(polyNormal);
			}
		}*/
	}

	void normalizeUV(){
		if (uvVertexList->getSize() != 0)
			for (CG_Point* uv = uvVertexList->first(); uv != NULL; uv = uvVertexList->next()){
				(*uv)[0] = (*uv)[0] / maxUV;
				(*uv)[1] = (*uv)[1] / maxUV;
			}
	}
};


typedef  LinkedList<Model*> CG_ModelList;

#endif