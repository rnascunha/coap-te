/**
 * This examples shows how to construct a /.well-known/core resource
 * information, based on RFC6690.
 */

#include "error.hpp"
#include "log.hpp"
#include "internal/tree.hpp"
#include "resource/types.hpp"
#include "resource/resource.hpp"
#include "resource/discovery.hpp"
#include "resource/link_format.hpp"

#include "debug/print_resource.hpp"

#define BUFFER_LEN		512

using namespace CoAP::Log;
using namespace CoAP::Resource;

/**
 * Comment the following line to disable description
 */
#define ENABLE_DESCRIPTION

#ifdef ENABLE_DESCRIPTION
#define DESCRIPTION		true
#else
#define DESCRIPTION		false
#endif /* ENABLE_DESCRIPTION */
/**
 * Defining a very simple resource just for test
 */
using endpoint = int;									//fake endpoint, just for example
using resource_t = resource<callback<endpoint>, DESCRIPTION>;	//defining a resource
using node = CoAP::branch<resource_t>;					//defining a node from resource

/**
 * Auxiliary function
 */
void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(ec, what);
	exit(EXIT_FAILURE);
}

int main()
{
	CoAP::Error ec;
	char buffer[BUFFER_LEN];

	debug("Initiating Resource discovery example...");

	/**
	 * Instantiating resources
	 */
	node root{nullptr, "title=\"root\" rt=sensor outsensor;if=my_sensor;href=/teste"},
			path10{"path10", "ct=40"}, path11{"path11", "sz=2345"}, path12{"path12", "anchor=/path21"},
			path21{"path21", "title=\"my title path\""}, path22{"path22", "rel=alternate"}, path23{"path23", "description23"},
			path24{"path24", "title=\"path 24 title\""}, path25{"path25", "if=sensor"}, path26{"path26", "if=\"test\""},
			path30{"path30", "some random stuff"}, path31{"path31", "\"description31\""}, path32{"path32", "ct="};

	/**
	 * Making resource tree
	 */
	root.add_child(path10, path11, path12);
	path10.add_child(path21, path22);
	path11.add_child(path23, path24, path25);
	path12.add_child(path26);
	path21.add_child(path30);
	path22.add_child(path31);
	path26.add_child(path32);

	/**
	 * This call will write to the buffer the /.well-known/core resource
	 * information with link format content format
	 *
	 * (*) Max depth: set how many levels it will read depth at
	 * the resource tree. 0 disables (read ALL levels).
	 * (**) Criteria: callback function that checks which resources will be
	 * included. If you don't provide this argument, the default criteria 
	 * (CoAP::Resource::default_criteria) will be used. The default criteria
	 * includes any resource that have at least one callback function registered.
	 * As our example we don't register any, it would return nothing. So. we are 
	 * using the CoAP::Resource::no_criteria that includes ALL resources.
	 */
	std::size_t size = discovery(root, 				// first branch
								buffer, BUFFER_LEN, // buffer were the data will be written
								0,					// (*) max depth (0 = no max)
								no_criteria,		// (**) criteria
								ec);				// error report
	//Checking resource;
	if(ec) exit_error(ec, "discovery");

	/**
	 * Printing result
	 */
	status("[%zu]: %.*s", size, static_cast<int>(size), buffer);

	/**
	 * Parsing
	 */
	debug("Parsing values...\n");
	/**
	 * We could use the 'print_link_format', but we want to show how to
	 * parse a link format buffer
	 */
	//CoAP::Debug::print_link_format(buffer, size);
	using namespace CoAP::Resource::Link_Format;

	/**
	 * Link format parser
	 */
	Parser parser(buffer, size);
	link_format const* lf;	//Holds resource description struct
	//Runs until the last resource
	while((lf = parser.next()))
	{
		std::printf("[%zu] %.*s\n",
				lf->link_len, static_cast<int>(lf->link_len), lf->link);

		/**
		 * Each resource can have many attributes.
		 * We are going to iterate through all
		 */
		Parser_Attr parser_attr(lf->description, lf->desc_len);
		attribute const* la;	//Holds attritube struct
		//Runs until the last attribute
		while((la = parser_attr.next()))
		{
			std::printf("\tattr[%zu]: %.*s\n",
					la->attr_len, static_cast<int>(la->attr_len), la->attr);

			/**
			 * Each attribute can have more than one value
			 * We are going to iterate through all
			 */
			Parser_Value parser_value(la);
			link_value const* lv;	//Holds value struct
			//Runs until the last value
			while((lv = parser_value.next()))
			{
				std::printf("\t\tvalue[%zu]: %.*s\n",
						lv->value_len, static_cast<int>(lv->value_len), lv->value);
			}
		}
	}


	return EXIT_SUCCESS;
}
