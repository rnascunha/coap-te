#ifndef COAP_TE_ERROR_CODE_HPP__
#define COAP_TE_ERROR_CODE_HPP__

namespace CoAP{

enum class errc : int{
	//General
	insufficient_buffer = 10,
	invalid_token_length = 11,
	//Code
	code_invalid = 20,
	//Options
	option_invalid = 30,
	option_out_of_order,
	option_repeated,
};

struct Error {
	int err_ = 0;
	const char* name() const noexcept;
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
