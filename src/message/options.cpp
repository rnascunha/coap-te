#include "options.hpp"
#include <cstring>
#include "internal/helper.hpp"

namespace CoAP{
namespace Message{
namespace Option{

void create(option& option, content_format const& value, bool is_request /* = false */) noexcept
{
	option.ocode = is_request ? code::accept : code::content_format;
	option.length = 1;
	option.value = &value;
}

bool is_critical(code code) noexcept { return static_cast<int>(code) & 1; }
bool is_unsafe(code code) noexcept { return static_cast<int>(code) & 2; }
bool is_no_cache_key(code code) noexcept { return (static_cast<int>(code) & 0x1e) == 0x1c; }

config const * get_config(code code) noexcept
{
	for(unsigned i = 0; i < (sizeof(options) / sizeof(config)); i++)
		if(code == options[i].ocode) return &options[i];

	return nullptr;
}

option::option(){}
option::option(code code, unsigned len, const void* val) :
	ocode(code), length(len), value(val){}

option::option(code code)
{
	create<false>(*this, code);
}

option::option(code code, const char* value)
{
	create<false>(*this, code, value);
}

option::option(code code, unsigned& value)
{
	create<false>(*this, code, value);
}

option::option(content_format const& value, bool is_request /* = false */)
{
	create(*this, value, is_request);
}

option::option(code code, const void* value, unsigned length)
{
	create<false>(*this, code, value, length);
}

bool option::operator==(option const& rhs) const noexcept
{
	return ocode == rhs.ocode;
}

bool option::operator!=(option const& rhs) const noexcept
{
	return !(*this == rhs);
}

bool option::operator<(option const& rhs) const noexcept
{
	return ocode < rhs.ocode;
}

bool option::operator>(option const& rhs) const noexcept
{
	return *this > rhs;
}

bool option::operator<=(option const& rhs) const noexcept
{
	return !(*this > rhs);
}

bool option::operator>=(option const& rhs) const noexcept
{
	return !(*this < rhs);
}

bool option::operator==(code rhs) const noexcept
{
	return ocode == rhs;
}

bool option::operator!=(code rhs) const noexcept
{
	return !(*this == rhs);
}

option::operator bool() const noexcept
{
	return ocode != code::invalid;
}

bool option::is_critical() const noexcept{ return CoAP::Message::Option::is_critical(ocode); }
bool option::is_unsafe() const noexcept{ return CoAP::Message::Option::is_unsafe(ocode); }
bool option::is_no_cache_key() const noexcept{ return CoAP::Message::Option::is_no_cache_key(ocode); }

std::size_t serialized_size(option const& op, option const& before) noexcept
{
	std::size_t size = 1;
	unsigned delta = before ?
								static_cast<unsigned>(op.ocode) - static_cast<unsigned>(before.ocode)
								: static_cast<unsigned>(op.ocode);

	size += delta > 12 ? (delta < 255 ? 1 : 2) : 0;
	size += op.length > 12 ? (op.length < 255 ? 1 : 2) : 0;

	return size + op.length;
}

void exchange(option* first, option* second) noexcept
{
	option middle{code::invalid, 0, nullptr};

	std::memcpy(&middle, first, sizeof(option));
	std::memcpy(first, second, sizeof(option));
	std::memcpy(second, &middle, sizeof(option));
}

void exchange(node* first, node* second) noexcept
{
	node middle;

	std::memcpy(&middle.value, &first->value, sizeof(option));
	std::memcpy(&first->value, &second->value, sizeof(option));
	std::memcpy(&second->value, &middle.value, sizeof(option));
}

void sort(option* options, std::size_t num) noexcept
{
	option* option = nullptr;
	for(std::size_t i = 0; i < num; i++)
	{
		option = &options[i];
		std::size_t exchange_index = i;
		std::size_t j = i + 1;
		for(; j < num; j++)
			if(option->ocode > options[j].ocode)
				if(options[exchange_index].ocode > options[j].ocode)
					exchange_index = j;

		if(i != exchange_index)
			exchange(option, &options[exchange_index]);
	}
}

void sort(node* list) noexcept
{
	option* option = nullptr;
	for(node* i = list; i != nullptr; i = i->next)
	{
		option = &i->value;
		node* exchange_opt = i;
		node* j = i->next;
		for(; j != nullptr; j = j->next)
			if(option->ocode > j->value.ocode)
				if(exchange_opt->value.ocode > j->value.ocode)
					exchange_opt = j;

		if(i != exchange_opt)
			exchange(i, exchange_opt);
	}
}

unsigned parse_unsigned(option const& opt)
{
	unsigned value;
	CoAP::Helper::array_to_unsigned(static_cast<std::uint8_t const*>(opt.value), opt.length, value);

	return value;
}

}//Option
}//Message
}//CoAP

