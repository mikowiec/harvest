#ifndef REAPER_MESH_GLHELPERS_H
#define REAPER_MESH_GLHELPERS_H

#include <GL/glut.h>

#include <string.h>

namespace reaper 
{
namespace gfx
{
namespace mesh
{


static void *font = GLUT_BITMAP_TIMES_ROMAN_24;
static void *fonts[] =
{
  GLUT_BITMAP_9_BY_15,
  GLUT_BITMAP_TIMES_ROMAN_10,
  GLUT_BITMAP_TIMES_ROMAN_24
};

void selectFont(int newfont);

void selectColor(int color);

void tick();

void output(int x, int y, char *string);


}
}
}
#endif
