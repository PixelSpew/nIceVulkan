#include "stdafx.h"
#include "util/file.h"
#include <fstream>

using namespace std;

namespace nif
{
	namespace file
	{
		string read_all_text(const string &filename)
		{
			ifstream in(filename, ios::in | ios::binary);
			if (!in)
				throw errno;

			string contents;
			in.seekg(0, ios::end);
			contents.resize(in.tellg());
			in.seekg(0, ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return contents;
		}
	}
}
