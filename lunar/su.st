31127 execve("/bin/su", ["su", "-", "codybot"], 0xbfaa54b4 /* 32 vars */) = 0
31127 brk(NULL)                         = 0x9ee8000
31127 access("/etc/ld.so.preload", R_OK) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/lib/tls/i686/libpam.so.0", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 stat64("/lib/tls/i686", 0xbffcb8f0) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/lib/tls/libpam.so.0", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 stat64("/lib/tls", 0xbffcb8f0)    = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/lib/i686/libpam.so.0", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 stat64("/lib/i686", 0xbffcb8f0)   = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/lib/libpam.so.0", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\220$\0\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=71916, ...}) = 0
31127 mmap2(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0xb7f5f000
31127 mmap2(NULL, 65832, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7f4e000
31127 mmap2(0xb7f50000, 32768, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0xb7f50000
31127 mmap2(0xb7f58000, 20480, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xa000) = 0xb7f58000
31127 mmap2(0xb7f5d000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xe000) = 0xb7f5d000
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/lib/libpam_misc.so.0", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\300\21\0\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=16924, ...}) = 0
31127 mmap2(NULL, 16532, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7f49000
31127 mmap2(0xb7f4a000, 4096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1000) = 0xb7f4a000
31127 mmap2(0xb7f4b000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0xb7f4b000
31127 mmap2(0xb7f4c000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0xb7f4c000
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/lib/libutil.so.1", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\20\22\0\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=16336, ...}) = 0
31127 mmap2(NULL, 16392, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7f44000
31127 mmap2(0xb7f45000, 4096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1000) = 0xb7f45000
31127 mmap2(0xb7f46000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0xb7f46000
31127 mmap2(0xb7f47000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0xb7f47000
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/lib/libc.so.6", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\3\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\240\350\1\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=1901020, ...}) = 0
31127 mmap2(NULL, 1703820, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7da4000
31127 mprotect(0xb7dc1000, 1560576, PROT_NONE) = 0
31127 mmap2(0xb7dc1000, 1146880, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1d000) = 0xb7dc1000
31127 mmap2(0xb7ed9000, 409600, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x135000) = 0xb7ed9000
31127 mmap2(0xb7f3e000, 16384, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x199000) = 0xb7f3e000
31127 mmap2(0xb7f42000, 8076, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0xb7f42000
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/lib/libdl.so.2", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0000\21\0\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=21344, ...}) = 0
31127 mmap2(NULL, 20532, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7d9e000
31127 mmap2(0xb7d9f000, 8192, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1000) = 0xb7d9f000
31127 mmap2(0xb7da1000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x3000) = 0xb7da1000
31127 mmap2(0xb7da2000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x3000) = 0xb7da2000
31127 close(3)                          = 0
31127 mmap2(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0xb7d9c000
31127 set_thread_area({entry_number=-1, base_addr=0xb7d9c700, limit=0x0fffff, seg_32bit=1, contents=0, read_exec_only=0, limit_in_pages=1, seg_not_present=0, useable=1}) = 0 (entry_number=6)
31127 mprotect(0xb7f3e000, 8192, PROT_READ) = 0
31127 mprotect(0xb7da2000, 4096, PROT_READ) = 0
31127 mprotect(0xb7f47000, 4096, PROT_READ) = 0
31127 mprotect(0xb7f5d000, 4096, PROT_READ) = 0
31127 mprotect(0xb7f4c000, 4096, PROT_READ) = 0
31127 mprotect(0x8058000, 4096, PROT_READ) = 0
31127 mprotect(0xb7f8d000, 4096, PROT_READ) = 0
31127 brk(NULL)                         = 0x9ee8000
31127 brk(0x9f09000)                    = 0x9f09000
31127 brk(0x9f0a000)                    = 0x9f0a000
31127 openat(AT_FDCWD, "/usr/lib/locale/locale-archive", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/share/locale/locale.alias", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/locale/en_CA.UTF-8/LC_IDENTIFICATION", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/locale/en_CA.utf8/LC_IDENTIFICATION", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/locale/en_CA/LC_IDENTIFICATION", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/locale/en.UTF-8/LC_IDENTIFICATION", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/locale/en.utf8/LC_IDENTIFICATION", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/locale/en/LC_IDENTIFICATION", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 getuid32()                        = 0
31127 geteuid32()                       = 0
31127 ioctl(0, TCGETS, {B38400 opost isig icanon echo ...}) = 0
31127 ioctl(0, TCGETS, {B38400 opost isig icanon echo ...}) = 0
31127 ioctl(0, TCGETS, {B38400 opost isig icanon echo ...}) = 0
31127 fstat64(0, {st_mode=S_IFCHR|0600, st_rdev=makedev(0x4, 0x3), ...}) = 0
31127 readlink("/proc/self/fd/0", "/dev/tty3", 4095) = 9
31127 stat64("/dev/tty3", {st_mode=S_IFCHR|0600, st_rdev=makedev(0x4, 0x3), ...}) = 0
31127 socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
31127 connect(3, {sa_family=AF_UNIX, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
31127 close(3)                          = 0
31127 socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
31127 connect(3, {sa_family=AF_UNIX, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/etc/nsswitch.conf", O_RDONLY|O_CLOEXEC) = 3
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=1622, ...}) = 0
31127 read(3, "#\n# /etc/nsswitch.conf\n#\n# An ex"..., 4096) = 1622
31127 read(3, "", 4096)                 = 0
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/lib/libnss_files.so.2", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\0#\0\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=57928, ...}) = 0
31127 mmap2(NULL, 78532, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7d88000
31127 mmap2(0xb7d8a000, 28672, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0xb7d8a000
31127 mmap2(0xb7d91000, 12288, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x9000) = 0xb7d91000
31127 mmap2(0xb7d94000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xb000) = 0xb7d94000
31127 mmap2(0xb7d96000, 21188, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0xb7d96000
31127 close(3)                          = 0
31127 mprotect(0xb7d94000, 4096, PROT_READ) = 0
31127 openat(AT_FDCWD, "/etc/passwd", O_RDONLY|O_CLOEXEC) = 3
31127 _llseek(3, 0, [0], SEEK_CUR)      = 0
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=1268, ...}) = 0
31127 read(3, "root:x:0:0:root:/root:/bin/bash\n"..., 4096) = 1268
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/proc/self/loginuid", O_RDONLY) = -1 ENOENT (No such file or directory)
31127 ioctl(0, TCGETS, {B38400 opost isig icanon echo ...}) = 0
31127 fstat64(0, {st_mode=S_IFCHR|0600, st_rdev=makedev(0x4, 0x3), ...}) = 0
31127 readlink("/proc/self/fd/0", "/dev/tty3", 511) = 9
31127 stat64("/dev/tty3", {st_mode=S_IFCHR|0600, st_rdev=makedev(0x4, 0x3), ...}) = 0
31127 access("/var/run/utmpx", F_OK)    = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/var/run/utmp", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 _llseek(3, 0, [0], SEEK_SET)      = 0
31127 alarm(0)                          = 0
31127 rt_sigaction(SIGALRM, {sa_handler=0xb7ec4680, sa_mask=[], sa_flags=0}, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
31127 alarm(10)                         = 0
31127 fcntl64(3, F_SETLKW, {l_type=F_RDLCK, l_whence=SEEK_SET, l_start=0, l_len=0}) = 0
31127 read(3, "", 384)                  = 0
31127 fcntl64(3, F_SETLKW, {l_type=F_UNLCK, l_whence=SEEK_SET, l_start=0, l_len=0}) = 0
31127 alarm(0)                          = 10
31127 rt_sigaction(SIGALRM, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, NULL, 8) = 0
31127 close(3)                          = 0
31127 getuid32()                        = 0
31127 openat(AT_FDCWD, "/etc/passwd", O_RDONLY|O_CLOEXEC) = 3
31127 _llseek(3, 0, [0], SEEK_CUR)      = 0
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=1268, ...}) = 0
31127 read(3, "root:x:0:0:root:/root:/bin/bash\n"..., 4096) = 1268
31127 close(3)                          = 0
31127 stat64("/etc/pam.d", {st_mode=S_IFDIR|0755, st_size=4096, ...}) = 0
31127 openat(AT_FDCWD, "/etc/pam.d/su-l", O_RDONLY|O_LARGEFILE) = 3
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=607, ...}) = 0
31127 read(3, "#%PAM-1.0\nauth      sufficient  "..., 4096) = 607
31127 openat(AT_FDCWD, "/lib/security/pam_rootok.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 4
31127 read(4, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0`\20\0\0004\0\0\0"..., 512) = 512
31127 fstat64(4, {st_mode=S_IFREG|0755, st_size=15260, ...}) = 0
31127 mmap2(NULL, 16412, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 4, 0) = 0xb7d83000
31127 mmap2(0xb7d84000, 4096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x1000) = 0xb7d84000
31127 mmap2(0xb7d85000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x2000) = 0xb7d85000
31127 mmap2(0xb7d86000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x2000) = 0xb7d86000
31127 close(4)                          = 0
31127 mprotect(0xb7d86000, 4096, PROT_READ) = 0
31127 openat(AT_FDCWD, "/etc/pam.d/system-auth", O_RDONLY|O_LARGEFILE) = 4
31127 fstat64(4, {st_mode=S_IFREG|0644, st_size=653, ...}) = 0
31127 read(4, "#%PAM-1.0\nauth        required  "..., 4096) = 653
31127 openat(AT_FDCWD, "/lib/security/pam_env.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 5
31127 read(5, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\260\21\0\0004\0\0\0"..., 512) = 512
31127 fstat64(5, {st_mode=S_IFREG|0755, st_size=20512, ...}) = 0
31127 mmap2(NULL, 20592, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 5, 0) = 0xb7d7d000
31127 mmap2(0xb7d7e000, 8192, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x1000) = 0xb7d7e000
31127 mmap2(0xb7d80000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x3000) = 0xb7d80000
31127 mmap2(0xb7d81000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x3000) = 0xb7d81000
31127 close(5)                          = 0
31127 mprotect(0xb7d81000, 4096, PROT_READ) = 0
31127 openat(AT_FDCWD, "/lib/security/pam_unix.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 5
31127 read(5, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0p%\0\0004\0\0\0"..., 512) = 512
31127 fstat64(5, {st_mode=S_IFREG|0755, st_size=61560, ...}) = 0
31127 mmap2(NULL, 106912, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 5, 0) = 0xb7d62000
31127 mmap2(0xb7d64000, 28672, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x2000) = 0xb7d64000
31127 mmap2(0xb7d6b000, 16384, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x9000) = 0xb7d6b000
31127 mmap2(0xb7d6f000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0xc000) = 0xb7d6f000
31127 mmap2(0xb7d71000, 45472, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0xb7d71000
31127 close(5)                          = 0
31127 openat(AT_FDCWD, "/lib/libcrypt.so.1", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/tls/i686/libcrypt.so.1", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 stat64("/usr/lib/tls/i686", 0xbffca620) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/tls/libcrypt.so.1", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 stat64("/usr/lib/tls", 0xbffca620) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/i686/libcrypt.so.1", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 stat64("/usr/lib/i686", 0xbffca620) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/usr/lib/libcrypt.so.1", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 stat64("/usr/lib", 0xbffca620)    = -1 ENOENT (No such file or directory)
31127 munmap(0xb7d62000, 106912)        = 0
31127 time([1567605567 /* 2019-09-04T13:59:27+0000 */]) = 1567605567 (2019-09-04T13:59:27+0000)
31127 openat(AT_FDCWD, "/usr/share/zoneinfo/America/Montreal", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 5
31127 connect(5, {sa_family=AF_UNIX, sun_path="/dev/log"}, 110) = -1 ENOENT (No such file or directory)
31127 close(5)                          = 0
31127 time([1567605567 /* 2019-09-04T13:59:27+0000 */]) = 1567605567 (2019-09-04T13:59:27+0000)
31127 socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 5
31127 connect(5, {sa_family=AF_UNIX, sun_path="/dev/log"}, 110) = -1 ENOENT (No such file or directory)
31127 close(5)                          = 0
31127 openat(AT_FDCWD, "/lib/security/pam_deny.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 5
31127 read(5, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0@\20\0\0004\0\0\0"..., 512) = 512
31127 fstat64(5, {st_mode=S_IFREG|0755, st_size=15060, ...}) = 0
31127 mmap2(NULL, 16404, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 5, 0) = 0xb7d78000
31127 mmap2(0xb7d79000, 4096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x1000) = 0xb7d79000
31127 mmap2(0xb7d7a000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x2000) = 0xb7d7a000
31127 mmap2(0xb7d7b000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x2000) = 0xb7d7b000
31127 close(5)                          = 0
31127 mprotect(0xb7d7b000, 4096, PROT_READ) = 0
31127 read(4, "", 4096)                 = 0
31127 close(4)                          = 0
31127 openat(AT_FDCWD, "/lib/security/pam_succeed_if.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 4
31127 read(4, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\200\21\0\0004\0\0\0"..., 512) = 512
31127 fstat64(4, {st_mode=S_IFREG|0755, st_size=20364, ...}) = 0
31127 mmap2(NULL, 20580, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 4, 0) = 0xb7d72000
31127 mmap2(0xb7d73000, 8192, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x1000) = 0xb7d73000
31127 mmap2(0xb7d75000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x3000) = 0xb7d75000
31127 mmap2(0xb7d76000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x3000) = 0xb7d76000
31127 close(4)                          = 0
31127 mprotect(0xb7d76000, 4096, PROT_READ) = 0
31127 openat(AT_FDCWD, "/etc/pam.d/system-auth", O_RDONLY|O_LARGEFILE) = 4
31127 fstat64(4, {st_mode=S_IFREG|0644, st_size=653, ...}) = 0
31127 read(4, "#%PAM-1.0\nauth        required  "..., 4096) = 653
31127 read(4, "", 4096)                 = 0
31127 close(4)                          = 0
31127 openat(AT_FDCWD, "/etc/pam.d/system-auth", O_RDONLY|O_LARGEFILE) = 4
31127 fstat64(4, {st_mode=S_IFREG|0644, st_size=653, ...}) = 0
31127 read(4, "#%PAM-1.0\nauth        required  "..., 4096) = 653
31127 openat(AT_FDCWD, "/lib/security/pam_cracklib.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 5
31127 read(5, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\320\21\0\0004\0\0\0"..., 512) = 512
31127 fstat64(5, {st_mode=S_IFREG|0755, st_size=20352, ...}) = 0
31127 mmap2(NULL, 20600, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 5, 0) = 0xb7d6c000
31127 mmap2(0xb7d6d000, 8192, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x1000) = 0xb7d6d000
31127 mmap2(0xb7d6f000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x3000) = 0xb7d6f000
31127 mmap2(0xb7d70000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 5, 0x3000) = 0xb7d70000
31127 close(5)                          = 0
31127 openat(AT_FDCWD, "/lib/libcrack.so.2", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 munmap(0xb7d6c000, 20600)         = 0
31127 time([1567605567 /* 2019-09-04T13:59:27+0000 */]) = 1567605567 (2019-09-04T13:59:27+0000)
31127 socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 5
31127 connect(5, {sa_family=AF_UNIX, sun_path="/dev/log"}, 110) = -1 ENOENT (No such file or directory)
31127 close(5)                          = 0
31127 time([1567605567 /* 2019-09-04T13:59:27+0000 */]) = 1567605567 (2019-09-04T13:59:27+0000)
31127 socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 5
31127 connect(5, {sa_family=AF_UNIX, sun_path="/dev/log"}, 110) = -1 ENOENT (No such file or directory)
31127 close(5)                          = 0
31127 read(4, "", 4096)                 = 0
31127 close(4)                          = 0
31127 openat(AT_FDCWD, "/lib/security/pam_keyinit.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 4
31127 read(4, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\0\21\0\0004\0\0\0"..., 512) = 512
31127 fstat64(4, {st_mode=S_IFREG|0755, st_size=15796, ...}) = 0
31127 mmap2(NULL, 16476, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 4, 0) = 0xb7d6d000
31127 mmap2(0xb7d6e000, 4096, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x1000) = 0xb7d6e000
31127 mmap2(0xb7d6f000, 4096, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x2000) = 0xb7d6f000
31127 mmap2(0xb7d70000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x2000) = 0xb7d70000
31127 close(4)                          = 0
31127 mprotect(0xb7d70000, 4096, PROT_READ) = 0
31127 openat(AT_FDCWD, "/lib/security/pam_limits.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 4
31127 read(4, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0p\22\0\0004\0\0\0"..., 512) = 512
31127 fstat64(4, {st_mode=S_IFREG|0755, st_size=29584, ...}) = 0
31127 mmap2(NULL, 28832, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 4, 0) = 0xb7d65000
31127 mmap2(0xb7d66000, 12288, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x1000) = 0xb7d66000
31127 mmap2(0xb7d69000, 8192, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x4000) = 0xb7d69000
31127 mmap2(0xb7d6b000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x5000) = 0xb7d6b000
31127 close(4)                          = 0
31127 mprotect(0xb7d6b000, 4096, PROT_READ) = 0
31127 openat(AT_FDCWD, "/lib/security/pam_xauth.so", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 4
31127 read(4, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\240\23\0\0004\0\0\0"..., 512) = 512
31127 fstat64(4, {st_mode=S_IFREG|0755, st_size=25640, ...}) = 0
31127 mmap2(NULL, 24812, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 4, 0) = 0xb7d5e000
31127 mmap2(0xb7d5f000, 8192, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x1000) = 0xb7d5f000
31127 mmap2(0xb7d61000, 8192, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x3000) = 0xb7d61000
31127 mmap2(0xb7d63000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 4, 0x4000) = 0xb7d63000
31127 close(4)                          = 0
31127 mprotect(0xb7d63000, 4096, PROT_READ) = 0
31127 read(3, "", 4096)                 = 0
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/etc/pam.d/other", O_RDONLY|O_LARGEFILE) = 3
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=146, ...}) = 0
31127 read(3, "#%PAM-1.0\nauth       required   "..., 4096) = 146
31127 read(3, "", 4096)                 = 0
31127 close(3)                          = 0
31127 getuid32()                        = 0
31127 getuid32()                        = 0
31127 openat(AT_FDCWD, "/etc/passwd", O_RDONLY|O_CLOEXEC) = 3
31127 _llseek(3, 0, [0], SEEK_CUR)      = 0
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=1268, ...}) = 0
31127 read(3, "root:x:0:0:root:/root:/bin/bash\n"..., 4096) = 1268
31127 close(3)                          = 0
31127 time([1567605567 /* 2019-09-04T13:59:27+0000 */]) = 1567605567 (2019-09-04T13:59:27+0000)
31127 socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 3
31127 connect(3, {sa_family=AF_UNIX, sun_path="/dev/log"}, 110) = -1 ENOENT (No such file or directory)
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/proc/sys/kernel/ngroups_max", O_RDONLY) = 3
31127 read(3, "65536\n", 31)            = 6
31127 close(3)                          = 0
31127 socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
31127 connect(3, {sa_family=AF_UNIX, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
31127 close(3)                          = 0
31127 socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0) = 3
31127 connect(3, {sa_family=AF_UNIX, sun_path="/var/run/nscd/socket"}, 110) = -1 ENOENT (No such file or directory)
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/etc/group", O_RDONLY|O_CLOEXEC) = 3
31127 _llseek(3, 0, [0], SEEK_CUR)      = 0
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=647, ...}) = 0
31127 read(3, "root:x:0:\ndaemon:x:1:\nbin:x:2:\ns"..., 4096) = 647
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 _llseek(3, 0, [647], SEEK_CUR)    = 0
31127 read(3, "", 4096)                 = 0
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/lib/libnss_nisplus.so.2", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\300\"\0\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=66592, ...}) = 0
31127 mmap2(NULL, 62040, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7d4e000
31127 mmap2(0xb7d50000, 36864, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0xb7d50000
31127 mmap2(0xb7d59000, 12288, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xb000) = 0xb7d59000
31127 mmap2(0xb7d5c000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xd000) = 0xb7d5c000
31127 close(3)                          = 0
31127 openat(AT_FDCWD, "/lib/libnsl.so.1", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\260F\0\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=116020, ...}) = 0
31127 mmap2(NULL, 112200, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7d32000
31127 mmap2(0xb7d36000, 53248, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x4000) = 0xb7d36000
31127 mmap2(0xb7d43000, 28672, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x11000) = 0xb7d43000
31127 mmap2(0xb7d4a000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x17000) = 0xb7d4a000
31127 mmap2(0xb7d4c000, 5704, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0xb7d4c000
31127 close(3)                          = 0
31127 mprotect(0xb7d4a000, 4096, PROT_READ) = 0
31127 mprotect(0xb7d5c000, 4096, PROT_READ) = 0
31127 uname({sysname="Linux", nodename="bsfc-vmlunar", ...}) = 0
31127 clock_gettime(CLOCK_REALTIME, {tv_sec=1567605567, tv_nsec=986813695}) = 0
31127 stat64("/var/nis/NIS_COLD_START", 0xbffcbbcc) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/var/nis/NIS_COLD_START", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
31127 openat(AT_FDCWD, "/lib/libnss_nis.so.2", O_RDONLY|O_LARGEFILE|O_CLOEXEC) = 3
31127 read(3, "\177ELF\1\1\1\0\0\0\0\0\0\0\0\0\3\0\3\0\1\0\0\0\340\"\0\0004\0\0\0"..., 512) = 512
31127 fstat64(3, {st_mode=S_IFREG|0755, st_size=57352, ...}) = 0
31127 mmap2(NULL, 53632, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0xb7d24000
31127 mmap2(0xb7d26000, 28672, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x2000) = 0xb7d26000
31127 mmap2(0xb7d2d000, 12288, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x9000) = 0xb7d2d000
31127 mmap2(0xb7d30000, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0xb000) = 0xb7d30000
31127 close(3)                          = 0
31127 mprotect(0xb7d30000, 4096, PROT_READ) = 0
31127 uname({sysname="Linux", nodename="bsfc-vmlunar", ...}) = 0
31127 setgroups32(1, [1001])            = 0
31127 openat(AT_FDCWD, "/etc/passwd", O_RDONLY|O_CLOEXEC) = 3
31127 _llseek(3, 0, [0], SEEK_CUR)      = 0
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=1268, ...}) = 0
31127 read(3, "root:x:0:0:root:/root:/bin/bash\n"..., 4096) = 1268
31127 close(3)                          = 0
31127 getuid32()                        = 0
31127 getgid32()                        = 0
31127 setregid32(1001, -1)              = 0
31127 setreuid32(1001, -1)              = 0
31127 keyctl(KEYCTL_GET_KEYRING_ID, KEY_SPEC_SESSION_KEYRING, 0) = 503412396
31127 keyctl(KEYCTL_GET_KEYRING_ID, KEY_SPEC_USER_SESSION_KEYRING, 0) = 257680108
31127 setreuid32(0, -1)                 = 0
31127 setregid32(0, -1)                 = 0
31127 openat(AT_FDCWD, "/etc/passwd", O_RDONLY|O_CLOEXEC) = 3
31127 _llseek(3, 0, [0], SEEK_CUR)      = 0
31127 fstat64(3, {st_mode=S_IFREG|0644, st_size=1268, ...}) = 0
31127 read(3, "root:x:0:0:root:/root:/bin/bash\n"..., 4096) = 1268
31127 close(3)                          = 0
31127 prlimit64(0, RLIMIT_CPU, NULL, {rlim_cur=RLIM64_INFINITY, rlim_max=RLIM64_INFINITY}) = 0
31127 prlimit64(0, RLIMIT_FSIZE, NULL, {rlim_cur=RLIM64_INFINITY, rlim_max=RLIM64_INFINITY}) = 0
31127 prlimit64(0, RLIMIT_DATA, NULL, {rlim_cur=RLIM64_INFINITY, rlim_max=RLIM64_INFINITY}) = 0
31127 prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
31127 prlimit64(0, RLIMIT_CORE, NULL, {rlim_cur=0, rlim_max=0}) = 0
31127 prlimit64(0, RLIMIT_RSS, NULL, {rlim_cur=RLIM64_INFINITY, rlim_max=RLIM64_INFINITY}) = 0
31127 prlimit64(0, RLIMIT_NPROC, NULL, {rlim_cur=1024, rlim_max=23533}) = 0
31127 prlimit64(0, RLIMIT_NOFILE, NULL, {rlim_cur=1024, rlim_max=512*1024}) = 0
31127 prlimit64(0, RLIMIT_MEMLOCK, NULL, {rlim_cur=64*1024, rlim_max=64*1024}) = 0
31127 prlimit64(0, RLIMIT_AS, NULL, {rlim_cur=RLIM64_INFINITY, rlim_max=RLIM64_INFINITY}) = 0
31127 prlimit64(0, RLIMIT_LOCKS, NULL, {rlim_cur=RLIM64_INFINITY, rlim_max=RLIM64_INFINITY}) = 0
31127 prlimit64(0, RLIMIT_SIGPENDING, NULL, {rlim_cur=23533, rlim_max=23533}) = 0
31127 prlimit64(0, RLIMIT_MSGQUEUE, NULL, {rlim_cur=800*1024, rlim_max=800*1024}) = 0
31127 prlimit64(0, RLIMIT_NICE, NULL, {rlim_cur=0, rlim_max=0}) = 0
31127 prlimit64(0, RLIMIT_RTPRIO, NULL, {rlim_cur=0, rlim_max=0}) = 0
31127 prlimit64(0, RLIMIT_RTTIME, NULL, {rlim_cur=RLIM64_INFINITY, rlim_max=RLIM64_INFINITY}) = 0
31127 getpriority(PRIO_PROCESS, 0)      = 20
31127 openat(AT_FDCWD, "/etc/security/limits.conf", O_RDONLY|O_LARGEFILE) = -1 ENOENT (No such file or directory)
31127 time([1567605567 /* 2019-09-04T13:59:27+0000 */]) = 1567605567 (2019-09-04T13:59:27+0000)
31127 socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 3
31127 connect(3, {sa_family=AF_UNIX, sun_path="/dev/log"}, 110) = -1 ENOENT (No such file or directory)
31127 close(3)                          = 0
31127 time([1567605567 /* 2019-09-04T13:59:27+0000 */]) = 1567605567 (2019-09-04T13:59:27+0000)
31127 socket(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0) = 3
31127 connect(3, {sa_family=AF_UNIX, sun_path="/dev/log"}, 110) = -1 ENOENT (No such file or directory)
31127 close(3)                          = 0
31127 access("/usr/X11R6/bin/xauth", X_OK) = -1 ENOENT (No such file or directory)
31127 access("/usr/bin/xauth", X_OK)    = -1 ENOENT (No such file or directory)
31127 access("/usr/bin/X11/xauth", X_OK) = -1 ENOENT (No such file or directory)
31127 munmap(0xb7d83000, 16412)         = 0
31127 munmap(0xb7d7d000, 20592)         = 0
31127 munmap(0xb7d78000, 16404)         = 0
31127 munmap(0xb7d72000, 20580)         = 0
31127 munmap(0xb7d6d000, 16476)         = 0
31127 munmap(0xb7d65000, 28832)         = 0
31127 munmap(0xb7d5e000, 24812)         = 0
31127 write(2, "su: ", 4)               = 4
31127 write(2, "cannot open session: Error in se"..., 44) = 44
31127 write(2, "\n", 1)                 = 1
31127 close(1)                          = 0
31127 close(2)                          = 0
31127 exit_group(1)                     = ?
31127 +++ exited with 1 +++
