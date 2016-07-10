/*
 * $Author: pstrand $
 * $Date: 2003/03/31 15:26:02 $
 * $Log: png.cpp,v $
 * Revision 1.34  2003/03/31 15:26:02  pstrand
 * *** empty log message ***
 *
 * Revision 1.33  2003/01/06 11:57:00  pstrand
 * free png-structures
 *
 * Revision 1.32  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.31  2002/09/23 18:37:18  pstrand
 * *** empty log message ***
 *
 * Revision 1.30  2002/09/23 12:08:43  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.29  2002/09/20 23:41:25  pstrand
 * *** empty log message ***
 *
 * Revision 1.28  2002/09/13 07:45:55  pstrand
 * gcc-varningar
 *
 * Revision 1.27  2002/06/12 17:50:39  pstrand
 * int != long
 *
 * Revision 1.26  2001/12/02 21:48:50  peter
 * ''
 *
 * Revision 1.25  2001/12/02 19:47:44  peter
 * get_row -> get_col
 *
 * Revision 1.24  2001/11/26 02:20:06  peter
 * ljudfixar (smartptr), andra mindre fixar..
 *
 * Revision 1.23  2001/08/06 12:16:15  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.22.4.1  2001/07/31 17:34:04  peter
 * testgren...
 *
 * Revision 1.22  2001/05/10 11:40:15  macke
 * häpp
 *
 */

#include "os/compat.h"

#include <stdlib.h>
#include <string>
#include <iostream>

#include "gfx/exceptions.h"
#include "res/res.h"

#include <png.h>

namespace reaper {
namespace {
using reaper::gfx::gfx_fatal_error;
using std::string;

void read_data_fn(png_structp png, png_bytep data, png_size_t length)
{
	std::istream* ds = reinterpret_cast<std::istream*>(png_get_io_ptr(png));
	ds->read(reinterpret_cast<char*>(data), length);
}

void write_data_fn(png_structp png, png_bytep data, png_size_t length)
{
	std::ostream* os = reinterpret_cast<std::ostream*>(png_get_io_ptr(png));
	os->write(reinterpret_cast<char*>(data), length);
}

void flush_data_fn(png_structp png)
{
	std::ostream* os = reinterpret_cast<std::ostream*>(png_get_io_ptr(png));
	os->flush();
}

void error(std::string id, std::string msg)
{
	throw gfx_fatal_error(string("PngLoadImage: While reading ") + id + ": " + msg + "\n");
}

// PNG image handler functions
void PngLoadImage (const std::string& id, png_byte **data,
        png_uint_32 *width, png_uint_32 *height, png_byte *channels, png_color *bkgcolor)
{
	png_byte sig[8];
	int i, bit_depth, color_type;
	double gamma;
	png_color_16 *background;
	png_uint_32 row_bytes;
	png_byte **row_pointers = 0;
	png_structp png_ptr = 0;
	png_infop info_ptr = 0;

	*data = 0;

	reaper::res::res_stream png_file(reaper::res::Texture, id);


	png_file.read(reinterpret_cast<char*>(sig), 8);

	if (!png_check_sig(sig, 8)) {
		error(id, "png signature error");
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr) {
		error(id, "png read error");
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, 0, 0);
		error(id, "png read error");
	}

	png_set_read_fn(png_ptr, &png_file, read_data_fn);

	png_set_sig_bytes(png_ptr, 8);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, width, height,
		     &bit_depth, &color_type, 0, 0, 0);

	if (bit_depth == 16)
		png_set_strip_16(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE || bit_depth < 8)
		png_set_expand(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);
	if (png_get_bKGD(png_ptr, info_ptr, &background))
		png_set_background(png_ptr, background, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
	if (png_get_gAMA(png_ptr, info_ptr, &gamma))
		png_set_gamma(png_ptr, 2.2, gamma);

	png_read_update_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, width, height,
		     &bit_depth, &color_type, 0, 0, 0);

	row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	*channels = png_get_channels(png_ptr, info_ptr);

	if ((*data = (png_byte*) malloc(row_bytes * *height * sizeof(png_byte))) == 0) {
		error(id, "png malloc error");
	}

	if ((row_pointers = (png_bytepp) malloc((*height) * sizeof(png_bytep))) == 0) {
		error(id, "png malloc error\n");
	}

	for (i = 0; i < *height; i++)
		row_pointers[i] = *data + i * row_bytes;

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);
	free(row_pointers);
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
}


void PngSaveImage (const std::string &id, png_byte *pDiData,
                   int iWidth, int iHeight, png_color bkgColor)
{
	const int           ciBitDepth = 8;
	const int           ciChannels = 3;
	
	static FILE        *pfFile;
	png_uint_32         ulRowBytes;
	static png_byte   **ppbRowPointers = NULL;
	int                 i;

        png_structp png_ptr = NULL;
        png_infop info_ptr = NULL;

	reaper::res::res_out_stream ro(reaper::res::Screenshot, id);

	// prepare the standard PNG structures

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
					  0, (png_error_ptr)NULL);
	if (!png_ptr) {
		throw gfx_fatal_error(string("PngSaveImage: Unable to create write structure: ") + id);
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
		throw gfx_fatal_error(string("PngSaveImage: Unable to create info structure: ") + id);
	}

	png_set_write_fn(png_ptr, &ro, write_data_fn, flush_data_fn);

	// we're going to write a very simple 3x8 bit RGB image

	png_set_IHDR(png_ptr, info_ptr, iWidth, iHeight, ciBitDepth,
		PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE);

	// write the file header information

	png_write_info(png_ptr, info_ptr);
	ulRowBytes = iWidth * ciChannels;

	// we can allocate memory for an array of row-pointers
	if ((ppbRowPointers = (png_bytepp) malloc(iHeight * sizeof(png_bytep))) == NULL)
		error(id, "Out of memory");

	// set the individual row-pointers to point at the correct offsets

	for (i = 0; i < iHeight; i++)
		ppbRowPointers[i] = pDiData + i * (((ulRowBytes + 3) >> 2) << 2);

	// write out the entire image data in one call
	png_write_image (png_ptr, ppbRowPointers);
	// write the additional chunks to the PNG file (not really needed)
	png_write_end(png_ptr, info_ptr);

	free (ppbRowPointers);
	ppbRowPointers = NULL;

	// clean up after the write, and free any memory allocated
	png_destroy_write_struct(&png_ptr, &info_ptr);
}

} // end anonymous namespace

namespace gfx {
namespace misc {

int load_png(const string &file,char *&data,int &w,int &h,bool check_dim)
{
        png_byte channels;
	png_uint_32 width, height;
        png_color bkg_color;

        PngLoadImage(file, (png_byte**)&data, &width, &height, &channels, &bkg_color);

	w = width; h = height;
	
	if (!check_dim)
		return channels;

        // ensure that all sides are a power of two
        bool w_ok = false;
        bool h_ok = false;

        // Texture dimensions supported are 1 to 4096 texels
        for(int i=0;i<13;++i) {
                if(w == (1 << i))
                        w_ok = true;
                if(h == (1 << i))
                        h_ok = true;
        }

        if(!(w_ok && h_ok)) {
                delete data;
                data = 0;
                throw gfx_fatal_error(string("Texture: Illegal dimensions for texture ") + file);
        }              

	return channels;
}

void save_png(const string &file, char *data, int w, int h)
{
	png_color bkg_color;
	bkg_color.red   = 0;
	bkg_color.green = 0;
	bkg_color.blue  = 0;
	PngSaveImage(file, (unsigned char*)data, w, h, bkg_color);

}

}
}
}
