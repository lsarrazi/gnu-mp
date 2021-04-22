#include <mpfr.h>
#include <iostream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <sstream>

using namespace emscripten;

#include "Float.hpp"
#include "utils.hpp"

Float::Float()
{
	mpfr_init(&wrapped);
}

Float::Float(val v)
{
	if (v.isNumber())
		mpfr_init2(&wrapped, v.as<size_t>());
	else
	{
		const Float &op = v.as<const Float &>();
		mpfr_init2(&wrapped, op.getPrecision());
		mpfr_set(&wrapped, &op.wrapped, rounding);
	}
}

Float::Float(prec_t prec)
{
	mpfr_init2(&wrapped, prec);
}

Float::Float(prec_t prec, double v) : Float(prec)
{
	*this = v;
}

Float::Float(const Float& op)
{
	mpfr_init2(&wrapped, op.getPrecision());
	mpfr_set(&wrapped, &op.wrapped, rounding);
}

Float& Float::operator=(const Float& op)
{
	mpfr_set(&wrapped, &op.wrapped, rounding);
	return *this;
}

Float& Float::operator=(double v)
{
	mpfr_set_d(&wrapped, v, rounding);
	return *this;
}

Float::~Float() { mpfr_clear(&wrapped); }

// rounding(mode)
int Float::getRounding() const { return rounding; }
const std::string &Float::getRoundingString()
{
	return RoundingStrings[rounding];
}

Float::builder_pattern Float::setRounding(int mode)
{
	rounding = static_cast<rnd_t>(mode);
}

// precision
Float::size_t Float::getPrecision() const { return mpfr_get_prec(&wrapped); };
Float::builder_pattern Float::setPrecision(size_t precision)
{
	mpfr_set_prec(&wrapped, precision);
}

// exponent
Float::exp_t Float::getExponent() const { return mpfr_get_exp(&wrapped); };
Float::builder_pattern Float::setExponent(exp_t exponent)
{
	mpfr_set_exp(&wrapped, exponent);
}

// sign
Float::sign_t Float::getSign() const { return MPFR_SIGN(&wrapped); }
Float::builder_pattern Float::setSign(sign_t sign)
{
	MPFR_SIGN(&wrapped) = sign;
}

// mantissa
Float::size_t Float::getMantissaSize() const
{
	return mpfr_custom_get_size(getPrecision());
}
val Float::getMantissaView()
{
	return val(typed_memory_view(getMantissaSize() / sizeof(mp_limb_t), wrapped._mpfr_d));
}

// set(string str, base = 10)
Float::builder_pattern Float::setString(const std::string &str) { return setString(str, 10); }
Float::builder_pattern Float::setString(const std::string &str, int base)
{
	mpfr_set_str(&wrapped, str.c_str(), base, rounding);
}
Float::builder_pattern Float::setDouble(double x)
{
	mpfr_set_d(&wrapped, x, rounding);
}
Float::builder_pattern Float::setFloat(const Float &tocopy)
{
	mpfr_set(&wrapped, &(tocopy).wrapped, rounding);
}
Float::builder_pattern Float::set(val v)
{
	if (v.isNumber())
		return setDouble(v.as<double>());
	else if (v.isString())
		return setString(v.as<std::string>());
	else
		return setFloat(v.as<const Float &>());
}

// toString(int base = 10, int n = 0)
std::string Float::toString(int base) { return toString(base, 0); }
std::string Float::toString() { return toString(10, 0); }
std::string Float::toString(int base, int n)
{
	std::stringstream ss;
	mpfr_exp_t exp;
	char *str = mpfr_get_str(nullptr, &exp, base, n, &wrapped, rounding);
	if (exp)
		exp--;
	if (getSign() > 0)
		ss << str[0] << '.' << &str[1];
	else
		ss << '-' << str[1] << '.' << &str[2];
	ss << "e" << (exp < 0 ? '-' : '+') << ::abs(exp);
	mpfr_free_str(str);
	return ss.str();
}

// toNumber()
double Float::toNumber()
{
	return mpfr_get_d(&wrapped, rounding);
}

bool Float::isInteger()
{
	return !!mpfr_integer_p(&wrapped);
}

// operator < <= > >= == !=
bool Float::operator<(const Float &op) const
{
	return mpfr_cmp(&wrapped, &op.wrapped) < 0;
}
bool Float::operator==(const Float &op) const
{
	return mpfr_cmp(&wrapped, &op.wrapped) == 0;
}
bool Float::operator<=(const Float &op) const { return !(op < *this); }
bool Float::operator>(const Float &op) const { return op < *this; }
bool Float::operator>=(const Float &op) const { return !(*this < op); }
bool Float::operator!=(const Float &op) const { return !(*this == op); }

