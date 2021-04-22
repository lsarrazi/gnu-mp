#include "Float.hpp"
#include "FloatRegister.hpp"

int main()
{
	Float a(256, 1.4);
	Float b(256, 2.6);

	FloatRegister& r = a + b + b + a;

	std::cout << "output:" << r.toString() << std::endl;

	FloatRegister::clean();
}