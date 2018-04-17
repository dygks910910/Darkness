#include "structForFBX.h"

bool Vertex::operator==(const Vertex & oth)
{
	
	return cmpf( pos.x , oth.pos.x) &&
		cmpf(pos.y, oth.pos.y) &&
		cmpf(pos.z, oth.pos.z);

}

bool VertexCompare::operator()(const Vertex & a, const Vertex & b)const
{
	return cmpf(a.pos.x, b.pos.x) &&
		cmpf(a.pos.y, b.pos.y) &&
		cmpf(a.pos.z, b.pos.z);
}
