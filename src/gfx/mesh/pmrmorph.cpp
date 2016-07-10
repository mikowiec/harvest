#include "os/compat.h"
#include "main/types_io.h"
#include "main/types_ops.h"
#include "pmrmorph.h"
#include "world/query_obj.h"
#include "world/geom_helper.h"
#include "res/res.h"
#include "misc/iostream_helper.h"
#include "ext/nvparse.h"
#include "glhelpers.h"
#include <iostream>

namespace reaper 
{
namespace gfx
{
namespace mesh
{
	using std::cout;
	using std::cin;
	using std::endl;

	void test_and_apply_rc(const char *rc)
	{
		nvparse(rc);

		bool error = false;
		for (const char* const* errors = nvparse_get_errors(); *errors; errors++) {
			error = true;
			cout << *errors << "\n";
		}

		if(error)
			exit(1);
	}


	void PMRMorph::setup_render_arrays()
	{
		int tex_cnt = mesh.tex.cnt();

		// Resize

		static bool alc = true;
		if(alc) {
			vertex_array.resize( mesh.vertices.size() );

                        texture_array.resize(tex_cnt);
			for(int i = 0; i < tex_cnt; ++i)
				texture_array[i].resize( mesh.vertices.size() );

			res::res_stream is(res::Shader, "texture_blend_2_tmu.rc");
			int filesize = reaper::misc::get_size(is);
			char *rcfile = new char[filesize+1];
			is.read(rcfile, filesize);
			rcfile[filesize] = 0;

			reg_comb.begin();
			test_and_apply_rc(rcfile);
			reg_comb.end();

			alc = false;
		}

		int i = 0;

		TexCoord tcd;
		tcd.u = 0.0;
		tcd.v = 0.0;

		mcnt = 0;

		for(std::list<MorphNode>::iterator li = mesh.active_vertices.begin(); li != mesh.active_vertices.end(); li++){			

			(*li).rendi = i;

			MorphState st = (*li).state;
			if(st == COL || st == SPLIT) {

				mcnt++;

				vertex_array[i] = *li;

				for(int j = 0; j < tex_cnt; j++){
					texture_array[j][i] = (*li).tc[j];
				}
			} else {
				vertex_array[i] = mesh.vertices[(*li).id];

				for(int j = 0; j < tex_cnt; j++){
					texture_array[j][i] = mesh.vertices[(*li).id].tc[j];
				}
			}

			//for(int j = 0; j < tex_cnt; j++)
			//	texture_array[j][i] = (*li).tc[j];

			//cout << i << ":\t" << vertex_array[i].normal << "\n";
			//cin.get();

			i++;
		}

		if(face_array.size() < (mesh.active_faces.size()*3))
			face_array.resize(mesh.active_faces.size()*3);

		lint::iterator li;
		i = 0;

		// aight this is the serious
		//cout << "active faces: " << mesh.active_faces.size() << "\n";
		for(li = mesh.active_faces.begin(); li != mesh.active_faces.end(); li++)
			for(int j = 0; j < 3; ++j) {
				int vi = mesh.get_active_vertex_relative(mesh.faces[*li].vertices[j]);
				face_array[i] = (*mesh.vertices[vi].active).rendi;
				++i;

			}

	}

	void PMRMorph::fast_render()
	{

		render_timer.start();

		static bool sup = true;
		if(sup){
			setup_render_arrays();
			sup = false;
		}
		
	        GLfloat pos1[4]={5000,5000,5000};
	        GLfloat white[3]={1,1,1};
	        GLfloat nocolor[4] = {0.0, 0.0, 0.0, 1.0};
        	
	        //::glEnable(GL_LIGHT0); 
	        //::glDisable(GL_LIGHT0);

	        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	        //::glEnable(GL_NORMALIZE);


		StateKeeper s1(GL_DEPTH_TEST, true);
		StateKeeper s2(GL_LIGHTING, true);

		ClientStateKeeper s7(GL_VERTEX_ARRAY, true);
		ClientStateKeeper s3(GL_NORMAL_ARRAY, true);
		ClientStateKeeper s4(GL_COLOR_ARRAY, true);

		glNormalPointer(GL_FLOAT, sizeof(GeomCore), vertex_array[0].normal.get());		 
		glVertexPointer(3,GL_FLOAT,sizeof(GeomCore), vertex_array[0].point.get());		

		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		
		
		float color[4]   = { 1.0, 1.0, 1.0, 1.0 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, nocolor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, nocolor);

		glDepthFunc(GL_LEQUAL);

		StateKeeper s6(GL_BLEND, true);
		glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

                int tex_pass = mesh.tex.cnt() < max_tex_pass ? mesh.tex.cnt() : max_tex_pass;

		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//	cout << "tex_pass: " << tex_pass << "\n";

		for(int ti = 0; ti < tex_pass; ++ti){

			//cout << "ti " << ti << endl;

			glColorPointer(4, GL_FLOAT, sizeof(TexCoord), &texture_array[ti][0].a[0]);

			mesh.tex.use(ti, false);
			ClientStateKeeper s5(GL_TEXTURE_COORD_ARRAY, true);
			glTexCoordPointer(2,GL_FLOAT,sizeof(TexCoord),&texture_array[ti][0].u);

			glDrawElements(GL_TRIANGLES, 3 * mesh.active_faces.size(), GL_UNSIGNED_INT, &face_array[0]);

			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}

		glDepthMask(GL_TRUE);

		render_timer.stop();
		render_debug();
	}


