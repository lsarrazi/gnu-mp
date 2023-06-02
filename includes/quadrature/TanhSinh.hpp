#pragma once

#include "Float.hpp"

class TanhSinh
{
public:
	typedef void (*Integrand)(GnuMPFloat &y, const GnuMPFloat &x, const GnuMPFloat& xc);
	typedef void (*Integrand)(GnuMPFloat &y, const GnuMPFloat &x, const GnuMPFloat& xc);

private:
	GnuMPFloat::prec_t safe_prec;
	GnuMPFloat::prec_t user_prec;
	Integrand integrand;
	GnuMPFloat *abscissas_and_weights;
	GnuMPFloat upper_bound = GnuMPFloat(safe_prec);
	GnuMPFloat error_approximation = GnuMPFloat((GnuMPFloat::prec_t)16);
	GnuMPFloat integral_approximation = GnuMPFloat(safe_prec);
	GnuMPFloat lower_bound = GnuMPFloat(safe_prec);
	int call_count = 0;

	int points;

	void intdeini(int lenaw, const GnuMPFloat &tiny, const GnuMPFloat &eps, GnuMPFloat *aw);
	void intde(Integrand, const GnuMPFloat &a, const GnuMPFloat &b, GnuMPFloat *aw, GnuMPFloat *i, GnuMPFloat *err);

public:
	TanhSinh(GnuMPFloat::prec_t prec);
	TanhSinh(GnuMPFloat::prec_t prec, int points);
	~TanhSinh();

	void compile();
	void integrate();
	void setIntegrand(Integrand);
	void setIntegrandCallback(IntegrandCallback);
	void setBounds(const GnuMPFloat &, const GnuMPFloat &);
	void setUpperBound(const GnuMPFloat &);
	GnuMPFloat &getUpperBound();
	void setLowerBound(const GnuMPFloat &);
	GnuMPFloat &getLowerBound();
	bool isConvergentError();
	const GnuMPFloat &getAbsoluteErrorApprox();
	GnuMPFloat::exp_t getRelativeErrorExponent();
	const GnuMPFloat &getIntegralApprox();
	int getIntegrandCallCount();

private:
	int callIntegrand(GnuMPFloat &y, FloatRegister &x, GnuMPFloat& xc)
	{
		GnuMPFloat& xf = reinterpret_cast<GnuMPFloat&>(x);
		integrand(y, xf, xc);
		call_count++;
		FloatRegister::release(x);
		if (GnuMPFloat::op_inf_p(y))
			return 0;
		else
			return 1;
	}
};