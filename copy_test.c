#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s SRC DST\n", argv[0]);
        return 1;
    }

    int in = open(argv[1], O_RDONLY);
    if (in < 0) {
        perror("open src");
        return 1;
    }

    int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        perror("open dst");
        return 1;
    }

    ssize_t n = copy_file_range(in, NULL, out, NULL, 1024 * 1024, 0);
    if (n < 0) {
        perror("copy_file_range");
        return 2;
    }

    printf("copied %zd bytes\n", n);

    close(in);
    close(out);
    return 0;
}

