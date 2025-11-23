#include "CommonUtils.h"

namespace Utils {
	std::random_device CommonUtils::rd;
	std::default_random_engine CommonUtils::dre{ CommonUtils::rd() };

	float CommonUtils::RandomFloat(float min, float max)
	{
		std::uniform_real_distribution<float> urd(min, max);
		return urd(dre);
	}

	int CommonUtils::RandomInt(int min, int max) {
		std::uniform_int_distribution<int> uid(min, max);
		return uid(dre);
	}

}