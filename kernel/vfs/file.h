#ifndef FILE_H
#define FILE_H




#define FILE_PROVIDER_NUC 0
#define FILE_PROVIDER_DEV 1
#define FILE_PROVIDER_USB 2



struct osext_file {
	uint8_t mode; // how the file was opened
	uint8_t provider;
	uint16_t flags;
	void* extra; // extra data specific to the provider
	/*
		for FILE_PROVIDER_NUC it's a pathname string, used to convert fstat to nuc_stat
		for FILE_PROVIDER_DEV it's the name of the device file, so the devfs driver can pass the requests on to the right drivers
	*/
	
};

















#endif