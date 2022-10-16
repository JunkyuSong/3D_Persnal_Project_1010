#include "RandMgr.h"

#include <random>
#include <math.h>

IMPLEMENT_SINGLETON(CRandMgr)


const _float & CRandMgr::Rand_Float(const _float & _fMin, const _float & _fMax)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(_fMin, _fMax);

	return distr(eng);
}

const _int & CRandMgr::Rand_Int(const _int & _iMin, const _int & _iMax)
{
	std::random_device random;
	std::mt19937 rd(random());
	std::uniform_int_distribution<int> range(_iMin, _iMax);
	int iNum = range(random);
	return iNum;
}

void CRandMgr::Free()
{
}
