#pragma once
#include <stdlib.h>

/**
 * FloatRegister API goal is to provide a float-like API for sugar and to facilitate re-using code of other libraries
 * */

#include "Float.hpp"

class FloatRegister : public Float
{
	typedef int offset_t;

	static std::vector<FloatRegister *> registers;
	static offset_t count;
	static offset_t allocated_count;
	static offset_t freed_count;

public:
	offset_t offset;

	FloatRegister(const Float &init, offset_t offset);
	
	FloatRegister &operator=(const Float &);
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
	static Float toFloat(FloatRegister& r);
	
	explicit operator int();
	explicit operator long();
	explicit operator unsigned();
	explicit operator unsigned long();

};

Float &operator+=(Float&, FloatRegister&);
Float &operator-=(Float&, FloatRegister&);
Float &operator*=(Float&, FloatRegister&);
Float &operator/=(Float&, FloatRegister&);

FloatRegister& operator-(FloatRegister&);
FloatRegister& operator-(const Float &);

FloatRegister &operator+(const Float &a, FloatRegister &b);
FloatRegister &operator+(FloatRegister &b, const Float &a);
FloatRegister &operator+(FloatRegister &a, FloatRegister &b);
FloatRegister &operator+(const Float &a, const Float &b);
FloatRegister &operator+(const Float &a, double b);
FloatRegister &operator+(FloatRegister &b, double a);
FloatRegister &operator+(double a, FloatRegister &b);
FloatRegister &operator+(double a, const Float &b);

FloatRegister &operator-(const Float &a, FloatRegister &b);
FloatRegister &operator-(FloatRegister &b, const Float &a);
FloatRegister &operator-(FloatRegister &a, FloatRegister &b);
FloatRegister &operator-(const Float &a, const Float &b);
FloatRegister &operator-(const Float &a, double b);
FloatRegister &operator-(FloatRegister &b, double a);
FloatRegister &operator-(double a, FloatRegister &b);
FloatRegister &operator-(double a, const Float &b);

FloatRegister &operator*(const Float &a, FloatRegister &b);
FloatRegister &operator*(FloatRegister &b, const Float &a);
FloatRegister &operator*(FloatRegister &a, FloatRegister &b);
FloatRegister &operator*(const Float &a, const Float &b);
FloatRegister &operator*(const Float &a, double b);
FloatRegister &operator*(FloatRegister &b, double a);
FloatRegister &operator*(double a, FloatRegister &b);
FloatRegister &operator*(double a, const Float &b);

FloatRegister &operator/(const Float &a, FloatRegister &b);
FloatRegister &operator/(FloatRegister &b, const Float &a);
FloatRegister &operator/(FloatRegister &a, FloatRegister &b);
FloatRegister &operator/(const Float &a, const Float &b);
FloatRegister &operator/(const Float &a, double b);
FloatRegister &operator/(FloatRegister &b, double a);
FloatRegister &operator/(double a, FloatRegister &b);
FloatRegister &operator/(double a, const Float &b);

bool operator<(FloatRegister& a, FloatRegister& b);
bool operator<(FloatRegister& a, const Float& b);
bool operator<(const Float& a, FloatRegister& b);

bool operator==(FloatRegister& a, FloatRegister& b);
bool operator==(FloatRegister& a, const Float& b);
bool operator==(const Float& a, FloatRegister& b);

bool operator!=(FloatRegister& a, FloatRegister& b);
bool operator!=(FloatRegister& a, const Float& b);
bool operator!=(const Float& a, FloatRegister& b);

bool operator>(FloatRegister& a, FloatRegister& b);
bool operator>(FloatRegister& a, const Float& b);
bool operator>(const Float& a, FloatRegister& b);

bool operator<=(FloatRegister& a, FloatRegister& b);
bool operator<=(FloatRegister& a, const Float& b);
bool operator<=(const Float& a, FloatRegister& b);

bool operator>=(FloatRegister& a, FloatRegister& b);
bool operator>=(FloatRegister& a, const Float& b);
bool operator>=(const Float& a, FloatRegister& b);



FloatRegister& atan(FloatRegister&);
FloatRegister& atan(const Float&);

FloatRegister& log(FloatRegister&);
FloatRegister& log(const Float&);

FloatRegister& exp(FloatRegister&);
FloatRegister& exp(const Float&);

FloatRegister& sqrt(FloatRegister&);
FloatRegister& sqrt(const Float&);

FloatRegister& fabs(FloatRegister&);
FloatRegister& fabs(const Float&);

void float_register_test();