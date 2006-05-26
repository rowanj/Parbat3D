#include "MathUtils.h"

int MathUtils::ipow(int value, int exponent)
{
	int rvalue = value;
	
	if (exponent == 0) return 1;

	while (exponent < 0) {
		rvalue = rvalue / value;
		exponent++;
	}
	
	while (exponent > 1) {
		rvalue = rvalue * value;
		exponent --;
	}
	
	return rvalue;
}
