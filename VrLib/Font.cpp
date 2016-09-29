#ifdef WIN32
#include <windows.h>
#endif

#include "Font.h"
#include <ft2build.h>
#include <fstream>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_GLYPH_H

#include <externals/poly2tri/poly2tri.h>

#include <VrLib/Log.h>
#include <VrLib/gl/Vertex.h>
#include <VrLib/gl/VBO.h>
#include <VrLib/gl/VAO.h>
#include <VrLib/Texture.h>
#include <VrLib/Image.h>

namespace vrlib
{
	FT_Library  Font::library = NULL;


	Font::Font(const std::string &name)
	{
		fontName = name;

		if (!library)
		{
			int error = FT_Init_FreeType(&library);
			if (error)
				logger << "Error initializing freetype" << Log::newline;
		}
		
		int error = FT_New_Face(library, "c:/windows/fonts/tahoma.ttf", 0, &face);
		if (error == FT_Err_Unknown_File_Format)
			logger << "the font file could be opened and read, but it appears that its font format is unsupported" << Log::newline;
		else if (error)
			logger << "Cannot open font" << Log::newline;
		error = FT_Set_Char_Size(
			face,    /* handle to face object           */
			0,       /* char_width in 1/64th of points  */
			16 * 64,   /* char_height in 1/64th of points */
			300,     /* horizontal device resolution    */
			300);   /* vertical device resolution      */


	}



	void Font::generate()
	{
	}


	const float scale = 10000.0f;


	int blaMove(FT_Vector* to, Font::Glyph* outline)
	{
		//logger << "Move To " << to->x << ", " << to->y << Log::newline;
		outline->vertices.push_back(std::vector<glm::vec2>());
		outline->vertices.back().push_back(glm::vec2(to->x / scale, to->y / scale));
		return 0;
	}

	int bla(FT_Vector* to, Font::Glyph* outline)
	{
		//logger << "Line To " << to->x << ", " << to->y << Log::newline;
		outline->vertices.back().push_back(glm::vec2(to->x / scale, to->y / scale));
		return 0;
	}


	int blaConic(FT_Vector* control, FT_Vector* to, Font::Glyph* outline)
	{
		//logger << "Conic " << to->x << ", " << to->y << Log::newline;
		outline->vertices.back().push_back(glm::vec2(to->x / scale, to->y / scale));
		return 0;
	}

	int blaCubic(FT_Vector* control1, FT_Vector* control2, FT_Vector* to, Font::Glyph* outline)
	{
		//logger << "Cubic " << to->x << ", " << to->y << Log::newline;
		outline->vertices.back().push_back(glm::vec2(to->x / scale, to->y / scale));
		return 0;
	}

