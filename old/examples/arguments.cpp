#include "arguments.hpp"
#include <cstring>
#include <cstdio>

bool Command_Line::get(const char* arg, char*& value) const noexcept
{
	for(int i = 1; i < argc_; i++)
	{
		unsigned count = 0;
		while(argv_[i][count] == '-') count++;
		if(count > 0)
		{
			if(argv_[i][count] == '\0') break;
			char* comp = &argv_[i][count];
			while(true)
			{
				if(*arg == '\0')
				{
					if(*comp == '\0')
					{
						value = comp;
						return true;
					}
					else if(*comp == '=')
					{
						value = comp + 1;
						return true;
					}
					break;
				}
				if(*comp == '\0') break;
				if(*comp != *arg) break;
				comp++; arg++;
			}
		}
	}
	return false;
}

bool Command_Line::has(const char* arg) const noexcept
{
	for(int i = 1; i < argc_; i++)
	{
		unsigned count = 0;
		while(argv_[i][count] == '-') count++;
		if(count > 0)
		{
			if(argv_[i][count] == '\0') return false;
			if(std::strcmp(&argv_[i][count], arg) == 0)
			{
				return true;
			}
		}
	}
	return false;
}

char* Command_Line::pos(unsigned position) const noexcept
{
	unsigned count = 0;
	bool is_pos = false;
	for(int i = 1; i < argc_; i++)
	{
		if(!is_pos)
		{
			unsigned count_ = 0;
			while(argv_[i][count_] == '-') count_++;
			if(count_ > 0)
			{
				if(argv_[i][count_] == '\0')
				{
					is_pos = true;
				}
				continue;
			}
		}

		if(position == count) return argv_[i];
		count++;
	}
	return nullptr;
}

bool Command_Line::operator()(const char* arg) const noexcept
{
	return has(arg);
}

bool Command_Line::operator()(const char* arg, char*& value) const noexcept
{
	return get(arg, value);
}

char* Command_Line::operator[](unsigned position) const noexcept
{
	return pos(position);
}
