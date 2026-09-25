#!/bin/bash

TARGET=/tmp/copy_file_range_override_test
trap 'rm -f "$TARGET"' EXIT

if ./copy_test README.md "$TARGET"; then
	echo "Copy without override did not fail, ???"
	exit 1
else
	status=$?
	if [ "$status" -eq 2 ]; then
		echo "Copy without override failed, OK"
	else
		echo "Copy without override failed with unexpected status $status"
		exit 1
	fi
fi

if LD_PRELOAD=./override_copy_file_range.so ./copy_test README.md "$TARGET"; then
	echo "Copy with override worked, OK"
else
	echo "Copy with override failed, FAIL"
	exit 1
fi