bool Float::operator<(double op) const
{
	return mpfr_cmp_d(&wrapped, op) < 0;
}
bool Float::operator>(double op) const
{
	return mpfr_cmp_d(&wrapped, op) > 0;
}
bool Float::operator==(double op) const
{
	return mpfr_cmp_d(&wrapped, op) == 0;
}
bool Float::operator<=(double op) const { return !(*this > op); }
bool Float::operator>=(double op) const { return !(*this < op); }
bool Float::operator!=(double op) const { return !(*this == op); }

bool Float::less(val v)
{
	if (v.isNumber())
		return *this < v.as<double>();
	else
		return *this < v.as<const Float &>();
}

bool Float::greater(val v)
{
	if (v.isNumber())
		return *this > v.as<double>();
	else
		return *this > v.as<const Float &>();
}

bool Float::greater_equal(val v) { return !less(v); }
bool Float::less_equal(val v) { return !greater(v); }
bool Float::equal(val v)
{
	if (v.isNumber())
		return *this == v.as<double>();
	else
		return *this == v.as<const Float &>();
}
bool Float::not_equal(val v) { return !equal(v); }

// arithmetical operators + - * / and more
Float::builder_pattern Float::operator+=(const Float &op) { mpfr_add(&wrapped, &wrapped, &op.wrapped, rounding); }
Float::builder_pattern Float::operator+=(double op) { mpfr_add_d(&wrapped, &wrapped, op, rounding); }

Float::builder_pattern Float::operator-=(const Float &op) { mpfr_sub(&wrapped, &wrapped, &op.wrapped, rounding); }
Float::builder_pattern Float::operator-=(double op) { mpfr_sub_d(&wrapped, &wrapped, op, rounding); }

Float::builder_pattern Float::operator*=(const Float &op) { mpfr_mul(&wrapped, &wrapped, &op.wrapped, rounding); }
Float::builder_pattern Float::operator*=(double op) { mpfr_mul_d(&wrapped, &wrapped, op, rounding); }

Float::builder_pattern Float::operator/=(const Float &op) { mpfr_div(&wrapped, &wrapped, &op.wrapped, rounding); }
Float::builder_pattern Float::operator/=(double op) { mpfr_div_d(&wrapped, &wrapped, op, rounding); }

Float::builder_pattern Float::add(val v) { Float::op_add(*this, *this, v); }
Float::builder_pattern Float::sub(val v) { Float::op_sub(*this, *this, v); }
Float::builder_pattern Float::mul(val v) { Float::op_mul(*this, *this, v); }
Float::builder_pattern Float::div(val v) { Float::op_div(*this, *this, v); }
Float::builder_pattern Float::sqrt() { Float::op_sqrt(*this, *this); }
Float::builder_pattern Float::rec_sqrt() { Float::op_rec_sqrt(*this, *this); }
Float::builder_pattern Float::cbrt() { Float::op_cbrt(*this, *this); }
Float::builder_pattern Float::root_ui(unsigned n) { Float::op_root_ui(*this, *this, n); }
Float::builder_pattern Float::neg() { Float::op_neg(*this, *this); }
Float::builder_pattern Float::abs() { Float::op_abs(*this, *this); }
Float::builder_pattern Float::dim(const Float &op) { Float::op_dim(*this, *this, op); }
Float::builder_pattern Float::fma(const Float &a, const Float &b) { Float::op_fma(*this, *this, a, b); }
Float::builder_pattern Float::fms(const Float &a, const Float &b) { Float::op_fms(*this, *this, a, b); }
Float::builder_pattern Float::fmma(const Float &a, const Float &b, const Float &c) { Float::op_fmma(*this, *this, a, b, c); }
Float::builder_pattern Float::fmms(const Float &a, const Float &b, const Float &c) { Float::op_fmms(*this, *this, a, b, c); }
Float::builder_pattern Float::swap(Float &op) { Float::op_swap(*this, op); }

