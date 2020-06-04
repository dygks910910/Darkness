// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

#pragma comment( lib, "d3dx9" ) 
#pragma comment( lib, "d3dx10" ) 
#pragma comment( lib, "d3dx11d" ) 
#pragma comment( lib, "d3dx11" ) 

#pragma comment( lib, "dxguid" ) 
#pragma comment( lib, "dxerr" ) 
#pragma comment( lib, "winmm" ) 
#pragma comment( lib, "comctl32" ) 
#pragma comment( lib, "imm32" ) 
#pragma comment( lib, "dsound" ) 
#pragma comment( lib, "version" )
#pragma comment( lib, "d3dcompiler" ) 
#pragma comment( lib, "legacy_stdio_definitions" ) 



// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
#include"DXUT.h"
#include"DXUTgui.h"
#include"DXUTmisc.h"
#include"DXUTcamera.h"
#include"DXUTsettingsdlg.h"
#include"SDKmisc.h"
#include"SDKmesh.h"

#endif //PCH_H