	void Font::generateGlyph(char charCode)
	{
		int glyph_index = FT_Get_Char_Index(face, charCode);

		int error = FT_Load_Glyph(
			face,          /* handle to face object */
			glyph_index,   /* glyph index           */
			FT_LOAD_DEFAULT);  /* load flags, see below */

		FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		FT_Glyph g;
		error = FT_Get_Glyph(face->glyph, &g);
		FT_Outline_Funcs interface_;

		FT_Outline  outline = face->glyph->outline;

		interface_.line_to = (FT_Outline_LineTo_Func)bla;
		interface_.conic_to = (FT_Outline_ConicTo_Func)blaConic;
		interface_.cubic_to = (FT_Outline_CubicTo_Func)blaCubic;
		interface_.move_to = (FT_Outline_MoveTo_Func)blaMove;
		interface_.shift = 0;
		interface_.delta = 0;

		Glyph* newGlyph = new Glyph();
		newGlyph->charCode = charCode;
		newGlyph->advance = glm::vec2(face->glyph->advance.x / scale, face->glyph->advance.y / scale);

		error = FT_Outline_Decompose(&outline, &interface_, newGlyph);
		FT_Done_Glyph((FT_Glyph)g);


		gl::VertexP3N3 v;
		for (std::vector<glm::vec2> line : newGlyph->vertices)
		{
			for (int i = 0; i < (int)line.size(); i++)
			{
				int ii = (i + 1) % line.size();

				glm::vec2 normal = glm::normalize(line[ii] - line[i]);
				normal = glm::vec2(-normal.y, normal.x);


				setN3(v, glm::vec3(normal.x, normal.y, 0));

				setP3(v, glm::vec3(line[i].x, line[i].y, 0));					newGlyph->verts.push_back(v);
				setP3(v, glm::vec3(line[ii].x, line[ii].y, 0));					newGlyph->verts.push_back(v);
				setP3(v, glm::vec3(line[i].x, line[i].y, thickness));			newGlyph->verts.push_back(v);

				setP3(v, glm::vec3(line[ii].x, line[ii].y, 0));					newGlyph->verts.push_back(v);
				setP3(v, glm::vec3(line[ii].x, line[ii].y, thickness));			newGlyph->verts.push_back(v);
				setP3(v, glm::vec3(line[i].x, line[i].y, thickness));			newGlyph->verts.push_back(v);
			}
		}



		std::vector<p2t::Point*> polyline;
		if (!newGlyph->vertices.empty())
		{
			for (int i = 0; i < (int)newGlyph->vertices[0].size() - 1; i++)
			{
				polyline.push_back(new p2t::Point(newGlyph->vertices[0][i].x, newGlyph->vertices[0][i].y));
			}
		}

		if (!polyline.empty())
		{
			p2t::CDT* cdt = new p2t::CDT(polyline);;

			for (size_t ii = 1; ii < newGlyph->vertices.size(); ii++)
			{
				std::vector<p2t::Point*> holeLine;
				for (int iii = 0; iii < ((int)newGlyph->vertices[ii].size()) - 1; iii++)
				{
					holeLine.push_back(new p2t::Point(newGlyph->vertices[ii][iii].x, newGlyph->vertices[ii][iii].y));
				}
				cdt->AddHole(holeLine);
			}

			cdt->Triangulate();
			std::vector<p2t::Triangle*> triangles = cdt->GetTriangles();
			for (auto triangle : triangles)
			{
				for (int ii = 0; ii < 3; ii++)
				{
					setN3(v, glm::vec3(0, 0, -1));
					setP3(v, glm::vec3(triangle->GetPoint(ii)->x, triangle->GetPoint(ii)->y, 0));						newGlyph->verts.push_back(v);
				}

				for (int ii = 0; ii < 3; ii++)
				{
					setN3(v, glm::vec3(0, 0, 1));
					setP3(v, glm::vec3(triangle->GetPoint(ii)->x, triangle->GetPoint(ii)->y, thickness));						newGlyph->verts.push_back(v);
				}

			}

			delete cdt;
		}
		for (p2t::Point* p : polyline)
			delete p;
		polyline.clear();






		glyphs[charCode] = newGlyph;

	}

	void Font::render(const char *fmt, ...)					// Custom GL "Print" Routine
	{
		float		length = 0;								// Used To Find The Length Of The Text
		char		text[2048];								// Holds Our String
		va_list		ap;										// Pointer To List Of Arguments

		if (fmt == NULL)									// If There's No Text
			return;											// Do Nothing

		va_start(ap, fmt);									// Parses The String For Variables
#ifdef WIN32
		vsprintf_s(text, 2048, fmt, ap);						// And Converts Symbols To Actual Numbers
#else
		vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
#endif
		va_end(ap);											// Results Are Stored In Text
		int len = strlen(text);


		std::vector<gl::VertexP3N3T2> vertices;
		gl::VertexP3N3T2 v;
		gl::setT2(v, glm::vec2(0.5,0.5));
		glm::vec3 pos(0, 0, 0);

		for (int i = 0; i < len; i++)
		{
			char c = text[i];
			if (glyphs.find(c) == glyphs.end())
				generateGlyph(c);


			Glyph* g = glyphs[c];
			for (const gl::VertexP3N3& p : g->verts)
			{
				setP3(v, pos + glm::vec3(p.px, p.py, p.pz));
				setN3(v, glm::vec3(p.nx, p.ny, p.nz));
				vertices.push_back(v);
			}
			pos += glm::vec3(g->advance.x, g->advance.y, 0);
		}

		gl::VBO<gl::VertexP3N3T2> vbo;
		vbo.bind();
		vbo.setData(vertices.size(), &vertices[0], GL_STATIC_DRAW);
		gl::VAO vao(&vbo);

		glDisable(GL_CULL_FACE);
		vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		vao.unBind();
		glEnable(GL_CULL_FACE);



	}

