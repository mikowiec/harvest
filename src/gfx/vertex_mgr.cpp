/*
 * $Author: pstrand $
 * $Date: 2003/03/31 15:25:14 $
 * $Log: vertex_mgr.cpp,v $
 * Revision 1.23  2003/03/31 15:25:14  pstrand
 * free-list handling
 *
 * Revision 1.22  2003/01/11 23:58:44  fizzgig
 * improved opengl implementation reporting
 *
 * Revision 1.21  2002/12/29 17:04:36  pstrand
 * hw/ moved to os/
 *
 * Revision 1.20  2002/09/23 12:08:43  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.19  2002/09/08 16:55:58  fizzgig
 * cleanup
 *
 * Revision 1.18  2002/04/14 19:27:06  pstrand
 * try again... (uniqueptr)
 *
 * Revision 1.17  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.16  2002/02/26 22:41:18  pstrand
 * mackefix
 *
 * Revision 1.15  2002/02/08 11:27:54  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.14  2002/01/28 00:50:42  macke
 * gfx reorg
 *
 * Revision 1.13  2002/01/26 23:27:13  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.12  2002/01/23 04:42:49  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.11  2002/01/22 23:44:03  peter
 * reversed last two revs
 *
 * Revision 1.9  2002/01/10 23:09:10  macke
 * massa bök
 *
 * Revision 1.8  2001/12/14 16:31:25  macke
 * meck å pul
 *
 * Revision 1.7  2001/12/13 17:03:29  peter
 * småfixar...
 *
 * Revision 1.6  2001/10/02 15:22:27  macke
 * mhm!
 *
 * Revision 1.5  2001/08/20 22:11:32  macke
 * Dum intel..
 *
 * Revision 1.4  2001/07/31 21:57:57  macke
 * Nytt försök med vertexarrays..
 *
 * Revision 1.3  2001/07/31 00:31:45  macke
 * Do'h
 *
 * Revision 1.2  2001/07/06 01:47:15  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.1  2001/06/09 01:58:55  macke
 * Grafikmotor reorg
 *
 * Revision 1.8  2001/06/07 05:14:21  macke
 * Reaper v0.9
 *
 * Revision 1.7  2001/05/15 01:42:27  peter
 * minskade ner på debugutskrifterna
 *
 * Revision 1.6  2001/05/10 11:40:18  macke
 * häpp
 *
 */

#include "os/compat.h"

#include <sstream>
#include <algorithm>
#include <list>

#include "misc/map.h"
#include "misc/stlhelper.h"

#include "os/gl.h"
#include "os/debug.h"

#include "gfx/managers.h"
#include "gfx/settings.h"
#include "gfx/exceptions.h"
#include "gfx/misc.h"
#include "gfx/fence.h"
#include "gfx/displaylist.h"


namespace reaper {
namespace {
        debug::DebugOutput dout("gfx::vertex_array",2);
}
namespace misc { 
        UniquePtr<gfx::VertexMgr>::I UniquePtr<gfx::VertexMgr>::inst; 
}
namespace gfx {
namespace lowlevel {
namespace {
        const int mem_size = 4*1024*1024; // amount of memory to allocate
}

using misc::Fence;

class VertexMgrImpl 
{
	class Alloc
	{
		bool var_mem; // is memory allocated by the Vertex Array Range extension?
		std::auto_ptr<Fence> fence;
	public:
		size_t size;
		const void* ptr;
		bool is_var_mem() const { return var_mem; }

		void set()  const { if(fence.get()) fence->set(); } 
		void wait() const { if(fence.get()) fence->wait(); }
		bool test() const { return fence.get() ? fence->test() : true; }

		Alloc(const void* p, size_t s, bool var) : var_mem(var), fence(0), size(s), ptr(p) {
			if(Settings::current.use_nv_fence) {
				fence = std::auto_ptr<Fence>(new Fence);
			}
		}

		~Alloc() {
			if(!var_mem) {
				::free(const_cast<void*>(ptr));
			} 
		}
	};

	class FreeSpace {
	public:
		const void* ptr;
		size_t size;

		FreeSpace(const void* p, size_t s) : ptr(p), size(s) {}
		FreeSpace(const FreeSpace& fs) : ptr(fs.ptr), size(fs.size) { }
	};

	typedef reaper::misc::Map<const void*, Alloc> AllocCont;
	typedef std::list<FreeSpace> FreeCont;

