#pragma once

#include "Float.hpp"

class TanhSinh
{
public:
	typedef Float (*Integrand)(const Float & x);

private:

	Float::prec_t safe_prec;
	Float::prec_t user_prec;
	Integrand integrand;
	Float *abscissas_and_weights;
	
	Float upper_bound = Float(safe_prec);
	Float error_approximation = Float(safe_prec);
	Float integral_approximation = Float(safe_prec);
	Float lower_bound = Float(safe_prec);

	void intdeini(int lenaw, const Float &tiny, const Float &eps, Float *aw);
	void intde(Float (*f)(const Float&), const Float& a, const Float& b, Float *aw, Float *i, Float *err);

public:
	TanhSinh(Float::prec_t);
	~TanhSinh();

	void integrate();
	void setIntegrand(Integrand integrand);
	void setBounds(const Float &, const Float &);
	void setUpperBound(const Float &);
	Float &getUpperBound();
	void setLowerBound(const Float &);
	Float &getLowerBound();
	bool isConvergentError();
	const Float &getAbsoluteErrorApprox();
	const Float &getIntegralApprox();
};