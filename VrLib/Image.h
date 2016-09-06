#pragma once

#include <string>

namespace vrlib
{
	class Image
	{
	public:
		int width;
		int height;
		int depth;
		unsigned char* data;

		bool usesAlpha;

		Image(const std::string &filename);
		Image(int width, int height);
		virtual ~Image();
		void unload();
		void save(const std::string &fileName);

		class Col
		{
		public:
			Image* img;
			int x;
			unsigned char* operator [] (int y) { return img->data + ((x + img->width*y) * 4); }
		};

		Col operator [] (int x) { return{ this, x }; }

	};
}