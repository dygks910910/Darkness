#include "Model.h"

CModel::CModel()
{
}


CModel::~CModel()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);

}
