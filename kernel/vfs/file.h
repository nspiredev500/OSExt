#ifndef FILE_H
#define FILE_H




#define FILE_PROVIDER_NUC 0
#define FILE_PROVIDER_DEV 1
#define FILE_PROVIDER_USB 2


struct nuc_provider_data {
	char* path; // string to the absolute path, to be used for conversion of fstat into nuc_stat
	NUC_FILE* file; // the actual file pointer
};

struct dev_provider_data {
	char* name; //path of the device file within the /dev directory, to forward requests to the right drivers
};

struct usb_provider_data {
	uint8_t device_address; // usb bus address of the mass storage device
	uint8_t partition; // the partition number of the partition with the file system
	char* filesystem_type; // name of the file system
	/// TODO inode number or something like that, when I actually know something about file systems
};

struct osext_file {
	uint8_t mode; // how the file was opened
	uint8_t provider;
	uint16_t flags;
	void* extra; // extra data specific to the provider
	
};













#endif