#include "names.h"

#include <iostream>
#include <fstream>

template <class charT, class traits>
  std::basic_istream<charT,traits>& operator>> (std::basic_istream<charT,traits>& is, const char& ch )
{
	char cc;
	is >> cc;
	if (cc != ch)
		is.setstate(std::ios::badbit);
	return is;
}

names_t *load_names(const std::string &filename)
{
	std::ifstream ifs(filename.c_str());

	uint16_t seg, ofs;
	std::string type, name;

	names_t *names = new names_t();

	while (ifs.good())
	{
		ifs >> std::hex >> seg >> ':' >> ofs;
		ifs >> type >> name;
		if (!ifs.good())
			break;

		names->insert(std::make_pair(segofs_t(seg, ofs), name));
	}

	ifs.close();

	return names;
}

std::string *get_name(names_t *names, segofs_t addr)
{
	if (!names)
		return 0;

	names_t::iterator i = names->find(addr);

	if (i != names->end())
		return &(i->second);
	
	return 0;
}
