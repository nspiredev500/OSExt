


int32_t __attribute__ ((aligned (8))) current_lcd_mirror[320*240];

uint32_t __attribute__ ((aligned (8))) lcd_framebuffer[320*240];



static volatile uint32_t *watchdog_load = (uint32_t*) 0x90060000,
                         *watchdog_control = (uint32_t*) 0x90060008,
                         *watchdog_intclear = (uint32_t*) 0x9006000C,
                         *watchdog_lock = (uint32_t*) 0x90060C00;

static bool lcd_timer_enabled = false;
uint32_t *buffupreal;


use_undef_access = false;
bool noflicker = false;



static void lcd_timer_enable();


static __attribute__ ((interrupt("FIQ"))) void lcd_compat_fiq()
{
    // Acknowledge interrupt
    *watchdog_lock = 0x1ACCE551;
    *watchdog_intclear = 1;

    volatile uint32_t *buffup = 0xc0000010;
	*buffup = lcd_framebuffer;
	
	
	*watchdog_lock = 0x1ACCE551;
    *watchdog_load = 33000000 / 1200;
	
	
	//bkpt();
}


void enableNoflicker()
{
	
	volatile uint32_t *buffup = 0xc0000010;
	buffupreal = *buffup;
	
	
	
	lcd_timer_enable();
	noflicker = true;
	
	
	
	
	
}


void disableNoflicker()
{
	
	
	
	*(volatile uint32_t*) 0xDC00000C &= ~(1 << 3);
	// Deactivate watchdog IRQ
	*(volatile uint32_t*) 0xDC000014 = 1 << 3;
	*watchdog_lock = 0x1ACCE551;
	*watchdog_control = 0;
	
	
	lcd_timer_enabled = false;
	
	noflicker = false;
	
}


static void lcd_timer_enable()
{
    // Enable timer for conversion
    *watchdog_lock = 0x1ACCE551;
    *watchdog_load = 33000000 / 1200;
    *watchdog_control = 1;

    // Install FIQ handler
    *(volatile uint32_t*)0x3C = (uint32_t) lcd_compat_fiq;

    // Set first watchdog interrupt as FIQ
    *(volatile uint32_t*) 0xDC00000C = 1 << 3;
    // Activate watchdog IRQ
    *(volatile uint32_t*) 0xDC000010 = 1 << 3;

    // Enable FIQs
    uint32_t spsr;
    asm volatile("mrs %[spsr], spsr" : [spsr] "=r" (spsr));
    spsr &= ~0x40;
    asm volatile("msr spsr_c, %[spsr]" :: [spsr] "r" (spsr));

    lcd_timer_enabled = true;
}









void blitMirrorToScreen()
{
	if (noflicker)
	{
		memcpy(lcd_framebuffer,current_lcd_mirror,320*240*2);
	}
}




