	void PMRMorph::fancy_render()
	{
		render_timer.start();

		static bool sup = true;
		if(sup){
			setup_render_arrays();
			sup = false;
		}
		
		GLfloat pos1[4]={5000,5000,5000};
		GLfloat white[3]={1,1,1};
		GLfloat nocolor[4] = {0.0, 0.0, 0.0, 1.0};
	
		ClientStateKeeper s12(GL_SECONDARY_COLOR_ARRAY_EXT, true);

		ClientStateKeeper s7(GL_VERTEX_ARRAY, true);
		//ClientStateKeeper s3(GL_NORMAL_ARRAY, true);
		ClientStateKeeper s4(GL_COLOR_ARRAY, true);

		//glNormalPointer(GL_FLOAT, sizeof(GeomCore), vertex_array[0].normal.get());		 
		glVertexPointer(3,GL_FLOAT,sizeof(GeomCore), vertex_array[0].point.get());		

		glDepthFunc(GL_LEQUAL);

		int tex_pass = mesh.tex.cnt() < max_tex_pass?mesh.tex.cnt():max_tex_pass;

		reg_comb.call();
                {
		StateKeeper s11(GL_REGISTER_COMBINERS_NV, true);

		// draw first pass
		
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,sizeof(TexCoord),&texture_array[0][0].u);
		mesh.tex.use(0, false);
		//tr->use("terrain_snow");
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,sizeof(TexCoord),&texture_array[1][0].u);
		mesh.tex.use(1, false);
		//tr->use("terrain_rock");

		glColorPointer(            3, GL_FLOAT, sizeof(TexCoord), &texture_array[0][0].a[0]);		
		glSecondaryColorPointerEXT(3, GL_FLOAT, sizeof(TexCoord), &texture_array[1][0].a[3]);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glDrawElements(GL_TRIANGLES, 3 * mesh.active_faces.size(), GL_UNSIGNED_INT, &face_array[0]);
                }
		// draw second pass 
		
		glActiveTextureARB(GL_TEXTURE0_ARB); 
		glTexCoordPointer(2,GL_FLOAT,sizeof(TexCoord),&texture_array[2][0].u);
		mesh.tex.use(2, false);
		//tr->use("terrain_dryland");

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glTexCoordPointer(2,GL_FLOAT,sizeof(TexCoord),&texture_array[2][0].u);
		mesh.tex.use(2, false);
		//tr->use("terrain_grass");

		glColorPointer( 3, GL_FLOAT, sizeof(TexCoord), &texture_array[2][0].a[0]);		
		glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);
		glSecondaryColor3fEXT(0.0f, 0.0f, 0.0f);
		//glSecondaryColorPointerEXT(3, GL_FLOAT, sizeof(Color), &grass_col[0]);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
                glDepthFunc(GL_EQUAL);
		glDepthMask(GL_FALSE);

