#include "os/compat.h"

#include <vector>

#include "main/types.h"
#include "main/types_ops.h"
#include "gfx/gfx_types.h"
#include "gfx/misc.h"
#include "os/gl.h"

using std::vector;

namespace reaper {
namespace gfx {
namespace misc {
namespace {

/* Billboards are specialized for round particles
   Rendered as triangles to trade fillrate for geometric performance
*/
class ParticleBillBoardImpl
{
	Point    points[3];
	TexCoord texcoords[3];	

	vector<Point>    rend_points;
	vector<TexCoord> rend_texcoords;
	vector<Color>    rend_colors;
public:
	ParticleBillBoardImpl() {
		texcoords[0] = TexCoord(-0.4, 0);
		texcoords[1] = TexCoord( 1.4, 0);
		texcoords[2] = TexCoord( 0.5, 1.4);
	}

	inline void init(const Vector& right, const Vector &up) {
		points[0] = (-1.8*right) - up;
		points[1] = 1.8*right - up;
		points[2] = 1.8*up;
	}

	inline void add_pts(float size, const Point &pos)
	{
		rend_points.push_back(points[0]*size + pos);
		rend_points.push_back(points[1]*size + pos);
		rend_points.push_back(points[2]*size + pos);

		rend_texcoords.insert(rend_texcoords.end(), &texcoords[0], &texcoords[3]);
	}

	inline void add(float size, const Point &pos)
	{
		add_pts(size,pos);
	}

	inline void add(float size, const Point &pos, const Color &c)
	{		
		add_pts(size,pos);
		rend_colors.insert(rend_colors.end(), 3, c);		
	}

	inline void render()
	{
		ClientStateKeeper s(GL_TEXTURE_COORD_ARRAY, true);

		glTexCoordPointer(2, GL_FLOAT, sizeof(TexCoord), &rend_texcoords[0]);
		glVertexPointer(3, GL_FLOAT, sizeof(Point), &rend_points[0]);
		glDrawArrays(GL_TRIANGLES,0,rend_points.size());

		rend_texcoords.clear();
		rend_points.clear();
	}

	inline void render_colors()
	{
		ClientStateKeeper s(GL_COLOR_ARRAY, true);
		glColorPointer(4, GL_FLOAT, sizeof(Color), &rend_colors[0]);
		render();
		rend_colors.clear();
	}	
};

// Classical quad-billboards
class BillBoardImpl
{
	vector<Point>    points;
	vector<TexCoord> texcoords;	

	vector<Point>    rend_points;
	vector<TexCoord> rend_texcoords;
	vector<Color>    rend_colors;
public:
	BillBoardImpl() {
		texcoords.push_back(TexCoord(0,0));
		texcoords.push_back(TexCoord(1,0));
		texcoords.push_back(TexCoord(1,1));
		texcoords.push_back(TexCoord(0,1));
	}

	inline void init(const Vector& right, const Vector &up) {
		points.clear();
		points.push_back(-right-up);
		points.push_back( right-up);
		points.push_back( right+up);
		points.push_back(-right+up);
	}

	inline void add(float size, const Point &pos)
	{
		rend_points.push_back(points[0]*size + pos);
		rend_points.push_back(points[1]*size + pos);
		rend_points.push_back(points[2]*size + pos);
		rend_points.push_back(points[3]*size + pos);

		rend_texcoords.insert(rend_texcoords.end(), texcoords.begin(), texcoords.end());
	}

	inline void add(float size, const Point &pos, const Color &c)
	{		
		rend_colors.insert(rend_colors.end(), 4, c);		
		add(size, pos);				
	}

	inline void render()
	{
		ClientStateKeeper s(GL_TEXTURE_COORD_ARRAY, true);

		glTexCoordPointer(2, GL_FLOAT, sizeof(TexCoord), &rend_texcoords[0]);
		glVertexPointer(3, GL_FLOAT, sizeof(Point), &rend_points[0]);
		glDrawArrays(GL_QUADS,0,rend_points.size());

		rend_texcoords.clear();
		rend_points.clear();
	}

	inline void render_colors()
	{
		ClientStateKeeper s(GL_COLOR_ARRAY, true);
		glColorPointer(4, GL_FLOAT, sizeof(Color), &rend_colors[0]);
		render();
		rend_colors.clear();
	}	
};

//FIXME: Allow both round/square billboards to exists peacefully--
ParticleBillBoardImpl bbimp;
}	// end anonymous namespace

void BillBoard::render()                                           { bbimp.render(); }
void BillBoard::render_colors()                                    { bbimp.render_colors(); }
void BillBoard::add(float size, const Point &pos)                  { bbimp.add(size,pos); }
void BillBoard::add(float size, const Point &pos, const Color &c)  { bbimp.add(size,pos,c); }
void BillBoard::set_vectors(const Vector &right, const Vector &up) { bbimp.init(right,up); }
void BillBoard::set_vectors()
{
	// Take vectors from inverse matrix
	// (for orthogonal matrices, transpose equals inverse)	

	Matrix4 m(get_matrix(GL_MODELVIEW_MATRIX));
	bbimp.init(Vector(m[0][0],m[1][0],m[2][0]),Vector(m[0][1],m[1][1],m[2][1]));
}

}
}
}

