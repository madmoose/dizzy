#include "misc.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef _WIN32
int asprintf(char **strp, const char *fmt, ...)
{
	va_list args;
	int len;
	va_start(args, fmt);
	len = vsnprintf(0, 0, fmt, args);

	*strp = (char*)malloc(len+1);
	if (!*strp)
		return -1;

	vsnprintf(*strp, len+1, fmt, args);

	va_end(args);
	return len;
}
#endif
