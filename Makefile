override CFLAGS += -std=c99 -Wall -Wextra

override_copy_file_range.so: override_copy_file_range.c
	gcc $(CFLAGS) -shared -fPIC -o $@ $<
	
copy_test: copy_test.c
	gcc $(CFLAGS) -o $@ $<

all: override_copy_file_range.so copy_test

clean:
	rm -f *.so
	rm -f copy_test

.PHONY: all clean
