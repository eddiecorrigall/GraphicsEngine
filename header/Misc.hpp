#ifndef __MISC_HPP__
#define __MISC_HPP__

#include <cassert>
#include <cstdlib>

#define sizeof_member(T,F) (sizeof(((T*)0)->F))

template<typename T>
inline T random(const T& min, const T& max) {
	
	assert(min < max);
	
	T r = min + (max - min)*((double)rand() / RAND_MAX);
	
	//assert(r >= min);
	//assert(r < max);
	
	return r;
}

#endif
