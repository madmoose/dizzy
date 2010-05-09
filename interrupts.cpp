
#include <stdio.h>

const char *get_dos_int_description(int interrupt, int ah)
{
	switch (interrupt)
	{
		case 0x10:
			switch(ah)
			{
				case 0x00: return "AH = 00 - Set video mode";
				case 0x02: return "AH = 02 - Set cursor position";
				case 0x03: return "AH = 03 - Get cursor position and size";
				case 0x0F: return "AH = 0F - Get current video mode";
				case 0x10: return "AH = 10 - Video stuff";
				case 0x12: return "AH = 12 - Video stuff";
				case 0x1A: return "AH = 1A - Set video mode";
				default:;
			}
			break;
		case 0x1a:
			switch(ah)
			{
				case 0x00: return "AH = 00 - Get system time";
				default:;
			}
			break;
		case 0x21:
			switch(ah)
			{
				case 0x09: return "AH = 09 - Print string";
				case 0x19: return "AH = 19 - Get current default drive";
				case 0x1B: return "AH = 1B - Get allocation information for default drive";
				case 0x25: return "AH = 25 - Set interrupt vector";
				case 0x30: return "AH = 30 - Get DOS version";
				case 0x35: return "AH = 35 - Get interrupt vector";
				case 0x36: return "AH = 36 - Get free disk space";
				case 0x3D: return "AH = 3D - Open existing file";
				case 0x3E: return "AH = 3E - Close file";
				case 0x3F: return "AH = 3F - Read from file or device";
				case 0x40: return "AH = 40 - Write to file or device";
				case 0x42: return "AH = 42 - Set current file position";
				case 0x47: return "AH = 47 - Get current directory";
				case 0x48: return "AH = 48 - Allocate memory";
				case 0x49: return "AH = 49 - Free memory";
				case 0x4A: return "AH = 4A - Resize memory block";
				case 0x4C: return "AH = 4C - Exit program";
				default:;
			}
			break;
		default:
			return 0;
	}

	static char c[32];
	sprintf(c, "AH = %02X", ah);
	return c;
}

#if 0
const char *get_dos_int_10_description(int ah)
{
	switch(ah)
	{
		case 0x00: return "AH = 00 - Set video mode";
		case 0x02: return "AH = 02 - Set cursor position";
		case 0x03: return "AH = 03 - Get cursor position and size";
		case 0x0F: return "AH = 0F - Get current video mode";
		case 0x10: return "AH = 10 - Video stuff";
		case 0x12: return "AH = 12 - Video stuff";
		case 0x1A: return "AH = 1A - Set video mode";
		default:
		{
			char *c;
			asprintf(&c, "AH = %02X", ah);
			return c;
		}
	}
}

const char *get_dos_int_21_description(int ah)
{
	switch(ah)
	{
		case 0x09: return "AH = 09 - Print string";
		case 0x19: return "AH = 19 - Get current default drive";
		case 0x1B: return "AH = 1B - Get allocation information for default drive";
		case 0x25: return "AH = 25 - Set interrupt vector";
		case 0x30: return "AH = 30 - Get DOS version";
		case 0x35: return "AH = 35 - Get interrupt vector";
		case 0x36: return "AH = 36 - Get free disk space";
		case 0x3D: return "AH = 3D - Open existing file";
		case 0x3E: return "AH = 3E - Close file";
		case 0x3F: return "AH = 3F - Read from file or device";
		case 0x40: return "AH = 40 - Write to file or device";
		case 0x42: return "AH = 42 - Set current file position";
		case 0x47: return "AH = 47 - Get current directory";
		case 0x48: return "AH = 48 - Allocate memory";
		case 0x49: return "AH = 49 - Free memory";
		case 0x4A: return "AH = 4A - Resize memory block";
		case 0x4C: return "AH = 4C - Exit program";
		default:
		{
			char *c;
			asprintf(&c, "AH = %02X", ah);
			return c;
		}
	}
}
#endif

