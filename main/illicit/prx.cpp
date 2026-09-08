#include "includes.h"
#include <sys/syscall.h>
#include "addresses.h"
#include "buttons.h"
#include "colors.h"
#include "core.h"
#include "hook.h"
#include "hud.h"
#include "misc.h"
#include "menu.h"
#include "natives.h"
#include "police.h"
#include "police_config.h"
#include "prx.h"

using namespace Buttons;
using namespace Hud;
using namespace Natives;
using namespace Menu_f;
using namespace Misc;

static bool g_initialized=false;

void Hook(){ if(!g_initialized)return; updateInput(); PoliciaBR::Tick(); if(isOpen)drawMenu(); else ENABLE_ALL_CONTROL_ACTIONS(); }
void Monitor(uint64_t){ sleep(2000); createMenuStruct(); PoliciaBR::Init(); g_initialized=true; for(;;)sleep(100); }

SYS_MODULE_INFO(policia_br,0,1,1);
SYS_MODULE_START(Main);

extern "C" int Main(void)
{
    hookFunctionStart(POLICIA_BR_HOOK_ADDRESS,(int)IS_PLAYER_ONLINE,(int)Hook);
    sys_ppu_thread_t thread;
    sys_ppu_thread_create(&thread,Monitor,0,2000,0x5000,0,"Policia BR GTA V");
    return SYS_PRX_RESIDENT;
}
