#pragma once

#include <mpfr.h>
#include <iostream>
#include <emscripten/val.h>

using namespace emscripten;

#include "Float.hpp"
#include "utils.hpp"

class Float
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
	Float();
	Float(val);
	Float(prec_t);
	Float(prec_t, double);
	Float(const Float&);
	Float& operator=(double);
	Float& operator=(const Float&);
	~Float();

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
	builder_pattern setFloat(const Float &tocopy);
	builder_pattern set(val v);
	std::string toString(int base);
	std::string toString();
	std::string toString(int base, int n);
	double toNumber();
	bool isInteger();
	bool isNaN();
	bool isInfinity();
	bool isNumber();
	bool isZero();
	bool isRegular();
	static int op_cmp_abs();
	static int op_cmp_abs_ui();
	bool less_greater(const Float &);

	bool operator<(const Float &op) const;
	bool operator==(const Float &op) const;
	bool operator<=(const Float &op) const;
	bool operator>(const Float &op) const;
	bool operator>=(const Float &op) const;
	bool operator!=(const Float &op) const;
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

	builder_pattern operator+=(const Float &op);
	builder_pattern operator+=(double op);
	builder_pattern operator-=(const Float &op);
	builder_pattern operator-=(double op);
	builder_pattern operator*=(const Float &op);
	builder_pattern operator*=(double op);
	builder_pattern operator/=(const Float &op);
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
	builder_pattern dim(const Float &op);
	builder_pattern fma(const Float &a, const Float &b);
	builder_pattern fms(const Float &a, const Float &b);
	builder_pattern fmma(const Float &a, const Float &b, const Float &c);
	builder_pattern fmms(const Float &a, const Float &b, const Float &c);
	builder_pattern swap(Float &op);

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
	builder_pattern atan2(const Float &y, const Float &x);
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
	builder_pattern gamma_inc(const Float &);
	builder_pattern lngamma();
	builder_pattern lgamma(val signp);
	builder_pattern digamma();
	builder_pattern beta(const Float &);
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
	builder_pattern agm(const Float &);
	builder_pattern ai(const Float &x);
	builder_pattern const_log2();
	builder_pattern const_pi();
	builder_pattern const_euler();
	builder_pattern const_catalan();

	static int op_add(Float &out, const Float &a, val v);
	static int op_sub(Float &out, const Float &a, val v);
	static int op_mul(Float &out, const Float &a, val v);
	static int op_div(Float &out, const Float &a, val v);
	static int op_sqrt(Float &out, const Float &src);
	static int op_rec_sqrt(Float &out, const Float &src);
	static int op_cbrt(Float &out, const Float &src);
	static int op_root_ui(Float &out, const Float &src, unsigned n);
	static int op_neg(Float &out, const Float &src);
	static int op_abs(Float &out, const Float &src);
	static int op_dim(Float &out, const Float &src, const Float &op);
	static int op_fac(Float &out, unsigned);
	static int op_fma(Float &out, const Float &src, const Float &a, const Float &b);
	static int op_fms(Float &out, const Float &src, const Float &a, const Float &b);
	static int op_fmma(Float &out, const Float &src, const Float &a, const Float &b, const Float &c);
	static int op_fmms(Float &out, const Float &src, const Float &a, const Float &b, const Float &c);
	static void op_swap(Float &a, Float &b);
	static int op_hypot(Float &out, const Float &x, const Float &y);
	static int op_sum(Float &out, val iterable);
	static int op_dot(Float &out, val a, val b);

	// Integer & Remainders

	static int op_rint(Float &out, const Float &op);
	static int op_ceil(Float &out, const Float &op);
	static int op_floor(Float &out, const Float &op);
	static int op_round(Float &out, const Float &op);
	static int op_roundeven(Float &out, const Float &op);
	static int op_trunc(Float &out, const Float &op);
	static int op_rint_ceil(Float &out, const Float &op);
	static int op_rint_floor(Float &out, const Float &op);
	static int op_rint_round(Float &out, const Float &op);
	static int op_rint_roundeven(Float &out, const Float &op);
	static int op_rint_trunc(Float &out, const Float &op);

	// TRANSCENDENTALES

	static int op_log(Float &out, const Float &op);
	static int op_log_ui(Float &out, unsigned long op);
	static int op_log2(Float &out, const Float &op);
	static int op_log10(Float &out, const Float &op);
	static int op_log1p(Float &out, const Float &op);
	static int op_exp(Float &out, const Float &op);
	static int op_exp2(Float &out, const Float &op);
	static int op_exp10(Float &out, const Float &op);
	static int op_expm1(Float &out, const Float &op);
	static int op_pow(Float &out, const Float &op1, const Float &op2);
	static int op_pow_si(Float &out, const Float &op1, long op2);
	static int op_ui_pow_ui(Float &out, unsigned long op1, unsigned long op2);
	static int op_ui_pow(Float &out, unsigned long op1, const Float &op2);
	static int op_cos(Float &out, const Float &op);
	static int op_sin(Float &out, const Float &op);
	static int op_tan(Float &out, const Float &op);
	static int op_sin_cos(Float &sop, Float &cop, const Float &op);
	static int op_sec(Float &out, const Float &op);
	static int op_csc(Float &out, const Float &op);
	static int op_cot(Float &out, const Float &op);
	static int op_acos(Float &out, const Float &op);
	static int op_asin(Float &out, const Float &op);
	static int op_atan(Float &out, const Float &op);
	static int op_atan2(Float &out, const Float &y, const Float &x);
	static int op_cosh(Float &out, const Float &op);
	static int op_sinh(Float &out, const Float &op);
	static int op_tanh(Float &out, const Float &op);
	static int op_sinh_cosh(Float &sop, Float &cop, const Float &op);
	static int op_sech(Float &out, const Float &op);
	static int op_csch(Float &out, const Float &op);
	static int op_coth(Float &out, const Float &op);
	static int op_acosh(Float &out, const Float &op);
	static int op_asinh(Float &out, const Float &op);
	static int op_atanh(Float &out, const Float &op);
	static int op_eint(Float &out, const Float &op);
	static int op_li2(Float &out, const Float &op);
	static int op_gamma(Float &out, const Float &op);
	static int op_gamma_inc(Float &out, const Float &op, const Float &op2);
	static int op_lngamma(Float &out, const Float &op);
	static int op_lgamma(Float &out, val signp, const Float &op);
	static int op_digamma(Float &out, const Float &op);
	static int op_beta(Float &out, const Float &op1, const Float &op2);
	static int op_zeta(Float &out, const Float &op);
	static int op_zeta_ui(Float &out, unsigned long op);
	static int op_erf(Float &out, const Float &op);
	static int op_erfc(Float &out, const Float &op);
	static int op_j0(Float &out, const Float &op);
	static int op_j1(Float &out, const Float &op);
	static int op_jn(Float &out, long n, const Float &op);
	static int op_y0(Float &out, const Float &op);
	static int op_y1(Float &out, const Float &op);
	static int op_yn(Float &out, long n, const Float &op);
	static int op_agm(Float &out, const Float &op1, const Float &op2);
	static int op_ai(Float &out, const Float &x);
	static int op_const_log2(Float &out);
	static int op_const_pi(Float &out);
	static int op_const_euler(Float &out);
	static int op_const_catalan(Float &out);

	// others

	static int op_sqr(Float &out, const Float &op);
	static int op_cmp(const Float &op1, const Float &op2);
	static int op_cmp_ui(const Float &op1, unsigned long int op2);
	static int op_cmp_si(const Float &op1, long int op2);
	static int op_cmp_d(const Float &op1, double op2);
	static int op_cmp_ui_2exp(const Float &op1, unsigned long int op2, exp_t e);
	static int op_cmp_si_2exp(const Float &op1, long int op2, exp_t e);
	static int op_cmpabs(const Float &op1, const Float &op2);
	static int op_cmpabs_ui(const Float &op1, unsigned long op2);
	static int op_nan_p(const Float &op);
	static int op_inf_p(const Float &op);
	static int op_number_p(const Float &op);
	static int op_zero_p(const Float &op);
	static int op_regular_p(const Float &op);
	static int op_sgn(const Float &op);
	static int op_greater_p(const Float &op1, const Float &op2);
	static int op_greaterequal_p(const Float &op1, const Float &op2);
	static int op_less_p(const Float &op1, const Float &op2);
	static int op_lessequal_p(const Float &op1, const Float &op2);
	static int op_equal_p(const Float &op1, const Float &op2);
	static int op_lessgreater_p(const Float &op1, const Float &op2);
	static int op_unordered_p(const Float &op1, const Float &op2);
	static int op_total_order_p(Float &out, const Float &y);
	static int op_frac(Float &out, const Float &op);
	static int op_modf(Float &iop, Float &fop, const Float &op);
	static int op_fmod(Float &out, const Float &x, const Float &y);
	static int op_fmodquo(Float &out, val q, const Float &x, const Float &y);
	static int op_remainder(Float &out, const Float &x, const Float &y);
	static int op_remquo(Float &out, val q, const Float &x, const Float &y);
	static void op_set_default_rounding_mode(int rnd);
	static int op_prec_round(Float &out, prec_t prec);
	static int op_can_round(const Float &b, exp_t err, int rnd1, int rnd2, prec_t prec);
	static prec_t op_min_prec(Float &out);
	static void op_nexttoward(Float &out, const Float &y);
	static void op_nextabove(Float &out);
	static void op_nextbelow(Float &out);
	static int op_min(Float &out, const Float &op1, const Float &op2);
	static int op_max(Float &out, const Float &op1, const Float &op2);
	static int op_nrandom(Float &out1, gmp_randstate_t state);
	static int op_grandom(Float &out1, Float &out2, gmp_randstate_t state);
	static int op_erandom(Float &out1, gmp_randstate_t state);
	static exp_t op_get_exp(Float &out);
	static int op_set_exp(Float &out, exp_t e);
	static int op_signbit(const Float &op);
	static int op_setsign(Float &out, const Float &op, int s);
	static int op_copysign(Float &out, const Float &op1, const Float &op2);
	static int op_set_emin(exp_t exp);
	static int op_set_emax(exp_t exp);
	static int op_check_range(Float &out, int t);
	static int op_subnormalize(Float &out, int t);
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
