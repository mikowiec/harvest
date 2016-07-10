#include "os/compat.h"
#include "os/snd.h"
//#include "res/res.h"

using namespace reaper;
using namespace reaper::os::snd;

void main()
{
	SoundSystem ss;

	Effect* eff1 = ss.PrepareEffect("test1");
	eff1->play();
	getchar();
}
