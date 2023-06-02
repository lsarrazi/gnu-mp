#include <stdlib.h>
#include "Float.hpp"
#include "FloatRegister.hpp"
#include <vector>
#include <emscripten.h>

#define FLOAT_REG_FUNCTION(NAME, FUNC) FloatRegister& NAME(FloatRegister& r) { FUNC(r, r); return r; }; \
FloatRegister& NAME(const GnuMPFloat& f) { auto& r = FloatRegister::adopt(); FUNC(r, f); return r; };

std::vector<FloatRegister*> FloatRegister::registers = std::vector<FloatRegister *>();

FloatRegister::offset_t FloatRegister::count = 0;
FloatRegister::offset_t FloatRegister::allocated_count = 0;
FloatRegister::offset_t FloatRegister::freed_count = 0;

FloatRegister::FloatRegister(double v, offset_t offset) : GnuMPFloat(v), offset(offset) { }
FloatRegister::FloatRegister(const GnuMPFloat &init, offset_t offset) : GnuMPFloat(init), offset(offset) {}
FloatRegister::FloatRegister(offset_t offset) : GnuMPFloat(), offset(offset) {}
FloatRegister &FloatRegister::operator=(const GnuMPFloat &r) { GnuMPFloat::operator=(r); return *this; };
FloatRegister &FloatRegister::operator=(double r) { GnuMPFloat::operator=(r); return *this; };

GnuMPFloat &operator+=(GnuMPFloat& f, FloatRegister& r) { f += (GnuMPFloat&)r; FloatRegister::release(r); return f; };
GnuMPFloat &operator-=(GnuMPFloat& f, FloatRegister& r) { f -= (GnuMPFloat&)r; FloatRegister::release(r); return f; };
GnuMPFloat &operator*=(GnuMPFloat& f, FloatRegister& r) { f *= (GnuMPFloat&)r; FloatRegister::release(r); return f; };
GnuMPFloat &operator/=(GnuMPFloat& f, FloatRegister& r) { f /= (GnuMPFloat&)r; FloatRegister::release(r); return f; };

void FloatRegister::release(const FloatRegister &regist)
{
	if (count == 0)
	{
		std::cerr << "release register but no register adopted" << std::endl;
		throw std::runtime_error("release register but no register adopted");
	}
	FloatRegister *&entry = registers[regist.offset];
	FloatRegister *&last = registers[--count];
	std::swap(entry->offset, last->offset);
	std::swap(entry, last);
}

void FloatRegister::clean()
{
	count = 0;
	freed_count += registers.size();
	registers.clear();
}

int FloatRegister::getActiveRegisterCount() { return count; }
int FloatRegister::getAllocationCount() { return allocated_count; }
int FloatRegister::getFreedRegisterCount() { return freed_count; }

FloatRegister::operator int() { FloatRegister::release(*this); return GnuMPFloat::operator int(); }
FloatRegister::operator long() { FloatRegister::release(*this); return GnuMPFloat::operator long(); }
FloatRegister::operator unsigned() { FloatRegister::release(*this); return GnuMPFloat::operator unsigned(); }
FloatRegister::operator unsigned long() { FloatRegister::release(*this); return GnuMPFloat::operator unsigned long(); }

FloatRegister& operator-(FloatRegister& r){ r.neg(); return r; }
FloatRegister& operator-(const GnuMPFloat& f){ auto& r = FloatRegister::adopt(f); r.neg(); return r; }

FloatRegister &operator+(const GnuMPFloat &a, FloatRegister &b) { b += a; return b; }
FloatRegister &operator+(FloatRegister &b, const GnuMPFloat &a) { b += a; return b; }
FloatRegister &operator+(FloatRegister &a, FloatRegister &b) { b += a; return b; }
FloatRegister &operator+(const GnuMPFloat &a, const GnuMPFloat &b) { FloatRegister &r = FloatRegister::adopt(a); r += b; return r; }

FloatRegister &operator+(const GnuMPFloat &a, double b) { FloatRegister &r = FloatRegister::adopt(a); r += b; return r; }
FloatRegister &operator+(FloatRegister &b, double a) { b += a; return b; }
FloatRegister &operator+(double a, FloatRegister &b) { b += a; return b; }
FloatRegister &operator+(double a, const GnuMPFloat &b) { FloatRegister &r = FloatRegister::adopt(b); r += a; return r; }