Float::builder_pattern Float::rint() { Float::op_rint(*this, *this); };
Float::builder_pattern Float::ceil() { Float::op_ceil(*this, *this); };
Float::builder_pattern Float::floor() { Float::op_floor(*this, *this); };
Float::builder_pattern Float::round() { Float::op_round(*this, *this); };
Float::builder_pattern Float::roundeven() { Float::op_roundeven(*this, *this); };
Float::builder_pattern Float::trunc() { Float::op_trunc(*this, *this); };
Float::builder_pattern Float::rint_ceil() { Float::op_rint_ceil(*this, *this); };
Float::builder_pattern Float::rint_floor() { Float::op_rint_floor(*this, *this); };
Float::builder_pattern Float::rint_round() { Float::op_rint_round(*this, *this); };
Float::builder_pattern Float::rint_roundeven() { Float::op_rint_roundeven(*this, *this); };
Float::builder_pattern Float::rint_trunc() { Float::op_rint_trunc(*this, *this); };

Float::builder_pattern Float::log() { Float::op_log(*this, *this); }
Float::builder_pattern Float::log2() { Float::op_log2(*this, *this); }
Float::builder_pattern Float::log10() { Float::op_log10(*this, *this); }
Float::builder_pattern Float::log1p() { Float::op_log1p(*this, *this); }
Float::builder_pattern Float::log_ui(unsigned long op) { Float::op_log_ui(*this, op); }
Float::builder_pattern Float::exp() { Float::op_exp(*this, *this); }
Float::builder_pattern Float::exp2() { Float::op_exp2(*this, *this); }
Float::builder_pattern Float::exp10() { Float::op_exp10(*this, *this); }
Float::builder_pattern Float::expm1() { Float::op_expm1(*this, *this); }
Float::builder_pattern Float::pow(val v)
{
	if (v.isNumber())
	{
		double d = v.as<double>();
		if (d - (double)(long)d == 0.0)
			Float::op_pow_si(*this, *this, (long)d);
		else
			std::cerr << "error: floating numbers are prohibited in the pow function, only integers and Float objects are tolerated" << std::endl;
	}
	else
		Float::op_pow(*this, *this, v.as<const Float &>());
}
Float::builder_pattern Float::expow(val v)
{
	if (v.isNumber())
	{
		double d = v.as<double>();
		if (d < 0)
			std::cerr << "error: negative integers are prohibited in the expow function" << std::endl;
		else if (d - (double)(long)d == 0.0)
			Float::op_ui_pow(*this, (long)d, *this);
		else
			std::cerr << "error: floating numbers are prohibited in the expow function, only integers and Float objects are tolerated" << std::endl;
	}
	else
		Float::op_pow(*this, v.as<const Float &>(), *this);
}
Float::builder_pattern Float::pow_si(long op2) { Float::op_pow_si(*this, *this, op2); }
Float::builder_pattern Float::ui_pow(unsigned long op1) { Float::op_ui_pow(*this, op1, *this); }
Float::builder_pattern Float::ui_pow_ui(unsigned long op1, unsigned long op2) { Float::op_ui_pow_ui(*this, op1, op2); }
Float::builder_pattern Float::cos() { Float::op_cos(*this, *this); }
Float::builder_pattern Float::sin() { Float::op_sin(*this, *this); }
Float::builder_pattern Float::tan() { Float::op_tan(*this, *this); }
Float::builder_pattern Float::sec() { Float::op_sec(*this, *this); }
Float::builder_pattern Float::csc() { Float::op_csc(*this, *this); }
Float::builder_pattern Float::cot() { Float::op_cot(*this, *this); }
Float::builder_pattern Float::acos() { Float::op_acos(*this, *this); }
Float::builder_pattern Float::asin() { Float::op_asin(*this, *this); }
Float::builder_pattern Float::atan() { Float::op_atan(*this, *this); }
Float::builder_pattern Float::atan2(const Float &y, const Float &x) { Float::op_atan2(*this, y, x); }
Float::builder_pattern Float::cosh() { Float::op_cosh(*this, *this); }
Float::builder_pattern Float::sinh() { Float::op_sinh(*this, *this); }
Float::builder_pattern Float::tanh() { Float::op_tanh(*this, *this); }
Float::builder_pattern Float::sech() { Float::op_sech(*this, *this); }
Float::builder_pattern Float::csch() { Float::op_csch(*this, *this); }
Float::builder_pattern Float::coth() { Float::op_coth(*this, *this); }
Float::builder_pattern Float::acosh() { Float::op_acosh(*this, *this); }
Float::builder_pattern Float::asinh() { Float::op_asinh(*this, *this); }
Float::builder_pattern Float::atanh() { Float::op_atanh(*this, *this); }
Float::builder_pattern Float::eint() { Float::op_eint(*this, *this); }
Float::builder_pattern Float::li2() { Float::op_li2(*this, *this); }
Float::builder_pattern Float::gamma() { Float::op_gamma(*this, *this); }
Float::builder_pattern Float::gamma_inc(const Float &op2) { Float::op_gamma_inc(*this, *this, op2); }
Float::builder_pattern Float::lngamma() { Float::op_lngamma(*this, *this); }
Float::builder_pattern Float::lgamma(val signp) { Float::op_lgamma(*this, signp, *this); }
Float::builder_pattern Float::digamma() { Float::op_digamma(*this, *this); }
Float::builder_pattern Float::beta(const Float &op2) { Float::op_beta(*this, *this, op2); }
Float::builder_pattern Float::zeta() { Float::op_zeta(*this, *this); }
Float::builder_pattern Float::zeta_ui(unsigned long op) { Float::op_zeta_ui(*this, op); }
Float::builder_pattern Float::erf() { Float::op_erf(*this, *this); }
Float::builder_pattern Float::erfc() { Float::op_erfc(*this, *this); }
Float::builder_pattern Float::j0() { Float::op_j0(*this, *this); }
Float::builder_pattern Float::j1() { Float::op_j1(*this, *this); }
Float::builder_pattern Float::jn(long n) { Float::op_jn(*this, n, *this); }
Float::builder_pattern Float::y0() { Float::op_y0(*this, *this); }
Float::builder_pattern Float::y1() { Float::op_y1(*this, *this); }
Float::builder_pattern Float::yn(long n) { Float::op_yn(*this, n, *this); }
Float::builder_pattern Float::agm(const Float &op2) { Float::op_agm(*this, *this, op2); }
Float::builder_pattern Float::ai(const Float &x) { Float::op_ai(*this, *this); }
Float::builder_pattern Float::const_log2() { Float::op_const_log2(*this); }
Float::builder_pattern Float::const_pi() { Float::op_const_pi(*this); }
Float::builder_pattern Float::const_euler() { Float::op_const_euler(*this); }
Float::builder_pattern Float::const_catalan() { Float::op_const_catalan(*this); }
// MEMBERS

