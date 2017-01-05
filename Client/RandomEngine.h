#pragma once
#include<random>
class CRandomEngine
{
	std::random_device m_rd;
	std::mt19937_64 m_rnd;
public:
	CRandomEngine();
	~CRandomEngine();
	float GetRandomNumFloat(const float& start, const float& end);
	int GetRandomNumInt(const int& start, const int& end);
};

