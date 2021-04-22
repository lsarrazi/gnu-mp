#pragma once
#include <stdlib.h>
#include "Float.hpp"

extern int count;

class FloatRegister : public Float
{
	typedef int offset_t;

	static std::vector<FloatRegister *> registers;

public:
	offset_t offset;

	FloatRegister(const Float &init, offset_t offset);
	FloatRegister& operator=(const Float& r);

	static FloatRegister &adopt(const Float &init);
	static void release(const FloatRegister &regist);
	static void clean();
};

FloatRegister &operator+(Float &a, FloatRegister &b);
FloatRegister &operator+(FloatRegister &b, Float &a);
FloatRegister &operator+(FloatRegister &a, FloatRegister &b);
FloatRegister &operator+(Float &a, Float &b);