int Float::op_add(Float &out, const Float &a, val v)
{
	if (v.isNumber())
		return mpfr_add_d(&out.wrapped, &a.wrapped, v.as<double>(), out.rounding);
	else
		return mpfr_add(&out.wrapped, &a.wrapped, &v.as<const Float &>().wrapped, out.rounding);
}

int Float::op_sub(Float &out, const Float &a, val v)
{
	if (v.isNumber())
		return mpfr_sub_d(&out.wrapped, &a.wrapped, v.as<double>(), out.rounding);
	else
		return mpfr_sub(&out.wrapped, &a.wrapped, &v.as<const Float &>().wrapped, out.rounding);
}

int Float::op_mul(Float &out, const Float &a, val v)
{
	if (v.isNumber())
		return mpfr_mul_d(&out.wrapped, &a.wrapped, v.as<double>(), out.rounding);
	else
		return mpfr_mul(&out.wrapped, &a.wrapped, &v.as<const Float &>().wrapped, out.rounding);
}

int Float::op_div(Float &out, const Float &a, val v)
{
	if (v.isNumber())
		return mpfr_div_d(&out.wrapped, &a.wrapped, v.as<double>(), out.rounding);
	else
		return mpfr_div(&out.wrapped, &a.wrapped, &v.as<const Float &>().wrapped, out.rounding);
}

int Float::op_sqrt(Float &out, const Float &src)
{
	return mpfr_sqrt(&out.wrapped, &src.wrapped, out.rounding);
}

int Float::op_rec_sqrt(Float &out, const Float &src)
{
	return mpfr_rec_sqrt(&out.wrapped, &src.wrapped, out.rounding);
}

int Float::op_cbrt(Float &out, const Float &src)
{
	return mpfr_cbrt(&out.wrapped, &src.wrapped, out.rounding);
}

int Float::op_root_ui(Float &out, const Float &src, unsigned n)
{
	return mpfr_rootn_ui(&out.wrapped, &src.wrapped, n, out.rounding);
}

int Float::op_neg(Float &out, const Float &src)
{
	return mpfr_neg(&out.wrapped, &src.wrapped, out.rounding);
}

int Float::op_abs(Float &out, const Float &src)
{
	return mpfr_abs(&out.wrapped, &src.wrapped, out.rounding);
}

int Float::op_dim(Float &out, const Float &src, const Float &op)
{
	return mpfr_dim(&out.wrapped, &src.wrapped, &op.wrapped, out.rounding);
}