	float Font::getLength(const char *fmt, ...)
	{
		float		length = 0;								// Used To Find The Length Of The Text
		char		text[2048];								// Holds Our String
		va_list		ap;										// Pointer To List Of Arguments

		if (fmt == NULL)									// If There's No Text
			return 0;											// Do Nothing

		va_start(ap, fmt);									// Parses The String For Variables
#ifdef WIN32
		vsprintf_s(text, 2048, fmt, ap);						// And Converts Symbols To Actual Numbers
#else
		vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
#endif
		va_end(ap);											// Results Are Stored In Text
		int len = strlen(text);


		std::vector<gl::VertexP3N3T2> vertices;
		gl::VertexP3N3T2 v;
		gl::setT2(v, glm::vec2(0.5, 0.5));
		glm::vec3 pos(0, 0, 0);

		for (int i = 0; i < len; i++)
		{
			char c = text[i];
			if (glyphs.find(c) == glyphs.end())
				generateGlyph(c);


			Glyph* g = glyphs[c];
			length += g->advance.x;
		}

		return length;
	}






	///////////////////////////////////////////////////////////////btimapfont


	std::map<std::string, BitmapFont*> BitmapFont::fonts;

	BitmapFont* BitmapFont::getFontInstance(std::string name)
	{
		std::map<std::string, BitmapFont*>::iterator it = fonts.find(name);
		if (it == fonts.end())
		{
			BitmapFont* font = new BitmapFont(name);
			fonts[name] = font;
			return font;
		}
		return it->second;
	}

	void BitmapFont::clearCache()
	{
		for (std::map<std::string, BitmapFont*>::iterator it = fonts.begin(); it != fonts.end(); it++)
			delete it->second;
		fonts.clear();
	}


	static std::vector<std::string> split(std::string value, std::string seperator)
	{
		std::vector<std::string> ret;
		while (value.find(seperator) != std::string::npos)
		{
			int index = value.find(seperator);
			if (index != 0)
				ret.push_back(value.substr(0, index));
			value = value.substr(index + seperator.length());
		}
		ret.push_back(value);
		return ret;
	}



	BitmapFont::BitmapFont(std::string fileName)
	{
		std::ifstream file(fileName);
		if (!file.is_open())
		{
			logger << "Could not open font file: " << fileName << Log::newline;
			return;
		}
		std::string textureFileName;

		std::string path = "";
		if (fileName.find('/') != std::string::npos)
			path = fileName.substr(0, fileName.rfind('/'));
		if (fileName.find('\\') != std::string::npos)
			path = fileName.substr(0, fileName.rfind('\\'));

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);
			if (line.length() < 5)
				continue;

