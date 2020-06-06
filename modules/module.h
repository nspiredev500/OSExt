#ifndef MODULE_H
#define MODULE_H


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// copied from ndless-sdk/include/nucleus.h

typedef struct {
  char * str;
  int len;
  int chunk_size;
  int unknown_field;
} * String;


typedef void NU_TASK;
typedef int PCFD;

typedef struct dstat {
	char unknown1[13];
	char filepath[266];		/* Null terminated: A:\... */
	unsigned char fattribute;	/* File attributes: TODO BROKEN! */
	unsigned short unknown2;
	unsigned short unknown3;
	unsigned short unknown4;
	unsigned short unknown5;
	unsigned long fsize;		/* File size */
	void *unknown6, *unknown7;
	unsigned int unknown8;
	unsigned short unknown9;
} DSTAT;

struct s_ns_event {
 unsigned int timestamp;
 unsigned short type; // 0x8 is key down, 0x10 is key up. 0x20 may be APD
 unsigned short ascii;
 unsigned int key;
 unsigned int cursor_x;
 unsigned int cursor_y;
 unsigned int unknown;
 unsigned short modifiers; // Shift = 3, Ctrl = 4, Caps = 0x10
 unsigned char click; // Click = 8
};

//Ascii representation of the 4 chars
typedef enum {
    RES_CLNK = 0x636C6E6B,
    RES_CTLG = 0x63746C67,
    RES_DCOL = 0x64636F6C,
    RES_DLOG = 0x646C6F67,
    RES_DTST = 0x64747374,
    RES_GEOG = 0x67656F67,
    RES_MATH = 0x6D617468,
    RES_MWIZ = 0x6D77697A,
    RES_NTPD = 0x6E747064,
    RES_PGED = 0x70676564,
    RES_QCKP = 0x71636B70,
    RES_QUES = 0x71756573,
    RES_SCPD = 0x73637064,
    RES_SYST = 0x73797374,
    RES_TBLT = 0x74626C74,
} e_resourceID;

// If the syscalls are used directly and not through newlib
struct nuc_stat {
	unsigned short st_dev;
	unsigned int st_ino; // 0
	unsigned int st_mode;
	unsigned short st_nlink; // 1
	unsigned short st_uid; // 0
	unsigned short st_gid; // 0
	unsigned short st_rdev; // = st_dev
	unsigned int st_size;
	unsigned int st_atime;
	unsigned int st_mtime;
	unsigned int st_ctime;
};

typedef void NUC_FILE;
typedef void NUC_DIR;
struct nuc_dirent {
        char d_name[1];
};

//NavNet declarations
struct _nn_ch;
typedef struct _nn_ch* nn_ch_t;
struct _nn_nh;
typedef struct _nn_nh* nn_nh_t;
struct _nn_oh;
typedef struct _nn_oh* nn_oh_t;

// end copy

// define the keycodes
enum keycode {KEY_RET = 0, KEY_ENTER, KEY_MATH_MINUS, KEY_SPACE, KEY_Z, KEY_Y, KEY_ZERO, KEY_QUESTION, KEY_ON,
			  KEY_X, KEY_W, KEY_V, KEY_THREE, KEY_U, KEY_T, KEY_S, KEY_ONE, KEY_PI, KEY_TRIG, KEY_TEN_POW,
			  KEY_R, KEY_Q, KEY_P, KEY_SIX, KEY_O, KEY_N, KEY_M, KEY_4, KEY_EE, KEY_SQUARED,
			  KEY_L, KEY_K, KEY_J, KEY_NINE, KEY_I, KEY_H, KEY_G, KEY_SEVEN, KEY_SLASH, KEY_E_POW,
			  KEY_F, KEY_E, KEY_D, KEY_C, KEY_B, KEY_A, KEY_EQUALS, KEY_STAR, KEY_CIRCUMFLEX,
			  KEY_VAR, KEY_MINUS, KEY_CLOSING_BRACKET, KEY_DOT, KEY_OPENING_BRACKET, KEY_FIVE, KEY_CAT, KEY_FRAC, KEY_DEL, KEY_SCRATCH,
			  KEY_FLAG, KEY_PLUS, KEY_DOC, KEY_TWO, KEY_MENU, KEY_EIGHT, KEY_ESCAPE, KEY_TAB,
			  KEY_SHIFT, KEY_CTRL, KEY_COMMA};
//
enum touchpad_arrow {ARROW_UP = 0, ARROW_RIGHT, ARROW_DOWN, ARROW_LEFT};
struct touchpad_report {
	bool contact;
	uint8_t proximity;
	uint16_t x;
	uint16_t y;
	uint8_t xrel;
	uint8_t yrel;
	bool pressed;
	//uint8_t status;
};



#define SMALL_PAGE_SIZE (1024*4)
#define LARGE_PAGE_SIZE (1024*64)
#define SECTION_SIZE (1024*1024)


#define virtual_base_address ((void* const) 0xe0000000)
#define remapped_RAM ((const void*) 0xec000000)
#define old_RAM ((const void*) 0x10000000)




#define SEARCH_AND_TEST(function) \
									function = search(#function); \
									if (function == NULL) return NULL;
//


void module_end();
// should return the address of module_end, or NULL if an error occurred. If it returns NULL, the module is instantly uninstalled
void* module_start(void* search_func, uint32_t osext_version, bool standalone);


#ifndef OSEXT_VERSION
	#error Modules have to define the macro OSEXT_VERSION with the version they were compiled for
#endif

#endif