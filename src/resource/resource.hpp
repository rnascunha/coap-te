#ifndef COAP_TE_RESOURCE_HPP__
#define COAP_TE_RESOURCE_HPP__

#include "message/factory.hpp"
#include "message/types.hpp"

namespace CoAP{
namespace Resource{

using resource_cb = void(CoAP::Message::Factory&, CoAP::Message::message const&);

class resource
{
	public:
	resource(const char* path,
			resource_cb get = nullptr, resource_cb post = nullptr,
			resource_cb put = nullptr, resource_cb cdelete = nullptr)
	: path_(path), get_(get), post_(post), put_(put), cdelete_(cdelete){}
	private:
		resource_cb*	get_;
		resource_cb*	post_;
		resource_cb*	put_;
		resource_cb*	cdelete_;
		const char* 	path_;
};

}//Resource
}//CoAP



#endif /* COAP_TE_RESOURCE_HPP__ */
