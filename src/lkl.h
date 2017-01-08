#define LKL_PATH_MAX 4096
#define LKL_MS_RDONLY 1

// extern struct lkl_host_operations lkl_host_ops;
//lkl_host_ops.print = NULL;
extern int lkl_host_ops;

int lkl_start_kernel(void *lkl_ops, unsigned long mem_size, const char *cmd_line, ...);

long lkl_sys_halt(void);

struct lkl_disk {
    void *dev;
    union {
        int fd; 
        void *handle;
    };  
};

int lkl_disk_add(struct lkl_disk *disk);

long lkl_mount_dev(unsigned int disk_id, unsigned int part, const char *fs_type,
           int flags, const char *opts,
           char *mnt_str, unsigned int mnt_str_len);

long lkl_sys_sync(void);

long lkl_umount_dev(unsigned int disk_id, unsigned int part, int flags, long timeout_ms);

long lkl_syscall(long no, long *params);
