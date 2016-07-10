
/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:39 $
 * $Log: font.cpp,v $
 * Revision 1.25  2002/12/29 17:04:39  pstrand
 * hw/ moved to os/
 *
 * Revision 1.24  2002/01/10 23:09:12  macke
 * massa bök
 *
 * Revision 1.23  2001/12/14 16:10:50  macke
 * powermenus!
 *
 * Revision 1.22  2001/11/26 19:35:39  peter
 * no message
 *
 * Revision 1.21  2001/11/26 03:19:31  peter
 * font reinit
 *
 * Revision 1.20  2001/07/09 15:02:48  peter
 * vc
 *
 * Revision 1.19  2001/07/09 13:33:09  peter
 * gcc-3.0 fixar
 *
 * Revision 1.18  2001/06/07 05:14:24  macke
 * Reaper v0.9
 *
 * Revision 1.17  2001/05/10 20:50:01  peter
 * *** empty log message ***
 *
 * Revision 1.16  2001/05/10 11:40:20  macke
 * häpp
 *
 * Revision 1.15  2001/05/10 10:01:20  peter
 * *** empty log message ***
 *
 * Revision 1.14  2001/05/04 09:24:23  peter
 * *** empty log message ***
 *
 * Revision 1.13  2001/04/23 18:59:54  macke
 * VertexArray - funkar inte helt optimalt ännu..
 *
 * Revision 1.12  2001/04/21 13:59:45  peter
 * *** empty log message ***
 *
 * Revision 1.11  2001/04/20 06:21:25  macke
 * headerfilsstädning
 *
 *
 */

#include "os/compat.h"

#include <algorithm>
#include <cctype>

#include "gfx/misc.h"
#include "gfx/exceptions.h"
#include "gfx/abstracts.h"

#include "misc/font.h"
#include "os/gl.h"
#include "os/debug.h"

#include <stdio.h>