			if (line.substr(0, 5) == "page ")
			{
				textureFileName = line.substr(16);
				textureFileName = path + "/" + textureFileName.substr(0, textureFileName.length() - 1);
			}
			if (line.substr(0, 5) == "char ")
			{
				std::vector<std::string> params = split(line.substr(5), " ");
				Glyph* glyph = new Glyph();
				glyph->id = 0;

				for (size_t i = 0; i < params.size(); i++)
				{
					std::string name = params[i].substr(0, params[i].find("="));
					if (name == "id")			glyph->id = atoi(params[i].substr(name.length() + 1).c_str());
					else if (name == "x")			glyph->x = atoi(params[i].substr(name.length() + 1).c_str());
					else if (name == "y")			glyph->y = atoi(params[i].substr(name.length() + 1).c_str());
					else if (name == "width")		glyph->width = atoi(params[i].substr(name.length() + 1).c_str());
					else if (name == "height")		glyph->height = atoi(params[i].substr(name.length() + 1).c_str());
					else if (name == "xoffset")		glyph->xoffset = atoi(params[i].substr(name.length() + 1).c_str());
					else if (name == "yoffset")		glyph->yoffset = atoi(params[i].substr(name.length() + 1).c_str());
					else if (name == "xadvance")		glyph->xadvance = atoi(params[i].substr(name.length() + 1).c_str());
					else if (name == "page" || name == "chnl") {}
					else
						logger << "Didn't parse " << name << Log::newline;
				}

				if (glyph->id != 0)
					charmap[glyph->id] = glyph;
				else
					delete glyph;
			}
		}

		logger << "Loaded font " << fileName << ", " << charmap.size() << " glyphs" << Log::newline;

		texture = vrlib::Texture::loadCached(textureFileName);
	}

	BitmapFont::~BitmapFont()
	{
		for (std::map<char, Glyph*>::iterator it = charmap.begin(); it != charmap.end(); it++)
			delete it->second;
		charmap.clear();
	}



	float BitmapFont::textlen(std::string text)
	{
		float scale = 1;//0.00075f;

		float posX = 0;

		for (size_t i = 0; i < text.size(); i++)
		{
			if (charmap.find(text[i]) == charmap.end())
				continue;
			Glyph* g = charmap[text[i]];
			posX += g->xadvance * scale;

		}
		return posX;
	}

	const BitmapFont::Glyph* BitmapFont::getGlyph(const char &character) const
	{
		return charmap.find(character)->second;
	}


	template <class T>
	void BitmapFont::drawText(const std::string &text, const T &base)
	{
		glm::vec2 cursor;
		int wrapWidth = -1;
		std::vector<T> verts;
		
		glm::vec2 texFactor(1.0f / texture->image->width, 1.0f / texture->image->height);

		float x = cursor.x;
		float y = cursor.y;
		int lineHeight = 12;
		for (size_t i = 0; i < text.size(); i++)
		{
			if (text[i] == '\n' || (x > wrapWidth && wrapWidth != -1))
			{
				x = 0;
				y += lineHeight;
				lineHeight = 12;
			}
			if (charmap.find(text[i]) == charmap.end())
				continue;
			const Glyph* g = getGlyph(text[i]);
			lineHeight = glm::max(lineHeight, g->height);

			T v = base;
			gl::setP2(v, glm::vec2(x + g->xoffset, y + g->yoffset));						gl::setT2(v, glm::vec2(g->x*texFactor.x, g->y*texFactor.y));
			verts.push_back(v);
			gl::setP2(v, glm::vec2(x + g->xoffset + g->width, y + g->yoffset));				gl::setT2(v, glm::vec2((g->x + g->width)*texFactor.x, g->y*texFactor.y));
			verts.push_back(v);
			gl::setP2(v, glm::vec2(x + g->xoffset + g->width, y + g->yoffset + g->height));	gl::setT2(v, glm::vec2((g->x + g->width)*texFactor.x, (g->y + g->height)*texFactor.y));
			verts.push_back(v);
			gl::setP2(v, glm::vec2(x + g->xoffset, y + g->yoffset + g->height));			gl::setT2(v, glm::vec2(g->x*texFactor.x, (g->y + g->height)*texFactor.y));
			verts.push_back(v);			
			x += g->xadvance;
		}
		
		texture->bind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		gl::setAttributes<T>(&verts[0]);
 
		glDrawArrays(GL_QUADS, 0, verts.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	template void BitmapFont::drawText<vrlib::gl::VertexP2T2>(const std::string &text, const vrlib::gl::VertexP2T2 &base);





	TrueTypeFont::TrueTypeFont(const std::string &name, float size)
	{
		fileData = nullptr;
		logger << "Loading font 'c:\\windows\\fonts\\" << name << ".ttf" << Log::newline;
		FILE* pFile = fopen(("c:/windows/fonts/" + name + ".ttf").c_str(), "rb");
		if (!pFile)
		{
			logger << "Error opening font" << Log::newline;
			return;
		}
		fseek(pFile, 0L, SEEK_END);
		int fileSize = ftell(pFile);
		rewind(pFile);

		fileData = new unsigned char[fileSize];
		fread(fileData, 1, fileSize, pFile);

		vrlib::Image *image = new vrlib::Image(1024, 1024);
		unsigned char* tmpImage = new unsigned char[1024 * 1024];

		stbtt_pack_context pc;
		stbtt_PackBegin(&pc, tmpImage, 1024, 1024, 0, 1, NULL);
		stbtt_PackSetOversampling(&pc, 2, 2);
		stbtt_PackFontRange(&pc, fileData, 0, size, 0, 256, fontData);
		stbtt_PackEnd(&pc);
		//stbtt_GetPackedQuad(fontData)

		for (int x = 0; x < 1024; x++)
		{
			for (int y = 0; y < 1024; y++)
			{
				image->data[(x + 1024 * y) * 4 + 0] = 255;
				image->data[(x + 1024 * y) * 4 + 1] = 255;
				image->data[(x + 1024 * y) * 4 + 2] = 255;
				image->data[(x + 1024 * y) * 4 + 3] = tmpImage[x + 1024*y];
			}
		}
		texture = new Texture(image);
		delete[] tmpImage;
	}

	


	template <class T>
	void TrueTypeFont::drawText(const std::string &text, const T &base)
	{
		glm::vec2 cursor;
		int wrapWidth = -1;
		std::vector<T> verts;

		glm::vec2 texFactor(1.0f / texture->image->width, 1.0f / texture->image->height);

		float x = cursor.x;
		float y = cursor.y;
		int lineHeight = 12;
		for (size_t i = 0; i < text.size(); i++)
		{
			if (text[i] == '\n' || (x > wrapWidth && wrapWidth != -1))
			{
				x = 0;
				y += lineHeight;
				lineHeight = 12;
			}
//			if (charmap.find(text[i]) == charmap.end())
//				continue;

			stbtt_aligned_quad q;
			stbtt_GetPackedQuad(fontData, 512, 512, text[i], &x, &y, &q, 0);
//			lineHeight = glm::max(lineHeight, g->height);

			T v = base;
			gl::setP2(v, glm::vec2(q.x0, q.y0));	gl::setT2(v, glm::vec2(q.s0, q.t0));
			verts.push_back(v);
			gl::setP2(v, glm::vec2(q.x1, q.y0));	gl::setT2(v, glm::vec2(q.s1, q.t0));
			verts.push_back(v);
			gl::setP2(v, glm::vec2(q.x1, q.y1));	gl::setT2(v, glm::vec2(q.s1, q.t1));
			verts.push_back(v);
			gl::setP2(v, glm::vec2(q.x0, q.y1));	gl::setT2(v, glm::vec2(q.s0, q.t1));
			verts.push_back(v);
		}

		texture->bind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		gl::setAttributes<T>(&verts[0]);

		glDrawArrays(GL_QUADS, 0, verts.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	template void TrueTypeFont::drawText<vrlib::gl::VertexP2T2>(const std::string &text, const vrlib::gl::VertexP2T2 &base);


}