		glDrawElements(GL_TRIANGLES, 3 * mesh.active_faces.size(), GL_UNSIGNED_INT, &face_array[0]);

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);


		glActiveTextureARB(GL_TEXTURE0_ARB);

		/* ------------

		glColorPointer(4, GL_FLOAT, sizeof(TexCoord), &texture_array[ti][0].a[0]);

		mesh.tex.use(ti, false);
		ClientStateKeeper s5(GL_TEXTURE_COORD_ARRAY, true);
		glTexCoordPointer(2,GL_FLOAT,sizeof(TexCoord),&texture_array[ti][0].u);

		glDrawRangeElements(GL_TRIANGLES, 0, vertex_array.size(), 3 * mesh.active_faces.size(), GL_UNSIGNED_INT, &face_array[0]);

		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		
		------------- */
	
		glDepthMask(GL_TRUE);

		/*
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		*/
		render_timer.stop();

	}

	void PMRMorph::render()
	{
		test_timer.start();

		static int i;
		static double qrta;

		//cout << "camang: " << (camang*180/3.14) << "\n";

		lint::iterator li;
		GeomCore* p[3];

		if(mesh.active_faces.size() == 0)return;

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //clears the colour and depth buffers
		GLfloat nocolor[4] = {0.0, 0.0, 0.0, 1.0};
		GLfloat yellow[3]={1.0,1.0, 0.0};
		GLfloat pos1[4]={0,0,10,0};
		glLightfv(GL_LIGHT0, GL_AMBIENT, nocolor);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, yellow);
		glLightfv(GL_LIGHT0, GL_SPECULAR, nocolor);
		glLightfv(GL_LIGHT0, GL_POSITION, pos1);
		::glEnable(GL_LIGHT0); 


		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		render_timer.start();

		//mesh.tex.use();

		glBegin(GL_TRIANGLES);  //tells OpenGL that we're going to start drawing triangles


			//float colorBronzeSpec[4] = { 1.0, 1.0, 0.4, 1.0 };
			//glMaterialfv(GL_FRONT, GL_SPECULAR, colorBronzeSpec);
			//glMaterialf(GL_FRONT, GL_SHININESS, 70.0);


			for(li = mesh.active_faces.begin(); li != mesh.active_faces.end(); li++){


				Point fn = mesh.faces[*li].normal;

				for(int j = 0; j < 3; j++)
					p[j] = morph_point(mesh.faces[*li].vertices[j]);

				//cout << "\n";
			
				//glColor3f(mesh.faces[*li].color.x, mesh.faces[*li].color.y, mesh.faces[*li].color.z);

				/*
				float color[4]= {1.0, 1.0, 1.0, 0.3};;
				float nocolor[4] = {0.0, 0.0, 0.0, 0.0};
				float red[4]  = {1.0, 0.0, 0.0, 0.0};
				float dark[4]  = {0.1, 0.1, 0.1, 0.0};
				color[4] = 1.0;
				glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
				glMaterialfv(GL_FRONT, GL_AMBIENT, dark);
				glMaterialfv(GL_FRONT, GL_SPECULAR, color);
				*/

				for(int j = 0; j < 3; j++){

					//int vi = mesh.get_active_vertex_relative(mesh.faces[*li].vertices[j]);
					//PMVertex& pmv = mesh.vertices[vi];

					/*
					glMaterialfv(GL_FRONT, GL_DIFFUSE, (*p[j]).color.get());
					glMaterialfv(GL_FRONT, GL_AMBIENT, dark);
					glMaterialfv(GL_FRONT, GL_SPECULAR, dark);
					*/
					
					//for(int ti = 0; ti < mesh.tex.cnt(); ++ti)
					//	glMultiTexCoord2fARB( (GL_TEXTURE0_ARB + ti), (*p[j]).tc[ti].u, (*p[j]).tc[ti].v );

					//glTexCoord2f( (*p[j]).tc[0].u, (*p[j]).tc[0].v );
					//glNormal3f(mesh.faces[*li].normal.x, mesh.faces[*li].normal.y, mesh.faces[*li].normal.z);
					glNormal3f((*p[j]).normal.x, (*p[j]).normal.y, (*p[j]).normal.z);
					glVertex3f((*p[j]).point.x, (*p[j]).point.y, (*p[j]).point.z);

					//cout << "debug " <<  (*p[j]).normal << "\n";

					//cout << (*p[j]).point.z << "\n";cin.get();
					
					//cout << "x, y, z: " << (*p[j]).point.x << ", " << (*p[j]).point.y << ", " << (*p[j]).point.z << "\n";
					//cout << "u, v: " << (*p[j]).tc[0].u << ", " << (*p[j]).tc[0].v << "\n";
					
					//glTexCoord2f((*p[j]).point.x / 10.0 , (*p[j]).point.y / 10.0);
				}


					//glFlush();
			}

		glEnd();


		render_timer.stop();


		//mesh.render();

		test_timer.stop();
	}

	void PMRMorph::render_debug()
	{

		//cout << cam.front << "\n";
		Point p(1.0, 0.0, 0.0);
		 float camang = 3.14*reaper::world::angle(cam.front, p)/180.0;
		
		//cout << camang << "\n";
		if(dot(cam.front, Point(0.0, 1.0, 0.0)) < 0)camang = -camang;
		//static float camang = 0.0;
		//camang+= 0.1;

		float len = 10.0;
		float da = (3.14*cam.horiz_fov/180.0) / 2.0;
		float x0 = len * cos(camang + da);
		float y0 = len * sin(camang + da);
		float x1 = len * cos(camang - da);
		float y1 = len * sin(camang - da);


		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		mesh.tex.stop();

		glBegin(GL_LINES);

			float dx = cos(cam.horiz_fov) * 10;
			float dy = sin(cam.horiz_fov) * 10;

			glColor4f(1.0, 1.0, 1.0, 1.0);
			glVertex3f(cam.pos.x + x0, cam.pos.y + y0, cam.pos.z);
			glVertex3f(cam.pos.x, cam.pos.y, cam.pos.z);
			glVertex3f(cam.pos.x + x1, cam.pos.y + y1, cam.pos.z);
			glVertex3f(cam.pos.x, cam.pos.y, cam.pos.z);

		glEnd();


		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
	    glLoadIdentity();
	    glOrtho(0,100,0,100,-10.0,10.0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		mesh.tex.stop();
		glLoadIdentity();

		glColor3f(1.0, 1.0, 1.0);
		char dt[256];

		int fc = mesh.active_faces.size();
		int tfc = mesh.faces.size();
		sprintf(dt, "face count: %d / %d (%f)", fc, tfc, 100.0*((float)fc)/((float)tfc));
		output(10, 90, dt);

		sprintf(dt, "test time: %f ms", test_timer.avg()*1000.0);
		output(50, 90, dt);

		sprintf(dt, "mesh time: %f ms", (frame_timer.avg())*1000.0);
		output(50, 87, dt);

		sprintf(dt, "tau: %f", tau);
		output(50, 84, dt);

		sprintf(dt, "upper bound: %d faces", upper_bound);
		output(50, 81, dt);

		sprintf(dt, "morphing vertices: %d faces", mcnt);
		output(50, 78, dt);

		sprintf(dt, "fps: %f", 1.0 / frame_timer.avg());
		output(10, 87, dt);

		sprintf(dt, "total frame time: %f ms", frame_timer.avg()*1000.0);
		output(10, 84, dt);

		sprintf(dt, "morph time: %f ms", alpha_timer.avg()*1000.0);
		output(10, 81, dt);

		sprintf(dt, "render time: %f ms", render_timer.avg()*1000.0);
		output(10, 78, dt);

		sprintf(dt, "refine time: %f ms", ref_timer.avg()*1000.0);
		output(10, 75, dt);

		glPopAttrib();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}


	GeomCore* PMRMorph::morph_point(int vi)
	{
		int ind = mesh.get_active_vertex_relative(vi);
		MorphState st = (*mesh.vertices[ind].active).state;
		int id = (*mesh.vertices[ind].active).id;
		int src = (*mesh.vertices[ind].active).src;
		if(id == src)throw MeshException("morph_point: equality.");

		if(st == COL || st == SPLIT){
			return &(*mesh.vertices[ind].active);
		}
		
		return &mesh.vertices[ind];
	}

	void PMRMorph::morph_split(int vi, moli::iterator* li)
	{
		PMVertex& pmv = mesh.vertices[vi];
		int src;
		MorphNode& mn = (*mesh.vertices[vi].active);

		if(mn.state == SPLIT)src = mn.src;
		else src = vi;

		mesh.split_children_at(vi, *li, SPLIT);
		(*mesh.vertices[pmv.vu].active).src = src;
		(*mesh.vertices[pmv.vt].active).src = src;

		(*mesh.vertices[pmv.vu].active).tc.resize(mesh.tex.cnt());
		(*mesh.vertices[pmv.vt].active).tc.resize(mesh.tex.cnt());

		if(mesh.vertices[vi].is_active){
			throw MeshException("cannot add active");
		}

		mesh.add_active_vertex_at(vi, mesh.active_vertices.begin());
		(*mesh.vertices[vi].active).state = ZOMBIE;
	}
	
	void PMRMorph::vsplit(int vi, moli::iterator* li)
	{

		int src = (*(*li)).id;

		split_cnt++;
	
		if(li == NULL)throw MeshException("PMRMorph::vsplit: Dont give me NULL.");
		else {

			if(!outside_view_frustum(mesh.vertices[vi]))morph_split(vi, li);
			else mesh.split_children_at(vi, *li);
		}

		if(look_for_fff(vi))throw MeshException("vsplit fff");
		

	}

	void PMRMorph::backtrace_ecol(int vi, moli::iterator& li)
	{

		if(mesh.vertices[vi].is_active)throw MeshException("backtrace_ecol");

		while( (( (*li).id == mesh.vertices[vi].vt) || ( (*li).id == mesh.vertices[vi].vu)) && li != mesh.active_vertices.end()) li++; 

		mesh.col_parent_at(vi, li);

	}

	void PMRMorph::morph_col(int vi)
	{

		if(mesh.vertices[vi].is_active){
			int st = (*mesh.vertices[vi].active).state;
			throw MeshException("cannot add active");
		}

		mesh.add_active_vertex(vi);

		(*mesh.vertices[vi].active).state = FIXED;

		PMVertex& pmv = mesh.vertices[vi];
		(*mesh.vertices[pmv.vt].active).state = COL;
		(*mesh.vertices[pmv.vt].active).alpha = 0.0;
		(*mesh.vertices[pmv.vt].active).src = vi;

		(*mesh.vertices[pmv.vu].active).state = COL;
		(*mesh.vertices[pmv.vu].active).alpha = 0.0;
		(*mesh.vertices[pmv.vu].active).src = vi;
	}
	
	bool PMRMorph::ecol(int vi, moli::iterator& li)
	{

		PMVertex& pmv = mesh.vertices[vi];
		bool bgn = false;
		bool zombie = mesh.vertices[vi].is_active;
		bool outside = outside_view_frustum(mesh.vertices[vi]) &&
					   outside_view_frustum(mesh.vertices[pmv.vu]) &&
					   outside_view_frustum(mesh.vertices[pmv.vt]);

		if(li == mesh.active_vertices.begin()){
			
			if(zombie || outside){
				//cout << "zombie\n";
				if(zombie)mesh.remove_active_vertex(vi);
				mesh.col(vi);
			} else morph_col(vi);			

			bgn = true;
		} else {

			li--;
			if( ((*li).id == mesh.vertices[vi].vt) || ((*li).id == mesh.vertices[vi].vu) )
				if(li == mesh.active_vertices.begin()) bgn = true;
				else li--;

			if( ((*li).id == mesh.vertices[vi].vt) || ((*li).id == mesh.vertices[vi].vu) )
				if(li == mesh.active_vertices.begin()) bgn = true;
				else li--;


			if(zombie || outside){
				mesh.col(vi);
			} else {
				morph_col(vi);			
			}


			li++;
		}


		if(bgn)li = mesh.active_vertices.begin();

		// Reconsider vl/vr 
		int vl = (mesh.vertices[vi].flr[0] == -1)?-1:mesh.get_active_vertex_relative(mesh.faces[mesh.vertices[vi].flr[0]].vertices[2]);
		int vr = (mesh.vertices[vi].flr[1] == -1)?-1:mesh.get_active_vertex_relative(mesh.faces[mesh.vertices[vi].flr[1]].vertices[1]);


		//cout << "vl/vr:" << vl << "/" << vr << "\n";

		if(vl != -1){relocate_vertex(vl, li);}
		if(vr != -1){relocate_vertex(vr, li);}

		//cout << "col_out\n";

		return true;
	}

	bool PMRMorph::look_for_fff(int vi)
	{
		if(vi == -1)return false;
		if(!mesh.vertices[vi].is_active)return false;
		
		PMVertex& pmv = mesh.vertices[vi];

		MorphState s = (*mesh.vertices[vi].active).state;

		MorphState svu = pmv.vu != -1 && (mesh.vertices[pmv.vu].is_active)?(*mesh.vertices[pmv.vu].active).state:ZOMBIE;
		MorphState svt = pmv.vt != -1 && (mesh.vertices[pmv.vt].is_active)?(*mesh.vertices[pmv.vt].active).state:ZOMBIE;

		if( s == FIXED && svu == FIXED && svt == FIXED ){
			return true;
		}

		return false;

	}
		
	void PMRMorph::force_vsplit(moli::iterator& li)
	{
		int vi = (*li).id;

		moli::iterator restore = li;
		//lin::iterator tracer = li;

		restore++;

		int fwd = 0;
		
		vint vstack;
		vstack.clear();

		vint back_trace;

		vstack.push_back(vi);

		static int cnt;
		cnt = 0;

		/*
		if(mesh.vertices[995].is_active){
			cout << "#995: " << (*mesh.vertices[995].active).state << "\n";
			mesh.vertices[995].report(995);
			cin.get();
		}
	*/

		while(!vstack.empty()){

			if(vstack.size() > stack_depth) stack_depth = vstack.size();

			int vind = vstack.back();
			PMVertex& v = mesh.vertices[vind];
			
			bool force_restore = mesh.vertices[vind].is_active && ((*mesh.vertices[vind].active).state == COL);

			//cout << "\nvstack.size: " << vstack.size() << "\tvind: " << vind << "\t";
			
			// Need to clear the stack at some point or it may grow
			// inefficiently deep
			if( (vstack.size() > max_stack_depth) || force_restore){
				vstack.clear();
				//restore--;
				li = restore;
				return;
			}

			
			//cin.get();
			// already taken care of
			if(v.has_children() && mesh.vertex_spawned(vind)) {
				//cout << " " << vind << " already taken care of \n";
				vstack.pop_back();
			// inactive, split parent
			} else if(!v.is_active) {
				//cout << " " << vind << " inactive, split parent (" << v.parent << ")\n";
				vstack.push_back(v.parent);
			// geometry ok - split
			} else if(vsplit_legal(v)){

				//back_trace.push_back(vind);
				//fwd++;
				if((*restore).id == vind)restore++;
				//cout << " " << vind << " geometry ok - split to " << v.vu << "/" << v.vt << " open: " << v.flr[0] << "/" << v.flr[1] << "\n";
				//cout << "$" << vind << mesh.vertices[19327].is_active << "\t";
				moli::iterator tli = li;
				if((*li).id == vind){
					//cout << " match ";
					tli++;
					if(li == mesh.active_vertices.begin()){
						vstack.pop_back();
						vsplit(vind, &restore);
						li = mesh.active_vertices.begin();
					} else {
						li--;
						vstack.pop_back();
						vsplit(vind, &restore);
						li++;
					}
				} else {
					//cout << " nocheck ";
					tli++;
					vstack.pop_back();
					vsplit(vind, &restore);

				}
				
			// geometry NOT ok - split to create necessary faces
			} else {
				//cout << " " << vind << " geometry NOT ok - split to create necessary faces: ";
				for(int i = 0; i < 4; i++) 
					if((v.fn[i] != -1) && !mesh.faces[v.fn[i]].is_active){
						//int tmp = mesh.vertices[mesh.faces[v.fn[i]].vertices[0]].parent;
						int tmp = find_collapsor(v.fn[i]);
						vstack.push_back(tmp);
						//cout << tmp << "(" << v.fn[i] << ") ";
					}
				//cout << "\n";


			}

		//	cin.get();

		//cout << "\n";	
			
				
		}
	}

	int PMRMorph::find_collapsor(int fi)
	{

		int vtmp = mesh.faces[fi].vertices[0];
		while(  (mesh.vertices[vtmp].flr[0] != fi) && (mesh.vertices[vtmp].flr[1] != fi) )
			vtmp = mesh.vertices[vtmp].parent;

		return vtmp;
	}

	void PMRMorph::update_alpha(float& alpha)
	{
		alpha+= 0.2;
	}

	void PMRMorph::morph_col_vertex(MorphNode& mn)
	{
		int i;

		mn.point  = (1.0 - mn.alpha)*mesh.vertices[mn.id].point + mn.alpha*mesh.vertices[mn.src].point;
		mn.normal = (1.0 - mn.alpha)*mesh.vertices[mn.id].normal + mn.alpha*mesh.vertices[mn.src].normal;
		mn.color  = (1.0 - mn.alpha)*mesh.vertices[mn.id].color + mn.alpha*mesh.vertices[mn.src].color;

		for(i = 0; i < mesh.tex.cnt(); i++)
			mn.tc[i] = mesh.vertices[mn.id].tc[i]*(1.0 - mn.alpha) + mesh.vertices[mn.src].tc[i]*mn.alpha;
/*
			TexCoord tca = mesh.vertices[mn.id].tc[i] * 0.1;
			TexCoord tcb = mesh.vertices[mn.src].tc[i];

			cout << "id :" << tca.u << ", " << tca.v << "\n";
			cout << "src:" << tcb.u << ", " << tcb.v << "\n";

			cout << "col: " << mn.tc[i].u << ", " << mn.tc[i].v << "\n";
			cin.get();
			
		}
*/
	}
	
	void PMRMorph::morph_split_vertex(MorphNode& mn)
	{
		int i;

		Point p(0.0, 0.0, 1.0);

		mn.point  = mn.alpha*mesh.vertices[mn.id].point + (1.0 - mn.alpha)*mesh.vertices[mn.src].point;
		mn.normal = mn.alpha*mesh.vertices[mn.id].normal + (1.0 - mn.alpha)*mesh.vertices[mn.src].normal;
		mn.color  = mn.alpha*mesh.vertices[mn.id].color + (1.0 - mn.alpha)*mesh.vertices[mn.src].color;

		for(i = 0; i < mesh.tex.cnt(); i++)
			mn.tc[i] = mesh.vertices[mn.id].tc[i]*mn.alpha + mesh.vertices[mn.src].tc[i]*(1.0 - mn.alpha);
/*
			// cout << "split: " << mn.tc[i].u << ", " << mn.tc[i].v << "\n";
			// cin.get();
		}
		*/
	}

void PMRMorph::morph_pass()
{
	alpha_timer.start();

	int p;
	//cout << "\n\t\t\tentering morph_pass";

	//cout << "\nvsize: " << mesh.active_vertices.size() << "\n";

	moli::iterator li = mesh.active_vertices.begin();
	moli::iterator tmp;

	while(li !=  mesh.active_vertices.end()){

		//for(moli::iterator tli = mesh.active_vertices.begin(); tli != mesh.active_vertices.end(); tli++)cout << (*tli).id << ", ";
		//cout << "\n";

		//cout << (*li).id << "\n";
		
		tmp = li;
		li++;
		int size = mesh.active_vertices.size();
		int id = (*tmp).id;

		if(!mesh.vertices[id].is_active){
			//for(moli::iterator tli = mesh.active_vertices.begin(); tli != mesh.active_vertices.end(); tli++)cout << (*tli).id << ", ";
			//cout << "\n";

			throw MeshException("inactive foo");
		}

	//	cout << "#115: " << mesh.vertices[115].is_active  << "\t";
	//	cout << "#114: " << mesh.vertices[114].is_active  << "\t";
	//	cout << "#89: " << mesh.vertices[89].is_active  << "\n";

		

		if( look_for_fff(id) ){
			throw MeshException("morph_pass: someone else was naughty!");
		}


		//cout << mesh.active_vertices.size() << "#";
		switch((*tmp).state){

			case ZOMBIE: //cout << (*tmp).id << " zombie\n";
						    mesh.remove_active_vertex((*tmp).id);
						    break;

			case SPLIT:	 //cout << (*tmp).id << " split\n";
						    update_alpha( (*tmp).alpha);
						    if( (*tmp).alpha >= 1.0){
							    //(*tmp).alpha -= 0.2;
							    (*tmp).state = FIXED;
						    } else {
							    /*
							    int tmp_id = (*tmp).id;
							    int tmp_src = (*tmp).src;
							    Point p0 = mesh.vertices[tmp_id].point;
							    Point p1 = mesh.vertices[tmp_src].point;
							    float alpha = (*tmp).alpha;
							    (*tmp).point = alpha*p0 + (1.0 - alpha)*p1;
							    */
							    morph_split_vertex(*tmp);
	
							    //(*tmp).point = (*tmp).alpha*mesh.vertices[(*tmp).id].point + (1.0 - (*tmp).alpha)*mesh.vertices[(*tmp).src].point;
						    }
						    break;

			case COL:	 p = mesh.vertices[(*tmp).id].parent;
						    update_alpha( (*tmp).alpha);
						    if( (*tmp).alpha >= 1.0){
							    int vu = mesh.vertices[p].vu;
							    int vt = mesh.vertices[p].vt;
							    //cout << (*tmp).id << " col parens is:" << p << "|" << vu << " / " << vt << "\n";
							    
							    if(li != mesh.active_vertices.end())if(mesh.vertices[p].vu == (*li).id)li++;
							    if(li != mesh.active_vertices.end())if(mesh.vertices[p].vt == (*li).id)li++;
							    mesh.col(p, false);
						    } else {
							    morph_col_vertex(*tmp);
							    //(*tmp).point = (1.0 - (*tmp).alpha)*mesh.vertices[(*tmp).id].point + (*tmp).alpha*mesh.vertices[(*tmp).src].point;
						    }
						    break;

			case FIXED:  //cout << (*tmp).id << " fixed\n";
						    break;

		}


		if( look_for_fff(id) ){
			throw MeshException("morph_pass: I was naughty!");
		}
	}

	alpha_timer.stop();

	//cout << "\n\t\t\texiting morph_pass";

}

bool PMRMorph::vertex_setup_zss(int vi)
{
	if(!mesh.vertices[vi].is_active)return false;
	PMVertex& pmv = mesh.vertices[vi];

	MorphState s = (*mesh.vertices[vi].active).state;
	if(s != ZOMBIE)return false;

	MorphState svu = (*mesh.vertices[pmv.vu].active).state;
	if(svu != SPLIT)return false;

	MorphState svt = (*mesh.vertices[pmv.vt].active).state;
	if(svt != SPLIT)return false;

	return true;
}

bool PMRMorph::vertex_setup_iff(int vi)
{
	if(mesh.vertices[vi].is_active)return false;
	PMVertex& pmv = mesh.vertices[vi];

	MorphState svu = (*mesh.vertices[pmv.vu].active).state;
	if(svu != FIXED)return false;

	MorphState svt = (*mesh.vertices[pmv.vt].active).state;
	if(svt != FIXED)return false;

	
	PMVertex pmvc = mesh.vertices[pmv.vu];
	if(pmvc.has_children()){

		if( mesh.vertices[pmvc.vu].is_active)
			return false;

		if( mesh.vertices[pmvc.vt].is_active)
			return false;
		//svu = (*mesh.vertices[pmvc.vu].active).state;
		//if(svu != FIXED)return false;
	}


	pmvc = mesh.vertices[pmv.vt];
	if(pmvc.has_children()){

		if( mesh.vertices[pmvc.vu].is_active)
			return false;

		if( mesh.vertices[pmvc.vt].is_active)
			return false;
		//svu = (*mesh.vertices[pmvc.vu].active).state;
		//if(svu != FIXED)return false;
	}
	

	return true;
}


void PMRMorph::refine()
{
	// for each {v | v in ActiveVertices}
	// if v.vt and qrefine(v)
	//		force_vsplit(v)
	// else if v.parent and ecol_legal(v.parent) and not qrefine(v.parent)
	//		ecol(v.parent)  ((and reconsider some vertices)) <- WHICH???
	//qtab.clear();


	static bool cond;
	cond = false;//render_cnt > 100;
	qrefine_cnt = split_cnt = 0;
	static int cnt, scnt, ecnt;
	cnt = scnt = ecnt = 0;

	static int morph_trig = 0;

	ref_timer.start();

	moli::iterator li, tmp, lj;

	li = mesh.active_vertices.begin();

	int smax = 0;

	//cout << "\n\t\t\tentering refine";


	if(morph_trig == 0)while(li !=  mesh.active_vertices.end()){
/*
		cout << "\nactives: ";
		for(moli::iterator tli = mesh.active_vertices.begin(); tli != mesh.active_vertices.end(); tli++)cout << (*tli).id << ", ";
		cout << "\n";
		cout << "looking at: " << (*li).id << "\n";
*/

		static int fcnt;
		fcnt = mesh.active_faces.size();


		if(mesh.active_vertices.size() > smax) smax = mesh.active_vertices.size();
		cnt++;

		if(false){
			lj = mesh.active_vertices.begin();
			for(int i = 0; i < 17; i++){
				//cout << (*lj).id << ", ";
				if(lj != mesh.active_vertices.end())lj++;
			}
		}

		
		int id = (*li).id;
		
		PMVertex& v = mesh.vertices[(*li).id];

	//	if(look_for_fff(id) || (v.has_parent() && look_for_fff(v.parent)))throw MeshException("refine got there first!");
	

			if(  ((*li).state != ZOMBIE) && 
				    ((*li).state != COL) && 
				    v.has_children() && 
				    qrefine( (*li).id)){
				    //cout << "vsplit..." << v.vt << " / " << v.vu;

				scnt++;
				int backup = (*li).id;
				force_vsplit(li);	
				if(li != mesh.active_vertices.end())li++;
				if(cond)cin.get();
			} else if(v.has_parent() &&
					    !qrefine(v.parent) &&
					    ecol_legal(mesh.vertices[v.parent]) &&
					    ( vertex_setup_zss(v.parent) ||  vertex_setup_iff(v.parent) )
					    ){
	
				//if(true)cout << "ecol..." << mesh.vertices[v.parent].vu << " / " << mesh.vertices[v.parent].vt;
				//cout << "col ";
				ecol(v.parent, li);
				ecnt++;
				//if(access[*li] == 1)access[*li] = 2;;
			} else {
			
				if(col_debug){

				if(v.has_parent()){
					cout << !qrefine(v.parent) << ", " << ecol_legal(mesh.vertices[v.parent]) << "\n";

					PMVertex& vp = mesh.vertices[v.parent];

					bool b0a = vp.vt != -1 || mesh.vertices[vp.vt].is_active;
					bool b0b = vp.vu != -1 || mesh.vertices[vp.vu].is_active;


					// Matching neighbours?		
					bool b0c = (vp.flr[0] == -1) || (mesh.faces[vp.flr[0]].nf[1] == vp.fn[0]);
					bool b0d = (vp.flr[0] == -1) || (mesh.faces[vp.flr[0]].nf[0] == vp.fn[1]);
					bool b0e = (vp.flr[1] == -1) || (mesh.faces[vp.flr[1]].nf[2] == vp.fn[2]);
					bool b0f = (vp.flr[1] == -1) || (mesh.faces[vp.flr[1]].nf[0] == vp.fn[3]);

					cout << "\n" << b0a << b0b << b0c << b0d << b0e << b0f << "\n";
				}

				
				if( (*li).state != COL && v.has_parent() && !qrefine(v.parent) && ecol_legal(mesh.vertices[v.parent])){

					PMVertex& pmv = mesh.vertices[mesh.vertices[(*li).id].parent];
		 			static const char ss[4][7] = { "FIXED", "SPLIT", "COL", "ZOMBIE" };

					cout << "no action: ";
					if(mesh.vertices[mesh.vertices[(*li).id].parent].is_active){
						cout << ss[(*mesh.vertices[mesh.vertices[(*li).id].parent].active).state] << "\n";
					}  else cout << "vi not active";

					if(mesh.vertices[pmv.vu].is_active){
						MorphState svu = (*mesh.vertices[pmv.vu].active).state;
						cout << " svu: " << ss[svu] << "\n";
						PMVertex& pmvu = mesh.vertices[pmv.vu];
							if(  pmvu.vt != -1 && (mesh.vertices[pmvu.vt].is_active) )
									cout << "\tsvuvt: " << ss[(*mesh.vertices[pmvu.vt].active).state] << "\n";
							if(  pmvu.vt != -1 && (mesh.vertices[pmvu.vu].is_active) )
									cout << "\tsvuvu: " << ss[(*mesh.vertices[pmvu.vu].active).state] << "\n";

					} else cout << " svu: N/A";


					if(mesh.vertices[pmv.vt].is_active){
						MorphState svt = (*mesh.vertices[pmv.vt].active).state;
						cout << " svt: " << ss[svt] << "\n";
						PMVertex& pmvt = mesh.vertices[pmv.vt];
							if( pmvt.vt != -1 && (mesh.vertices[pmvt.vt].is_active) )
									cout << "\tsvtvt: " << ss[(*mesh.vertices[pmvt.vt].active).state] << "\n";
							if( pmvt.vt != -1 && (mesh.vertices[pmvt.vu].is_active) )
									cout << "\tsvtvu: " << ss[(*mesh.vertices[pmvt.vu].active).state] << "\n";

					} else cout << " svt: N/A";
					cout << "\n";
				}

				}
				
				//if(true)cout << "take no action...";  
				if(li != mesh.active_vertices.end())li++;
			}

		//cout << "\n";
	
	}
	//spec_report(true);

	//cout << "\n\t\t\toff to morphy";

	ref_timer.stop();

	morph_pass();
	//cspec_report(true);

	morph_trig = (morph_trig+1) % 1;

	if(mesh.active_faces.size() > upper_bound){
		tau *= 1.05;
	} else if (mesh.active_faces.size() < upper_bound - 100) {
		tau *= 0.95;
	} else if(mesh.active_faces.size() < lower_bound) tau *= 0.99;
	

}

void PMRMorph::spec_report(bool verbose)
{
	char ss[4][7] = { "FIXED", "SPLIT", "COL", "ZOMBIE" };

	cout << "\n";
	cout << "-----------------------\n";
	cout << "| PMRMorph reporting |" << "\n";
	cout << "-----------------------" << "\n";
	cout << "Vertex count:\t " << (mesh).vertices.size() << "\n";
	cout << "Faces count:\t" << (mesh).faces.size() << "\n";
	cout << "\n";
	cout << "Active Vertex count:\t " << (mesh).active_vertices.size() << "\n";
	cout << "Active Faces count:\t" << (mesh).active_faces.size() << "\n";
	cout << "\n";

	if(verbose){
		cout << "#\tactive\tvt\tvu\tparent\tx\ty\tz\tstate\n";
		for(int i = 0; i < (mesh).vertices.size(); i++){
			cout << i << "\t" 
				 << (mesh).vertices[i].is_active << "\t" 
				 << (mesh).vertices[i].vt << "\t" 
				 << (mesh).vertices[i].vu << "\t" 
				 << (mesh).vertices[i].parent << "\t";

			cout << (mesh).vertices[i].point.x << "\t";
			cout << (mesh).vertices[i].point.y << "\t";
			cout << (mesh).vertices[i].point.z << "\t";
			if( mesh.vertices[i].is_active )cout << ss[(*(mesh.vertices[i].active)).state] << "\t";
			else cout << "-1\t";
			cout << "\n";
		}
	}


	
}


}
}
}
