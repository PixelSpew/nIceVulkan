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

		void write_all_text(const std::string &filename, const std::string &text)
		{
			ofstream out(filename, ios::out | ios::binary | ios::trunc);
			if (!out)
				throw errno;

			out.write(text.c_str(), text.size());
			out.close();
		}

		void append_all_text(const std::string &filename, const std::string &text)
		{
			ofstream out(filename, ios::out | ios::binary | ios::app);
			if (!out)
				throw errno;

			out.write(text.c_str(), text.size());
			out.close();
		}
	}
}
