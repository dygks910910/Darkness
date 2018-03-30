#pragma once
#include"Common\d3dUtil.h"
#include"Global.h"
#include"TestScene.h"
class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	bool Init();
	void UpdateScene(const float& dt, MSG& msg);
	void Draw();
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnResize();
	void OnKeyboardButtonDown(const WPARAM& wParam);
private:
	CTestScene* scene;

};

