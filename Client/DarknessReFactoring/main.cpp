#include"Common\d3dApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
#ifdef _DEBUG


#include <iostream>
#pragma comment( linker, "/entry:WinMainCRTStartup /subsystem:console" )
#endif

	//_CrtSetBreakAlloc(3054771);

	D3DApp mainGame(hInstance);

	if (!mainGame.Init())
		return 0;

	return mainGame.Run();
}