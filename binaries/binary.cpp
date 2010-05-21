#include "binary.h"

#include "exe_mz.h"
#include "exe_ne.h"

#define SIG_MZ 0x4d5a
#define SIG_NE 0x4e45

binary_loader_t *exe_identifier(raw_istream_t &is);



binary_loader_t *binary_identifier(raw_istream_t &is)
{
	binary_loader_t *loader = NULL;

	if (is.readaheadbe16() == SIG_MZ && (loader = exe_identifier(is)))
		return loader;

	return 0;
}

binary_loader_t *exe_identifier(raw_istream_t &is)
{
	binary_loader_t *loader = NULL;

	exe_mz_header_t head;
	head.load(is);

	// All MZ extensions have the relocation table at offset 0x40
	if (head.e_lfarlc == 0x40)
	{
		is.seek_set(head.e_lfanew);

		uint16 ext_sig = is.readaheadbe16();
		switch (ext_sig) {
		case SIG_NE:
			loader = new exe_ne_loader_t;
			break;
		default:;
		}
	}

	if (!loader)
		loader = new exe_mz_loader_t;

	is.reset();
	return loader;
}

memory_t binary_loader_t::load_image()
{
	return _load_image;
}

uint32 binary_loader_t::base()
{
	return _base;
}

void binary_loader_t::set_load_image(memory_t &load_image)
{
	_load_image = load_image;
}
