/*
 * Constant.h
 *
 *  Created on: Feb 20, 2012
 *      Author: nhsan
 */

#ifndef CONSTANT_H_
#define CONSTANT_H_

#define NUMBER_REPLICAS 100
#define MAXPACKETSIZE 	(63*1024)
#define REPLY_CONTEXT 	1

/* component id */
#define COMP_SERVERCONF		"serverconf"
#define COMP_MANAGER		"manager"

/*****************************************************************************************/
namespace MessageType
{
enum
{
	/* core */
	PING = 1, PONG = 2, REPLY = 3, BACKUP = 4, HELLO_REQ = 5,
	/* filedb  */
	CREATE = 80, WRITE = 81, READ = 82, SET_SIZE = 83, GET_SIZE = 84,
	/* object */
	OBJ_SET = 100, OBJ_GET = 101, OBJ_DEL = 102,
	/* sequencer */
	SEQ_CREATE = 113, SEQ_DESTROY=114, SEQ_INC = 115, SEQ_DEC = 116,
	/* list */
	LIST_CREATE = 120, LIST_DESTROY = 121, LIST_PUSHBACK = 122, LIST_PUSHFRONT = 123, LIST_SIZE = 124, LIST_INDEX = 125, LIST_REMOVE = 126, LIST_RANGE = 127,
};
} // namespace MessageType

/*****************************************************************************************/
namespace ErrorCode
{

enum
{
	/* core */
	OK = 0, IO_ERROR = 1, TIMEOUT = 2, DB_ERROR = 3, IS_NOT_FOUND = 4, BAD_FORMAT = 5, ITEM_EXISTED = 6, NO_MORE_SPACE = 7
};

} // namespace ErrorCode

/*****************************************************************************************/
namespace KeyType
{

enum
{
	STRING = 1, INT = 2, DOUBLE = 3, SEQ = 4, LIST = 5, LIST_ITEM = 6
};

} // namespace KeyType

#endif /* CONSTANT_H_ */
