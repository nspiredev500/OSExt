#include "kernel.h"

#define STRINGIFYMAGIC(x) #x
#define STRINGIFY(x) STRINGIFYMAGIC(x)


#include "hook.h"
#include "nclockfragments.h"

// have to change clear_cache to clear_caches
#undef HOOK_INSTALL
#define HOOK_INSTALL(address, hookname) do { \
	void hookname(void); \
	extern unsigned __##hookname##_end_instrs[4]; /* orig_instrs1; orig_instrs2; ldr pc, [pc, #-4]; .long return_addr */ \
	__##hookname##_end_instrs[3] = (unsigned)(address) + 8; \
	__##hookname##_end_instrs[0] = *(unsigned*)(address); \
	*(unsigned*)(address) = 0xE51FF004; /* ldr pc, [pc, #-4] */ \
	__##hookname##_end_instrs[1] = *(unsigned*)((address) + 4); \
	*(unsigned*)((address) + 4) = (unsigned)hookname; \
	__##hookname##_end_instrs[2] = 0xE51FF004; /* ldr pc, [pc, #-4] */ \
	clear_caches(); \
	} while (0)

#undef HOOK_UNINSTALL
#define HOOK_UNINSTALL(address, hookname) do { \
	extern unsigned __##hookname##_end_instrs[4]; /* orig_instrs1; orig_instrs2; ... */ \
	*(unsigned*)(address) = __##hookname##_end_instrs[0]; \
	*(unsigned*)((address) + 4) = __##hookname##_end_instrs[1]; \
	clear_caches(); \
} while (0)




/*
static void* drawhook_adrs[] = {
	0, 0, 0, 0, 0, 0, // 3.1 nonCAS/Cas, CX, CM
	0, 0, 0, 0, // 3.6
	0, 0, 0, 0, 0, 0,// 3.9
	0, 0, // 3.91 CX nonCAS/CAS
	0, 0, 0, 0, // 4.0
	0, 0, // 4.0.3
	0, 0, // 4.2
	0, 0, // 4.4
	0, 0, // 4.5
	0, 0, // 4.5.1
};
*/

// from ndless sdk utils.c
static void ut_disable_watchdog(void)
{
	// Disable the watchdog on CX that may trigger a reset
	*(volatile unsigned*)0x90060C00 = 0x1ACCE551; // enable write access to all other watchdog registers
	*(volatile unsigned*)0x90060008 = 0; // disable reset, counter and interrupt
	*(volatile unsigned*)0x90060C00 = 0; // disable write access to all other watchdog registers
}


static const uint32_t filehook_addrs[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,//0x10011178 works with fopen!   at osgc read
 0x10011178, 0x10011134,
 //0x100237d4,//0x10023810
 0x0, 0x0
};


static const uint32_t os_draw_addrs[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x100237c8,
 0x0, 0x0
};
static const uint32_t os_framebuffer2_addrs[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x13fb5000,
 0x0, 0x0
};
static const uint32_t os_framebuffer1_addrs[] =
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0, 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x0,
 0x0, 0x13fda800,
 0x0, 0x0
};
static uint32_t os_draw_inst[4];
static bool draw_clock = true;

uint32_t get_os_draw_address()
{
	return os_value(os_draw_addrs,sizeof(os_draw_addrs)/sizeof(uint32_t));
}

static void file_hookfunc();
static void draw_hookfunc();


HOOK_DEFINE(filehook)
{
	// seems to be in the gui-thread, because file syscalls are working
	ut_disable_watchdog();
	int intmask = TCT_Local_Control_Interrupts(-1);
	
	
	disableIRQ();
	//call_with_stack((void*)(0xe8000000+SMALL_PAGE_SIZE-8),file_hookfunc);
	// some syscalls might not work with a relocated stack, so it has to be changed before changing the address space
	file_hookfunc();
	enableIRQ();
	TCT_Local_Control_Interrupts(intmask);
	HOOK_RESTORE_RETURN(filehook);
};

HOOK_DEFINE(drawhook)
{
	ut_disable_watchdog();
	int intmask = TCT_Local_Control_Interrupts(-1);
	
	disableIRQ();
	call_with_stack((void*)(0xe8000000+SMALL_PAGE_SIZE-8),draw_hookfunc);
	enableIRQ();
	TCT_Local_Control_Interrupts(intmask);
	HOOK_RESTORE_RETURN(drawhook);
};






static volatile bool drawhook_enabled = false;
static uint32_t hookcounter = 0;
static uint32_t lastchanged = 0;
static uint32_t lastrun = 0;
void file_hookfunc()
{
	
	if (rtc_get_value()-lastrun == 0)
	{
		return;
	}
	
	
	if (isKeyPressed(KEY_CTRL) && isKeyPressed(KEY_EE) && isKeyPressed(KEY_G))
	{
		set_time_dialog();
	}
	
	if (isKeyPressed(KEY_CTRL) && isKeyPressed(KEY_PI))
	{
		background_update();
	}
	
	if (isKeyPressed(KEY_CTRL) && isKeyPressed(KEY_EE))
	{
		if (draw_clock)
		{
			draw_clock = false;
		}
		else
		{
			draw_clock = true;
		}
		lastchanged = rtc_get_value();
	}
	
	
	lastrun = rtc_get_value();
}




