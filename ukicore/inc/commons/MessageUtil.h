/*
 * MessageUtil.h
 *
 *  Created on: Jan 16, 2012
 *      Author: nhsan
 */

#ifndef MESSAGEUTIL_H_
#define MESSAGEUTIL_H_

#define _var_uint32_(buf,nindex,value)\
{\
	buf[nindex++] = ((value & 0x7F) | (0x80));\
	if (value>=128)\
	{\
		value >>= 7;\
		buf[nindex++] = ((value & 0x7F) | (0x80));\
		if (value>=128)\
		{\
			value >>= 7;\
			buf[nindex++] = ((value & 0x7F) | (0x80));\
			if (value>=128)\
			{\
				value >>= 7;\
				buf[nindex++] = ((value & 0x7F) | (0x80));\
				if (value>=128)\
				{\
					value >>= 7;\
					buf[nindex++] = ((value & 0x7F) | (0x80));\
				}\
			}\
		}\
	}\
	buf[nindex-1] &= 0x7F;\
}

#define _var_uint64_(buf,nindex,value)\
{\
	buf[nindex++] = ((value & 0x7F) | (0x80));/*1*/\
	if (value>=128)\
	{\
		value >>= 7;\
		buf[nindex++] = ((value & 0x7F) | (0x80));/*2*/\
		if (value>=128)\
		{\
			value >>= 7;\
			buf[nindex++] = ((value & 0x7F) | (0x80));/*3*/\
			if (value>=128)\
			{\
				value >>= 7;\
				buf[nindex++] = ((value & 0x7F) | (0x80));/*4*/\
				if (value>=128)\
				{\
					value >>= 7;\
					buf[nindex++] = ((value & 0x7F) | (0x80));/*5*/\
					if (value>=128)\
					{\
						value >>= 7;\
						buf[nindex++] = ((value & 0x7F) | (0x80));/*6*/\
						if (value>=128)\
						{\
							value >>= 7;\
							buf[nindex++] = ((value & 0x7F) | (0x80));/*7*/\
							if (value>=128)\
							{\
								value >>= 7;\
								buf[nindex++] = ((value & 0x7F) | (0x80));/*8*/\
								if (value>=128)\
								{\
									value >>= 7;\
									buf[nindex++] = ((value & 0x7F) | (0x80));/*9*/\
									if (value>=128)\
									{\
										value >>= 7;\
										buf[nindex++] = ((value & 0x7F) | (0x80));/*10*/\
									}\
								}\
							}\
						}\
					}\
				}\
			}\
		}\
	}\
	buf[nindex-1] &= 0x7F;\
}

/************************************** ENCODER **************************************************/
#define _enc_declare_(enc, size)\
	unsigned char enc##_buf[size];\
	int enc##_index = 0;\

#define _enc_declare2_(enc, data)\
	unsigned char* enc##_buf=(unsigned char*)data;\
	int enc##_index = 0;\

#define _enc_size_(enc)		(enc##_index)
#define _enc_data_(enc)		((char*)enc##_buf)

#define _enc_put_byte_(enc, val)\
{\
	enc##_buf[enc##_index++] = val;\
}

#define _enc_put_fix16_(enc, val)\
{\
	enc##_buf[enc##_index++] = (val >> 8);\
	enc##_buf[enc##_index++] = (val & 0xFF);\
}

#define _enc_put_fix32_(enc, val)\
{\
	enc##_buf[enc##_index++] = (val>>24);\
	enc##_buf[enc##_index++] = ((val>>16) & 0xFF);\
	enc##_buf[enc##_index++] = ((val>>8) & 0xFF);\
	enc##_buf[enc##_index++] = (val & 0xFF);\
}

#define _enc_put_fix24_(enc, val)\
{\
	enc##_buf[enc##_index++] = ((val>>16) & 0xFF);\
	enc##_buf[enc##_index++] = ((val>>8) & 0xFF);\
	enc##_buf[enc##_index++] = (val & 0xFF);\
}

#define _enc_put_fix64_(enc, val)\
{\
	int64_t val1 = val;\
	_enc_put_fix32_(enc, (val1 >> 32));\
	_enc_put_fix32_(enc, (val1 & 0xFFFFFFFF));\
}

#define _enc_put_string_(enc, val)\
{\
	std::string val1 = val;\
	size_t size = val1.size();\
	_enc_put_var32_(enc, size);\
	for (size_t i=0;i<size; i++) enc##_buf[enc##_index++] = val1[i];\
}

#define _enc_put_buffer_(enc, val, size)\
{\
	_enc_put_var32_(enc, size);\
	for (size_t i=0; i<size; i++) enc##_buf[enc##_index++] = val[i];\
}

#define _enc_append_key_(enc, val)\
{\
	std::string val1 = val;\
	size_t size = val1.size();\
	for (size_t i=0;i<size; i++) enc##_buf[enc##_index++] = val1[i];\
}

#define _enc_put_var32_(enc, val) {uint32_t val1 = val; _var_uint32_(enc##_buf,enc##_index,val1);}
#define _enc_put_var64_(enc, val) {uint64_t val1 = val; _var_uint64_(enc##_buf,enc##_index,val1);}

