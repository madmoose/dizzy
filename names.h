#ifndef NAME_LIST_H
#define NAME_LIST_H

#include <map>
#include <string>

#include "segofs.h"

/*
struct name_t {
	segofs_t addr;
	std::string name;
};
*/

typedef std::map<segofs_t, std::string> names_t;

/*
inline
bool operator<(const name_t &a, const name_t &b) {
	return a.addr < b. addr;
}
*/

names_t *load_names(const std::string &filename);

std::string *get_name(names_t *names, segofs_t addr);

#endif
