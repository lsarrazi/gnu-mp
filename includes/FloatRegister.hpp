#pragma once
#include <stdlib.h>

/**
 * FloatRegister API goal is to provide a float-like API for sugar and to facilitate re-using code of other libraries
 * */

#include "Float.hpp"

class FloatRegister : public GnuMPFloat
{
	typedef int offset_t;

	static std::vector<FloatRegister *> registers;
	static offset_t count;
	static offset_t allocated_count;
	static offset_t freed_count;

public:
	offset_t offset;

	FloatRegister(const GnuMPFloat &init, offset_t offset);
	
	FloatRegister &operator=(const GnuMPFloat &);
	FloatRegister &operator=(double);
	FloatRegister(double, offset_t);
	FloatRegister(offset_t);

	template<typename InitType>
	static FloatRegister &adopt(const InitType& init)
	{
		if (count == registers.size())
		{
			registers.push_back(new FloatRegister(init, count));
			allocated_count++;
			return *registers[count++];
		}
		else
			return *registers[count++] = init;
	}

	static FloatRegister &adopt()
	{
		if (count == registers.size())
		{
			registers.push_back(new FloatRegister(count));
			allocated_count++;
		}
		return *registers[count++];
	}

	static void release(const FloatRegister &regist);
	static void clean();
	static int getActiveRegisterCount();
	static int getAllocationCount();
	static int getFreedRegisterCount();
	static GnuMPFloat toFloat(FloatRegister& r);
	
	explicit operator int();
	explicit operator long();
	explicit operator unsigned();
	explicit operator unsigned long();

};

GnuMPFloat &operator+=(GnuMPFloat&, FloatRegister&);
GnuMPFloat &operator-=(GnuMPFloat&, FloatRegister&);
GnuMPFloat &operator*=(GnuMPFloat&, FloatRegister&);
GnuMPFloat &operator/=(GnuMPFloat&, FloatRegister&);

FloatRegister& operator-(FloatRegister&);
FloatRegister& operator-(const GnuMPFloat &);

FloatRegister &operator+(const GnuMPFloat &a, FloatRegister &b);
FloatRegister &operator+(FloatRegister &b, const GnuMPFloat &a);
FloatRegister &operator+(FloatRegister &a, FloatRegister &b);
FloatRegister &operator+(const GnuMPFloat &a, const GnuMPFloat &b);
FloatRegister &operator+(const GnuMPFloat &a, double b);
FloatRegister &operator+(FloatRegister &b, double a);
FloatRegister &operator+(double a, FloatRegister &b);
FloatRegister &operator+(double a, const GnuMPFloat &b);

FloatRegister &operator-(const GnuMPFloat &a, FloatRegister &b);
FloatRegister &operator-(FloatRegister &b, const GnuMPFloat &a);
FloatRegister &operator-(FloatRegister &a, FloatRegister &b);
FloatRegister &operator-(const GnuMPFloat &a, const GnuMPFloat &b);
FloatRegister &operator-(const GnuMPFloat &a, double b);
FloatRegister &operator-(FloatRegister &b, double a);
FloatRegister &operator-(double a, FloatRegister &b);
FloatRegister &operator-(double a, const GnuMPFloat &b);

FloatRegister &operator*(const GnuMPFloat &a, FloatRegister &b);
FloatRegister &operator*(FloatRegister &b, const GnuMPFloat &a);
FloatRegister &operator*(FloatRegister &a, FloatRegister &b);
FloatRegister &operator*(const GnuMPFloat &a, const GnuMPFloat &b);
FloatRegister &operator*(const GnuMPFloat &a, double b);
FloatRegister &operator*(FloatRegister &b, double a);
FloatRegister &operator*(double a, FloatRegister &b);
FloatRegister &operator*(double a, const GnuMPFloat &b);

FloatRegister &operator/(const GnuMPFloat &a, FloatRegister &b);
FloatRegister &operator/(FloatRegister &b, const GnuMPFloat &a);
FloatRegister &operator/(FloatRegister &a, FloatRegister &b);
FloatRegister &operator/(const GnuMPFloat &a, const GnuMPFloat &b);
FloatRegister &operator/(const GnuMPFloat &a, double b);
FloatRegister &operator/(FloatRegister &b, double a);
FloatRegister &operator/(double a, FloatRegister &b);
FloatRegister &operator/(double a, const GnuMPFloat &b);

bool operator<(FloatRegister& a, FloatRegister& b);
bool operator<(FloatRegister& a, const GnuMPFloat& b);
bool operator<(const GnuMPFloat& a, FloatRegister& b);

bool operator==(FloatRegister& a, FloatRegister& b);
bool operator==(FloatRegister& a, const GnuMPFloat& b);
bool operator==(const GnuMPFloat& a, FloatRegister& b);

bool operator!=(FloatRegister& a, FloatRegister& b);
bool operator!=(FloatRegister& a, const GnuMPFloat& b);
bool operator!=(const GnuMPFloat& a, FloatRegister& b);

bool operator>(FloatRegister& a, FloatRegister& b);
bool operator>(FloatRegister& a, const GnuMPFloat& b);
bool operator>(const GnuMPFloat& a, FloatRegister& b);

bool operator<=(FloatRegister& a, FloatRegister& b);
bool operator<=(FloatRegister& a, const GnuMPFloat& b);
bool operator<=(const GnuMPFloat& a, FloatRegister& b);

bool operator>=(FloatRegister& a, FloatRegister& b);
bool operator>=(FloatRegister& a, const GnuMPFloat& b);
bool operator>=(const GnuMPFloat& a, FloatRegister& b);



FloatRegister& atan(FloatRegister&);
FloatRegister& atan(const GnuMPFloat&);

FloatRegister& log(FloatRegister&);
FloatRegister& log(const GnuMPFloat&);

FloatRegister& exp(FloatRegister&);
FloatRegister& exp(const GnuMPFloat&);

FloatRegister& sqrt(FloatRegister&);
FloatRegister& sqrt(const GnuMPFloat&);

FloatRegister& fabs(FloatRegister&);
FloatRegister& fabs(const GnuMPFloat&);

void float_register_test();