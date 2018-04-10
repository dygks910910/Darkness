#include"Material.h"

namespace MeterialList
{
	Material Brick = {
		XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f),//Ambient
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f),//Diffuse
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f),//Specular+SpecPower
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)//reflect
	};
	Material Grid = {
		XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f),
		XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 5.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	};

}