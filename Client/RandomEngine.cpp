#include "RandomEngine.h"



CRandomEngine::CRandomEngine()
{
	m_rnd.seed(m_rd());
}


CRandomEngine::~CRandomEngine()
{
}

float CRandomEngine::GetRandomNumFloat(const float& start, const float& end)
{
	std::uniform_real_distribution<float> range(start, end);
	return range(m_rnd);
}

int CRandomEngine::GetRandomNumInt(const int& start, const int& end)
{
	std::uniform_int_distribution<int> range(start, end);
	return range(m_rnd);
}