int Float::op_fma(Float &out, const Float &src, const Float &a, const Float &b)
{
	return mpfr_fma(&out.wrapped, &src.wrapped, &a.wrapped, &b.wrapped, out.rounding);
}

int Float::op_fms(Float &out, const Float &src, const Float &a, const Float &b)
{
	return mpfr_fms(&out.wrapped, &src.wrapped, &a.wrapped, &b.wrapped, out.rounding);
}

int Float::op_fmma(Float &out, const Float &src, const Float &a, const Float &b, const Float &c)
{
	return mpfr_fmma(&out.wrapped, &src.wrapped, &a.wrapped, &b.wrapped, &c.wrapped, out.rounding);
}

int Float::op_fmms(Float &out, const Float &src, const Float &a, const Float &b, const Float &c)
{
	return mpfr_fmms(&out.wrapped, &src.wrapped, &a.wrapped, &b.wrapped, &c.wrapped, out.rounding);
}

void Float::op_swap(Float &a, Float &b)
{
	mpfr_swap(&a.wrapped, &b.wrapped);
	std::swap(a.rounding, b.rounding);
}

int Float::op_hypot(Float &out, const Float &x, const Float &y)
{
	return mpfr_hypot(&out.wrapped, &x.wrapped, &y.wrapped, out.rounding);
}

int Float::op_sum(Float &out, val array)
{
	int length = array["length"].as<int>();
	mpfr_ptr v[length];
	jsArrayToMpfrArray(array, v, length);
	return mpfr_sum(&out.wrapped, v, length, out.rounding);
}

int Float::op_dot(Float &out, val a, val b)
{
	int alength = a["length"].as<int>();
	int blength = b["length"].as<int>();
	if (alength != blength)
	{
		std::cerr << "error: dot product of array of different size" << std::endl;
		return 0;
	}
	mpfr_ptr aa[alength];
	mpfr_ptr bb[blength];
	jsArrayToMpfrArray(a, aa, alength);
	jsArrayToMpfrArray(b, bb, blength);
	return mpfr_dot(&out.wrapped, aa, bb, alength, out.rounding);
}

int Float::op_fac(Float &out, unsigned n)
{
	return mpfr_fac_ui(&out.wrapped, n, out.rounding);
}

// integer & remainders

int Float::op_rint(Float &out, const Float &op) { return mpfr_rint(&out.wrapped, &op.wrapped, out.rounding); };
int Float::op_ceil(Float &out, const Float &op) { return mpfr_ceil(&out.wrapped, &op.wrapped); };
int Float::op_floor(Float &out, const Float &op) { return mpfr_floor(&out.wrapped, &op.wrapped); };
int Float::op_round(Float &out, const Float &op) { return mpfr_round(&out.wrapped, &op.wrapped); };
int Float::op_roundeven(Float &out, const Float &op) { return mpfr_roundeven(&out.wrapped, &op.wrapped); };
int Float::op_trunc(Float &out, const Float &op) { return mpfr_trunc(&out.wrapped, &op.wrapped); };
int Float::op_rint_ceil(Float &out, const Float &op) { return mpfr_rint_ceil(&out.wrapped, &op.wrapped, out.rounding); };
int Float::op_rint_floor(Float &out, const Float &op) { return mpfr_rint_floor(&out.wrapped, &op.wrapped, out.rounding); };
int Float::op_rint_round(Float &out, const Float &op) { return mpfr_rint_round(&out.wrapped, &op.wrapped, out.rounding); };
int Float::op_rint_roundeven(Float &out, const Float &op) { return mpfr_rint_roundeven(&out.wrapped, &op.wrapped, out.rounding); };
int Float::op_rint_trunc(Float &out, const Float &op) { return mpfr_rint_trunc(&out.wrapped, &op.wrapped, out.rounding); };

// TRANSCENDENTAL

