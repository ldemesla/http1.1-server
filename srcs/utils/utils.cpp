#include "../webserver.hpp"

int		ft_strlen(char *s)
{
	int i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	*ft_free(void *ptr)
{
	if (ptr != NULL)
	{
		delete((char *)ptr);
		ptr = NULL;
	}
	return (ptr);
}

std::string ft_int_to_hex(int nbr, int mode)
{
	char            c;
	std::string     temp;
	std::string     result;
	do
	{
		c = nbr % 16;
		temp = (c > 9 ? c + (mode == UPPER_CASE ? 55 : 87): c + 48);
		result = temp + result;
		nbr /= 16;
	} while (nbr > 0);
	return (result);
}

std::string ft_int_to_string(int nbr)
{
	char            c;
	std::string     temp;
	std::string     result;
	do
	{
		c = nbr % 10;
		temp = c + 48;
		result = temp + result;
		nbr /= 10;
	} while (nbr > 0);
	return (result);
}

int		ft_hex_to_dec(const std::string &hex_str)
{
	int dec_result(0);
	
	for (int i(0); i < (int)hex_str.size(); i++)
	{
		dec_result *= 16;															 	 /* - 87     for LowerCase */ 
		dec_result += hex_str[i] >= '0' && hex_str[i] <= '9' ? hex_str[i] - 48 : hex_str[i] - 87; /*  -55š for UpperCase */
	}
	return (dec_result);
}
