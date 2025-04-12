#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int _close(int fd) { return -1; }
int _lseek(int fd, int offset, int whence) { return -1; }
int _read(int fd, char *ptr, int len) { return -1; }
int _write(int fd, char *ptr, int len) { return -1; }
#pragma GCC diagnostic pop