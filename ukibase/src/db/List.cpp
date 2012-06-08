/*
 * List.cpp
 *
 *  Created on: May 15, 2012
 *      Author: hongsan
 */

#include "Database.h"
#include "leveldb/cache.h"
#include <glog/logging.h>
#include "commons/Constant.h"
#include "commons/MessageUtil.h"
#include "leveldb/write_batch.h"

#define HALF_RANGE (1<<31)

#define set_list_info(size, first, last, val)\
	_enc_declare_(linfo, 64);\
	_enc_put_var32_(linfo, size);\
	_enc_put_fix32_(linfo, first);\
	_enc_put_fix32_(linfo, last);\
	string val(_enc_data_ (linfo), _enc_size_ (linfo));

#define get_list_info(size, first, last, val, valid)\
	_dec_declare1_(linfo2, val);\
	_dec_get_var32_(linfo2, size);\
	_dec_get_fix32_(linfo2, first);\
	_dec_get_fix32_(linfo2, last);\
	valid = _dec_valid_(linfo2);

#define list_key_encode(key, skey)\
	_enc_declare_(key, key.size() + 32);\
	_enc_put_var32_(key, KeyType::LIST);\
	_enc_put_buffer_(key, key, key.size());\
	leveldb::Slice skey(_enc_data_(key), _enc_size_(key));

