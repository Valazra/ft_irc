#ifndef TO_STRING_HPP
# define TO_STRING_HPP

# include <sstream>

template <typename T>
std::string to_string(T n)
{
	std::ostringstream ss;
	ss << n;
	return (ss.str());
}	


#endif
