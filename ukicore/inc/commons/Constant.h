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
#define COMP_SERVERCONF		"ukinodes"
#define COMP_MANAGER		"ukiman"
#define COMP_DBSERVER		"dbserver"
#define COMP_DBCLIENT		"dbclient"

#define SEQ_DATATYPE		"datatype"

#define METADATA_TYPE		0

/*****************************************************************************************/
namespace MessageType
{
enum
{
	/* core */
	PING = 1, PONG = 2, REPLY = 3, BACKUP = 4, HELLO_REQ = 5, NODE_STATE = 6,
	/* sequence */
	SEQ_TYPE = 50, SEQ_CREATE = 51, SEQ_DROP = 52, SEQ_NEXT = 53,
	/* collection */
	COL_CREATE = 100, COL_DROP = 101, COL_ID = 102, COL_GET = 103, COL_PUT = 104, COL_DEL = 105, COL_EXIST = 106,
	/* list */
	LIST_CREATE = 150, LIST_DROP = 151, LIST_SIZE = 152, LIST_CLEAR = 153, LIST_PUSH_BACK = 154, LIST_PUSH_FRONT = 155, LIST_INDEX = 156, LIST_REMOVE = 157, LIST_RANGE = 158,
};

} // namespace MessageType

/*****************************************************************************************/
namespace DataType
{
enum
{
	SEQUENCE = 1, COLLECTION = 2, LIST = 3,
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