int Float::op_log(Float &out, const Float &op)
{
	return mpfr_log(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_log_ui(Float &out, unsigned long op)
{
	return mpfr_log_ui(&out.wrapped, op, out.rounding);
};

int Float::op_log2(Float &out, const Float &op)
{
	return mpfr_log2(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_log10(Float &out, const Float &op)
{
	return mpfr_log10(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_log1p(Float &out, const Float &op)
{
	return mpfr_log1p(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_exp(Float &out, const Float &op)
{
	return mpfr_exp(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_exp2(Float &out, const Float &op)
{
	return mpfr_exp2(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_exp10(Float &out, const Float &op)
{
	return mpfr_exp10(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_expm1(Float &out, const Float &op)
{
	return mpfr_expm1(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_pow(Float &out, const Float &op1, const Float &op2)
{
	return mpfr_pow(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding);
};

int Float::op_pow_si(Float &out, const Float &op1, long op2)
{
	return mpfr_pow_si(&out.wrapped, &op1.wrapped, op2, out.rounding);
};

int Float::op_ui_pow_ui(Float &out, unsigned long op1, unsigned long op2)
{
	return mpfr_ui_pow_ui(&out.wrapped, op1, op2, out.rounding);
};

int Float::op_ui_pow(Float &out, unsigned long op1, const Float &op2)
{
	return mpfr_ui_pow(&out.wrapped, op1, &op2.wrapped, out.rounding);
};

int Float::op_cos(Float &out, const Float &op)
{
	return mpfr_cos(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_sin(Float &out, const Float &op)
{
	return mpfr_sin(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_tan(Float &out, const Float &op)
{
	return mpfr_tan(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_sin_cos(Float &sop, Float &cop, const Float &op)
{
	return mpfr_sin_cos(&sop.wrapped, &cop.wrapped, &op.wrapped, sop.rounding);
};

int Float::op_sec(Float &out, const Float &op)
{
	return mpfr_sec(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_csc(Float &out, const Float &op)
{
	return mpfr_csc(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_cot(Float &out, const Float &op)
{
	return mpfr_cot(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_acos(Float &out, const Float &op)
{
	return mpfr_acos(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_asin(Float &out, const Float &op)
{
	return mpfr_asin(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_atan(Float &out, const Float &op)
{
	return mpfr_atan(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_atan2(Float &out, const Float &y, const Float &x)
{
	return mpfr_atan2(&out.wrapped, &y.wrapped, &x.wrapped, out.rounding);
};

int Float::op_cosh(Float &out, const Float &op)
{
	return mpfr_cosh(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_sinh(Float &out, const Float &op)
{
	return mpfr_sinh(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_tanh(Float &out, const Float &op)
{
	return mpfr_tanh(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_sinh_cosh(Float &sop, Float &cop, const Float &op)
{
	return mpfr_sinh_cosh(&sop.wrapped, &cop.wrapped, &op.wrapped, sop.rounding);
};

int Float::op_sech(Float &out, const Float &op)
{
	return mpfr_sech(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_csch(Float &out, const Float &op)
{
	return mpfr_csch(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_coth(Float &out, const Float &op)
{
	return mpfr_coth(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_acosh(Float &out, const Float &op)
{
	return mpfr_acosh(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_asinh(Float &out, const Float &op)
{
	return mpfr_asinh(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_atanh(Float &out, const Float &op)
{
	return mpfr_atanh(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_eint(Float &out, const Float &op)
{
	return mpfr_eint(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_li2(Float &out, const Float &op)
{
	return mpfr_li2(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_gamma(Float &out, const Float &op)
{
	return mpfr_gamma(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_gamma_inc(Float &out, const Float &op, const Float &op2)
{
	return mpfr_gamma_inc(&out.wrapped, &op.wrapped, &op2.wrapped, out.rounding);
};

int Float::op_lngamma(Float &out, const Float &op)
{
	return mpfr_lngamma(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_lgamma(Float &out, val signp, const Float &op)
{
	int _signp;
	int *addr;

	if (signp.isNull() || signp.isFalse() || signp.isUndefined())
		addr = 0;
	else
		addr = &_signp;
	int t = mpfr_lgamma(&out.wrapped, addr, &op.wrapped, out.rounding);
	if (addr)
		signp["signp"] = val(_signp);
	return t;
};

int Float::op_digamma(Float &out, const Float &op)
{
	return mpfr_digamma(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_beta(Float &out, const Float &op1, const Float &op2)
{
	return mpfr_beta(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding);
};

int Float::op_zeta(Float &out, const Float &op)
{
	return mpfr_zeta(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_zeta_ui(Float &out, unsigned long op)
{
	return mpfr_zeta_ui(&out.wrapped, op, out.rounding);
};

int Float::op_erf(Float &out, const Float &op)
{
	return mpfr_erf(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_erfc(Float &out, const Float &op)
{
	return mpfr_erfc(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_j0(Float &out, const Float &op)
{
	return mpfr_j0(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_j1(Float &out, const Float &op)
{
	return mpfr_j1(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_jn(Float &out, long n, const Float &op)
{
	return mpfr_jn(&out.wrapped, n, &op.wrapped, out.rounding);
};

int Float::op_y0(Float &out, const Float &op)
{
	return mpfr_y0(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_y1(Float &out, const Float &op)
{
	return mpfr_y1(&out.wrapped, &op.wrapped, out.rounding);
};

int Float::op_yn(Float &out, long n, const Float &op)
{
	return mpfr_yn(&out.wrapped, n, &op.wrapped, out.rounding);
};

int Float::op_agm(Float &out, const Float &op1, const Float &op2)
{
	return mpfr_agm(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding);
};

int Float::op_ai(Float &out, const Float &x)
{
	return mpfr_ai(&out.wrapped, &x.wrapped, out.rounding);
};

int Float::op_const_log2(Float &out)
{
	return mpfr_const_log2(&out.wrapped, out.rounding);
};

int Float::op_const_pi(Float &out)
{
	return mpfr_const_pi(&out.wrapped, out.rounding);
};

int Float::op_const_euler(Float &out)
{
	return mpfr_const_euler(&out.wrapped, out.rounding);
};

int Float::op_const_catalan(Float &out)
{
	return mpfr_const_catalan(&out.wrapped, out.rounding);
};

void Float::jsArrayToMpfrArray(val array, mpfr_ptr *out, int length)
{
	for (int i = 0; i < length; i++)
		out[i] = &array[i].as<Float &>().wrapped;
};

std::string Float::op_get_version(){ return std::string(mpfr_get_version()); }
std::string Float::op_get_patches(){ return std::string(mpfr_get_patches()); }
std::string Float::op_buildopt_tune_case(){ return std::string(mpfr_buildopt_tune_case()); }
int Float::op_sqr(Float &out, const Float &op) { return mpfr_sqr(&out.wrapped, &op.wrapped, out.rounding); }
int Float::op_cmp(const Float &op1, const Float &op2) { return mpfr_cmp(&op1.wrapped, &op2.wrapped); }
int Float::op_cmp_ui(const Float &op1, unsigned long int op2) { return mpfr_cmp_ui(&op1.wrapped, op2); }
int Float::op_cmp_si(const Float &op1, long int op2) { return mpfr_cmp_si(&op1.wrapped, op2); }
int Float::op_cmp_d(const Float &op1, double op2) { return mpfr_cmp_d(&op1.wrapped, op2); }
int Float::op_cmp_ui_2exp(const Float &op1, unsigned long int op2, exp_t e) { return mpfr_cmp_ui_2exp(&op1.wrapped, op2, e); }
int Float::op_cmp_si_2exp(const Float &op1, long int op2, exp_t e) { return mpfr_cmp_si_2exp(&op1.wrapped, op2, e); }
int Float::op_cmpabs(const Float &op1, const Float &op2) { return mpfr_cmpabs(&op1.wrapped, &op2.wrapped); }
int Float::op_cmpabs_ui(const Float &op1, unsigned long op2) { return mpfr_cmpabs_ui(&op1.wrapped, op2); }
int Float::op_nan_p(const Float &op) { return mpfr_nan_p(&op.wrapped); }
int Float::op_inf_p(const Float &op) { return mpfr_inf_p(&op.wrapped); }
int Float::op_number_p(const Float &op) { return mpfr_number_p(&op.wrapped); }
int Float::op_zero_p(const Float &op) { return mpfr_zero_p(&op.wrapped); }
int Float::op_regular_p(const Float &op) { return mpfr_regular_p(&op.wrapped); }
int Float::op_sgn(const Float &op) { return mpfr_sgn(&op.wrapped); }
int Float::op_greater_p(const Float &op1, const Float &op2) { return mpfr_greater_p(&op1.wrapped, &op2.wrapped); }
int Float::op_greaterequal_p(const Float &op1, const Float &op2) { return mpfr_greaterequal_p(&op1.wrapped, &op2.wrapped); }
int Float::op_less_p(const Float &op1, const Float &op2) { return mpfr_less_p(&op1.wrapped, &op2.wrapped); }
int Float::op_lessequal_p(const Float &op1, const Float &op2) { return mpfr_lessequal_p(&op1.wrapped, &op2.wrapped); }
int Float::op_equal_p(const Float &op1, const Float &op2) { return mpfr_equal_p(&op1.wrapped, &op2.wrapped); }
int Float::op_lessgreater_p(const Float &op1, const Float &op2) { return mpfr_lessgreater_p(&op1.wrapped, &op2.wrapped); }
int Float::op_unordered_p(const Float &op1, const Float &op2) { return mpfr_unordered_p(&op1.wrapped, &op2.wrapped); }
int Float::op_total_order_p(Float &out, const Float &y) { return mpfr_total_order_p(&out.wrapped, &y.wrapped); }
int Float::op_frac(Float &out, const Float &op) { return mpfr_frac(&out.wrapped, &op.wrapped, out.rounding); }
int Float::op_modf(Float &iop, Float &fop, const Float &op) { return mpfr_modf(&iop.wrapped, &fop.wrapped, &op.wrapped, iop.rounding); }
int Float::op_fmod(Float &out, const Float &x, const Float &y) { return mpfr_fmod(&out.wrapped, &x.wrapped, &y.wrapped, out.rounding); }
int Float::op_fmodquo(Float &out, val q, const Float &x, const Float &y) { 
	long q_; 
	auto r = mpfr_fmodquo(&out.wrapped, &q_, &x.wrapped, &y.wrapped, out.rounding);
	if (!q.isUndefined() && !q.isNull())
		q.set("q", q_);
	return r;
}
int Float::op_remainder(Float &out, const Float &x, const Float &y) { return mpfr_remainder(&out.wrapped, &x.wrapped, &y.wrapped, out.rounding); }
int Float::op_remquo(Float &out, val q, const Float &x, const Float &y) { 
	long q_;
	auto r = mpfr_remquo(&out.wrapped, &q_, &x.wrapped, &y.wrapped, out.rounding);
	if (!q.isUndefined() && !q.isNull())
		q.set("q", q_);
	return r;
}
void Float::op_set_default_rounding_mode(int rnd) { return mpfr_set_default_rounding_mode((rnd_t)rnd); }
int Float::op_prec_round(Float &out, prec_t prec) { return mpfr_prec_round(&out.wrapped, prec, out.rounding); }
int Float::op_can_round(const Float &b, exp_t err, int rnd1, int rnd2, prec_t prec) { return mpfr_can_round(&b.wrapped, err, (rnd_t)rnd1, (rnd_t)rnd2, prec); }
Float::prec_t Float::op_min_prec(Float &out) { return mpfr_min_prec(&out.wrapped); }
void Float::op_nexttoward(Float &out, const Float &y) { return mpfr_nexttoward(&out.wrapped, &y.wrapped); }
void Float::op_nextabove(Float &out) { return mpfr_nextabove(&out.wrapped); }
void Float::op_nextbelow(Float &out) { return mpfr_nextbelow(&out.wrapped); }
int Float::op_min(Float &out, const Float &op1, const Float &op2) { return mpfr_min(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding); }
int Float::op_max(Float &out, const Float &op1, const Float &op2) { return mpfr_max(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding); }
int Float::op_nrandom(Float &out1, gmp_randstate_t state) { return mpfr_nrandom(&out1.wrapped, state, out1.rounding); }
int Float::op_grandom(Float &out1, Float &out2, gmp_randstate_t state) { return mpfr_grandom(&out1.wrapped, &out2.wrapped, state, out1.rounding); }
int Float::op_erandom(Float &out1, gmp_randstate_t state) { return mpfr_erandom(&out1.wrapped, state, out1.rounding); }
Float::exp_t Float::op_get_exp(Float &out) { return mpfr_get_exp(&out.wrapped); }
int Float::op_set_exp(Float &out, exp_t e) { return mpfr_set_exp(&out.wrapped, e); }
int Float::op_signbit(const Float &op) { return mpfr_signbit(&op.wrapped); }
int Float::op_setsign(Float &out, const Float &op, int s) { return mpfr_setsign(&out.wrapped, &op.wrapped, s, out.rounding); }
int Float::op_copysign(Float &out, const Float &op1, const Float &op2) { return mpfr_copysign(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding); }
int Float::op_set_emin(exp_t exp) { return mpfr_set_emin(exp); }
int Float::op_set_emax(exp_t exp) { return mpfr_set_emax(exp); }
int Float::op_check_range(Float &out, int t) { return mpfr_check_range(&out.wrapped, t, out.rounding); }
int Float::op_subnormalize(Float &out, int t) { return mpfr_subnormalize(&out.wrapped, t, out.rounding); }
void Float::op_flags_clear(flags_t mask) { return mpfr_flags_clear(mask); }
void Float::op_flags_set(flags_t mask) { return mpfr_flags_set(mask); }
Float::flags_t Float::op_flags_test(flags_t mask) { return mpfr_flags_test(mask); }
void Float::op_flags_restore(flags_t flags, flags_t mask) { return mpfr_flags_restore(flags, mask); }