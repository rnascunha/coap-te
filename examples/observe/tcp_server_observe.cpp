/**
 * This examples implements a TCP server with 3 resources that can be
 * observed, and the discovery '.well-know/core' resource.
 *
 * We are going to create a resource tree as presented below.
 *
 * 			           root
 * 			____________|____________
 * 			|	 |	   	   |		 |
 * 		   time	type	sensors	.well-known
 *									 |
 *					 				core
 *
 * A overview of the resources:
 * -------
 * * time: Observable. Returns the device current time. If register to observe will receive
 * time every 30 seconds.
 * -------
 * * sensors: Observable. Returns random values simulating a
 * read sensor. If you register to observe this resource, you can define the frequency
 * that you want to receive notifications using the query 'freq=<frequency>'.
 * -------
 * * type: Observable. Request the last typed key that was typed (you must type something,
 * and enter...). It will only send if the state change (type a different key).
 * -------
 * * .well-known/core: provide resource information as defined at RFC6690
 * ------
 *
 * This example is to be run with the 'tcp_client_observe' example.
 */

#include <cstdio>
#include <chrono>
#include <thread>

#include "log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

using namespace CoAP::Log;

/**
 * Example log module
 */
static constexpr module example_mod = {
		/*.name = */"EXAMPLE",
		/*.max_level = */CoAP::Log::type::debug
};

/**
 * Endpoint definition
 */
#define COAP_PORT		CoAP::default_port		//5683
#define HOST_ADDR		"127.0.0.1"				//Address

/**
 * Endpoint type
 */
using endpoint_t = CoAP::Port::POSIX::endpoint_ipv4;

/**
 * Observers must be held in a container. To use the vector implementation,
 * uncomment the following line.
 */
//#define USE_OBSERVER_VECTOR

static constexpr const CoAP::Transmission::Reliable::csm_configure csm = {
		/*.max_message_size = */CoAP::Transmission::Reliable::default_max_message_size,
		/*.block_wise_transfer = */true
};

/**
 * Reliable Server Engine definition
 *
 * To know how to define a engine, see 'engine_tcp_client'/'engine_tcp_server'
 * examples
 */
using engine = CoAP::Transmission::Reliable::engine_server<
		CoAP::Port::POSIX::tcp_server<			///< TCP server socket definition
			endpoint_t
		>,
		csm,									///< CSM paramenter configuration
		CoAP::disable,							///< Connection list disabled
		CoAP::disable,							///< Trasaction list disabled
		CoAP::Transmission::Reliable::default_cb<CoAP::Port::POSIX::tcp_server<endpoint_t>::handler>,	///< Default callback signature function
		CoAP::Resource::resource<				/// Resource definition
			CoAP::Resource::callback_reliable<
				CoAP::Message::Reliable::message,
				CoAP::Transmission::Reliable::Response<CoAP::Port::POSIX::tcp_server<endpoint_t>::handler>
												///< TCP server socket definition
			>,
		true>
	>;

/**
 * Response callback declaration
 *
 * Detailed explanation of the callbacks below 'main'
 */
static void get_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_type_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_sensor_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_discovery_handler(engine::message const& request,
								engine::response& response, void*) noexcept;

/**
 * Here we are going to define the observe variables and containers.
 */
#ifdef USE_OBSERVER_VECTOR
/**
 * the observe list_vector container can hold a unlimited number of observers.
 * As it uses 'dynamic allocation', you must include it manually.
 */
#include "observe/list_vector.hpp"

using observe_list = CoAP::Observe::list_vector<CoAP::Observe::observe<engine::socket, false>>;
#else
/**
 * The default observe container. It can hold a predefined number of observers... if
 * more observers try to register, it will reject! More suitable for constrained devices.
 */
using observe_list = CoAP::Observe::list<
						CoAP::Observe::observe<				//Observer type
							engine::socket,					//Socket
							false							//Don't store order information (used just at clients)
						>,
						5									//Observer number
					>;
#endif

/**
 * Instantiation of the observer container
 */
static observe_list time_list;		///< time observer container
static observe_list type_list;		///< type observer container
/**
 * Holds the last typed characters
 */
static char typed = 0;

