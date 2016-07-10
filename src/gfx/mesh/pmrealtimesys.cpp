#include "os/compat.h"
#include "pmrealtimesys.h"
#include "shared.h"
#include "glhelpers.h"

#include "main/types_ops.h"
#include "world/query_obj.h"
#include "world/geom_helper.h"
#include "misc/font.h"
#include <iostream>

namespace reaper 
{
namespace gfx
{
namespace mesh
{


	using std::cout;
	using std::cin;

	void PMRealTimeSys::render()
	{
		Timer tim;
		static int i;
		static double qrta;
		static int qrt[100];// = {0, 0, 0, 0, 0, 0, 0 , 0, 0, 0};
		static int qrti = 0;
		static int qrt_max = 0;
		

		if(qrefine_cnt > qrt_max) qrt_max = qrefine_cnt;

		qrt[qrti] = qrefine_cnt;
		qrti++;
		if(qrti == 100)qrti = 0;

		qrta = 0.0;
		for(i = 0; i < 100; i++)qrta += (float) qrt[i];
		qrta = qrta / 100.0;

		static float afps = 0.0;
		static int cnt = 0;
		cnt++;
		float camang = 3.14*reaper::world::angle(cam.front, Point(1.0, 0.0, 0.0)) / 180.0;
		if(dot(cam.front, Point(0.0, 1.0, 0.0)) < 0)camang = -camang;


		//cout << "camang: " << (camang*180/3.14) << "\n";
		float len = 50.0;
		float da = (3.14*cam.horiz_fov/180.0) / 2.0;
		float x0 = len * cos(camang + da);
		float y0 = len * sin(camang + da);
		float x1 = len * cos(camang - da);
		float y1 = len * sin(camang - da);


		tim.start();

		mesh.render();

		tim.stop();		

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
		glEnable (GL_BLEND);

		glBegin(GL_LINES);

			float dx = cos(cam.horiz_fov) * 10;
			float dy = sin(cam.horiz_fov) * 10;


			glColor4f(1.0, 1.0, 1.0, 0.2);
			glVertex3f(cam.pos.x + x0, cam.pos.y + y0, cam.pos.z);
			glVertex3f(cam.pos.x, cam.pos.y, cam.pos.z);
			glVertex3f(cam.pos.x + x1, cam.pos.y + y1, cam.pos.z);
			glVertex3f(cam.pos.x, cam.pos.y, cam.pos.z);

		glEnd();

					
		glColor3f(1.0, 1.0, 1.0);
		char dt[256];
		sprintf(dt, "face count: %d", mesh.active_faces.size());
		output(20.0, 20.0, dt);
		sprintf(dt, "qrefine cnt: %d ", (int) qrta);
		output(20.0, 40.0, dt);
		sprintf(dt, "qrefine max: %d ", (int) qrt_max);
		output(20.0, 50.0, dt);
		sprintf(dt, "recursion: %d ", (int) stack_depth);
		output(20.0, 60.0, dt);

		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);


	}

	void PMRealTimeSys::set_cam(Point origin, Point dir, float fov)
	{
		cam.pos = origin;
		cam.front = norm(dir);
		cam.horiz_fov = fov;
		cam_fov_cos = cos( (cam.horiz_fov*3.14/180.0) / 2.0);
	}



	void PMRealTimeSys::relocate_vertex(int vi, moli::iterator& li)
	{

		if(!mesh.vertices[vi].is_active)throw MeshException("relocate_vertex::cannot relocate inactive vertex");

		if(li == mesh.vertices[vi].active)return;

		MorphState s = (*mesh.vertices[vi].active).state;
		int src = (*mesh.vertices[vi].active).src;
		float alpha = (*mesh.vertices[vi].active).alpha;


		mesh.remove_active_vertex(vi);

		moli::iterator tli = li;
		tli++;
		mesh.add_active_vertex_at(vi, tli);
		(*mesh.vertices[vi].active).state = s;
		(*mesh.vertices[vi].active).src = src;
		(*mesh.vertices[vi].active).alpha = alpha;

	}

	void PMRealTimeSys::setup()
	{
		mesh.tex.prepare();
	}


	bool PMRealTimeSys::qrefine(int vi)
	{
		/*
		int hval = qtab.lookup(vi);
		if(hval != -1){
			return hval;
		}
		*/

		PMVertex& v = mesh.vertices[vi];
		qrefine_cnt++;

		if(outside_view_frustum(v))	return false;

		if(oriented_away(v)) return false;

		if(screen_space_error(v) < tau) return false;
		
		return true;
	}

