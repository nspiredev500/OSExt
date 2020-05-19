#ifndef TI_OS
#define TI_OS


#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3

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



void ascii2utf16(char *string16,char *string, size_t size);
void* ti_malloc(uint32_t size); // not 8-byte-aligned !
void ti_free(void *ptr);
void* ti_calloc(uint32_t size); // not 8-byte-aligned !
int TCT_Local_Control_Interrupts(int mask);
//int32_t show_1_numeric_input(char* title,char* subtitle,char* msg,int32_t *val);


NUC_FILE* nuc_fopen(char *path,char *mode);
int32_t nuc_fclose(NUC_FILE* f);
uint32_t nuc_fread(void* buffer,uint32_t elements,uint32_t size,NUC_FILE* f);
uint32_t nuc_fwrite(void* buffer,uint32_t elements,uint32_t size,NUC_FILE* f);
int32_t nuc_fseek(NUC_FILE* f,int32_t offset, int32_t whence);
int32_t nuc_ftell(NUC_FILE* f);
int32_t nuc_fflush(NUC_FILE* f);



int32_t nuc_stat(const char *path, struct nuc_stat *stat);


int32_t nuc_rename(const char *old_name, const char *new_name);
int32_t nuc_unlink(const char *path);
int32_t nuc_mkdir(const char *path, int32_t mode);
int32_t nuc_rmdir(const char *path);

NUC_DIR* nuc_opendir(const char *path);
struct nuc_dirent* nuc_readdir(NUC_DIR *d);
int32_t nuc_closedir(NUC_DIR *d);












#endif