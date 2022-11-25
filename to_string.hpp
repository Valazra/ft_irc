/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   to_string.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jholl <jholl@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/25 15:13:50 by jholl             #+#    #+#             */
/*   Updated: 2022/11/25 15:14:11 by jholl            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
