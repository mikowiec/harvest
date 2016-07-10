#include "os/compat.h"
#include "pmrfast.h"
#include <iostream>

namespace reaper 
{
namespace gfx
{
namespace mesh
{

	using std::cout;
	using std::cin;

	
	void PMRFast::vsplit(int vi, moli::iterator* li)
	{

		split_cnt++;
	
                if(li == NULL) { 
                        mesh.split(vi);
                } else {
			mesh.split_children_at(vi, *li);
		}
	}

	void PMRFast::backtrace_ecol(int vi, moli::iterator& li)
	{

		if(mesh.vertices[vi].is_active)throw MeshException("backtrace_ecol");

		while( (( (*li).id == mesh.vertices[vi].vt) || ( (*li).id == mesh.vertices[vi].vu)) && li != mesh.active_vertices.end()) li++; 

		mesh.col_parent_at(vi, li);

	}

	
	void PMRFast::ecol(int vi, moli::iterator& li)
	{
		bool bgn = false;

		if(li == mesh.active_vertices.begin()){
			mesh.col(vi);
			bgn = true;
		} else {
			li--;
			if( ((*li).id == mesh.vertices[vi].vt) || ((*li).id == mesh.vertices[vi].vu) )
				if(li == mesh.active_vertices.begin()) bgn = true;
				else li--;
				mesh.col(vi);
			li++;
		}

		if(bgn)li = mesh.active_vertices.begin();

		// Reconsider vl/vr 
		int vl = (mesh.vertices[vi].flr[0] == -1)?-1:mesh.get_active_vertex_relative(mesh.faces[mesh.vertices[vi].flr[0]].vertices[2]);
		int vr = (mesh.vertices[vi].flr[1] == -1)?-1:mesh.get_active_vertex_relative(mesh.faces[mesh.vertices[vi].flr[1]].vertices[1]);

		if(vl != -1){relocate_vertex(vl, li);}
		if(vr != -1){relocate_vertex(vr, li);}
	}


		
	void PMRFast::force_vsplit(moli::iterator& li)
	{
		int vi = (*li).id;

		moli::iterator restore = li;
		//lin::iterator tracer = li;

		restore++;

		int fwd = 0;
		
		vint vstack;
		vint back_trace;

		vstack.push_back(vi);

		while(!vstack.empty()){

			if(vstack.size() > stack_depth) stack_depth = vstack.size();

			int vind = vstack.back();
			PMVertex& v = mesh.vertices[vind];

			//cout << "vstack.size: " << vstack.size() << "\n";
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
						int tmp = mesh.vertices[mesh.faces[v.fn[i]].vertices[0]].parent;
						vstack.push_back(tmp);
						//cout << tmp << "(" << v.fn[i] << ") ";
					}
				//cout << "\n";


			}

		//	cin.get();

			// Need to clear the stack at some point or it may grow
			// inefficiently deep
			if(vstack.size() > max_stack_depth){

				vstack.clear();
				
				//  for(int i = back_trace.size() - 1; i >= 0; i--){
				//	if(mesh.vertices[back_trace[i]].is_active)throw MeshException("backtracing..");
				//	backtrace_ecol(back_trace[i],  li);

				restore--;
				li = restore;
			}
			
			
				
		}
	}

	void PMRFast::refine()
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

		//static int VMAX = 100000;

		//for(int i = 0; i < VMAX; i++)access[i] = 0;

		ref_timer.start();

		moli::iterator li, tmp, lj;



		li = mesh.active_vertices.begin();

		int smax = 0;

		//cout << mesh.active_vertices.size() << "/";

		while(li !=  mesh.active_vertices.end()){

			static int fcnt;
			fcnt = mesh.active_faces.size();


			// cout << mesh.active_vertices.size() << "(" << *li << ")\t";

			if(mesh.active_vertices.size() > smax) smax = mesh.active_vertices.size();
			cnt++;

			

			//if(cond)cout << "\nActive vertices: " << mesh.active_vertices.size();
			//if(cond)cout << "refine active vertices: ";
			//for(lj = mesh.active_vertices.begin(); lj != mesh.active_vertices.end(); lj++)cout << *lj << " ";
			//cout << "\n";

			if(false){
				lj = mesh.active_vertices.begin();
				for(int i = 0; i < 17; i++){
					cout << (*lj).id << ", ";
					if(lj != mesh.active_vertices.end())lj++;
				}
				//cout << "\nrefine considering vertex: " << *li << "\t";
			}

			


			PMVertex& v = mesh.vertices[(*li).id];

			//cout << "p:" << v.has_parent() << " ";

			//if(v.has_parent())cout << "q:" << !qrefine(mesh.vertices[v.parent]) << " ";

			if(  v.has_children() && qrefine( (*li).id)){
				//cout << "vsplit..." << v.vt << " / " << v.vu;

				scnt++;
				int backup = (*li).id;
				force_vsplit(li);	
				if(li != mesh.active_vertices.end())li++;
				if(cond)cin.get();
			} else if((fcnt > lower_bound) && v.has_parent() && ecol_legal(mesh.vertices[v.parent]) && !qrefine(v.parent)) {
				//if(true)cout << "ecol..." << mesh.vertices[v.parent].vu << " / " << mesh.vertices[v.parent].vt;
				//cout << "col ";
				ecnt++;
				//if(access[*li] == 1)access[*li] = 2;;
				ecol(v.parent, li);
				if(cond)cin.get();
			} else {
				//if(true)cout << "take no action...";  
				if(li != mesh.active_vertices.end())li++;
			}
		 	//cout << "done\n";
			//cout << "\n";


		
		}

		//cin.get();
		/*
		int mac = 0;
		for(int i = 0; i < VMAX; i++)if(access[i] == 2){
			mac++;
			//cout << "\naccess[" << i << "] accessed: " << access[i] << "\n";
			//cin.get();
		}
		*/
		//cout << cnt << "/" << mac << "/" << ecnt << "/" << split_cnt << "\t";
		ref_timer.stop();

	}


}
}
}
