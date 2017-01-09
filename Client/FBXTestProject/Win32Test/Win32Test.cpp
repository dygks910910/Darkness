// Win32Test.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//


#include "stdafx.h"
/**
* Main function - loads the hard-coded fbx file,
* and prints its contents in an xml format to stdout.
*/
#include "FbxLoader.h"
int main(int argc, char** argv)
{
	CFbxLoader loader;
	loader.Init("test.fbx");
	loader.PrintElement();
	loader.Destroy();
	
}
