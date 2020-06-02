#pragma once
class CInputClass
{
public:
	CInputClass();
	CInputClass(const CInputClass&);
	~CInputClass();

	void Initialize();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	bool IsKeyDown(unsigned int);

private:
	bool m_bKeys[256];
};

