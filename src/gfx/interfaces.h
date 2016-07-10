#ifndef REAPER_GFX_INTERFACES_H
#define REAPER_GFX_INTERFACES_H

#include <memory>
#include <string>

namespace reaper {
namespace world { class Frustum; }
namespace gfx {
class Color;
namespace pm { class Pmd; }

class Camera;

class Lake
{
	class Impl;
	std::auto_ptr<Impl> i;
public:
	Lake(const std::string &file, const std::string &texture, const Vector &wave_dir, float amplitude);
	~Lake();
	void render(const world::Frustum &); 
	void simulate(float dt);
};

class River
{
	class Impl;
	std::auto_ptr<Impl> i;
public:
	River(const std::string &file, const std::string &texture, float speed);
	~River();
	void render(const world::Frustum &);
	void simulate(float dt);
};

class Sky
{
	class Impl;
	std::auto_ptr<Impl> i;
public:
        Sky(const std::string &tex_file, const Color &col, float alt, float reps);
        ~Sky();

        void render(const Camera &cam);
	void simulate(float dt);
};

class Terrain
{
	class Impl;
	std::auto_ptr<Impl> i;
public:
	static int num_vertices;
	static int num_triangles;

        Terrain(const std::string &mesh_id, const std::string &main_id, const std::string &detail_id, float dr);
        ~Terrain();

	void render(const Camera &c);
	pm::Pmd* get_pmd();
};

class ShadowRenderer
{
public:
	/// returns number of triangles rendered
	virtual int render(const world::Frustum &frustum) = 0;
	virtual ~ShadowRenderer() {}

	static ShadowRenderer* get(int type);
};

class EnvMapper
{
	class Impl;
	std::auto_ptr<Impl> i;
public:
	EnvMapper();
	~EnvMapper();

	void generate(const Point &pos, const world::Frustum &frustum, Sky &sky, Terrain &tr);
	void setup();
	void restore();
};


}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:29:58 $
 * $Log: interfaces.h,v $
 * Revision 1.12  2003/06/04 16:29:58  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.11  2002/05/20 09:59:58  fizzgig
 * dynamic environment mapping!
 *
 * Revision 1.10  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.9  2002/05/10 14:12:50  fizzgig
 * shadow code cleanup
 *
 * Revision 1.8  2002/02/11 11:12:30  macke
 * shadows für alle .. dynamics.. :)
 *
 * Revision 1.7  2002/02/08 11:27:51  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.6  2002/02/07 00:02:51  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.5  2001/12/17 16:28:31  macke
 * div bök
 *
 * Revision 1.4  2001/12/14 16:31:24  macke
 * meck å pul
 *
 * Revision 1.3  2001/08/06 12:16:13  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.2.4.2  2001/08/04 20:37:05  peter
 * flyttad hud, vapenknôk..
 *
 * Revision 1.2.4.1  2001/08/03 13:43:52  peter
 * pragma once obsolete...
 *
 * Revision 1.2  2001/07/06 01:47:11  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.1  2001/06/09 01:58:48  macke
 * Grafikmotor reorg
 *
 */

