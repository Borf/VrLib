#pragma once

#define NOMINMAX

#include <Windows.h>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <map>
#include <vector>
#include <VrLib/gl/Vertex.h>
#include <VrLib/math/Polygon.h>

typedef struct FT_LibraryRec_  *FT_Library;
typedef struct FT_FaceRec_*  FT_Face;

namespace vrlib
{
	class Font
	{
//		GLuint base;
//		GLYPHMETRICSFLOAT gmf[256];	// Storage For Information About Our Outline Font Characters
		std::string fontName;


		static FT_Library  library;
		FT_Face     face;

	public:
		class Glyph
		{
		public:
			char charCode;

			glm::vec2 advance;

			std::vector<math::Polygon2> vertices;
			std::vector<gl::VertexP3N3> verts;
		};
	private:
		std::map<char, Glyph*> glyphs;
	public:

		float thickness = 0.025f;


		Font(const std::string &font);
		void generate();



		void generateGlyph(char charCode);

		void render(const char *fmt, ...);
		float getLength(const char *fmt, ...);


	};
}