#define list_item_key_encode(key, id, slice)\
	_enc_declare_(item_##id, 64);\
	_enc_put_var32_(item_##id, KeyType::LIST_ITEM);\
	_enc_put_string_(item_##id, key);\
	_enc_put_fix32_(item_##id, id);\
	leveldb::Slice slice(_enc_data_(item_##id), _enc_size_(item_##id));\

namespace ukibase
{

int Database::list_create(string& key)
{
	list_key_encode(key, skey);
	if (exist(skey)) return ErrorCode::ITEM_EXISTED;
	set_list_info(0, HALF_RANGE, HALF_RANGE, val)
	int r = set(skey, val);
	return r;
}

int Database::list_destroy(string& key)
{
	list_key_encode(key, skey)
	string val;
	int r = get(skey, &val);
	if (r != ErrorCode::OK) return r;
	uint32_t size;
	uint64_t first, last;
	bool valid;
	get_list_info(size, first, last, val, valid);
	if (!valid) return ErrorCode::DB_ERROR;

	list_item_key_encode(key, first, sfirst);
	leveldb::ReadOptions options;
	options.fill_cache = false;
	leveldb::Iterator* it = db->NewIterator(options);
	it->Seek(sfirst);
	uint32_t i = 0;

	leveldb::WriteBatch batch;

	while (i < size && it->Valid())
	{
		batch.Delete(it->key());
		it->Next();
		i++;
	}
	batch.Delete(skey);
	leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);
	if (s.ok()) return ErrorCode::OK;
	return ErrorCode::DB_ERROR;
}

int Database::list_pushback(string& key, string& val)
{
	list_key_encode(key, skey)
	string lval;
	int r = get(skey, &lval);
	if (r != ErrorCode::OK) return r;

	uint32_t size;
	uint64_t first, last;
	bool valid;
	get_list_info(size, first, last, lval, valid);
	if (!valid) return ErrorCode::DB_ERROR;
	size++;
	last++;
	set_list_info(size, first, last, lval2);

	list_item_key_encode(key, last, slast);
	leveldb::WriteBatch batch;
	batch.Put(slast, val);
	batch.Put(skey, lval2);
	leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);
	if (s.ok()) return ErrorCode::OK;
	return ErrorCode::DB_ERROR;
}

int Database::list_pushfront(string& key, string& val)
{
	list_key_encode(key, skey)
	string lval;
	int r = get(skey, &lval);
	if (r != ErrorCode::OK) return r;

	uint32_t size;
	uint64_t first, last;
	bool valid;
	get_list_info(size, first, last, lval, valid);
	if (!valid) return ErrorCode::DB_ERROR;
	size++;
	first--;
	set_list_info(size, first, last, lval2);

	list_item_key_encode(key, first, sfirst);
	leveldb::WriteBatch batch;
	batch.Put(sfirst, val);
	batch.Put(skey, lval2);
	leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);
	if (s.ok()) return ErrorCode::OK;
	return ErrorCode::DB_ERROR;
}

int Database::list_size(string& key, uint32_t& size)
{
	list_key_encode(key, skey)
	string lval;
	int r = get(skey, &lval);
	if (r != ErrorCode::OK) return r;

	uint64_t first, last;
	bool valid;
	get_list_info(size, first, last, lval, valid);
	if (!valid) return ErrorCode::DB_ERROR;
	return ErrorCode::OK;
}

int Database::list_index(string& key, uint32_t index, string& val)
{
	list_key_encode(key, skey)
	string lval;
	int r = get(skey, &lval);
	if (r != ErrorCode::OK) return r;
	uint32_t size;
	uint64_t first, last;
	bool valid;
	get_list_info(size, first, last, lval, valid);
	if (!valid) return ErrorCode::DB_ERROR;

	list_item_key_encode(key, first, sfirst);
	leveldb::ReadOptions options;
	//options.fill_cache = false;
	leveldb::Iterator* it = db->NewIterator(options);
	it->Seek(sfirst);

	uint32_t i = 0;

	while (i < size && it->Valid())
	{
		if (i == index)
		{
			val = it->value().ToString();
			return ErrorCode::OK;
		}
		it->Next();
		i++;
	}
	return ErrorCode::IS_NOT_FOUND;
}

int Database::list_remove(string& key, uint32_t index)
{
	list_key_encode(key, skey)
	string lval;
	int r = get(skey, &lval);
	if (r != ErrorCode::OK) return r;
	uint32_t size;
	uint64_t first, last;
	bool valid;
	get_list_info(size, first, last, lval, valid);
	if (!valid) return ErrorCode::DB_ERROR;

	list_item_key_encode(key, first, sfirst);
	leveldb::ReadOptions options;
	options.fill_cache = false;
	leveldb::Iterator* it = db->NewIterator(options);
	it->Seek(sfirst);

	uint32_t i = 0;

	while (i < size && it->Valid())
	{
		if (i == index)
		{
			leveldb::WriteBatch batch;
			size--;
			set_list_info(size, first, last, val);
			batch.Put(skey, val);
			batch.Delete(it->key());
			leveldb::Status status = db->Write(leveldb::WriteOptions(), &batch);
			if (status.ok()) return ErrorCode::OK;
			else return ErrorCode::DB_ERROR;
		}
		it->Next();
		i++;
	}
	return ErrorCode::IS_NOT_FOUND;
}

int Database::list_range(string& key, uint32_t index, uint32_t& count, char* data, uint32_t& data_size)
{
	list_key_encode(key, skey)
	string lval;
	int r = get(skey, &lval);
	if (r != ErrorCode::OK) return r;
	uint32_t size;
	uint64_t first, last;
	bool valid;
	get_list_info(size, first, last, lval, valid);
	if (!valid) return ErrorCode::DB_ERROR;

	list_item_key_encode(key, first, sfirst);
	leveldb::ReadOptions options;
	//options.fill_cache = false;
	leveldb::Iterator* it = db->NewIterator(options);
	it->Seek(sfirst);

	uint32_t i = 0;

	while (i < size && it->Valid())
	{
		if (i == index)
		{
			uint32_t c = 0;
			_enc_declare2_(result, data);
			while (c < count && it->Valid())
			{
				if (it->value().size() + _enc_size_(result) > data_size) break;
				_enc_put_string_(result, it->value().ToString());
				it->Next();
				c++;
			}
			count = c;
			data_size = _enc_size_(result);
			return ErrorCode::OK;
		}
		it->Next();
		i++;
	}
	return ErrorCode::IS_NOT_FOUND;
}

int Database::list_clear(string& key)
{
	list_key_encode(key, skey)
	string val;
	int r = get(skey, &val);
	if (r != ErrorCode::OK) return r;
	uint32_t size;
	uint64_t first, last;
	bool valid;
	get_list_info(size, first, last, val, valid);
	if (!valid) return ErrorCode::DB_ERROR;

	list_item_key_encode(key, first, sfirst);
	leveldb::ReadOptions options;
	options.fill_cache = false;
	leveldb::Iterator* it = db->NewIterator(options);
	it->Seek(sfirst);
	uint32_t i = 0;

	leveldb::WriteBatch batch;

	while (i < size && it->Valid())
	{
		batch.Delete(it->key());
		it->Next();
		i++;
	}
	set_list_info(0, HALF_RANGE, HALF_RANGE, val1)
	batch.Put(skey,val1);
	leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);
	if (s.ok()) return ErrorCode::OK;
	return ErrorCode::DB_ERROR;
}

}

