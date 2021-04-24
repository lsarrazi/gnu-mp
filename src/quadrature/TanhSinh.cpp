/*
	Implementation of the quadrature by Takuya OOURA, thanks to him

	1996 Takuya OOURA (email: ooura@mmm.t.u-tokyo.ac.jp).
*/

#include "quadrature/TanhSinh.hpp"
#include "Float.hpp"
#include "FloatRegister.hpp"

void TanhSinh::intdeini(int lenaw, const Float &tiny, const Float &eps, Float *aw)
{

	/* ---- adjustable parameter ---- */
	const Float efs("0.1"), hoff("8.5");
	/* ------------------------------ */
	int noff, nk, k, j;
	Float pi2, tinyln, epsln, h0, ehp, ehm, h, t, ep, em, xw, wg;

	pi2 = 2 * atan(1.0);
	tinyln = -log(tiny);
	epsln = 1 - log(efs * eps);
	h0 = hoff / epsln;
	ehp = exp(h0);
	ehm = 1 / ehp;
	aw[2] = eps;
	aw[3] = exp(-ehm * epsln);
	aw[4] = sqrt(efs * eps);
	noff = 5;
	aw[noff] = 0.5;
	aw[noff + 1] = h0;
	aw[noff + 2] = pi2 * h0 * 0.5;
	h = 2;
	nk = 0;
	k = noff + 3;
	do
	{
		t = h * 0.5;
		do
		{
			em = exp(h0 * t);
			ep = pi2 * em;
			em = pi2 / em;
			j = k;
			do
			{
				xw = 1 / (1 + exp(ep - em));
				wg = xw * (1 - xw) * h0;
				aw[j] = xw;
				aw[j + 1] = wg * 4;
				aw[j + 2] = wg * (ep + em);
				ep *= ehp;
				em *= ehm;
				j += 3;
			} while (ep < tinyln && j <= lenaw - 3);
			t += h;
			k += nk;
		} while (t < 1);
		h *= 0.5;
		if (nk == 0)
		{
			if (j > lenaw - 6)
				j -= 3;
			nk = j - noff;
			k += nk;
			aw[1] = nk;
		}
	} while (2 * k - noff - 3 <= lenaw);
	aw[0] = k - 3;
}

void TanhSinh::intde(Float (*f)(const Float &), const Float &a, const Float &b, Float *aw,
					 Float *i, Float *err)
{
	int noff, lenawm, nk, k, j, jtmp, jm, m, klim;
	Float epsh, ba, ir, xa, fa, fb, errt, errh, errd, h, iback, irback;

	noff = 5;
	lenawm = (int)(aw[0] + 0.5);
	nk = (int)(aw[1] + 0.5);
	epsh = aw[4];
	ba = b - a;
	*i = (*f)((a + b) * aw[noff]);
	ir = *i * aw[noff + 1];
	*i *= aw[noff + 2];
	*err = fabs(*i);
	k = nk + noff;
	j = noff;
	do
	{
		j += 3;
		xa = ba * aw[j];
		fa = (*f)(a + xa);
		fb = (*f)(b - xa);
		ir += (fa + fb) * aw[j + 1];
		fa *= aw[j + 2];
		fb *= aw[j + 2];
		*i += fa + fb;
		*err += fabs(fa) + fabs(fb);
	} while (aw[j] > epsh && j < k);
	errt = *err * aw[3];
	errh = *err * epsh;
	errd = 1 + 2 * errh;
	jtmp = j;
	while (fabs(fa) > errt && j < k)
	{
		j += 3;
		fa = (*f)(a + ba * aw[j]);
		ir += fa * aw[j + 1];
		fa *= aw[j + 2];
		*i += fa;
	}
	jm = j;
	j = jtmp;
	while (fabs(fb) > errt && j < k)
	{
		j += 3;
		fb = (*f)(b - ba * aw[j]);
		ir += fb * aw[j + 1];
		fb *= aw[j + 2];
		*i += fb;
	}
	if (j < jm)
		jm = j;
	jm -= noff + 3;
	h = 1;
	m = 1;
	klim = k + nk;
	while (errd > errh && klim <= lenawm)
	{
		iback = *i;
		irback = ir;
		do
		{
			jtmp = k + jm;
			for (j = k + 3; j <= jtmp; j += 3)
			{
				xa = ba * aw[j];
				fa = (*f)(a + xa);
				fb = (*f)(b - xa);
				ir += (fa + fb) * aw[j + 1];
				*i += (fa + fb) * aw[j + 2];
			}
			k += nk;
			j = jtmp;
			do
			{
				j += 3;
				fa = (*f)(a + ba * aw[j]);
				ir += fa * aw[j + 1];
				fa *= aw[j + 2];
				*i += fa;
			} while (fabs(fa) > errt && j < k);
			j = jtmp;
			do
			{
				j += 3;
				fb = (*f)(b - ba * aw[j]);
				ir += fb * aw[j + 1];
				fb *= aw[j + 2];
				*i += fb;
			} while (fabs(fb) > errt && j < k);
		} while (k < klim);
		errd = h * (fabs(*i - 2 * iback) + fabs(ir - 2 * irback));
		h *= 0.5;
		m *= 2;
		klim = 2 * klim - noff;
	}
	*i *= h * ba;
	if (errd > errh)
	{
		*err = -errd * (m * fabs(ba));
	}
	else
	{
		*err = *err * aw[2] * (m * fabs(ba));
	}
}

TanhSinh::TanhSinh(Float::prec_t prec) : user_prec(prec), safe_prec(prec + 16)
{
	auto last = Float::getDefaultPrecision();
	Float::setDefaultPrecision(safe_prec);
	Float tiny = 1.0;
	tiny.setExponent(mpfr_get_emin() + 1);
	Float eps = 1.0;
	eps.setExponent(-prec);
	abscissas_and_weights = new Float[8000];
	intdeini(8000, tiny, eps, abscissas_and_weights);
	Float::setDefaultPrecision(last);
}

TanhSinh::~TanhSinh(){ delete[] abscissas_and_weights; }

void TanhSinh::integrate()
{
	auto last = Float::getDefaultPrecision();
	Float::setDefaultPrecision(safe_prec);
	intde(integrand, lower_bound, upper_bound, abscissas_and_weights, &integral_approximation, &error_approximation);
	Float::op_prec_round(integral_approximation, user_prec);
	Float::setDefaultPrecision(last);
}

void TanhSinh::setBounds(const Float &a, const Float &b)
{
	setLowerBound(a);
	setUpperBound(b);
}

void TanhSinh::setIntegrand(Integrand i) { integrand = i; }
void TanhSinh::setUpperBound(const Float &b) { upper_bound = b; }
Float &TanhSinh::getUpperBound() { return upper_bound; }
void TanhSinh::setLowerBound(const Float &b) { lower_bound = b; }
Float &TanhSinh::getLowerBound() { return lower_bound; }
bool TanhSinh::isConvergentError() { return error_approximation.getSign() == -1; }
const Float &TanhSinh::getAbsoluteErrorApprox() { return error_approximation; }
const Float &TanhSinh::getIntegralApprox() { return integral_approximation; }