#pragma once
#include <string>

extern const std::string RoundingStrings[];

namespace Utils
{
	typedef struct Rounding
	{
		int Nearest = (int)mpfr_rnd_t::MPFR_RNDN;
		int TowardZero = (int)mpfr_rnd_t::MPFR_RNDZ;
		int TowardInfinity = (int)mpfr_rnd_t::MPFR_RNDU;
		int TowardMinusInfinity = (int)mpfr_rnd_t::MPFR_RNDD;
		int AwayZero = (int)mpfr_rnd_t::MPFR_RNDA;
		int Faithful = (int)mpfr_rnd_t::MPFR_RNDF;
	} RoundingModes;
}