/* MPEG Sound library

   (C) 1997 by Jung woo-jae */

// Soundplayer.cc
// Superclass of Rawplayer and Rawtofile
// It's used for set player of Mpegtoraw

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mpegsound.h"

namespace mpegsound {

/*********************/
/* Soundplayer class */
/*********************/
Soundplayer::~Soundplayer()
{
  // Nothing...
}

void Soundplayer::abort(void)
{
  // Nothing....
}

int Soundplayer::getprocessed(void)
{
  return 0;
}
bool Soundplayer::resetsoundtype(void)
{
  return true;
}

int Soundplayer::getblocksize(void)
{
  return 1024; // Default value
}

}
