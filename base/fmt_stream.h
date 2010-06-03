#ifndef FMT_STREAM_H
#define FMT_STREAM_H

#include "raw_stream.h"

#include <string>

class fmt_stream {
	uint _col;
	std::string _line_id;
public:
	fmt_stream();

	void set_line_id(const char *fmt, ...);
	uint col();

	void set_col(uint col);

	void putchar(int c);
	void printf(const char *fmt, ...);
	void puts(const char *s);
	void endl();
};

#endif
