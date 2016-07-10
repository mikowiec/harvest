#ifndef REAPER_GFX_VERTEX_ARRAY_H
#define REAPER_GFX_VERTEX_ARRAY_H

#include "gfx/managers.h"
#include "os/gl.h"

namespace reaper {
namespace gfx {
namespace lowlevel  {

template<class T>
class VertexArray
{
	T* ptr;
	mutable VertexRef ref;
public:
	VertexArray();
	VertexArray(size_t size);
	~VertexArray();

	// keep_data ensures all data is intact in the new array (using memcpy)
	void resize(size_t size, bool keep_data = false);
	void free();

	bool writable() const; ///< checks if memory region is safe for writing
	void wait() const;     ///< waits for region to be accessible

	int size() const;

	void glDrawArrays(GLenum mode, GLuint first, GLsizei count) const;
	void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) const ;
	void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices) const;

	T& operator*()       { return *ptr; }
	T* operator->()      { return *ptr; }
	T& operator[](int i) { return ptr[i]; }
	T* get()             { return ptr; }

	const T& operator*() const       { return *ptr; }
	const T* operator->() const      { return *ptr; }
	const T& operator[](int i) const { return ptr[i]; }
	const T* get() const             { return ptr; }

};

//////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////////////

template<class T>
inline VertexArray<T>::VertexArray() : ptr(0)
{
	if (!ref.valid())
		VertexRef::create();
}

template<class T>
inline VertexArray<T>::VertexArray(size_t n) : ptr(0)
{
	if (!ref.valid())
		VertexRef::create();
	ptr = static_cast<T*>(ref->malloc(n*sizeof(T)));
}

template<class T>
inline VertexArray<T>::~VertexArray()
{
	if(ptr != NULL) ref->free(ptr);
	if (ref.count() == 1)
		VertexRef::destroy();
}

template<class T>
inline void VertexArray<T>::free()
{
	if(ptr != null) ref->free(ptr);
	ptr = 0;
	size = 0;
}

template<class T>
inline void VertexArray<T>::resize(size_t n, bool keep_data)
{
	if(ptr == 0) {
		ptr = static_cast<T*>(ref->malloc(n*sizeof(T)));
	} else {
		ptr = static_cast<T*>(ref->realloc(ptr, n*sizeof(T), keep_data));
	}
}

template<class T>
inline bool VertexArray<T>::writable() const
{
	return ref->writable(ptr);
}

template<class T>
inline void VertexArray<T>::wait() const
{
	ref->wait(ptr);
}

template<class T>
inline int VertexArray<T>::size() const
{ 
	return ref->size(ptr) / sizeof(T); 
}

template<class T>
inline void VertexArray<T>::glDrawArrays(GLenum mode, GLuint first, GLsizei count) const
{
	ref->glDrawArrays(ptr, mode, first, count);
}

template<class T>
inline void VertexArray<T>::glDrawElements(GLenum mode, GLsizei count, 
					   GLenum type, const GLvoid *indices) const
{
	ref->glDrawElements(ptr, mode, count, type, indices, sizeof(T));
}

template<class T>
inline void VertexArray<T>::glDrawRangeElements(GLenum mode, GLuint start, GLuint end, 
						GLsizei count, GLenum type, const GLvoid *indices) const

{
	ref->glDrawRangeElements(ptr, mode, start, end, count, type, indices, sizeof(T));
}

}
}
}

#endif

/*
 * $Author: fizzgig $
 * $Date: 2003/05/20 10:48:09 $
 * $Log: vertex_array.h,v $
 * Revision 1.21  2003/05/20 10:48:09  fizzgig
 * added includes
 *
 * Revision 1.20  2003/01/11 23:58:44  fizzgig
 * improved opengl implementation reporting
 *
 * Revision 1.19  2002/05/22 17:32:38  pstrand
 * no more glut.h
 *
 * Revision 1.18  2002/05/22 10:04:46  fizzgig
 * no message
 *
 * Revision 1.17  2002/05/20 20:44:25  myrzlok
 * free på null verkar inte alltid funka
 *
 * Revision 1.16  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.15  2002/02/07 00:02:52  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.14  2002/01/10 23:09:09  macke
 * massa bök
 *
 * Revision 1.13  2001/12/14 16:31:25  macke
 * meck å pul
 *
 * Revision 1.12  2001/08/06 12:16:17  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.11.4.1  2001/08/03 13:43:54  peter
 * pragma once obsolete...
 *
 * Revision 1.11  2001/07/06 01:47:15  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.10  2001/06/09 01:58:55  macke
 * Grafikmotor reorg
 *
 * Revision 1.9  2001/06/07 05:14:21  macke
 * Reaper v0.9
 *
 * Revision 1.8  2001/05/10 11:40:19  macke
 * häpp
 *
 */