namespace reaper {
namespace {

class FontData
{
	GLuint list, texture;
public:
	FontData() : list(0), texture(0) { }
	FontData(GLuint l, GLuint t) : list(l), texture(t) { }
	void use() {
		glListBase(list);
		glBindTexture2D(texture);
	}
};

debug::DebugOutput dout("gfx::vertex_array",0);
}

namespace gfx {
namespace font {

#include "misc/alpha.xpm"

int ctoi_xpm(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 1;
	if (c >= '0' && c <= '9')
		return c - '0' + 37;
	if (c >= 'a' && c <= 'z')
		return c - 'a' + 1;
	switch (c) {
	case ' ': return 0;
	case ',': return 27;
	case '.': return 28;
	case '<': return 29;
	case '>': return 30;
	case '(': return 31;
	case ')': return 32;
	case ';': return 33;
	case ':': return 34;
	case '\'': return 35;
	case '"': return 36;
	case '-': return 35;
	default: return 0;
	}
	return 0;
}

typedef int (*convfunc)(char);

FontData prep_tex(unsigned char* tex_data, GLenum format,
	int tex_w, int tex_h, float font_w, float font_h, convfunc ctoi)
{
	GLuint id;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, format, GL_UNSIGNED_BYTE, tex_data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	float w_step = font_w / tex_w;
	float h_step = font_h / tex_h;
	const int max_char = 127;
	GLuint base = glGenLists(max_char);
	glColor4f(1, 1, 1, 0.5);

	for (char c = 0; c < max_char; ++c) {
		glNewList(base + c, GL_COMPILE);
		//glBindTexture(GL_TEXTURE_2D, id);
		glBegin(GL_QUADS);
		float pos = ctoi(c) * w_step;

		glTexCoord2f(pos, 0);
		glVertex2f(0, 1);

		glTexCoord2f(pos, h_step);
		glVertex2f(0, 0);

		glTexCoord2f(pos + w_step, h_step);
		glVertex2f(1, 0);

		glTexCoord2f(pos + w_step, 0);
		glVertex2f(1, 1);
		glEnd();
		glTranslatef(1, 0, 0);
		glEndList();
	}
	return FontData(base,id);
}

FontData prep_xpm(char* alpha[])
{
	unsigned char tex_data[16][1024][2];
	
	memset(tex_data, 0, 2 * 16 * 1024);
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 47 * 14; j++) {
			if (alpha[i+4][j] == '+') {
				tex_data[i][j][0] = 255;
				tex_data[i][j][1] = 255;
			}
		}
	}
	return prep_tex(tex_data[0][0], GL_LUMINANCE_ALPHA, 1024, 16, 14, 16, ctoi_xpm);
}

int ctoi_small(char c) {
	using namespace std;
	if (islower(c))
		return c - 'a';
	if (isupper(c))
		return c - 'A';
	if (isdigit(c))
		return c - '0' + 31;
	switch (c) {
	case '\"': return 26;
	case '@': return 27;
	case ':': return 43;
	case '(': return 44;
	case ')': return 45;
	case '-': return 46;
	case '\'': return 47;
	case '!': return 48;
	case '_': return 49;
	case '+': return 50;
	case '\\': return 51;
	case '/': return 52;
	case '[': return 53;
	case ']': return 54;
	case '^': return 55;
	case '&': return 56;
	case '%': return 57;
	case '.': return 58;
	case '=': return 59;
	case '$': return 60;
	case '*': return 61;
	case 'å': return 62;
	case 'Å': return 62;
	case 'ö': return 63;
	case 'Ö': return 63;
	case 'ä': return 64;
	case 'Ä': return 64;
	case '?': return 65;
	case '#': return 66;
	default:  return 30;
	}
}

int ctoi_large(char c)
{
	using namespace std;
	if (islower(c))
		return c - 'a';
	if (isupper(c))
		return c - 'A';
	if (isdigit(c))
		return c - '0' + 26;
	return 36;
}

FontData prep_large()
{
	char* data;
	int w, h;
	gfx::misc::load_png("Large_font2", data, w, h, false);

	const int width = 1024;
	const int side = 24;
	const int img_width = 624;
	const int scale = 1;
	unsigned char tex_data[32][width][4];
	memset(tex_data, 0, 32*width*4);
	for (int i = 0; i < side/scale; i++) {
		for (int j = 0; j < (side*37)/scale; j++) {
			int img_idx = i * scale * img_width + ((j*scale)%img_width)
				    + ((j*scale)/img_width) * side*img_width;
			tex_data[i][j][0] = data[img_idx*3];
			tex_data[i][j][1] = data[img_idx*3 + 1];
			tex_data[i][j][2] = data[img_idx*3 + 2];

			// divide by 3 intentionally left out
			int luminance = (int)tex_data[i][j][0] + (int)tex_data[i][j][2] + (int)tex_data[i][j][2];
			if(luminance > 255) luminance = 255;
			if(luminance < 0)   luminance = 0;
			tex_data[i][j][3] = luminance;
		}
	}
	FontData fd = prep_tex(tex_data[0][0], GL_RGBA,
				width, 32, side, side, ctoi_large);
	free(data);
	return fd;
}

FontData prep_small()
{
	char* data;
	int w, h;
	gfx::misc::load_png("smallfont", data, w, h, false);

	unsigned char tex_data[8][512][2];
	memset(tex_data, 0, 8*512*2);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 335; j++) {
			int img_idx = i * 155 + (j%155) + (j/155) * 5*6*31;
			if (data[img_idx*3+2] != 0) {
				tex_data[i][j][0] = 255;
				tex_data[i][j][1] = 255;
			}
		}
	}
	FontData fd = prep_tex(tex_data[0][0], GL_LUMINANCE_ALPHA, 512, 8, 5, 6, ctoi_small);
	free(data);
	return fd;
}

class Fonts : public Initializer
{
public:
	void init();
	void exit() { }
	static FontData small_data, medium_data, large_data;
};

FontData Fonts::small_data, Fonts::medium_data, Fonts::large_data;
Fonts fonts;

void Fonts::init() {
	fonts.small_data = prep_small();
	fonts.medium_data = prep_xpm(alpha_xpm);
	fonts.large_data = prep_large();
}

void glPutStr(float x, float y, const std::string& s, Font font, float scale_x, float scale_y)
{
	throw_on_gl_error("glPutStr - entering");

	StateKeeper s1(GL_TEXTURE_2D, true);
	StateKeeper s2(GL_LIGHTING, false);
	StateKeeper s4(GL_BLEND, true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int mode = glGetInteger(GL_MATRIX_MODE);
	glMatrixMode(GL_MODELVIEW);

	switch(font) {
	case Small:  fonts.small_data.use();  break;
	case Medium: fonts.medium_data.use(); break;
	case Large:  fonts.large_data.use();  break;
	}

	glPushMatrix();	
	glTranslatef(x, y, 0);
	glScalef(scale_x, scale_y, 1.0);
	glCallLists(s.size(), GL_UNSIGNED_BYTE, s.c_str());
	glPopMatrix();
	glMatrixMode(mode);

	throw_on_gl_error("glPutStr");
}


}

namespace initializers {
	Initializer* font_init = &font::fonts;
}

}
}

