#include <stdlib.h>
#include "Float.hpp"
#include "FloatRegister.hpp"

int count = 0;

FloatRegister::FloatRegister(const Float &init, offset_t offset) : Float(init), offset(offset) {}
FloatRegister &FloatRegister::operator=(const Float &r) { Float::operator=(r); };

FloatRegister &FloatRegister::adopt(const Float &init)
{
	if (count == registers.size())
	{
		registers.push_back(new FloatRegister(init, count));
		return *registers[count++];
	}
	else
		return *registers[count++] = init;
}

void FloatRegister::release(const FloatRegister &regist)
{
	if (count == 0)
		throw std::runtime_error("release register but no register adopted");
	FloatRegister *&entry = registers[regist.offset];
	FloatRegister *&last = registers[--count];
	std::swap(entry->offset, last->offset);
	std::swap(entry, last);
}

void FloatRegister::clean()
{
	count = 0;
	for (auto &regist : registers)
		delete regist;
	registers.resize(0);
	registers.shrink_to_fit();
}

FloatRegister &operator+(Float &a, FloatRegister &b)
{
	b += a;
	return b;
}

FloatRegister &operator+(FloatRegister &b, Float &a)
{
	b += a;
	return b;
}

FloatRegister &operator+(FloatRegister &a, FloatRegister &b)
{
	b += a;
	FloatRegister::release(a);
	return b;
}

FloatRegister &operator+(Float &a, Float &b)
{
	FloatRegister &r = FloatRegister::adopt(a);
	r += b;
	return r;
}
