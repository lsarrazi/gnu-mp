#include <mpfr.h>
#include <iostream>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <sstream>

using namespace emscripten;

#include "Float.hpp"
#include "utils.hpp"

GnuMPFloat::GnuMPFloat(val v)
{
	if (v.isNumber())
		mpfr_init2(&wrapped, v.as<size_t>());
	else
	{
		const GnuMPFloat &op = v.as<const GnuMPFloat &>();
		mpfr_init2(&wrapped, op.getPrecision());
		mpfr_set(&wrapped, &op.wrapped, rounding);
	}
}

GnuMPFloat::GnuMPFloat(prec_t prec)
{
	mpfr_init2(&wrapped, prec);
}

GnuMPFloat::GnuMPFloat(double v, prec_t prec) : GnuMPFloat(prec)
{
	*this = v;
}

GnuMPFloat::GnuMPFloat(const GnuMPFloat& op)
{
	std::cout << "alloc copy" << std::endl;
	mpfr_init2(&wrapped, op.getPrecision());
	mpfr_set(&wrapped, &op.wrapped, rounding);
}

GnuMPFloat::GnuMPFloat(const FloatRegister& op) : GnuMPFloat((const GnuMPFloat&)op)
{
	FloatRegister::release(op);
}

GnuMPFloat::GnuMPFloat(const char* v, prec_t prec, int base) : GnuMPFloat(prec)
{
	mpfr_set_str(&wrapped, v, base, rounding);
}

GnuMPFloat& GnuMPFloat::operator=(const GnuMPFloat& op)
{
	mpfr_set(&wrapped, &op.wrapped, rounding);
	return *this;
}

GnuMPFloat& GnuMPFloat::operator=(const char * str)
{
	mpfr_set_str(&wrapped, str, 10, rounding);
	return *this;
}

GnuMPFloat& GnuMPFloat::operator=(double v)
{
	mpfr_set_d(&wrapped, v, rounding);
	return *this;
}

GnuMPFloat& GnuMPFloat::operator=(FloatRegister& r)
{
	if (r.getPrecision() == getPrecision())
		swap(reinterpret_cast<GnuMPFloat&>(r));
	else
		*this = reinterpret_cast<GnuMPFloat&>(r);
	FloatRegister::release(r);
	return *this;
}

GnuMPFloat::operator int(){ return mpfr_get_si(&wrapped, MPFR_RNDZ); }
GnuMPFloat::operator long(){ return mpfr_get_si(&wrapped, MPFR_RNDZ); }
GnuMPFloat::operator unsigned(){ return mpfr_get_ui(&wrapped, MPFR_RNDZ); }
GnuMPFloat::operator unsigned long(){ return mpfr_get_ui(&wrapped, MPFR_RNDZ); }
GnuMPFloat::operator double(){ return mpfr_get_d(&wrapped, MPFR_RNDN); }

GnuMPFloat::~GnuMPFloat() { mpfr_clear(&wrapped); }

// rounding(mode)
int GnuMPFloat::getRounding() const { return rounding; }
const std::string &GnuMPFloat::getRoundingString()
{
	return RoundingStrings[rounding];
}

GnuMPFloat::builder_pattern GnuMPFloat::setRounding(int mode)
{
	rounding = static_cast<rnd_t>(mode);
}

GnuMPFloat::prec_t GnuMPFloat::getDefaultPrecision() { return mpfr_get_default_prec(); };
void GnuMPFloat::setDefaultPrecision(prec_t prec) { mpfr_set_default_prec(prec); };

// precision
GnuMPFloat::size_t GnuMPFloat::getPrecision() const { return mpfr_get_prec(&wrapped); };
GnuMPFloat::builder_pattern GnuMPFloat::setPrecision(size_t precision)
{
	mpfr_set_prec(&wrapped, precision);
}

// exponent
GnuMPFloat::exp_t GnuMPFloat::getExponent() const { return mpfr_get_exp(&wrapped); };
GnuMPFloat::builder_pattern GnuMPFloat::setExponent(exp_t exponent)
{
	mpfr_set_exp(&wrapped, exponent);
}

// sign
GnuMPFloat::sign_t GnuMPFloat::getSign() const { return MPFR_SIGN(&wrapped); }
GnuMPFloat::builder_pattern GnuMPFloat::setSign(sign_t sign)
{
	MPFR_SIGN(&wrapped) = sign;
}

// mantissa
GnuMPFloat::size_t GnuMPFloat::getMantissaSize() const
{
	return mpfr_custom_get_size(getPrecision());
}
val GnuMPFloat::getMantissaView()
{
	return val(typed_memory_view(getMantissaSize() / sizeof(mp_limb_t), wrapped._mpfr_d));
}

