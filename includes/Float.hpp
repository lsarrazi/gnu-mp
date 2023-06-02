#pragma once

#include <mpfr.h>
#include <iostream>
#include <emscripten/val.h>

using namespace emscripten;

class FloatRegister;


#include "utils.hpp"

class GnuMPFloat
{

public:
	typedef mpfr_size_t size_t;
	typedef mpfr_rnd_t rnd_t;
	typedef mpfr_exp_t exp_t;
	typedef mpfr_sign_t sign_t;
	typedef mpfr_prec_t prec_t;
	typedef mpfr_flags_t flags_t;
	typedef memory_view<mp_limb_t> mantissa_t;
	typedef void builder_pattern;

private:
	__mpfr_struct wrapped;
	rnd_t rounding = MPFR_RNDN;

public:
	GnuMPFloat(val);
	explicit GnuMPFloat(prec_t = getDefaultPrecision());
	GnuMPFloat(double, prec_t = getDefaultPrecision());
	GnuMPFloat(const char*, prec_t = getDefaultPrecision(), int base = 10);
	GnuMPFloat(const GnuMPFloat&);
	GnuMPFloat(const FloatRegister&);
	GnuMPFloat& operator=(double);
	GnuMPFloat& operator=(const GnuMPFloat&);
	GnuMPFloat& operator=(FloatRegister& r);
	GnuMPFloat& operator=(const char *);

	~GnuMPFloat();

	int getRounding() const;
	const std::string &getRoundingString();
	builder_pattern setRounding(int mode);
	size_t getPrecision() const;
	builder_pattern setPrecision(size_t precision);
	exp_t getExponent() const;
	builder_pattern setExponent(exp_t exponent);
	sign_t getSign() const;
	builder_pattern setSign(sign_t sign);
	size_t getMantissaSize() const;
	val getMantissaView();
	builder_pattern setString(const std::string &str);
	builder_pattern setString(const std::string &str, int base);
	builder_pattern setDouble(double x);
	builder_pattern setFloat(const GnuMPFloat &tocopy);
	builder_pattern set(val v);
	std::string toString(int base) const;
	std::string toString() const;
	std::string toString(int base, int n) const;
	double toNumber() const;
	bool isInteger() const;
	bool isNaN() const ;
	bool isInfinity() const ;
	bool isNumber() const ;
	bool isZero() const ;
	bool isRegular() const ;
	static int op_cmp_abs();
	static int op_cmp_abs_ui();
	bool less_greater(const GnuMPFloat &);

	explicit operator int();
	explicit operator long();
	explicit operator unsigned();
	explicit operator unsigned long();
	explicit operator double();

	bool operator<(const GnuMPFloat &op) const;
	bool operator==(const GnuMPFloat &op) const;
	bool operator<=(const GnuMPFloat &op) const;
	bool operator>(const GnuMPFloat &op) const;
	bool operator>=(const GnuMPFloat &op) const;
	bool operator!=(const GnuMPFloat &op) const;
	bool operator<(double op) const;
	bool operator>(double op) const;
	bool operator==(double op) const;
	bool operator<=(double op) const;
	bool operator>=(double op) const;
	bool operator!=(double op) const;

	bool less(val v);
	bool greater(val v);
	bool greater_equal(val v);
	bool less_equal(val v);
	bool equal(val v);
	bool not_equal(val v);

	builder_pattern operator+=(const GnuMPFloat &op);
	builder_pattern operator+=(double op);
	builder_pattern operator-=(const GnuMPFloat &op);
	builder_pattern operator-=(double op);
	builder_pattern operator*=(const GnuMPFloat &op);
	builder_pattern operator*=(double op);
	builder_pattern operator/=(const GnuMPFloat &op);
	builder_pattern operator/=(double op);
	builder_pattern add(val v);
	builder_pattern sub(val v);
	builder_pattern mul(val v);
	builder_pattern div(val v);
	builder_pattern sqrt();
	builder_pattern rec_sqrt();
	builder_pattern cbrt();
	builder_pattern root_ui(unsigned n);
	builder_pattern neg();
	builder_pattern abs();
	builder_pattern dim(const GnuMPFloat &op);
	builder_pattern fma(const GnuMPFloat &a, const GnuMPFloat &b);
	builder_pattern fms(const GnuMPFloat &a, const GnuMPFloat &b);
	builder_pattern fmma(const GnuMPFloat &a, const GnuMPFloat &b, const GnuMPFloat &c);
	builder_pattern fmms(const GnuMPFloat &a, const GnuMPFloat &b, const GnuMPFloat &c);
	builder_pattern swap(GnuMPFloat &op);

	// integer and remainders
	builder_pattern rint();
	builder_pattern ceil();
	builder_pattern floor();
	builder_pattern round();
	builder_pattern roundeven();
	builder_pattern trunc();
	builder_pattern rint_ceil();
	builder_pattern rint_floor();
	builder_pattern rint_round();
	builder_pattern rint_roundeven();
	builder_pattern rint_trunc();
	// TRANCENDENTAL

