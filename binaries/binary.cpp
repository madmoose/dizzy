#include "binary.h"

#include "exe_mz.h"
#include "exe_ne.h"

#define SIG_MZ 0x4d5a
#define SIG_NE 0x4e45

binary_loader *exe_identifier(raw_istream &is);



binary_loader *binary_identifier(raw_istream &is)
{
	binary_loader *loader = NULL;

	if (is.readaheadbe16() == SIG_MZ && (loader = exe_identifier(is)))
		return loader;

	return 0;
}

binary_loader *exe_identifier(raw_istream &is)
{
	binary_loader *loader = NULL;

	exe_mz_header head;
	head.load(is);

	// All MZ extensions have the relocation table at offset 0x40
	if (head.e_lfarlc == 0x40)
	{
		is.seek_set(head.e_lfanew);

		uint16 ext_sig = is.readaheadbe16();
		switch (ext_sig) {
		case SIG_NE:
			loader = new exe_ne_loader;
			break;
		default:;
		}
	}

	if (!loader)
		loader = new exe_mz_loader;

	is.reset();
	return loader;
}
