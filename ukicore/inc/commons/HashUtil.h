/*
 * HashUtil.h
 *
 *  Created on: Feb 27, 2012
 *      Author: nhsan
 */

#ifndef HASHUTIL_H_
#define HASHUTIL_H_
#include "MurmurHash3.h"

class HashUtil
{
public:
	inline static uint32_t hash_code(const char* s)
	{
		uint32_t h = *s;
		if (h) for (++s; *s; ++s)
		{
			h = (h << 5) - h + *s;
		}
		return h;
//		uint32_t ret;
//		MurmurHash3_x86_32(s, strlen(s),0, &ret);
//		return ret;
	}

};

#endif /* HASHUTIL_H_ */