/**
 * As the sensor can send a notification at a frequency defined by the user,
 * we must hold the frequency value and the initial time it start to observe.
 *
 * So we are going to define our own observe type to hold all the necessary values,
 * to each registered user.
 */
class Sensor_Observe{
	public:
		using observe_type = CoAP::Observe::observe<engine::socket, false>;
		using endpoint_t = observe_type::endpoint_t;
		Sensor_Observe(){};

		/**
		 * Setting values
		 */
		template<typename Message>
		void set(endpoint_t const& ep, Message const& msg, unsigned freq) noexcept
		{
			obs_.set(ep, msg);
			freq_ = freq;
			init_time_ = CoAP::time() / 1000;
		}

		/**
		 * Check if received message is associated to this instance
		 */
		template<typename Message>
		bool check(endpoint_t const& ep, Message const& msg) const noexcept
		{
			return obs_.check(ep, msg);
		}

		/**
		 * Clear observe information data
		 */
		void clear() noexcept
		{
			obs_.clear();
		}

		endpoint_t const& endpoint() const noexcept{ return obs_.endpoint(); }
		/**
		 * Check if this observe variable is been used
		 */
		bool is_used() const noexcept{ return obs_.is_used(); }

		/**
		 * This is used to compose the notification
		 */
		observe_type const& observe() const noexcept
		{ return obs_; }

		/**
		 * Getters
		 */
		CoAP::time_t init_time() const noexcept{ return init_time_; }
		unsigned frequency() const noexcept{ return freq_; }
	private:
		observe_type	obs_;
		unsigned 		freq_ = 30;
		CoAP::time_t	init_time_ = 0;
};
/**
 * Defining the sensors container and instantiating
 */
using sensor_observe_list = CoAP::Observe::list<Sensor_Observe, 10>;
sensor_observe_list sensor_list;

/**
 * Auxiliary function
 */
static void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}
/**
 * We are going to simulate all resource notification running
 * in a different thread
 */

/**
 * This thread will send every 30 seconds the device local time to all
 * registered observes...
 */
