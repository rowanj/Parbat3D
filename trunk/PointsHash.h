#ifndef POINTS_HASH_H
#define POINTS_HASH_H

#include <hash_map.h>
#include <ext/hash_fun.h>

struct eqUnsignedInt
{
	bool operator()(unsigned int u1, unsigned int u2) const
	{
		return u1 == u2;
	}
};

typedef hash_map<unsigned int, unsigned int, hash<unsigned int>, eqUnsignedInt> points_hash_t;

#endif //POINTS_HASH_H
