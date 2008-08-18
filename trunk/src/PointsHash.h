/*
 PointsHash.h
 Seperates include files, structure definitions (for comparison argument)
 and typedefs for the hash table format used by FeatureSpace and FeatureSpaceGL.

 This was placed in a seperate header to isolate errors while porting sample
 code and finalizing the definition.
 */

#ifndef POINTS_HASH_H
#define POINTS_HASH_H

using namespace __gnu_cxx;
#include <ext/hash_map>

struct eqUnsignedInt
{
	bool operator()(unsigned int u1, unsigned int u2) const
	{
		return u1 == u2;
	}
};

typedef hash_map<unsigned int, unsigned int, hash<unsigned int>, eqUnsignedInt> points_hash_t;

#endif //POINTS_HASH_H
