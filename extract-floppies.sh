#!/bin/bash

modprobe loop >/dev/null 2>&1

for img in *.IMG; do
	mkdir -p "$img.d" || exit 1

	umount s >/dev/null 2>&1
	mkdir -p s || exit 1
	mount -o ro,loop "$img" s || exit 1
	cp -Rfv s/* "$img.d/" || exit 1
	umount s
	rmdir s
done

