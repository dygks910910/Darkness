// Win32Test.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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
