#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int is_ctext(char c)
{
	return isalnum(c) || ispunct(c);
}

int check_file(const char *fn)
{
	int r, fd, line, empty_line;
	struct stat sb;
	char *buf, *p, *end, *bol, c;

	//printf("Checking %s\n", fn);

	fd = open(fn, O_RDONLY);
	assert(fd);

	r = fstat(fd, &sb);
	assert(r == 0);

	buf = malloc(sb.st_size);
	assert(buf);

	r = read(fd, buf, sb.st_size);
	assert(r == sb.st_size);

	bol = p = buf;
	end = buf + sb.st_size;

	r = line = 0;

	while (p != end)
	{
		bol = p;
		++line;

		while (p != end && *p == '\t')
			++p;
		while (p != end && *p == ' ')
			++p;
		while (p != end && (*p == ' ' || is_ctext(*p)))
			++p;
		if (p > bol && !is_ctext(p[-1]))
		{
			printf("%s %d:%d:  Whitespace at end of line\n", fn, line, (int)(p - bol));
			r = 1;
		}
		if (*p == '\n')
			++p;
		else
		{
			if (p == end)
				printf("%s %d:%d:  No newline at end of file\n", fn, line, (int)(p - bol));
			else
				printf("%s %d:%d:  Invalid char '%c'\n", fn, line, (int)(p - bol), *p);
			r = 1;
			goto prologue;
		}
		empty_line = (p - bol) == 1;
	}

	if (empty_line)
	{
		printf("%s %d:%d:  Empty line at end of file!\n", fn, line, (int)(p - bol));
		r = 1;
	}

prologue:
	close(fd);
	free(buf);
	return r;
}

int main(int argc, char **argv)
{
	int arg, r = 0;
	for (arg = 1; arg != argc; ++arg)
		r = r | check_file(argv[arg]);
	return r;
}
