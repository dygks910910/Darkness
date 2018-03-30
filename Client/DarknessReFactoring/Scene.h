#pragma once
#include"Global.h"

class Scene
{
public:
	Scene();
	virtual ~Scene();

	//�����Լ�
	float AspectRatio();

	//���������Լ�
	virtual bool Init()=0;
	virtual void OnKeyboardButtonDown(WPARAM wParam)=0;
	virtual std::string UpdateScene(const float dt, MSG& msg)=0;
	virtual void Draw()=0;
	virtual void OnMouseDown(WPARAM btnState, int x, int y)=0;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)=0;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)=0;
	virtual void OnResize()=0;
};