	builder_pattern log();
	builder_pattern log2();
	builder_pattern log10();
	builder_pattern log1p();
	builder_pattern log_ui(unsigned long);
	builder_pattern exp();
	builder_pattern exp2();
	builder_pattern exp10();
	builder_pattern expm1();
	builder_pattern pow(val);
	builder_pattern expow(val);
	builder_pattern pow_si(long);
	builder_pattern ui_pow(unsigned long);
	builder_pattern ui_pow_ui(unsigned long, unsigned long);
	builder_pattern cos();
	builder_pattern sin();
	builder_pattern tan();
	builder_pattern sec();
	builder_pattern csc();
	builder_pattern cot();
	builder_pattern acos();
	builder_pattern asin();
	builder_pattern atan();
	builder_pattern atan2(const GnuMPFloat &y, const GnuMPFloat &x);
	builder_pattern cosh();
	builder_pattern sinh();
	builder_pattern tanh();
	builder_pattern sech();
	builder_pattern csch();
	builder_pattern coth();
	builder_pattern acosh();
	builder_pattern asinh();
	builder_pattern atanh();
	builder_pattern eint();
	builder_pattern li2();
	builder_pattern gamma();
	builder_pattern gamma_inc(const GnuMPFloat &);
	builder_pattern lngamma();
	builder_pattern lgamma(val signp);
	builder_pattern digamma();
	builder_pattern beta(const GnuMPFloat &);
	builder_pattern zeta();
	builder_pattern zeta_ui(unsigned long);
	builder_pattern erf();
	builder_pattern erfc();
	builder_pattern j0();
	builder_pattern j1();
	builder_pattern jn(long);
	builder_pattern y0();
	builder_pattern y1();
	builder_pattern yn(long);
	builder_pattern agm(const GnuMPFloat &);
	builder_pattern ai(const GnuMPFloat &x);
	builder_pattern const_log2();
	builder_pattern const_pi();
	builder_pattern const_euler();
	builder_pattern const_catalan();

	static prec_t getDefaultPrecision();
	static void setDefaultPrecision(prec_t prec);

	static int op_add(GnuMPFloat &out, const GnuMPFloat &a, val v);
	static int op_sub(GnuMPFloat &out, const GnuMPFloat &a, val v);
	static int op_mul(GnuMPFloat &out, const GnuMPFloat &a, val v);
	static int op_div(GnuMPFloat &out, const GnuMPFloat &a, val v);
	static int op_sqrt(GnuMPFloat &out, const GnuMPFloat &src);
	static int op_rec_sqrt(GnuMPFloat &out, const GnuMPFloat &src);
	static int op_cbrt(GnuMPFloat &out, const GnuMPFloat &src);
	static int op_root_ui(GnuMPFloat &out, const GnuMPFloat &src, unsigned n);
	static int op_neg(GnuMPFloat &out, const GnuMPFloat &src);
	static int op_abs(GnuMPFloat &out, const GnuMPFloat &src);
	static int op_dim(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &op);
	static int op_fac(GnuMPFloat &out, unsigned);
	static int op_fma(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &a, const GnuMPFloat &b);
	static int op_fms(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &a, const GnuMPFloat &b);
	static int op_fmma(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &a, const GnuMPFloat &b, const GnuMPFloat &c);
	static int op_fmms(GnuMPFloat &out, const GnuMPFloat &src, const GnuMPFloat &a, const GnuMPFloat &b, const GnuMPFloat &c);
	static void op_swap(GnuMPFloat &a, GnuMPFloat &b);
	static int op_hypot(GnuMPFloat &out, const GnuMPFloat &x, const GnuMPFloat &y);
	static int op_sum(GnuMPFloat &out, val iterable);
	static int op_dot(GnuMPFloat &out, val a, val b);

	// Integer & Remainders