FloatRegister &operator-(const GnuMPFloat &a, FloatRegister &b) { b.neg(); b += a; return b; }
FloatRegister &operator-(FloatRegister &b, const GnuMPFloat &a) { b -= a; return b; }
FloatRegister &operator-(FloatRegister &a, FloatRegister &b) { a -= b; return a; }
FloatRegister &operator-(const GnuMPFloat &a, const GnuMPFloat &b) { FloatRegister &r = FloatRegister::adopt(a); r -= b; return r; }

FloatRegister &operator-(const GnuMPFloat &a, double b) { FloatRegister &r = FloatRegister::adopt(a); r -= b; return r; }
FloatRegister &operator-(FloatRegister &b, double a) { b -= a; return b; }
FloatRegister &operator-(double a, FloatRegister &b) { b.neg(); b += a; return b; }
FloatRegister &operator-(double a, const GnuMPFloat &b) { FloatRegister &r = FloatRegister::adopt(b); r.neg(); r += a; return r; }

FloatRegister &operator*(const GnuMPFloat &a, FloatRegister &b) { b *= a; return b; }
FloatRegister &operator*(FloatRegister &b, const GnuMPFloat &a) { b *= a; return b; }
FloatRegister &operator*(FloatRegister &a, FloatRegister &b) { b *= a; return b; }
FloatRegister &operator*(const GnuMPFloat &a, const GnuMPFloat &b) { FloatRegister &r = FloatRegister::adopt(a); r *= b; return r; }

FloatRegister &operator*(const GnuMPFloat &a, double b) { FloatRegister &r = FloatRegister::adopt(a); r *= b; return r; }
FloatRegister &operator*(FloatRegister &b, double a) { b *= a; return b; }
FloatRegister &operator*(double a, FloatRegister &b) { b *= a; return b; }
FloatRegister &operator*(double a, const GnuMPFloat &b) { FloatRegister &r = FloatRegister::adopt(b); r *= a; return r; }

FloatRegister &operator/(const GnuMPFloat &a, FloatRegister &b) { GnuMPFloat::op_div(b, a, val((GnuMPFloat&)b)); return b; }
FloatRegister &operator/(FloatRegister &b, const GnuMPFloat &a) { b /= a; return b; }
FloatRegister &operator/(FloatRegister &b, FloatRegister &a) { b /= a; return b; }
FloatRegister &operator/(const GnuMPFloat &a, const GnuMPFloat &b) { FloatRegister &r = FloatRegister::adopt(a); r /= b; return r; }

FloatRegister &operator/(const GnuMPFloat &a, double b) { FloatRegister &r = FloatRegister::adopt(a); r /= b; return r; }
FloatRegister &operator/(FloatRegister &b, double a) { b /= a; return b; }
FloatRegister &operator/(double a, FloatRegister &b) { FloatRegister &r = FloatRegister::adopt(a); r /= b; return r; }
FloatRegister &operator/(double a, const GnuMPFloat &b) { FloatRegister &r = FloatRegister::adopt(a); r /= b; return r; }

bool operator<(FloatRegister& a, FloatRegister& b) { bool r = (GnuMPFloat&)a < (GnuMPFloat&)b; FloatRegister::release(a); FloatRegister::release(b); return r; }
bool operator<(FloatRegister& a, const GnuMPFloat& b) { bool r = (GnuMPFloat&)a < (GnuMPFloat&)b; FloatRegister::release(a); return r; }
bool operator<(const GnuMPFloat& a, FloatRegister& b) { bool r = (GnuMPFloat&)a < (GnuMPFloat&)b; FloatRegister::release(b); return r; }

bool operator==(FloatRegister& a, FloatRegister& b) { bool r = (GnuMPFloat&)a == (GnuMPFloat&)b; FloatRegister::release(a); FloatRegister::release(b); return r; }
bool operator==(FloatRegister& a, const GnuMPFloat& b) { bool r = (GnuMPFloat&)a == (GnuMPFloat&)b; FloatRegister::release(a); return r; }
bool operator==(const GnuMPFloat& a, FloatRegister& b) { bool r = (GnuMPFloat&)a == (GnuMPFloat&)b; FloatRegister::release(b); return r; }

