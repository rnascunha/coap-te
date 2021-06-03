#ifndef COAP_TE_COMMAND_LINE_ARGUMENTS_HPP__
#define COAP_TE_COMMAND_LINE_ARGUMENTS_HPP__

class Command_Line{
	public:
		Command_Line(int argc, char** argv)
			: argc_(argc), argv_(argv){}

		bool get(const char* arg, char*&) const noexcept;
		bool has(const char* arg) const noexcept;
		char* pos(unsigned position) const noexcept;

		bool operator()(const char* arg) const noexcept;
		bool operator()(const char* arg, char*&) const noexcept;
		char* operator[](unsigned) const noexcept;
	private:
		int argc_;
		char** argv_;
};

#endif /* COAP_TE_COMMAND_LINE_ARGUMENTS_HPP__ */