// set(string str, base = 10)
GnuMPFloat::builder_pattern GnuMPFloat::setString(const std::string &str) { return setString(str, 10); }
GnuMPFloat::builder_pattern GnuMPFloat::setString(const std::string &str, int base)
{
	mpfr_set_str(&wrapped, str.c_str(), base, rounding);
}
GnuMPFloat::builder_pattern GnuMPFloat::setDouble(double x)
{
	mpfr_set_d(&wrapped, x, rounding);
}
GnuMPFloat::builder_pattern GnuMPFloat::setFloat(const GnuMPFloat &tocopy)
{
	mpfr_set(&wrapped, &(tocopy).wrapped, rounding);
}
GnuMPFloat::builder_pattern GnuMPFloat::set(val v)
{
	if (v.isNumber())
		return setDouble(v.as<double>());
	else if (v.isString())
		return setString(v.as<std::string>());
	else
		return setFloat(v.as<const GnuMPFloat &>());
}

// toString(int base = 10, int n = 0)
std::string GnuMPFloat::toString(int base) const { return toString(base, 0); }
std::string GnuMPFloat::toString() const { return toString(10, 0); }
std::string GnuMPFloat::toString(int base, int n) const
{
	std::stringstream ss;
	mpfr_exp_t exp;
	char *str = mpfr_get_str(nullptr, &exp, base, n, &wrapped, rounding);
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
double GnuMPFloat::toNumber() const
{
	return mpfr_get_d(&wrapped, rounding);
}

bool GnuMPFloat::isInteger() const
{
	return !!mpfr_integer_p(&wrapped);
}

// operator < <= > >= == !=
bool GnuMPFloat::operator<(const GnuMPFloat &op) const
{
	return mpfr_cmp(&wrapped, &op.wrapped) < 0;
}
bool GnuMPFloat::operator==(const GnuMPFloat &op) const
{
	return mpfr_cmp(&wrapped, &op.wrapped) == 0;
}
bool GnuMPFloat::operator<=(const GnuMPFloat &op) const { return !(op < *this); }
bool GnuMPFloat::operator>(const GnuMPFloat &op) const { return op < *this; }
bool GnuMPFloat::operator>=(const GnuMPFloat &op) const { return !(*this < op); }
bool GnuMPFloat::operator!=(const GnuMPFloat &op) const { return !(*this == op); }

bool GnuMPFloat::operator<(double op) const
{
	return mpfr_cmp_d(&wrapped, op) < 0;
}
bool GnuMPFloat::operator>(double op) const
{
	return mpfr_cmp_d(&wrapped, op) > 0;
}
bool GnuMPFloat::operator==(double op) const
{
	return mpfr_cmp_d(&wrapped, op) == 0;
}
bool GnuMPFloat::operator<=(double op) const { return !(*this > op); }
bool GnuMPFloat::operator>=(double op) const { return !(*this < op); }
bool GnuMPFloat::operator!=(double op) const { return !(*this == op); }

bool GnuMPFloat::less(val v)
{
	if (v.isNumber())
		return *this < v.as<double>();
	else
		return *this < v.as<const GnuMPFloat &>();
}

bool GnuMPFloat::greater(val v)
{
	if (v.isNumber())
		return *this > v.as<double>();
	else
		return *this > v.as<const GnuMPFloat &>();
}

bool GnuMPFloat::greater_equal(val v) { return !less(v); }
bool GnuMPFloat::less_equal(val v) { return !greater(v); }
bool GnuMPFloat::equal(val v)
{
	if (v.isNumber())
		return *this == v.as<double>();
	else
		return *this == v.as<const GnuMPFloat &>();
}
bool GnuMPFloat::not_equal(val v) { return !equal(v); }

// arithmetical operators + - * / and more
GnuMPFloat::builder_pattern GnuMPFloat::operator+=(const GnuMPFloat &op) { mpfr_add(&wrapped, &wrapped, &op.wrapped, rounding); }
GnuMPFloat::builder_pattern GnuMPFloat::operator+=(double op) { mpfr_add_d(&wrapped, &wrapped, op, rounding); }

GnuMPFloat::builder_pattern GnuMPFloat::operator-=(const GnuMPFloat &op) { mpfr_sub(&wrapped, &wrapped, &op.wrapped, rounding); }
GnuMPFloat::builder_pattern GnuMPFloat::operator-=(double op) { mpfr_sub_d(&wrapped, &wrapped, op, rounding); }

GnuMPFloat::builder_pattern GnuMPFloat::operator*=(const GnuMPFloat &op) { mpfr_mul(&wrapped, &wrapped, &op.wrapped, rounding); }
GnuMPFloat::builder_pattern GnuMPFloat::operator*=(double op) { mpfr_mul_d(&wrapped, &wrapped, op, rounding); }

GnuMPFloat::builder_pattern GnuMPFloat::operator/=(const GnuMPFloat &op) { mpfr_div(&wrapped, &wrapped, &op.wrapped, rounding); }
GnuMPFloat::builder_pattern GnuMPFloat::operator/=(double op) { mpfr_div_d(&wrapped, &wrapped, op, rounding); }

GnuMPFloat::builder_pattern GnuMPFloat::add(val v) { GnuMPFloat::op_add(*this, *this, v); }
GnuMPFloat::builder_pattern GnuMPFloat::sub(val v) { GnuMPFloat::op_sub(*this, *this, v); }
GnuMPFloat::builder_pattern GnuMPFloat::mul(val v) { GnuMPFloat::op_mul(*this, *this, v); }
GnuMPFloat::builder_pattern GnuMPFloat::div(val v) { GnuMPFloat::op_div(*this, *this, v); }
GnuMPFloat::builder_pattern GnuMPFloat::sqrt() { GnuMPFloat::op_sqrt(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::rec_sqrt() { GnuMPFloat::op_rec_sqrt(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::cbrt() { GnuMPFloat::op_cbrt(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::root_ui(unsigned n) { GnuMPFloat::op_root_ui(*this, *this, n); }
GnuMPFloat::builder_pattern GnuMPFloat::neg() { GnuMPFloat::op_neg(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::abs() { GnuMPFloat::op_abs(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::dim(const GnuMPFloat &op) { GnuMPFloat::op_dim(*this, *this, op); }
GnuMPFloat::builder_pattern GnuMPFloat::fma(const GnuMPFloat &a, const GnuMPFloat &b) { GnuMPFloat::op_fma(*this, *this, a, b); }
GnuMPFloat::builder_pattern GnuMPFloat::fms(const GnuMPFloat &a, const GnuMPFloat &b) { GnuMPFloat::op_fms(*this, *this, a, b); }
GnuMPFloat::builder_pattern GnuMPFloat::fmma(const GnuMPFloat &a, const GnuMPFloat &b, const GnuMPFloat &c) { GnuMPFloat::op_fmma(*this, *this, a, b, c); }
GnuMPFloat::builder_pattern GnuMPFloat::fmms(const GnuMPFloat &a, const GnuMPFloat &b, const GnuMPFloat &c) { GnuMPFloat::op_fmms(*this, *this, a, b, c); }
GnuMPFloat::builder_pattern GnuMPFloat::swap(GnuMPFloat &op) { GnuMPFloat::op_swap(*this, op); }

GnuMPFloat::builder_pattern GnuMPFloat::rint() { GnuMPFloat::op_rint(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::ceil() { GnuMPFloat::op_ceil(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::floor() { GnuMPFloat::op_floor(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::round() { GnuMPFloat::op_round(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::roundeven() { GnuMPFloat::op_roundeven(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::trunc() { GnuMPFloat::op_trunc(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::rint_ceil() { GnuMPFloat::op_rint_ceil(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::rint_floor() { GnuMPFloat::op_rint_floor(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::rint_round() { GnuMPFloat::op_rint_round(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::rint_roundeven() { GnuMPFloat::op_rint_roundeven(*this, *this); };
GnuMPFloat::builder_pattern GnuMPFloat::rint_trunc() { GnuMPFloat::op_rint_trunc(*this, *this); };

GnuMPFloat::builder_pattern GnuMPFloat::log() { GnuMPFloat::op_log(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::log2() { GnuMPFloat::op_log2(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::log10() { GnuMPFloat::op_log10(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::log1p() { GnuMPFloat::op_log1p(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::log_ui(unsigned long op) { GnuMPFloat::op_log_ui(*this, op); }
GnuMPFloat::builder_pattern GnuMPFloat::exp() { GnuMPFloat::op_exp(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::exp2() { GnuMPFloat::op_exp2(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::exp10() { GnuMPFloat::op_exp10(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::expm1() { GnuMPFloat::op_expm1(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::pow(val v)
{
	if (v.isNumber())
	{
		double d = v.as<double>();
		if (d - (double)(long)d == 0.0)
			GnuMPFloat::op_pow_si(*this, *this, (long)d);
		else
			std::cerr << "error: floating numbers are prohibited in the pow function, only integers and Float objects are tolerated" << std::endl;
	}
	else
		GnuMPFloat::op_pow(*this, *this, v.as<const GnuMPFloat &>());
}
GnuMPFloat::builder_pattern GnuMPFloat::expow(val v)
{
	if (v.isNumber())
	{
		double d = v.as<double>();
		if (d < 0)
			std::cerr << "error: negative integers are prohibited in the expow function" << std::endl;
		else if (d - (double)(long)d == 0.0)
			GnuMPFloat::op_ui_pow(*this, (long)d, *this);
		else
			std::cerr << "error: floating numbers are prohibited in the expow function, only integers and Float objects are tolerated" << std::endl;
	}
	else
		GnuMPFloat::op_pow(*this, v.as<const GnuMPFloat &>(), *this);
}
GnuMPFloat::builder_pattern GnuMPFloat::pow_si(long op2) { GnuMPFloat::op_pow_si(*this, *this, op2); }
GnuMPFloat::builder_pattern GnuMPFloat::ui_pow(unsigned long op1) { GnuMPFloat::op_ui_pow(*this, op1, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::ui_pow_ui(unsigned long op1, unsigned long op2) { GnuMPFloat::op_ui_pow_ui(*this, op1, op2); }
GnuMPFloat::builder_pattern GnuMPFloat::cos() { GnuMPFloat::op_cos(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::sin() { GnuMPFloat::op_sin(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::tan() { GnuMPFloat::op_tan(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::sec() { GnuMPFloat::op_sec(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::csc() { GnuMPFloat::op_csc(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::cot() { GnuMPFloat::op_cot(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::acos() { GnuMPFloat::op_acos(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::asin() { GnuMPFloat::op_asin(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::atan() { GnuMPFloat::op_atan(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::atan2(const GnuMPFloat &y, const GnuMPFloat &x) { GnuMPFloat::op_atan2(*this, y, x); }
GnuMPFloat::builder_pattern GnuMPFloat::cosh() { GnuMPFloat::op_cosh(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::sinh() { GnuMPFloat::op_sinh(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::tanh() { GnuMPFloat::op_tanh(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::sech() { GnuMPFloat::op_sech(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::csch() { GnuMPFloat::op_csch(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::coth() { GnuMPFloat::op_coth(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::acosh() { GnuMPFloat::op_acosh(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::asinh() { GnuMPFloat::op_asinh(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::atanh() { GnuMPFloat::op_atanh(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::eint() { GnuMPFloat::op_eint(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::li2() { GnuMPFloat::op_li2(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::gamma() { GnuMPFloat::op_gamma(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::gamma_inc(const GnuMPFloat &op2) { GnuMPFloat::op_gamma_inc(*this, *this, op2); }
GnuMPFloat::builder_pattern GnuMPFloat::lngamma() { GnuMPFloat::op_lngamma(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::lgamma(val signp) { GnuMPFloat::op_lgamma(*this, signp, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::digamma() { GnuMPFloat::op_digamma(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::beta(const GnuMPFloat &op2) { GnuMPFloat::op_beta(*this, *this, op2); }
GnuMPFloat::builder_pattern GnuMPFloat::zeta() { GnuMPFloat::op_zeta(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::zeta_ui(unsigned long op) { GnuMPFloat::op_zeta_ui(*this, op); }
GnuMPFloat::builder_pattern GnuMPFloat::erf() { GnuMPFloat::op_erf(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::erfc() { GnuMPFloat::op_erfc(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::j0() { GnuMPFloat::op_j0(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::j1() { GnuMPFloat::op_j1(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::jn(long n) { GnuMPFloat::op_jn(*this, n, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::y0() { GnuMPFloat::op_y0(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::y1() { GnuMPFloat::op_y1(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::yn(long n) { GnuMPFloat::op_yn(*this, n, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::agm(const GnuMPFloat &op2) { GnuMPFloat::op_agm(*this, *this, op2); }
GnuMPFloat::builder_pattern GnuMPFloat::ai(const GnuMPFloat &x) { GnuMPFloat::op_ai(*this, *this); }
GnuMPFloat::builder_pattern GnuMPFloat::const_log2() { GnuMPFloat::op_const_log2(*this); }
GnuMPFloat::builder_pattern GnuMPFloat::const_pi() { GnuMPFloat::op_const_pi(*this); }
GnuMPFloat::builder_pattern GnuMPFloat::const_euler() { GnuMPFloat::op_const_euler(*this); }
GnuMPFloat::builder_pattern GnuMPFloat::const_catalan() { GnuMPFloat::op_const_catalan(*this); }
// MEMBERS

int GnuMPFloat::op_add(GnuMPFloat &out, const GnuMPFloat &a, val v)
{
	if (v.isNumber())
		return mpfr_add_d(&out.wrapped, &a.wrapped, v.as<double>(), out.rounding);
	else
		return mpfr_add(&out.wrapped, &a.wrapped, &v.as<const GnuMPFloat &>().wrapped, out.rounding);
}

int GnuMPFloat::op_sub(GnuMPFloat &out, const GnuMPFloat &a, val v)
{
	if (v.isNumber())
		return mpfr_sub_d(&out.wrapped, &a.wrapped, v.as<double>(), out.rounding);
	else
		return mpfr_sub(&out.wrapped, &a.wrapped, &v.as<const GnuMPFloat &>().wrapped, out.rounding);
}

int GnuMPFloat::op_mul(GnuMPFloat &out, const GnuMPFloat &a, val v)
{
	if (v.isNumber())
		return mpfr_mul_d(&out.wrapped, &a.wrapped, v.as<double>(), out.rounding);
	else
		return mpfr_mul(&out.wrapped, &a.wrapped, &v.as<const GnuMPFloat &>().wrapped, out.rounding);
}

int GnuMPFloat::op_div(GnuMPFloat &out, const GnuMPFloat &a, val v)
{
	if (v.isNumber())
		return mpfr_div_d(&out.wrapped, &a.wrapped, v.as<double>(), out.rounding);
	else
		return mpfr_div(&out.wrapped, &a.wrapped, &v.as<const GnuMPFloat &>().wrapped, out.rounding);
}

int GnuMPFloat::op_sqrt(GnuMPFloat &out, const GnuMPFloat &src)
{
	return mpfr_sqrt(&out.wrapped, &src.wrapped, out.rounding);
}

int GnuMPFloat::op_rec_sqrt(GnuMPFloat &out, const GnuMPFloat &src)
{
	return mpfr_rec_sqrt(&out.wrapped, &src.wrapped, out.rounding);
}

int GnuMPFloat::op_cbrt(GnuMPFloat &out, const GnuMPFloat &src)
{
	return mpfr_cbrt(&out.wrapped, &src.wrapped, out.rounding);
}

int GnuMPFloat::op_root_ui(GnuMPFloat &out, const GnuMPFloat &src, unsigned n)
{
	return mpfr_rootn_ui(&out.wrapped, &src.wrapped, n, out.rounding);
}

int GnuMPFloat::op_neg(GnuMPFloat &out, const GnuMPFloat &src)
{
	return mpfr_neg(&out.wrapped, &src.wrapped, out.rounding);
}

int GnuMPFloat::op_abs(GnuMPFloat &out, const GnuMPFloat &src)
{
	return mpfr_abs(&out.wrapped, &src.wrapped, out.rounding);
}

int GnuMPFloat::op_dim(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &op)
{
	return mpfr_dim(&out.wrapped, &src.wrapped, &op.wrapped, out.rounding);
}

int GnuMPFloat::op_fma(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &a, const GnuMPFloat &b)
{
	return mpfr_fma(&out.wrapped, &src.wrapped, &a.wrapped, &b.wrapped, out.rounding);
}

int GnuMPFloat::op_fms(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &a, const GnuMPFloat &b)
{
	return mpfr_fms(&out.wrapped, &src.wrapped, &a.wrapped, &b.wrapped, out.rounding);
}

int GnuMPFloat::op_fmma(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &a, const GnuMPFloat &b, const GnuMPFloat &c)
{
	return mpfr_fmma(&out.wrapped, &src.wrapped, &a.wrapped, &b.wrapped, &c.wrapped, out.rounding);
}

int GnuMPFloat::op_fmms(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &a, const GnuMPFloat &b, const GnuMPFloat &c)
{
	return mpfr_fmms(&out.wrapped, &src.wrapped, &a.wrapped, &b.wrapped, &c.wrapped, out.rounding);
}

void GnuMPFloat::op_swap(GnuMPFloat &a, GnuMPFloat &b)
{
	mpfr_swap(&a.wrapped, &b.wrapped);
	std::swap(a.rounding, b.rounding);
}

int GnuMPFloat::op_hypot(GnuMPFloat &out, const GnuMPFloat &x, const GnuMPFloat &y)
{
	return mpfr_hypot(&out.wrapped, &x.wrapped, &y.wrapped, out.rounding);
}

int GnuMPFloat::op_sum(GnuMPFloat &out, val array)
{
	int length = array["length"].as<int>();
	mpfr_ptr v[length];
	jsArrayToMpfrArray(array, v, length);
	return mpfr_sum(&out.wrapped, v, length, out.rounding);
}

int GnuMPFloat::op_dot(GnuMPFloat &out, val a, val b)
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

int GnuMPFloat::op_fac(GnuMPFloat &out, unsigned n)
{
	return mpfr_fac_ui(&out.wrapped, n, out.rounding);
}

// integer & remainders

int GnuMPFloat::op_rint(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_rint(&out.wrapped, &op.wrapped, out.rounding); };
int GnuMPFloat::op_ceil(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_ceil(&out.wrapped, &op.wrapped); };
int GnuMPFloat::op_floor(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_floor(&out.wrapped, &op.wrapped); };
int GnuMPFloat::op_round(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_round(&out.wrapped, &op.wrapped); };
int GnuMPFloat::op_roundeven(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_roundeven(&out.wrapped, &op.wrapped); };
int GnuMPFloat::op_trunc(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_trunc(&out.wrapped, &op.wrapped); };
int GnuMPFloat::op_rint_ceil(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_rint_ceil(&out.wrapped, &op.wrapped, out.rounding); };
int GnuMPFloat::op_rint_floor(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_rint_floor(&out.wrapped, &op.wrapped, out.rounding); };
int GnuMPFloat::op_rint_round(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_rint_round(&out.wrapped, &op.wrapped, out.rounding); };
int GnuMPFloat::op_rint_roundeven(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_rint_roundeven(&out.wrapped, &op.wrapped, out.rounding); };
int GnuMPFloat::op_rint_trunc(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_rint_trunc(&out.wrapped, &op.wrapped, out.rounding); };

// TRANSCENDENTAL

int GnuMPFloat::op_log(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_log(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_log_ui(GnuMPFloat &out, unsigned long op)
{
	return mpfr_log_ui(&out.wrapped, op, out.rounding);
};

int GnuMPFloat::op_log2(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_log2(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_log10(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_log10(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_log1p(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_log1p(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_exp(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_exp(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_exp2(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_exp2(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_exp10(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_exp10(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_expm1(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_expm1(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_pow(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2)
{
	return mpfr_pow(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding);
};

int GnuMPFloat::op_pow_si(GnuMPFloat &out, const GnuMPFloat &op1, long op2)
{
	return mpfr_pow_si(&out.wrapped, &op1.wrapped, op2, out.rounding);
};

int GnuMPFloat::op_ui_pow_ui(GnuMPFloat &out, unsigned long op1, unsigned long op2)
{
	return mpfr_ui_pow_ui(&out.wrapped, op1, op2, out.rounding);
};

int GnuMPFloat::op_ui_pow(GnuMPFloat &out, unsigned long op1, const GnuMPFloat &op2)
{
	return mpfr_ui_pow(&out.wrapped, op1, &op2.wrapped, out.rounding);
};

int GnuMPFloat::op_cos(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_cos(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_sin(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_sin(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_tan(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_tan(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_sin_cos(GnuMPFloat &sop, GnuMPFloat &cop, const GnuMPFloat &op)
{
	return mpfr_sin_cos(&sop.wrapped, &cop.wrapped, &op.wrapped, sop.rounding);
};

int GnuMPFloat::op_sec(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_sec(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_csc(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_csc(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_cot(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_cot(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_acos(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_acos(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_asin(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_asin(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_atan(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_atan(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_atan2(GnuMPFloat &out, const GnuMPFloat &y, const GnuMPFloat &x)
{
	return mpfr_atan2(&out.wrapped, &y.wrapped, &x.wrapped, out.rounding);
};

int GnuMPFloat::op_cosh(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_cosh(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_sinh(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_sinh(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_tanh(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_tanh(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_sinh_cosh(GnuMPFloat &sop, GnuMPFloat &cop, const GnuMPFloat &op)
{
	return mpfr_sinh_cosh(&sop.wrapped, &cop.wrapped, &op.wrapped, sop.rounding);
};

int GnuMPFloat::op_sech(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_sech(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_csch(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_csch(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_coth(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_coth(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_acosh(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_acosh(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_asinh(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_asinh(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_atanh(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_atanh(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_eint(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_eint(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_li2(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_li2(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_gamma(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_gamma(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_gamma_inc(GnuMPFloat &out, const GnuMPFloat &op, const GnuMPFloat &op2)
{
	return mpfr_gamma_inc(&out.wrapped, &op.wrapped, &op2.wrapped, out.rounding);
};

int GnuMPFloat::op_lngamma(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_lngamma(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_lgamma(GnuMPFloat &out, val signp, const GnuMPFloat &op)
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

int GnuMPFloat::op_digamma(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_digamma(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_beta(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2)
{
	return mpfr_beta(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding);
};

int GnuMPFloat::op_zeta(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_zeta(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_zeta_ui(GnuMPFloat &out, unsigned long op)
{
	return mpfr_zeta_ui(&out.wrapped, op, out.rounding);
};

int GnuMPFloat::op_erf(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_erf(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_erfc(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_erfc(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_j0(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_j0(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_j1(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_j1(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_jn(GnuMPFloat &out, long n, const GnuMPFloat &op)
{
	return mpfr_jn(&out.wrapped, n, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_y0(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_y0(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_y1(GnuMPFloat &out, const GnuMPFloat &op)
{
	return mpfr_y1(&out.wrapped, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_yn(GnuMPFloat &out, long n, const GnuMPFloat &op)
{
	return mpfr_yn(&out.wrapped, n, &op.wrapped, out.rounding);
};

int GnuMPFloat::op_agm(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2)
{
	return mpfr_agm(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding);
};

int GnuMPFloat::op_ai(GnuMPFloat &out, const GnuMPFloat &x)
{
	return mpfr_ai(&out.wrapped, &x.wrapped, out.rounding);
};

int GnuMPFloat::op_const_log2(GnuMPFloat &out)
{
	return mpfr_const_log2(&out.wrapped, out.rounding);
};

int GnuMPFloat::op_const_pi(GnuMPFloat &out)
{
	return mpfr_const_pi(&out.wrapped, out.rounding);
};

int GnuMPFloat::op_const_euler(GnuMPFloat &out)
{
	return mpfr_const_euler(&out.wrapped, out.rounding);
};

int GnuMPFloat::op_const_catalan(GnuMPFloat &out)
{
	return mpfr_const_catalan(&out.wrapped, out.rounding);
};

void GnuMPFloat::jsArrayToMpfrArray(val array, mpfr_ptr *out, int length)
{
	for (int i = 0; i < length; i++)
		out[i] = &array[i].as<GnuMPFloat &>().wrapped;
};

std::string GnuMPFloat::op_get_version(){ return std::string(mpfr_get_version()); }
std::string GnuMPFloat::op_get_patches(){ return std::string(mpfr_get_patches()); }
std::string GnuMPFloat::op_buildopt_tune_case(){ return std::string(mpfr_buildopt_tune_case()); }
int GnuMPFloat::op_sqr(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_sqr(&out.wrapped, &op.wrapped, out.rounding); }
int GnuMPFloat::op_cmp(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_cmp(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_cmp_ui(const GnuMPFloat &op1, unsigned long int op2) { return mpfr_cmp_ui(&op1.wrapped, op2); }
int GnuMPFloat::op_cmp_si(const GnuMPFloat &op1, long int op2) { return mpfr_cmp_si(&op1.wrapped, op2); }
int GnuMPFloat::op_cmp_d(const GnuMPFloat &op1, double op2) { return mpfr_cmp_d(&op1.wrapped, op2); }
int GnuMPFloat::op_cmp_ui_2exp(const GnuMPFloat &op1, unsigned long int op2, exp_t e) { return mpfr_cmp_ui_2exp(&op1.wrapped, op2, e); }
int GnuMPFloat::op_cmp_si_2exp(const GnuMPFloat &op1, long int op2, exp_t e) { return mpfr_cmp_si_2exp(&op1.wrapped, op2, e); }
int GnuMPFloat::op_cmpabs(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_cmpabs(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_cmpabs_ui(const GnuMPFloat &op1, unsigned long op2) { return mpfr_cmpabs_ui(&op1.wrapped, op2); }
int GnuMPFloat::op_nan_p(const GnuMPFloat &op) { return mpfr_nan_p(&op.wrapped); }
int GnuMPFloat::op_inf_p(const GnuMPFloat &op) { return mpfr_inf_p(&op.wrapped); }
int GnuMPFloat::op_number_p(const GnuMPFloat &op) { return mpfr_number_p(&op.wrapped); }
int GnuMPFloat::op_zero_p(const GnuMPFloat &op) { return mpfr_zero_p(&op.wrapped); }
int GnuMPFloat::op_regular_p(const GnuMPFloat &op) { return mpfr_regular_p(&op.wrapped); }
int GnuMPFloat::op_sgn(const GnuMPFloat &op) { return mpfr_sgn(&op.wrapped); }
int GnuMPFloat::op_greater_p(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_greater_p(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_greaterequal_p(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_greaterequal_p(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_less_p(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_less_p(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_lessequal_p(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_lessequal_p(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_equal_p(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_equal_p(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_lessgreater_p(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_lessgreater_p(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_unordered_p(const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_unordered_p(&op1.wrapped, &op2.wrapped); }
int GnuMPFloat::op_total_order_p(GnuMPFloat &out, const GnuMPFloat &y) { return mpfr_total_order_p(&out.wrapped, &y.wrapped); }
int GnuMPFloat::op_frac(GnuMPFloat &out, const GnuMPFloat &op) { return mpfr_frac(&out.wrapped, &op.wrapped, out.rounding); }
int GnuMPFloat::op_modf(GnuMPFloat &iop, GnuMPFloat &fop, const GnuMPFloat &op) { return mpfr_modf(&iop.wrapped, &fop.wrapped, &op.wrapped, iop.rounding); }
int GnuMPFloat::op_fmod(GnuMPFloat &out, const GnuMPFloat &x, const GnuMPFloat &y) { return mpfr_fmod(&out.wrapped, &x.wrapped, &y.wrapped, out.rounding); }
int GnuMPFloat::op_fmodquo(GnuMPFloat &out, val q, const GnuMPFloat &x, const GnuMPFloat &y) { 
	long q_; 
	auto r = mpfr_fmodquo(&out.wrapped, &q_, &x.wrapped, &y.wrapped, out.rounding);
	if (!q.isUndefined() && !q.isNull())
		q.set("q", q_);
	return r;
}
int GnuMPFloat::op_remainder(GnuMPFloat &out, const GnuMPFloat &x, const GnuMPFloat &y) { return mpfr_remainder(&out.wrapped, &x.wrapped, &y.wrapped, out.rounding); }
int GnuMPFloat::op_remquo(GnuMPFloat &out, val q, const GnuMPFloat &x, const GnuMPFloat &y) { 
	long q_;
	auto r = mpfr_remquo(&out.wrapped, &q_, &x.wrapped, &y.wrapped, out.rounding);
	if (!q.isUndefined() && !q.isNull())
		q.set("q", q_);
	return r;
}
void GnuMPFloat::op_set_default_rounding_mode(int rnd) { return mpfr_set_default_rounding_mode((rnd_t)rnd); }
int GnuMPFloat::op_prec_round(GnuMPFloat &out, prec_t prec) { return mpfr_prec_round(&out.wrapped, prec, out.rounding); }
int GnuMPFloat::op_can_round(const GnuMPFloat &b, exp_t err, int rnd1, int rnd2, prec_t prec) { return mpfr_can_round(&b.wrapped, err, (rnd_t)rnd1, (rnd_t)rnd2, prec); }
GnuMPFloat::prec_t GnuMPFloat::op_min_prec(GnuMPFloat &out) { return mpfr_min_prec(&out.wrapped); }
void GnuMPFloat::op_nexttoward(GnuMPFloat &out, const GnuMPFloat &y) { return mpfr_nexttoward(&out.wrapped, &y.wrapped); }
void GnuMPFloat::op_nextabove(GnuMPFloat &out) { return mpfr_nextabove(&out.wrapped); }
void GnuMPFloat::op_nextbelow(GnuMPFloat &out) { return mpfr_nextbelow(&out.wrapped); }
int GnuMPFloat::op_min(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_min(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding); }
int GnuMPFloat::op_max(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_max(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding); }
int GnuMPFloat::op_nrandom(GnuMPFloat &out1, gmp_randstate_t state) { return mpfr_nrandom(&out1.wrapped, state, out1.rounding); }
int GnuMPFloat::op_grandom(GnuMPFloat &out1, GnuMPFloat &out2, gmp_randstate_t state) { return mpfr_grandom(&out1.wrapped, &out2.wrapped, state, out1.rounding); }
int GnuMPFloat::op_erandom(GnuMPFloat &out1, gmp_randstate_t state) { return mpfr_erandom(&out1.wrapped, state, out1.rounding); }
GnuMPFloat::exp_t GnuMPFloat::op_get_exp(GnuMPFloat &out) { return mpfr_get_exp(&out.wrapped); }
int GnuMPFloat::op_set_exp(GnuMPFloat &out, exp_t e) { return mpfr_set_exp(&out.wrapped, e); }
int GnuMPFloat::op_signbit(const GnuMPFloat &op) { return mpfr_signbit(&op.wrapped); }
int GnuMPFloat::op_setsign(GnuMPFloat &out, const GnuMPFloat &op, int s) { return mpfr_setsign(&out.wrapped, &op.wrapped, s, out.rounding); }
int GnuMPFloat::op_copysign(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2) { return mpfr_copysign(&out.wrapped, &op1.wrapped, &op2.wrapped, out.rounding); }
int GnuMPFloat::op_set_emin(exp_t exp) { return mpfr_set_emin(exp); }
int GnuMPFloat::op_set_emax(exp_t exp) { return mpfr_set_emax(exp); }
int GnuMPFloat::op_check_range(GnuMPFloat &out, int t) { return mpfr_check_range(&out.wrapped, t, out.rounding); }
int GnuMPFloat::op_subnormalize(GnuMPFloat &out, int t) { return mpfr_subnormalize(&out.wrapped, t, out.rounding); }
void GnuMPFloat::op_flags_clear(flags_t mask) { return mpfr_flags_clear(mask); }
void GnuMPFloat::op_flags_set(flags_t mask) { return mpfr_flags_set(mask); }
GnuMPFloat::flags_t GnuMPFloat::op_flags_test(flags_t mask) { return mpfr_flags_test(mask); }
void GnuMPFloat::op_flags_restore(flags_t flags, flags_t mask) { return mpfr_flags_restore(flags, mask); }