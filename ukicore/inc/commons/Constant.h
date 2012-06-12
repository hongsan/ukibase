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

/*****************************************************************************************/
namespace MessageType
{
enum
{
	/* core */
	PING = 1, PONG = 2, REPLY = 3, BACKUP = 4, HELLO_REQ = 5, NODE_STATE = 6,
	/* object */
	OBJ_SET = 100, OBJ_GET = 101, OBJ_DEL = 102, OBJ_EXIST = 103, OBJ_INC = 104,
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
