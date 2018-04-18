#include "structForFBX.h"
//  
//  bool Vertex::operator==(const Vertex & oth)
//  {
//  	
//  	return cmpf( pos.x , oth.pos.x) &&
//  		cmpf(pos.y, oth.pos.y) &&
//  		cmpf(pos.z, oth.pos.z);
//  
//  }
//  
 bool VertexCompare::operator()(const GeometryGenerator::Vertex  & a, const GeometryGenerator::Vertex  & b)const
 {
 	return cmpf(a.Position.x, b.Position.x) &&
 		cmpf(a.Position.y, b.Position.y) &&
 		cmpf(a.Position.z, b.Position.z);
 }