static void thread_time(engine& engine)
{
	while(true)
	{
		CoAP::time_t t = CoAP::time() / 1000;
		if((t % 30) == 0)
		{
			/**
			 * Iterate thought the container
			 */
			for(unsigned i = 0; i < time_list.size(); i++)
			{
				/**
				 * Check if is a valid observer
				 */
				if(time_list[i]->is_used())
				{
					debug(example_mod, "sending time");
					/**
					 * Making a request
					 */
					engine::request<> req{*time_list[i]};

					/**
					 * At unreliable connection, all observe notification must be
					 * associated with a observe option with a sync number. This is not
					 * necessary for realiable connection. We are just going to use a dump
					 * number (0 by preference will set to option to empty)
					 */
					unsigned num = 0;
					CoAP::Message::Option::node obs_op{CoAP::Message::Option::code::observe, num};
					req.add_option(obs_op);

					/**
					 * Seting a payload
					 */
					char buff[20];
					std::snprintf(buff, 20, "%llu", (long long unsigned)CoAP::time());

					req.payload(buff);

					/**
					 * Sending
					 *
					 * The second option MUST be 'no_transaction'. As we are not going to
					 * receive any response, we must not hold a transaction
					 */
					CoAP::Error ec;
					engine.send(req, CoAP::Transmission::Reliable::no_transaction, ec);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

/**
 * The type resource will send to all the registered observes the
 * character typed (input by the user) at the server. It will only
 * send if the character typed is different from the last one (if
 * the state changed).
 */
/**
 * Type thread reading from input every second
 */
static void thread_type(engine& engine)
{
	while(true)
	{
		/**
		 * Reading input
		 */
		char i;
		i = static_cast<char>(getchar());//static_cast<char>(std::cin.get());

		/**
		 * Check if state changed (different character)
		 */
		if(i != typed)
		{

			typed = i;
			for(unsigned j = 0; j < type_list.size(); j++)
			{
				if(type_list[j]->is_used())
				{
					debug(example_mod, "sending type");

					/**
					 * Making request
					 */
					engine::request<> req{*type_list[j], CoAP::Message::code::content};

					/**
					 * At unreliable connection, all observe notification must be
					 * associated with a observe option with a sync number. This is not
					 * necessary for realiable connection. We are just going to use a dump
					 * number (0 by preference will set to option to empty)
					 */
					unsigned num = 0;
					CoAP::Message::Option::node obs_op{CoAP::Message::Option::code::observe, num};
					req.add_option(obs_op);

					/**
					 * Setting payload
					 */
					char buff[2];
					std::snprintf(buff, 2, "%c", typed);

					req.payload(buff);

					/**
					 * Sending
					 *
					 * The second option MUST be 'no_transaction'. As we are not going to
					 * receive any response, we must not hold a transaction
					 */
					CoAP::Error ec;
					engine.send(req, CoAP::Transmission::Reliable::no_transaction, ec);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
		int c;
		while((c = getchar()) != '\n' && c != EOF){}
	}
}

/**
 * The sensor resource send sensor data at a frequency defined by the user
 */

/**
 * Function simulating a read sensor (just a random number)
 */
static unsigned read_sensor()
{
	return CoAP::random_generator() % 100;
}

/**
 * Thread checking for each observer if should send a read sensor data
 */
static void thread_sensor(engine& engine)
{
	while(true)
	{
		CoAP::time_t t = CoAP::time();
		for(unsigned i = 0; i < sensor_list.size(); i++)
		{
			if(sensor_list[i]->is_used())
			{
				/**
				 * Checking if is the frequency defined by the user
				 */
				if(t >= static_cast<CoAP::time_t>(sensor_list[i]->init_time() + sensor_list[i]->frequency())
					&& ((t - sensor_list[i]->init_time()) % sensor_list[i]->frequency()) == 0)
				{
					debug(example_mod, "sending sensor");

					/**
					 * Making the request
					 */
					engine::request<> req{sensor_list[i]->observe(),
						CoAP::Message::code::content};

					/**
					 * At unreliable connection, all observe notification must be
					 * associated with a observe option with a sync number. This is not
					 * necessary for realiable connection. We are just going to use a dump
					 * number (0 by preference will set to option to empty)
					 */
					unsigned num = 0;
					CoAP::Message::Option::node obs_op{CoAP::Message::Option::code::observe, num};
					req.add_option(obs_op);

					/**
					 * Setting payload
					 */
					char buff[20];
					std::snprintf(buff, 20, "%u", read_sensor());

					req.payload(buff);

					/**
					 * Sending
					 *
					 * The second option MUST be 'no_transaction'. As we are not going to
					 * receive any response, we must not hold a transaction
					 */
					CoAP::Error ec;
					engine.send(req, CoAP::Transmission::Reliable::no_transaction, ec);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

static void default_callback(engine::socket socket, engine::message const* response, void*) noexcept
{
	/**
	 * Checking if socket was closed (response == nullptr)
	 */
	if(!response)
	{
		status(example_mod, "Socket [%d] closed! Deregistering from any observe list", socket);
		time_list.cancel(socket);
		type_list.cancel(socket);
		sensor_list.cancel(socket);
	}
}

int main()
{
	/**
	 * At Linux, does nothing. At Windows initiate winsock
	 */
	CoAP::Port::POSIX::init();
	
	CoAP::Error ec;

	//Initiating the endpoint to bind
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	/**
	 * Initiating CoAP engine
	 */
	engine coap_engine;

	/**
	 * Open, binding and configuring socket
	 */
	coap_engine.open(ep, ec);
	if(ec) exit_error(ec, "open");

	/**
	 * Instaling default callback
	 */
	coap_engine.default_cb(default_callback);

	/**
	 * Resource instantiation
	 */
	engine::resource_node	res_time{"time", "obs", get_time_handler},
							res_sensors{"sensor", "obs", get_sensor_handler},
							res_type{"type", "obs", get_type_handler},
							res_well_known{".well-known"},
								res_core{"core", get_discovery_handler};

	debug(example_mod, "Adding resources...");
	/**
	 * Adding resource to the tree
	 *
	 * There is no checking if a resource is added cyclicly.
	 */
	res_well_known.add_child(res_core);
	coap_engine.root_node().add_child(
			res_time,
			res_sensors,
			res_type,
			res_well_known);

	/**
	 * "Printing" resource tree
	 */
	std::printf("l: path: [children]:\n");
	std::printf("--------------------\n");
	CoAP::Debug::print_resource_branch(coap_engine.root_node().node());

	/**
	 * Initiating working threads
	 */
	std::thread ttime(thread_time, std::ref(coap_engine));
	std::thread ttype(thread_type, std::ref(coap_engine));
	std::thread tsensor(thread_sensor, std::ref(coap_engine));

	debug(example_mod, "Initiating CoAP engine loop...");
	//CoAP engine loop.
	while(coap_engine.run<-1>(ec))
	{
		/**
		 * Your code
		 */
	}

	/**
	 * Joining threads
	 */
	ttime.join();
	ttype.join();
	tsensor.join();

	return EXIT_SUCCESS;
}

/**
 * Resource callback
 */

static void get_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	using namespace CoAP::Message;

	/**
	 * Observe option to be add if necessary
	 */
	unsigned num = 10;
	Option::node obs_op{Option::code::observe, num};

	/**
	 * The process method will make all the necessary checks and
	 * register/deregister as necessary. If it returns true you
	 * must add the observe option
	 */
	if(time_list.process(response.socket(), request))
	{
		status(example_mod, "Time resource obseravable register");
		response.add_option(obs_op);
	}

	/**
	 * Setting payload and serilizing
	 */
	char buff[20];
	std::snprintf(buff, 20, "%llu", (long long unsigned)CoAP::time());

	response.payload(buff).serialize();
}

static void get_type_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	using namespace CoAP::Message;

	/**
	 * Observe option to be add if necessary
	 */
	unsigned num = 10;
	Option::node obs_op{Option::code::observe, num};

	/**
	 * The process method will make all the necessary checks and
	 * register/deregister as necessary. If it returns true you
	 * must add the observe option
	 */
	if(type_list.process(response.socket(), request))
	{
		status(example_mod, "Type resource obseravable register");
		response.add_option(obs_op);
	}

	/**
	 * Setting payload and serilizing
	 */
	char buff[2];
	std::snprintf(buff, 2, "%c", typed);

	response.payload(buff).serialize();
}

static void get_sensor_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	using namespace CoAP::Message;

	/**
	 * Observe option to be add if necessary
	 */
	unsigned num = 10;
	Option::node obs_op{Option::code::observe, num};

	/**
	 * Reading if a query value was passed to set the frequency.
	 *
	 * If not use default value of 30 (seconds)
	 */
	const void* value = nullptr;
	unsigned size;
	unsigned freq = 30;
	if(query_by_key(request, "freq", &value, size) && size)
	{
		freq = static_cast<unsigned>(strtoul(static_cast<const char*>(value), nullptr, 10));
	}

	/**
	 * The process method will make all the necessary checks and
	 * register/deregister as necessary. If it returns true you
	 * must add the observe option.
	 */
	if(sensor_list.process(response.socket(), request, freq))
	{
		status(example_mod, "Sensor resource obseravable register [%u]", freq);
		response.add_option(obs_op);
	}

	/**
	 * Setting payload and serilizing
	 */
	char buff[20];
	std::snprintf(buff, 20, "%u", read_sensor());

	response.payload(buff).serialize();
}

/**
 * \/well-known/core [GET]
 *
 * Respond to request with resource information as defined at RFC6690
 */
static void get_discovery_handler(engine::message const&,
								engine::response& response, void* eng_ptr) noexcept
{
	char buffer[512];
	CoAP::Error ec;
	engine* eng = static_cast<engine*>(eng_ptr);

	/**
	 * Constructing link format resource information
	 */
	std::size_t size = CoAP::Resource::discovery(eng->root_node().node(),
			buffer, 512, ec);

	/**
	 * Checking error
	 */
	if(ec)
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload(ec.message())
			.serialize();
		return;
	}

	/**
	 * Setting content type as link format
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::application_link_format;
	CoAP::Message::Option::node content{format};

	/**
	 * Sending response
	 */
	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer, size)
		.serialize();
}