	AllocCont allocs;	///< Allocations
	FreeCont  free_spaces;  ///< Free spots

	const GLenum var_enum;
	const void *var_mem;

	// data regarding state
	bool var_enabled;
	bool cva_locked;
	const void *current;
	
	bool slow_write; // if true, writes are slow too (as well as reads)

	void throw_if(const void *) const;

	void enable_var();
	void disable_var();
	void lock_array(int count);
	void unlock_array();

	void pre_call(const void *, int count = 0); // count = zero means all of array is to be used
	void post_call(const void *);

	void merge_more(FreeCont::iterator);
public:
	VertexMgrImpl();
	~VertexMgrImpl();

	void* malloc(size_t size);
	void* realloc(const void *ptr, size_t size, bool keep_data);
	void  free(const void *);

	bool writable(const void*);
	void wait(const void*);
	size_t size(const void*);

	void clear(); 
	int  total_size() const; 
	bool slow_writes() { return slow_write; }

	void glDrawArrays(const void *ptr, GLenum mode, GLuint first, GLsizei count);
	void glDrawElements(const void *ptr, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, int size);
	void glDrawRangeElements(const void *ptr, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices, int size);
};

class not_allocated : public fatal_error_base
{
	std::string error;
public:
	not_allocated(const void *ptr);
	const char* what() const { return error.c_str(); }
};

////////////////////////////////////////////////////////////////////////////////////
// MANAGER
////////////////////////////////////////////////////////////////////////////////////

VertexMgrImpl::VertexMgrImpl() : 
	allocs("gfx::vertex_array"), 
	var_enum(Settings::current.use_nv_vertex_array_range2 ?
	         GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV : GL_VERTEX_ARRAY_RANGE_NV),
	var_mem(0),
	var_enabled(false), 
	cva_locked(false), 
	current(0),
	slow_write(false)
{
	if(Settings::current.use_nv_vertex_array_range) {				
		var_mem = glAllocateMemoryNV(mem_size,0,1,.5);         // allocate in agp mem
		if(var_mem == 0) {
			var_mem = glAllocateMemoryNV(mem_size,0,0,1);  // allocate in video mem
			dout << "Note: Video ram used for geometry, not AGP!\n";
			slow_write = true;
		}
		if(var_mem == 0) {
			dout << "Failed to allocate any geometry memory\n";
		} else {
			glVertexArrayRangeNV(mem_size, var_mem);
			free_spaces.push_back(FreeSpace(var_mem, mem_size));
                        dout << "Allocated " << mem_size << " bytes at " << var_mem << "\n";
		}
	}

	//dout << "VertexMgrImpl created\n";
}

VertexMgrImpl::~VertexMgrImpl()
{
	throw_on_gl_error("vertex_array::VertexMgrImpl::~VertexMgrImpl() entering");
	dout << "VertexMgr destroyed\n";

	unlock_array();
	disable_var();

	if(var_mem != 0) {
		glFreeMemoryNV(const_cast<void*>(var_mem));
	}

	if(!allocs.empty()) {
		// ref counters are messed up, because this is not supposed to happen if all 
		// VertexArrays have been destroyed. (Since we're destroying this, refcount must be zero.)
		debug::DebugOutput dout("gfx::vertex_array",0);
		dout << "WARNING! Not all memory has been freed before calling destructor!!\n";
	}
	if (free_spaces.empty()) {
		if (var_mem != 0)
			dout << "WARNING! Free space list empty\n";
	} else if (free_spaces.begin()->ptr != var_mem ||
		   free_spaces.begin()->size != mem_size) {
		dout << "WARNING! Free space list is not restored correctly.. dumping:\n";
		for(FreeCont::iterator i = free_spaces.begin();
				i != free_spaces.end(); ++i) {
			dout << i->size << " bytes at " << i->ptr << '\n';
		}

	}

	throw_on_gl_error("vertex_array::VertexMgrImpl::~VertexMgrImpl() exit");
}

inline void* VertexMgrImpl::malloc(size_t size)
{
	/// FIXME: first fit strategy may not be optimal
	FreeCont::iterator i;
	for(i = free_spaces.begin(); i != free_spaces.end(); ++i) {
		if(i->size >= size) {
			break;
		}
	}

	Alloc* alloc = 0;
	if(i == free_spaces.end()) { 
		// no space found, allocate normal memory instead
		dout << "No free mem found, allocating from heap...\n";
		alloc = new Alloc(::malloc(size), size, false);
	} else {
		// space was found in var-mem, allocate and update free mem list
		alloc = new Alloc(i->ptr, size, true);

		if(size != i->size) {
			// if there is space left, insert entry for it in table
			const void* new_ptr = (char*)i->ptr + size;
			size_t new_size = i->size - size;
			free_spaces.push_back(FreeSpace(new_ptr, new_size));
		}
		free_spaces.erase(i);
	}

	allocs.insert(alloc, alloc->ptr);
	return const_cast<void*>(alloc->ptr);
}

inline void* VertexMgrImpl::realloc(const void *ptr, size_t size, bool keep_data)
{
	throw_if(ptr);

	dout << "realloc\n";

	void *temp = 0;
	if(keep_data) {
		temp = ::malloc(size);
		memcpy(temp,ptr,size);
	} 

	void *new_ptr = malloc(size);

	if(keep_data) {
		if(new_ptr != 0) {
			memcpy(new_ptr,temp,size);
		}
		::free(temp);
	}

	allocs.remove(ptr);
	return new_ptr;
}

void VertexMgrImpl::merge_more(FreeCont::iterator i)
{
	FreeCont::iterator j = free_spaces.begin();
	for (; j != free_spaces.end(); ++j) {
		if ((char*)i->ptr + i->size == j->ptr) {
			// merge up
			i->size += j->size;
			free_spaces.erase(j);
			break;
		} else if ((char*)j->ptr + j->size == i->ptr) {
			// merge down
			j->size += i->size;
			free_spaces.erase(i);
			break;
		}
	}
}

inline void VertexMgrImpl::free(const void *ptr)
{
	throw_if(ptr);

	Alloc &a = allocs[ptr];
	if(a.is_var_mem()) {
		wait(ptr);
                FreeCont::iterator i;
		for(i = free_spaces.begin(); i != free_spaces.end(); ++i) {
			if((const char*)a.ptr + a.size == i->ptr) {
				// free space is just before allocation
				i->ptr = a.ptr;
				i->size += a.size;
				merge_more(i);
				break;
			} else if((const char*)i->ptr + i->size == a.ptr) {
				// free space is just after allocation
				i->size += a.size;
				merge_more(i);
				break;
                        } 
		}
                if(i == free_spaces.end()) {
			FreeSpace fs(a.ptr, a.size);
			free_spaces.push_back(fs);
                }
	}
	allocs.remove(ptr); // deletes allocation

}

////////////////////////////////////////////////////////////////////////////////////////

inline int VertexMgrImpl::total_size() const 
{
	int mem = 0;	
	for(AllocCont::const_iterator i = allocs.begin(); i != allocs.end(); ++i) {
		mem += i->size;
	}
	return mem;
}

inline bool VertexMgrImpl::writable(const void *ptr)
{
	///@todo handle locked arrays here
	throw_if(ptr);
	Alloc &a = allocs[ptr];
	return a.is_var_mem() ? a.test() : true;
}
inline void VertexMgrImpl::wait(const void *ptr)
{
	///@todo handle locked arrays here
	throw_if(ptr);
	Alloc &a = allocs[ptr];
	if(a.is_var_mem()) a.wait();
}

inline size_t VertexMgrImpl::size(const void *ptr)
{
	throw_if(ptr);
	return allocs[ptr].size;
}

inline void VertexMgrImpl::throw_if(const void *ptr) const
{
	if(!allocs.is_loaded(ptr)) {
		throw not_allocated(ptr);
	}
}

//////////////////////////////////////////////////////////////////////////////////

inline void VertexMgrImpl::enable_var()
{
	if(!var_enabled) {
		::glEnableClientState(var_enum);
		var_enabled = true;
	}
}
inline void VertexMgrImpl::disable_var()
{
	if(var_enabled) {
		::glDisableClientState(var_enum);
		var_enabled = false;
	}
}
inline void VertexMgrImpl::lock_array(int count)
{
	if(Settings::current.use_ext_compiled_vertex_array) {
		glLockArraysEXT(0,count);
		cva_locked = true;
	}
}
inline void VertexMgrImpl::unlock_array()
{
	if(cva_locked) {
		glUnlockArraysEXT();
		cva_locked = false;
	}
}
///////////////////////////////////////////////////

inline void VertexMgrImpl::pre_call(const void *ptr, int count)
{
	if(current == ptr){
		return;
	}
	current = ptr;

	unlock_array();
	lock_array(count);

	Alloc &a = allocs[ptr];
	if(a.is_var_mem()) {
		enable_var();
	} else {
		disable_var();
	}
	throw_on_gl_error("VertexMgrImpl::prepare");
}
inline void VertexMgrImpl::post_call(const void *ptr)
{
/*
	if(allocs[ptr].is_var_mem()) {
		allocs[ptr].fence.set();
	}
*/
}

inline void VertexMgrImpl::clear()
{
	if(current != 0) {
		unlock_array();
		disable_var();
		current = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////

inline void VertexMgrImpl::glDrawArrays(const void *ptr, GLenum mode, GLuint first, GLsizei count)
{
	throw_if(ptr);
	pre_call(ptr, count);
	::glDrawArrays(mode, first, count);
	post_call(ptr);
	throw_on_gl_error("VertexMgrImpl::glDrawArrays");
}
inline void VertexMgrImpl::glDrawElements(const void *ptr, GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, int size)
{
	throw_if(ptr);
	pre_call(ptr, allocs[ptr].size/size);
	::glDrawElements(mode, count, type, indices);
	post_call(ptr);
	throw_on_gl_error("VertexMgrImpl::glDrawElements");
}

inline void VertexMgrImpl::glDrawRangeElements(const void *ptr, GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices, int size)
{
	throw_if(ptr);
	pre_call(ptr, allocs[ptr].size/size);
	::glDrawRangeElements(mode, start, end, count, type, indices);
	post_call(ptr);
	throw_on_gl_error("VertexMgrImpl::glDrawElements");
}

inline not_allocated::not_allocated(const void *ptr)
{
	std::stringstream ss;
	ss << "Memory at " << ptr << " does not point to a vertex array!\n";
	error = ss.str();
}
} // end lowlevel namespace
////////////////////////////////////////////////////////////////////////////////////
// VERTEXMGR
////////////////////////////////////////////////////////////////////////////////////

VertexMgr::VertexMgr() : i(new lowlevel::VertexMgrImpl) {}
VertexMgr::~VertexMgr() { }

//VertexRef VertexMgr::create()  { return VertexRef::create(new VertexMgr()); }
VertexRef VertexMgr::get_ref() { return VertexRef(); }

void* VertexMgr::malloc(size_t size) {
	return i->malloc(size);
}
void* VertexMgr::realloc(void *ptr, size_t size, bool keep_data) {
	return i->realloc(ptr, size, keep_data);
}
void  VertexMgr::free(void *ptr) {
	i->free(ptr);
}
size_t VertexMgr::size(void *ptr) {
	return i->size(ptr);
}
bool VertexMgr::writable(void *ptr) {
	return i->writable(ptr);
}
void VertexMgr::wait(void *ptr) {
	i->wait(ptr);
}
void VertexMgr::clear() {
	i->clear();
}
size_t VertexMgr::total_size() {
	return i->total_size();
}

void VertexMgr::glDrawArrays(const void *ptr, int mode, int first, int count) {
	i->glDrawArrays(ptr, mode, first, count);
}
void VertexMgr::glDrawElements(const void *ptr, int mode, int count, int type, const void *indices, int size) {
	i->glDrawElements(ptr,mode,count,type,indices,size);
}
void VertexMgr::glDrawRangeElements(const void *ptr, int mode, int start, int end, int count, int type, const void *indices, int size){
	i->glDrawRangeElements(ptr,mode,start,end,count,type,indices,size);
}

} // end namespace gfx
////////////////////////////////////////////////////////////////////////////////////
// GL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////

void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	if(gfx::VertexRef().valid() && !gfx::misc::DisplayList::compiling()) {
		gfx::VertexMgr::get_ref()->clear();
	} 
	::glDrawArrays(mode, first, count);
}
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	if(mode == 0 && count == 0) {
		return;
	}

	if(gfx::VertexRef().valid() && !gfx::misc::DisplayList::compiling()) {
		gfx::VertexMgr::get_ref()->clear();
	}
	::glDrawElements(mode, count, type, indices);
}

void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
{
	if(gfx::VertexRef().valid() && !gfx::misc::DisplayList::compiling()) {
		gfx::VertexMgr::get_ref()->clear();
	}
	::glDrawRangeElements(mode, start, end, count, type, indices);
}

} // end namespace reaper