	static int op_rint(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_ceil(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_floor(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_round(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_roundeven(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_trunc(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_rint_ceil(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_rint_floor(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_rint_round(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_rint_roundeven(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_rint_trunc(GnuMPFloat &out, const GnuMPFloat &op);

	// TRANSCENDENTALES

	static int op_log(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_log_ui(GnuMPFloat &out, unsigned long op);
	static int op_log2(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_log10(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_log1p(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_exp(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_exp2(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_exp10(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_expm1(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_pow(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_pow_si(GnuMPFloat &out, const GnuMPFloat &op1, long op2);
	static int op_ui_pow_ui(GnuMPFloat &out, unsigned long op1, unsigned long op2);
	static int op_ui_pow(GnuMPFloat &out, unsigned long op1, const GnuMPFloat &op2);
	static int op_cos(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_sin(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_tan(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_sin_cos(GnuMPFloat &sop, GnuMPFloat &cop, const GnuMPFloat &op);
	static int op_sec(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_csc(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_cot(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_acos(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_asin(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_atan(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_atan2(GnuMPFloat &out, const GnuMPFloat &y, const GnuMPFloat &x);
	static int op_cosh(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_sinh(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_tanh(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_sinh_cosh(GnuMPFloat &sop, GnuMPFloat &cop, const GnuMPFloat &op);
	static int op_sech(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_csch(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_coth(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_acosh(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_asinh(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_atanh(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_eint(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_li2(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_gamma(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_gamma_inc(GnuMPFloat &out, const GnuMPFloat &op, const GnuMPFloat &op2);
	static int op_lngamma(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_lgamma(GnuMPFloat &out, val signp, const GnuMPFloat &op);
	static int op_digamma(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_beta(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_zeta(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_zeta_ui(GnuMPFloat &out, unsigned long op);
	static int op_erf(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_erfc(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_j0(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_j1(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_jn(GnuMPFloat &out, long n, const GnuMPFloat &op);
	static int op_y0(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_y1(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_yn(GnuMPFloat &out, long n, const GnuMPFloat &op);
	static int op_agm(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_ai(GnuMPFloat &out, const GnuMPFloat &x);
	static int op_const_log2(GnuMPFloat &out);
	static int op_const_pi(GnuMPFloat &out);
	static int op_const_euler(GnuMPFloat &out);
	static int op_const_catalan(GnuMPFloat &out);

	// others

	static int op_sqr(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_cmp(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_cmp_ui(const GnuMPFloat &op1, unsigned long int op2);
	static int op_cmp_si(const GnuMPFloat &op1, long int op2);
	static int op_cmp_d(const GnuMPFloat &op1, double op2);
	static int op_cmp_ui_2exp(const GnuMPFloat &op1, unsigned long int op2, exp_t e);
	static int op_cmp_si_2exp(const GnuMPFloat &op1, long int op2, exp_t e);
	static int op_cmpabs(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_cmpabs_ui(const GnuMPFloat &op1, unsigned long op2);
	static int op_nan_p(const GnuMPFloat &op);
	static int op_inf_p(const GnuMPFloat &op);
	static int op_number_p(const GnuMPFloat &op);
	static int op_zero_p(const GnuMPFloat &op);
	static int op_regular_p(const GnuMPFloat &op);
	static int op_sgn(const GnuMPFloat &op);
	static int op_greater_p(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_greaterequal_p(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_less_p(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_lessequal_p(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_equal_p(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_lessgreater_p(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_unordered_p(const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_total_order_p(GnuMPFloat &out, const GnuMPFloat &y);
	static int op_frac(GnuMPFloat &out, const GnuMPFloat &op);
	static int op_modf(GnuMPFloat &iop, GnuMPFloat &fop, const GnuMPFloat &op);
	static int op_fmod(GnuMPFloat &out, const GnuMPFloat &x, const GnuMPFloat &y);
	static int op_fmodquo(GnuMPFloat &out, val q, const GnuMPFloat &x, const GnuMPFloat &y);
	static int op_remainder(GnuMPFloat &out, const GnuMPFloat &x, const GnuMPFloat &y);
	static int op_remquo(GnuMPFloat &out, val q, const GnuMPFloat &x, const GnuMPFloat &y);
	static void op_set_default_rounding_mode(int rnd);
	static int op_prec_round(GnuMPFloat &out, prec_t prec);
	static int op_can_round(const GnuMPFloat &b, exp_t err, int rnd1, int rnd2, prec_t prec);
	static prec_t op_min_prec(GnuMPFloat &out);
	static void op_nexttoward(GnuMPFloat &out, const GnuMPFloat &y);
	static void op_nextabove(GnuMPFloat &out);
	static void op_nextbelow(GnuMPFloat &out);
	static int op_min(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_max(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_nrandom(GnuMPFloat &out1, gmp_randstate_t state);
	static int op_grandom(GnuMPFloat &out1, GnuMPFloat &out2, gmp_randstate_t state);
	static int op_erandom(GnuMPFloat &out1, gmp_randstate_t state);
	static exp_t op_get_exp(GnuMPFloat &out);
	static int op_set_exp(GnuMPFloat &out, exp_t e);
	static int op_signbit(const GnuMPFloat &op);
	static int op_setsign(GnuMPFloat &out, const GnuMPFloat &op, int s);
	static int op_copysign(GnuMPFloat &out, const GnuMPFloat &op1, const GnuMPFloat &op2);
	static int op_set_emin(exp_t exp);
	static int op_set_emax(exp_t exp);
	static int op_check_range(GnuMPFloat &out, int t);
	static int op_subnormalize(GnuMPFloat &out, int t);
	static void op_flags_clear(flags_t mask);
	static void op_flags_set(flags_t mask);
	static flags_t op_flags_test(flags_t mask);
	static void op_flags_restore(flags_t flags, flags_t mask);
	static std::string op_get_version();
	static std::string op_get_patches();
	static std::string op_buildopt_tune_case();

private:
	static void jsArrayToMpfrArray(emscripten::val array, mpfr_ptr *out, int length);
};

#include "FloatRegister.hpp"