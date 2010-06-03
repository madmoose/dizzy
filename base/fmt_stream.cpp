#include "fmt_stream.h"

#include <assert.h>
#include <ctype.h>

fmt_stream::fmt_stream()
{
	_col = 0;
}

void fmt_stream::set_line_id(const char *fmt, ...)
{
	char line_id[32];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(line_id, 32, fmt, ap);
	va_end(ap);

	// assert line_id isgraph
	_line_id = line_id;
}

uint fmt_stream::col()
{
	return _col;
}

void fmt_stream::set_col(uint acol)
{
	if (_col > acol)
		putchar('\n');
	if (_col > acol)
		return;
	while (_col < acol)
		putchar(' ');
}

void fmt_stream::putchar(int c)
{
	assert(isgraph(c) || c == ' ' || c == '\n');

	if (_col == 0)
	{
		::printf("%s ", _line_id.c_str());
		_col += _line_id.length() + 1;
	}

	++_col;
	if (c == '\n')
		_col = 0;

	::putchar(c);
}

void fmt_stream::printf(const char *fmt, ...)
{
	// First figure out the size
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	// Make a buffer
	char s[n+1];

	// Print to the buffer
	va_start(ap, fmt);
	n = vsnprintf(s, n+1, fmt, ap);
	va_end(ap);

	const char *p = s;
	while (*p)
		putchar(*p++);
}

void fmt_stream::puts(const char *s)
{
	while (*s)
		putchar(*s++);

	putchar('\n');
}
