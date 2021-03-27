#include "error.hpp"
#include "log.hpp"
#include "internal/tree.hpp"
#include "resource/types.hpp"
#include "resource/resource.hpp"
#include "resource/discovery.hpp"

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

//	/**
//	 * Instantiating resources
//	 */
	node root{nullptr, "description"},
			path10{"path10", "description10"}, path11{"path11", "description11"}, path12{"path12", "description12"},
			path21{"path21", "description21"}, path22{"path22", "description22"}, path23{"path23", "description23"},
			path24{"path24", "description24"}, path25{"path25", "description25"}, path26{"path26", "description26"},
			path30{"path30", "description30"}, path31{"path31", "description31"}, path32{"path32", "description32"};

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

	std::size_t size = discovery(root, buffer, BUFFER_LEN, 0, ec);
	if(ec) {
		printf("size: %lu\n", size);
		exit_error(ec, "discovery");
	}

	status("[%lu] %s", size, buffer);

//	path_list list;
//	path_node t1{"t1"}, t2{"t2"}, t3{"last"};
//	list.add(t1);
//	list.add(t2);
//	list.add(t3);
//
//	std::size_t size = description(root.value(), &list, buffer, BUFFER_LEN, ec);
//	if(ec) {
//		printf("size: %lu\n", size);
//		exit_error(ec, "description");
//	}

	return EXIT_SUCCESS;
}
