#pragma once

#include "Float.hpp"

class TanhSinh
{
public:
	typedef void (*Integrand)(Float &y, const Float &x, const Float& xc);

private:
	Float::prec_t safe_prec;
	Float::prec_t user_prec;
	Integrand integrand;
	Float *abscissas_and_weights;

	Float upper_bound = Float(safe_prec);
	Float error_approximation = Float((Float::prec_t)16);
	Float integral_approximation = Float(safe_prec);
	Float lower_bound = Float(safe_prec);
	int points;

	void intdeini(int lenaw, const Float &tiny, const Float &eps, Float *aw);
	void intde(Integrand, const Float &a, const Float &b, Float *aw, Float *i, Float *err);

public:
	TanhSinh(Float::prec_t prec);
	TanhSinh(Float::prec_t prec, int points);
	~TanhSinh();

	void compile();
	void integrate();
	void setIntegrand(Integrand);
	void setBounds(const Float &, const Float &);
	void setUpperBound(const Float &);
	Float &getUpperBound();
	void setLowerBound(const Float &);
	Float &getLowerBound();
	bool isConvergentError();
	const Float &getAbsoluteErrorApprox();
	Float::exp_t getRelativeErrorExponent();
	const Float &getIntegralApprox();

private:
	int call(Float &y, FloatRegister &x, Float& xc)
	{
		Float& xf = reinterpret_cast<Float&>(x);
		integrand(y, xf, xc);
		FloatRegister::release(x);
		if (Float::op_inf_p(y))
			return 0;
		else
			return 1;
	}
};