	bool PMRealTimeSys::outside_view_frustum(PMVertex& v)
	{
		static Vector p; 
		static float dotp;
		static float dotpn;
		static float ret;

		p = v.point - cam.pos;

		dotp = (cam.front.x*p.x + cam.front.y*p.y + cam.front.z*p.z);
		if(dotp < 0.0)return true;

		dotpn = dotp / (length(p));		
		ret = dotpn < cam_fov_cos;

		return ret > 0.0;
	}

	inline bool PMRealTimeSys::oriented_away(PMVertex& v){

		return false;
		
	}


	inline float PMRealTimeSys::screen_space_error(PMVertex& v)
	{
		Point u = (v.point - cam.pos);

		float len = length(u);
		return v.flr_area/(len*len);

	}

	bool PMRealTimeSys::ecol_legal(PMVertex& v)
	{
		
		//return (faces_aligned(v.flr[0], v.fn[0]));
		//cout << "ecol legal in action...";

		//if(v.parent == -1)return false;

		// Sibling active?
		bool b0a = mesh.vertices[v.vt].is_active;
		bool b0b = mesh.vertices[v.vu].is_active;

		bool b1a = (v.fn[0] == -1) || ( mesh.faces[v.fn[0]].is_active );
		bool b1b = (v.fn[1] == -1) || ( mesh.faces[v.fn[1]].is_active );
		bool b1c = (v.fn[2] == -1) || ( mesh.faces[v.fn[2]].is_active );
		bool b1d = (v.fn[3] == -1) || ( mesh.faces[v.fn[3]].is_active );
		bool b1 = b1a && b1b && b1c && b1d;

		// Matching neighbours?		
		bool b0c = (v.flr[0] == -1) || (mesh.faces[v.flr[0]].nf[1] == v.fn[0]);
		bool b0d = (v.flr[0] == -1) || (mesh.faces[v.flr[0]].nf[0] == v.fn[1]);
		bool b0e = (v.flr[1] == -1) || (mesh.faces[v.flr[1]].nf[2] == v.fn[2]);
		bool b0f = (v.flr[1] == -1) || (mesh.faces[v.flr[1]].nf[0] == v.fn[3]);

		
		bool b0 = b0a && b0b && b0c && b0d && b0e && b0f;

		/*
		if( !(b0 && b1)){
			cout << "\n" << b0a << b0b << b0c << b0d << b0e << b0f << "\n";
			cout << b0 << b1 << "\n";
		}
		*/

		return b0 && b1;
	}


	bool PMRealTimeSys::vsplit_legal(PMVertex& v)
	{
		bool b0 = ((v.fn[0] == -1) || (mesh.faces[v.fn[0]]).is_active) &&
				  ((v.fn[1] == -1) || (mesh.faces[v.fn[1]]).is_active) &&
				  ((v.fn[2] == -1) || (mesh.faces[v.fn[2]]).is_active) &&
				  ((v.fn[3] == -1) || (mesh.faces[v.fn[3]]).is_active);

		return b0;

	}


	void PMRealTimeSys::report(bool verbose)
	{
		cout << "\n";
		cout << "---------------------------\n";
		cout << "| PMRealTimeSys reporting |" << "\n";
		cout << "---------------------------" << "\n";
		cout << "Vertex count:\t " << mesh.vertices.size() << "\n";
		cout << "Faces count:\t" << mesh.faces.size() << "\n";
		cout << "\n";
		cout << "Active Vertex count:\t " << mesh.active_vertices.size() << "\n";
		cout << "Active Faces count:\t" << mesh.active_faces.size() << "\n";
		cout << "\n";

		if(verbose){
			cout << "#\tactive\tvt\tvu\tparent\tx\ty\tz\tfl\tfr\n";
			for(int i = 0; i < mesh.vertices.size(); i++){
				cout << i << "\t" << mesh.vertices[i].is_active << "\t" << mesh.vertices[i].vt << "\t" << mesh.vertices[i].vu << "\t" << mesh.vertices[i].parent << "\t";
				cout << mesh.vertices[i].point.x << "\t";
				cout << mesh.vertices[i].point.y << "\t";
				cout << mesh.vertices[i].point.z << "\t";
				cout << mesh.vertices[i].flr[0] << "\t";
				cout << mesh.vertices[i].flr[1] << "\n";
			}
		}


		
	}

	void PMRealTimeSys::report_texture()
	{
		int tex_cnt = mesh.tex.cnt();

		for(int i = 0; i < mesh.vertices.size(); i++){
			
			cout << "Vertex #" << i << " ";

			cout << mesh.vertices[i].point.x << " " << mesh.vertices[i].point.y << " ";

			for(int j = 0; j < tex_cnt; j++){

				cout << "Texture #" << j << "\t";
				cout << mesh.vertices[i].tc[j].u << ", " << mesh.vertices[i].tc[j].v << "\n";
			}
		}

	}
		

}
}
}