bool operator!=(FloatRegister& a, FloatRegister& b) { return !(a == b); };
bool operator!=(FloatRegister& a, const GnuMPFloat& b) { return !(a == b); };
bool operator!=(const GnuMPFloat& a, FloatRegister& b) { return !(a == b); };

bool operator>(FloatRegister& a, FloatRegister& b) { return b < a; };
bool operator>(FloatRegister& a, const GnuMPFloat& b) { return b < a; };
bool operator>(const GnuMPFloat& a, FloatRegister& b) { return b < a; };

bool operator<=(FloatRegister& a, FloatRegister& b) { return !(b < a); };
bool operator<=(FloatRegister& a, const GnuMPFloat& b) { return !(b < a); };
bool operator<=(const GnuMPFloat& a, FloatRegister& b) { return !(b < a); };

bool operator>=(FloatRegister& a, FloatRegister& b) { return !(a < b); };
bool operator>=(FloatRegister& a, const GnuMPFloat& b) { return !(a < b); };
bool operator>=(const GnuMPFloat& a, FloatRegister& b) { return !(a < b); };

GnuMPFloat FloatRegister::toFloat(FloatRegister& r) { FloatRegister::release(r); return r; } 

FLOAT_REG_FUNCTION(atan, GnuMPFloat::op_atan)
FLOAT_REG_FUNCTION(log, GnuMPFloat::op_log)
FLOAT_REG_FUNCTION(exp, GnuMPFloat::op_exp)
FLOAT_REG_FUNCTION(sqrt, GnuMPFloat::op_sqrt)
FLOAT_REG_FUNCTION(fabs, GnuMPFloat::op_abs)

#include "quadrature/TanhSinh.hpp"
#include <emscripten.h>

void float_register_test()
{

	/*Float a("1.4", 1024);
	Float b("2.6", 1024);

	Float r = a*a+b*(a+b)-a-b + 1 - 2 * (a-b-(1/(b+a*0.25))); // ~1.243796e+1

	std::cout << r.toString() << std::endl;
	
	Float s = exp(sqrt(b + log(a + b)) - 1); // ~2.7089758e+0

	b *= a - a;

	std::cout << b.toString() << std::endl;
	std::cout << "s output:" << s.toString() << std::endl;

	std::cout << "s to integer:" << (int)s << std::endl;

	std::cout << "r: output: " << r.toString() << std::endl;*/

	EM_ASM(console.time('Integrator compilation'));

	TanhSinh integrator(2048);

	integrator.compile();

	EM_ASM(console.timeEnd('Integrator compilation'));

	int i = 0;

	integrator.setIntegrand([](GnuMPFloat& y, const GnuMPFloat& x, const GnuMPFloat& xc){
		if (x < -.5)
			y = 1 / sqrt(-xc * (-1 + x));
		else if (x > .5)
			y = 1 / sqrt((x+1)*xc);
		else
			y = 1 / sqrt(1 - x*x);
	});


	integrator.setBounds(-1, 1);

	EM_ASM(console.time('Integration'));

	integrator.integrate();

	EM_ASM(console.timeEnd('Integration'));

	const GnuMPFloat& integral = integrator.getIntegralApprox();

	std::cout << "prec: " << integral.getPrecision() << std::endl << "integral: " << integral.toString() << std::endl;
	
	std::cout << "calls: " << integrator.getIntegrandCallCount() << std::endl;

	std::cout << "iteration:" << i << std::endl;

	std::cout << "absolute error " << integrator.getAbsoluteErrorApprox().toString() << std::endl;

	std::cout << "relative error exponent: " << integrator.getRelativeErrorExponent() << std::endl;

	std::cout << "convergent error ? " << integrator.isConvergentError() << std::endl;

	std::cout << "active registers: " << FloatRegister::getActiveRegisterCount() << std::endl;

	std::cout << "number of alloc: " << FloatRegister::getAllocationCount() << std::endl;

	std::cout << "freed registers: " << FloatRegister::getFreedRegisterCount() << std::endl;

	

	FloatRegister::clean();

	std::cout << "freed registers after cleaning: " << FloatRegister::getFreedRegisterCount() << std::endl;
}