#include "DynamicObject.h"



CDynamicObject::CDynamicObject()
{
}


CDynamicObject::~CDynamicObject()
{
	ReleaseCOM(mObjVB);
	ReleaseCOM(mObjIB);
	ReleaseCOM(mObjMapSRV);

}
