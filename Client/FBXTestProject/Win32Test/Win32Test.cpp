// Win32Test.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//


#include "stdafx.h"
using namespace std;
/**
* Main function - loads the hard-coded fbx file,
* and prints its contents in an xml format to stdout.
*/
#include "LoadFBX.h"
int main(int argc, char** argv)
{
	CLoadFBX loader("true_clown.fbx");
	//loader.ShowDetails();
	
	
}
