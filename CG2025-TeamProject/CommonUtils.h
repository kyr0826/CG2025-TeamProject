#pragma once
#include "Common_Headers.h"

namespace Utils {
	class CommonUtils {
	public:
		static float RandomFloat(float min, float max);
		static int RandomInt(int min, int max);
	private:
		static std::random_device rd;
		static std::default_random_engine dre;
	};
}