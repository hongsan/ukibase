/*
 * TrackingService.h
 *
 *  Created on: Feb 17, 2012
 *      Author: nhsan
 */

#ifndef REPLYSERVICE_H_
#define REPLYSERVICE_H_
#include <google/dense_hash_set>
#include "Service.h"
#include "commons/MessageUtil.h"
#include "Engine.h"
#include "Context.h"
#include "commons/Constant.h"
using namespace std;

namespace ukicore
{

class CoreService: public Service
{

public:
	bool process(message_ptr const & message, connection_ptr const & connection)
	{
		static Engine& engine = Engine::get_instance();

		if (message->type == MessageType::REPLY)
		{
			Context* c = engine.get_context(message->id);
			if (c != NULL && c->get_type() == REPLY_CONTEXT)
			{
				ReplyContext* rc = (ReplyContext*) c;
				rc->notify(message);
				return true;
			}
			else
			{
				DLOG(ERROR)<<"Received reply message without context";
			}
		}
		return false;
	}
};

} /* namespace core */
#endif /* REPLYSERVICE_H_ */
