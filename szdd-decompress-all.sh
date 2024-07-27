#!/bin/bash

szdd() {
	if [ -f "$1" ]; then
		x=`dd if="$1" bs=4 count=1 2>/dev/null | sed -e 's/[\x00-\x1F]/_/g'`
		if [ "$x" == "SZDD" ]; then return 0; fi
	fi

	return 1
}

expand() {
	if [ -f "$1" ]; then
		msexpand <"$1" >"$1.expand.tmp" || return 1
		mv -v "$1.expand.tmp" "$1" || exit 1
	fi
	return 0
}

find -type f | while read X; do
	if [ -f "$X" ]; then
		if szdd "$X"; then
			echo "Expanding $X"
			expand "$X" || exit 1
		fi
	fi
done