void draw_hookfunc()
{
	
	//DEBUGPRINTLN_1("drawhook: %d",(uint32_t) drawhook_enabled)
	if (! drawhook_enabled)
	{
		void* old_framebuffer = get_old_framebuffer_address();
		if (HOOK_SAVED_REGS(drawhook)[2] == os_value(os_framebuffer1_addrs,sizeof(os_framebuffer1_addrs)/sizeof(uint32_t)))
		{
			old_framebuffer = (void*) os_value(os_framebuffer1_addrs,sizeof(os_framebuffer1_addrs)/sizeof(uint32_t));
		}
		else
		{
			old_framebuffer = (void*) os_value(os_framebuffer2_addrs,sizeof(os_framebuffer2_addrs)/sizeof(uint32_t));
		}
		*LCD_UPBASE = old_framebuffer;
		return;
	}
	
	*LCD_UPBASE = getKernelPhysicalAddress(get_front_framebuffer_address());
	void* old_framebuffer = get_old_framebuffer_address();
	if (HOOK_SAVED_REGS(drawhook)[2] == os_value(os_framebuffer1_addrs,sizeof(os_framebuffer1_addrs)/sizeof(uint32_t)))
	{
		old_framebuffer = (void*) os_value(os_framebuffer1_addrs,sizeof(os_framebuffer1_addrs)/sizeof(uint32_t));
	}
	else
	{
		old_framebuffer = (void*) os_value(os_framebuffer2_addrs,sizeof(os_framebuffer2_addrs)/sizeof(uint32_t));
	}
	if (old_framebuffer == NULL)
	{
		old_framebuffer = get_old_framebuffer_address();
	}
	void* framebuffer = get_front_framebuffer_address();
	
	background_draw_image(old_framebuffer,old_framebuffer);
	
	if (draw_clock)
	{
		uint32_t clockx = 180;
		uint32_t clocky = 1;
		framebuffer_fillrect(old_framebuffer,180,1,70,10,0,0,0);
		framebuffer_drawrect(old_framebuffer,179,0,72,12,255,255,255);
		
		uint32_t hr = 0,min = 0,sec = 0;
		
		systime_unix_to_timestamp(systime_unix(),NULL,NULL,NULL,&hr,&min,&sec);
		
		framebuffer_write10pchar(old_framebuffer,clockx+60,clocky,255,0,0,sec%10,digits10p);
		framebuffer_write10pchar(old_framebuffer,clockx+50,clocky,255,0,0,(sec/10)%10,digits10p);
		
		framebuffer_setpixel(old_framebuffer,clockx+47,clocky+3,255,0,0);
		framebuffer_setpixel(old_framebuffer,clockx+47,clocky+7,255,0,0);
		
		
		framebuffer_write10pchar(old_framebuffer,clockx+35,clocky,255,0,0,min%10,digits10p);
		framebuffer_write10pchar(old_framebuffer,clockx+25,clocky,255,0,0,(min/10)%10,digits10p);
		
		
		framebuffer_setpixel(old_framebuffer,clockx+23,clocky+3,255,0,0);
		framebuffer_setpixel(old_framebuffer,clockx+23,clocky+7,255,0,0);
		
		
		framebuffer_write10pchar(old_framebuffer,clockx+10,clocky,255,0,0,hr%10,digits10p);
		framebuffer_write10pchar(old_framebuffer,clockx,clocky,255,0,0,(hr/10)%10,digits10p);
	}
	
	
	
	k_memcpy(framebuffer,old_framebuffer,320*240*2);
	
	
	
	
}






void install_hooks()
{
	uint32_t adr = os_value(filehook_addrs,sizeof(filehook_addrs)/sizeof(uint32_t));
	DEBUGPRINTLN_1("adr: 0x%x",adr)
	if (adr != 0)
	{
		HOOK_INSTALL(adr,filehook);
	}
	
	if (get_os_draw_address() != 0)
	{
		uint32_t* draw = (uint32_t*) get_os_draw_address();
		os_draw_inst[0] = *draw;
		os_draw_inst[1] = *(draw+1);
		os_draw_inst[2] = *(draw+2);
		os_draw_inst[3] = *(draw+3);
		draw = (uint32_t*) get_os_draw_address();
		*draw = 0xe1a00000; // nop
		*(draw+1) = 0xe1a00000; // nop
		*(draw+2) = 0xe1a00000; // nop
		*(draw+3) = 0xe1a00000; // nop
		
		
		HOOK_INSTALL(get_os_draw_address(),drawhook);
		drawhook_enabled = true;
	}
	
	
	
	
}
void uninstall_hooks()
{
	uint32_t adr = os_value(filehook_addrs,sizeof(filehook_addrs)/sizeof(uint32_t));
	if (adr != 0)
	{
		HOOK_UNINSTALL(adr,filehook);
	}
	
	if (get_os_draw_address() != 0)
	{
		HOOK_UNINSTALL(get_os_draw_address(),drawhook);
		uint32_t* draw = (uint32_t*) get_os_draw_address();
		*draw = os_draw_inst[0];
		*(draw+1) = os_draw_inst[1];
		*(draw+2) = os_draw_inst[2];
		*(draw+3) = os_draw_inst[3];
		
		drawhook_enabled = false;
	}
	
	
	
	
}










