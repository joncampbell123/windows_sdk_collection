#!/bin/bash

modprobe loop >/dev/null 2>&1

for img in *.ISO *.iso; do
	if [ -f "$img" ]; then
		mkdir -p "$img.c" || exit 1

		umount s >/dev/null 2>&1
		mkdir -p s || exit 1
		mount -o ro,loop "$img" s || exit 1
		cp -Rfv s/* "$img.c/" || exit 1
		umount s
		rmdir s
	fi
done

