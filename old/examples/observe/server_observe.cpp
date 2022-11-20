/**
 * This examples implements a server with 3 resources that can be
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
 * Notification can be sent in a confirmable or non-confirmable way. time and sensor resource
 * are sent non-confirmable, type resource is confirmable.
 *
 * \note When using confirmable type to send notification, it will allocate one transaction slot
 * that will be only released after receive a ack from the client. If there is no available slots,
 * You must wait to free one slot to send the remaining notifications. This behaivour is NOT handled
 * by the CoAP-te engine... So prefer using non-confirmable notifications or limit the number of
 * observers...
 *
 * This example is to be run with the 'client_observe' example.
 */

#include <cstdio>
#include <chrono>
#include <thread>
//#include <iostream>

#include "coap-te/log.hpp"				//Log header
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
using endpoint = CoAP::Port::POSIX::endpoint_ipv4;

/**
 * Observers must be held in a container. To use the vector implementation,
 * uncomment the following line.
 */
//#define USE_OBSERVER_VECTOR

/**
 * Engine definition
 *
 * To know how to define a engine, see 'raw_engine'/'server_engine' examples
 */
using engine = CoAP::Transmission::engine<
		CoAP::Port::POSIX::udp<endpoint>,			/* socket type */
		CoAP::Message::message_id,					/* message id generator type */
		CoAP::Transmission::transaction_list<		/* transaction list */
			CoAP::Transmission::transaction<		/* transaction list */
				512,									/* transaction packet size */
				CoAP::Transmission::transaction_cb,		/* transaction callback type */
				endpoint>,								/* transaction endpoint type */
			4>,										/* number of transaction */
		CoAP::disable,								/* default callback disabled */
		CoAP::Resource::resource<					/* resource */
			CoAP::Resource::callback<endpoint>,			/* resource callback type */
			true										/* enabling resource description */
		>
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

using observe_list = CoAP::Observe::list_vector<CoAP::Observe::observe<endpoint, false>>;
#else
/**
 * The default observe container. It can hold a predefined number of observers... if
 * more observers try to register, it will reject! More suitable for constrained devices.
 */
using observe_list = CoAP::Observe::list<
						CoAP::Observe::observe<				//Observer type
							endpoint,						//Endpoint
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
		using observe_type = CoAP::Observe::observe<endpoint, false>;
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
					 *
					 * By default it uses non-confirmable type message
					 */
					engine::request req{*time_list[i]};

					/**
					 * All observe notification must be associated with a observe
					 * option with a sync number.
					 *
					 * We are going to generate this number based on the system clock
					 */
					unsigned num = CoAP::Observe::generate_option_value_from_clock();
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
					 */
					CoAP::Error ec;
					engine.send(req, ec);
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
 *
 * All notitications from this example are sent confirmable.
 */

/**
 * As the messages are sent confirmable, a ack message is expected back.
 *
 * If we do not receive a ack (transaction timeout) we are going to remove
 * the observer from the observe list.
 *
 * This callback will be call at timeout/success transaction
 */
static void type_callback(const void* transaction,
		engine::message const*,
		void*) noexcept
{
	auto const* t = static_cast<engine::transaction_t const*>(transaction);
	if(t->status() == CoAP::Transmission::status_t::timeout)	//Checking if it was timeout
	{
		/**
		 * Message timeout, removing from list
		 */
		status(example_mod, "Type Observable timeout... removing from list");
		type_list.remove(t->endpoint(), t->request());
	}
}

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
					 * Making a confirmable request
					 */
					engine::request req{*type_list[j],
						CoAP::Message::type::confirmable,
						CoAP::Message::code::content};

					/**
					 * Setting callback that will remove observer if
					 * transaction timeout
					 */
					req.callback(type_callback);

					/**
					 * All observe notification must be associated with a observe
					 * option with a sync number.
					 *
					 * We are going to generate this number based on the system clock
					 */
					unsigned num = CoAP::Observe::generate_option_value_from_clock();
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
					 */
					CoAP::Error ec;
					engine.send(req, ec);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
		//Clearing buffer
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
	/**
	 * Variable that will be updated and sent as observe value
	 */
	unsigned option_value = 0;
	while(true)
	{
		CoAP::time_t t = CoAP::time() / 1000;
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
					engine::request req{sensor_list[i]->observe(),
						CoAP::Message::code::content};

					/**
					 * All observe notification must be associated with a observe
					 * option with a sync number.
					 *
					 * We are going to generate this value by updating a variable
					 */
					unsigned num = ++option_value;
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
					 */
					CoAP::Error ec;
					engine.send(req, ec);
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main()
{
	/**
	 * At Linux, does nothing. At Windows initiate winsock
	 */
	CoAP::Port::POSIX::init();
	
	CoAP::Error ec;
	/**
	 * Socket
	 */
	//Initiating the endpoint to bind
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	engine::connection socket;

	socket.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");
	socket.bind(ep, ec);
	if(ec) exit_error(ec, "Error trying to bind socket...");

	debug(example_mod, "Socket opened and binded...");

	debug(example_mod, "Initiating server engine...");
	/**
	 * Initiating CoAP engine
	 */
	engine coap_engine(std::move(socket),
			CoAP::Message::message_id((unsigned)CoAP::time()));

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
	while(coap_engine(ec))
	{
		/**
		 * Your code
		 */
	}

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
	if(time_list.process(response.endpoint(), request))
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
	if(type_list.process(response.endpoint(), request))
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
	if(sensor_list.process(response.endpoint(), request, freq))
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
