#pragma once

struct BOM
{
	static bool IsBigEndian()
	{
		unsigned int i = 1;
		char* byte = (char*)&i;
		
		return (*byte) ? false : true;
	};
};
