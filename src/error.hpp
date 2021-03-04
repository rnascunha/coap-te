#ifndef COAP_TE_ERROR_CODE_HPP__
#define COAP_TE_ERROR_CODE_HPP__

namespace CoAP{

enum class errc : int{
	//General
	insufficient_buffer		= 10,
	invalid_token_length,
	message_too_small,
	version_invalid,
	type_invalid,
	//Code
	code_invalid 			= 20,
	//Options
	option_invalid 			= 30,
	option_out_of_order,
	option_repeated,
	option_parse_error,
	//payload
	payload_no_marker		= 40,
	//socket
	socket_error			= 50,
	endpoint_error,
	//transmission
	no_free_slots			= 60
};

struct Error {
	int err_ = 0;

	int value() const noexcept;
	const char* message() const noexcept;
	static const char* message(int error);

	operator bool() const;
	inline bool operator==(Error const& rhs)
	{
		return err_ == rhs.err_;
	}

	inline bool operator!=(Error const& rhs)
	{
		return !(*this == rhs);
	}

	inline Error& operator=(Error const& other) noexcept
	{
		err_ = other.err_;
		return *this;
	}

	inline Error& operator=(errc const& error) noexcept
	{
		err_ = static_cast<int>(error);
		return *this;
	}
};

}//CoAP

#endif /* COAP_TE_ERROR_CODE_HPP__ */
