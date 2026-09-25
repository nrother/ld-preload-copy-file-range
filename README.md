# ld-preload-copy-file-range / override_copy_file_range.so

This library overrides calls to `copy_file_range()` and provides a fallback for copies between different filesystems.

> [!WARNING]
> This library is somewhat of a hack.
> **DO NOT use it for mission-critical software!**

The [`copy_file_range()` function in libc](https://man7.org/linux/man-pages/man2/copy_file_range.2.html) uses the syscall of the same name to perform fast file copies.
However, `copy_file_range()` only works for copies on the same (type of) filesystem, otherwise it fails with `EXDEV`.

This library overrides the libc implementation with its own version, which:

- first tries the `copy_file_range` syscall
- falls back to a regular `read()`/`write()` loop if the syscall fails with `EXDEV`

## Compiling and installation

Run `make` to build the library.

This creates a shared object named `override_copy_file_range.so`. Place it wherever convenient and provide its path via `LD_PRELOAD` when starting the target application.

For example:

```bash
# build
$ make all
gcc -std=c99 -Wall -Wextra -shared -fPIC -o override_copy_file_range.so override_copy_file_range.c
gcc -std=c99 -Wall -Wextra -o copy_test copy_test.c
# test
$ ./test.sh
copy_file_range: Invalid cross-device link
Copy without override failed, OK
copied 10251 bytes
Copy with override worked, OK
# run application
$ LD_PRELOAD=/path/to/override_copy_file_range.so ./problematic_application
```

## Tests

Run `make copy_test` to build a small test application that uses `copy_file_range()`.
Then run `./test.sh` to verify that the fallback works as intended.

## Acknowledgments

This library was inspired by [`ld-preload-open`, created by Fritz Webering](https://github.com/fritzw/ld-preload-open).

## License

MIT

