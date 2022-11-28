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

std::string insert_zeros(int nbr)
{
	if (nbr >= 0 && nbr <= 9)
		return (to_string(00));
	if (nbr >= 10 && nbr <= 99)
		return (to_string(0));
	return(std::string());
}

#endif