#define _enc_put_msg_header_(enc, msg_type, msg_id, okey)\
{\
	_enc_put_byte_(enc, 0xEE);\
	_enc_put_byte_(enc, 0xEE);\
	_enc_put_fix16_(enc, 0)\
	_enc_put_fix24_(enc, msg_type)\
	char tmp = 0;\
	if (okey > 0) tmp += 1;\
	if (msg_id > 0) tmp += 2;\
	_enc_put_byte_(enc, tmp);\
	if (okey > 0) _enc_put_fix32_(enc, okey);\
	if (msg_id > 0) _enc_put_fix64_(enc, msg_id);\
}

#define _enc_set_msg_size_(enc, size)\
{\
	enc##_buf[2] = ((size >>8) & 0xFF);\
	enc##_buf[3] = (size & 0xFF);\
}

#define _enc_update_msg_size_(enc)\
{\
	enc##_buf[2] = ((enc##_index >>8) & 0xFF);\
	enc##_buf[3] = (enc##_index & 0xFF);\
}
/************************************** DECODER **************************************************/
#define _dec_declare1_(dec, data)\
	const unsigned char* dec##_buf = (unsigned char*)data.c_str();\
	int dec##_index = 0;\
	int dec##_size = data.size();\
	bool dec##_valid = true;

#define _dec_declare2_(dec, data, size)\
	const unsigned char* dec##_buf = (unsigned char*)data;\
	int dec##_index = 0;\
	int dec##_size = size;\
	bool dec##_valid = true;

#define _dec_valid_(dec)	(dec##_valid)

#define _dec_get_byte_(dec, val)\
{\
	if (dec##_index > dec##_size - 1) dec##_valid = false;\
	else val = dec##_buf[dec##_index++];\
}

#define _dec_get_fix16_(dec, val)\
{\
	if (dec##_index > dec##_size - 2) dec##_valid = false;\
	else\
	{\
		val = dec##_buf[dec##_index++];\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
	}\
}

#define _dec_get_fix32_(dec, val)\
{\
	if (dec##_index > dec##_size - 4) dec##_valid = false;\
	else\
	{\
		val = dec##_buf[dec##_index++];\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
	}\
}

#define _dec_get_fix64_(dec, val)\
{\
	if (dec##_index > dec##_size - 8) dec##_valid = false;\
	else\
	{\
		val = dec##_buf[dec##_index++];\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
		val = (val<<8) + (dec##_buf[dec##_index++] & 0xFF);\
	}\
}

#define _dec_get_string_(dec, val)\
{\
	if (dec##_index > dec##_size - 1) dec##_valid = false;\
	else\
	{\
		int size;\
		_dec_get_var32_(dec, size);\
		if (dec##_index > dec##_size - size) dec##_valid = false;\
		else\
		{\
			std::string ret( size, 0 );\
			for (int i=0;i<size;i++) ret[i]=dec##_buf[dec##_index++];\
			val.swap(ret);\
		}\
	}\
}

#define _dec_get_var32_(dec, val)\
{\
	unsigned char* m = (unsigned char*) (dec##_buf + dec##_index);\
	val = (m[0] & 0x7F);\
	if (m[0] >= 128)\
	{\
		val += ((m[1] & 0x7F)<<7);\
		if (m[1] >= 128)\
		{\
			val += ((m[2] & 0x7F)<<14);\
			if (m[2] >= 128)\
			{\
				val += ((m[3] & 0x7F)<<21);\
				if (m[3] >= 128)\
				{\
					val += ((m[4] & 0x7F)<<28);\
					dec##_index += 5;\
				}\
				else dec##_index += 4;\
			}\
			else dec##_index += 3;\
		}\
		else dec##_index += 2;\
	}\
	else dec##_index += 1;\
}

#define _dec_get_var64_(dec, val)\
{\
	unsigned char* m = (unsigned char*) (dec##_buf + dec##_index);\
	val = (m[0] & 0x7F);\
	if (m[0] >= 128)\
	{\
		val += ((m[1] & 0x7F)<<7);\
		if (m[1] >= 128)\
		{\
			val += ((m[2] & 0x7F)<<14);\
			if (m[2] >= 128)\
			{\
				val += ((m[3] & 0x7F)<<21);\
				if (m[3] >= 128)\
				{\
					val += ((m[4] & 0x7F)<<28);\
					if (m[4] >= 128)\
					{\
						val += ((m[5] & 0x7F)<<28);\
						if (m[5] >= 128)\
						{\
							val += ((m[6] & 0x7F)<<28);\
							if (m[6] >= 128)\
							{\
								val += ((m[7] & 0x7F)<<28);\
								if (m[7] >= 128)\
								{\
									val += ((m[8] & 0x7F)<<28);\
									if (m[8] >= 128)\
									{\
										val += ((m[9] & 0x7F)<<28);\
										dec##_index += 10;\
									}\
									else dec##_index += 9;\
								}\
								else dec##_index += 8;\
							}\
							else dec##_index += 7;\
						}\
						else dec##_index += 6;\
					}\
					else dec##_index += 5;\
				}\
				else dec##_index += 4;\
			}\
			else dec##_index += 3;\
		}\
		else dec##_index += 2;\
	}\
	else dec##_index += 1;\
}

#endif /* MESSAGEUTIL_H_ */
