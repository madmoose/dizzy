#include "base/base.h"

#include "binaries/binary.h"
#include "binaries/exe_mz.h"

#include "analyzer/exe_mz_analyzer.h"

#include <libgen.h>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Syntax: %s file.exe\n", argv[0]);
		exit(1);
	}

	raw_imstream_t is(argv[1]);

	binary_t *binary = new exe_mz_t();
	binary->load(is);

	printf("Binary identified as: %s\n", binary->name().c_str());

	exe_mz_analyzer_t analyzer;
	analyzer.init(dynamic_cast<exe_mz_t*>(binary));

	char *annotions_fn;
	asprintf(&annotions_fn, "%s.names.txt", basename(argv[1]));
	analyzer.load_annotations(annotions_fn);
	free(annotions_fn);

	analyzer.analyze();

	fmt_stream fs;
	analyzer.output(fs);
}
