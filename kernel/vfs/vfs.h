#ifndef VFS_H
#define VFS_H

struct mountpoint;
struct mountpoint {
	char* path; // path in the file system where it's mounted
	uint8_t provider;
	void* data; // data specific to the provider
	struct mountpoint *next; // mountpoints are ordered in a linked list
};


bool vfs_init();


// checks if path1 contains path2
bool vfs_path_contains(const char* path1,const char* path2,uint32_t max);

char* vfs_resolve(const char* path);




struct osext_file* vfs_open(const char* path);

void vfs_close(struct osext_file* file);











#endif