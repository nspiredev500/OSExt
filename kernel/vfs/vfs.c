#include "../kernel.h"


static struct mountpoint rootfs;
static struct mountpoint* mountpoints = NULL;



bool vfs_init()
{
	rootfs.path = kmalloc(2);
	if (rootfs.path == NULL)
	{
		return false;
	}
	rootfs.path[0] = '/';
	rootfs.path[1] = '\0';
	rootfs.provider = FILE_PROVIDER_NUC;
	rootfs.data = NULL;
	rootfs.next = NULL;
	return true;
}


// resolves symlinks in a path
// symlinks for the nucleus filesystem will be stored in a special file, to support symlinks even on the native filesystem
char* vfs_resolve(const char* path)
{
	char *resolved = kmalloc(k_strlen(path,100)+1);
	if (resolved == NULL)
	{
		return NULL;
	}
	k_memcpy(resolved,path,k_strlen(path,100)+1); // also copy the null byte
	
	
	
	
	
	return resolved; // for now no symlink support
}


// checks if path1 contains path2
bool vfs_path_contains(const char* path1,const char* path2,uint32_t max)
{
	for (uint32_t i = 0;i<max;i++)
	{
		if (path1[i] == '\0')
		{
			if (path2[i] == '\0')
			{
				return true;
			}
			else
			{
				return false; // other string too long
			}
		}
		if (path2[i] == '\0')
		{
			if (path1[i] == '/')
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		if (path1[i] != path2[i])
		{
			return false;
		}
	}
	return true;
}



struct osext_file* vfs_open(const char* path)
{
	char* resolved = vfs_resolve(path);
	
	struct mountpoint *mnt = mountpoints;
	char *relative = resolved; // assume the path is in the root filesystem if it isn't in any mountpoints. This also has the effect that you can access the device filesystem even if there is no root filesystem
	
	while (mnt != NULL)
	{
		
		mnt = mnt->next;
	}
	
	
	
	
	
	
	/// TODO remove if the actual string is stored for nuc filesystem compatibility
	kfree(resolved);
	return NULL;
}

void vfs_close(struct osext_file* file)
{
	
	
	
	
	
	
	
	
	
}














