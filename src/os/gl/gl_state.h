/*
 * $Author: pstrand $
 * $Date: 2002/12/29 16:58:14 $
 * $Log: gl_state.h,v $
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.5  2002/05/20 09:59:58  fizzgig
 * dynamic environment mapping!
 *
 * Revision 1.4  2002/05/16 23:59:00  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.3  2002/01/10 23:09:11  macke
 * massa bök
 *
 * Revision 1.2  2001/12/11 17:27:06  macke
 * GL states now behaves properly upon restart...
 *
 * Revision 1.1  2001/06/07 05:14:24  macke
 * Reaper v0.9
 *
 * Revision 1.30  2001/05/08 17:30:38  macke
 * s3tc texturer sparas till disk
 */

#ifndef REAPER_HW_GL_STATE_H
#define REAPER_HW_GL_STATE_H

namespace reaper
{
	/*
	GLvoid glEnable(GLenum state);
	GLvoid glDisable(GLenum state);

	GLvoid glEnableClientState(GLenum state);
	GLvoid glDisableClientState(GLenum state);

	GLvoid glActiveTextureARB(GLenum unit);
	GLvoid glClientActiveTextureARB(GLenum unit);

	GLvoid glBindTexture(GLenum target, GLuint texture);
	GLvoid glBlendFunc(GLenum src, GLenum dst);

	GLboolean glIsEnabled(GLenum state);
	*/
	/// Reset all gl-states
	void reinit_glstates();
}

#endif

