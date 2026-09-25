#define _GNU_SOURCE

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef SYS_copy_file_range
#error "SYS_copy_file_range is not available on this platform"
#endif

#define COPY_BUF_SIZE (64 * 1024)

// copy_file_range fallback implementation
static ssize_t copy_fallback(
    int fd_in,
    off64_t *off_in,
    int fd_out,
    off64_t *off_out,
    size_t len)
{
    char buf[64 * 1024];
    size_t total = 0;

    while (total < len) {
        size_t n = len - total;
        if (n > sizeof(buf))
            n = sizeof(buf);

        ssize_t r = off_in
            ? pread64(fd_in, buf, n, *off_in)
            : read(fd_in, buf, n);

        if (r <= 0)
            return total ? (ssize_t)total : r;

        ssize_t w = off_out
            ? pwrite64(fd_out, buf, r, *off_out)
            : write(fd_out, buf, r);

        if (w < 0)
            return total ? (ssize_t)total : -1;

        if (off_in)
            *off_in += w;
        if (off_out)
            *off_out += w;

        total += w;

        if (w < r)
            break;
    }

    return total;
}

// override the libc function
ssize_t copy_file_range(
    int fd_in,
    off64_t *off_in,
    int fd_out,
    off64_t *off_out,
    size_t len,
    unsigned int flags)
{
    ssize_t ret;

    /*
     * First try the real kernel syscall. Calling syscall() directly avoids
     * recursively calling this LD_PRELOAD override.
     */
    ret = syscall(
        SYS_copy_file_range,
        fd_in,
        off_in,
        fd_out,
        off_out,
        len,
        flags);

    if (ret >= 0)
        return ret;

    /*
     * If it failed with EXDEV, try the fallback.
     *
     * Don't silently emulate non-zero flags because the fallback doesn't
     * know their semantics.
     */
    if (errno != EXDEV || flags != 0)
        return -1;

    return copy_fallback(fd_in, off_in, fd_out, off_out, len);
}
