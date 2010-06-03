#include "base/base.h"

#include "binaries/binary.h"
#include "binaries/exe_mz.h"

#include "analyzer/exe_mz_analyzer.h"

int main(int argc, char **argv)
{
	/*
	if (argc != 2)
	{
		printf("Syntax: %s file.exe\n", argv[0]);
		exit(1);
	}

	raw_imstream_t is(argv[1]);
	*/

	raw_imstream_t is("/Users/madmoose/Development/bins/zak.exe");

	binary_t *binary = new exe_mz_t();
	binary->load(is);

	printf("Binary identified as: %s\n", binary->name().c_str());

	exe_mz_analyzer_t analyzer;
	analyzer.init(dynamic_cast<exe_mz_t*>(binary));

	analyzer.analyze();

	fmt_stream fs;
	analyzer.output(fs);

	//uint32 base = 0x1000;
	//loader->load(is, base);

	/*
	puts("1");
	fmt_stream os;
	puts("2");

	os.set_line_id("seg000:0001");
	os.puts("; this is a test!");
	os.puts("; Hallo? :)");

	os.set_line_id("seg000:002e");
	os.set_col(28);
	os.printf("mov ax, %04x", 32);

	os.set_col(52);
	os.printf("; CODE XREF: ");

	os.putchar('\n');
	*/
}
