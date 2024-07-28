#!/bin/bash

szdd() {
	if [ -f "$1" ]; then
		x=`dd if="$1" bs=4 count=1 2>/dev/null | sed -e 's/[\x00-\x1F]/_/g'`
		if [ "$x" == "SZDD" ]; then return 0; fi
	fi

	return 1
}

szdd_missingchar() {
	if [ -f "$1" ]; then
		# the missing char is USUALLY stored at byte offset 9 of SZDD
		dd if="$1" bs=1 count=1 skip=9 2>/dev/null | sed -e 's/[^a-zA-Z0-9]//g'
	fi

	return 0
}

expand() {
	if [ -f "$1" ]; then
		fname="$1"
		# if the file ends in _ or $ the missing char is at byte 9
		case "$fname" in
			*[_\$])
				# an empty missingchar is OK, it happens for filenames with extensions less than 3 chars.
				# sometimes the file has no extension i,e, "makefile" and the trailing dot must be removed.
				missingchar=`szdd_missingchar "$fname"`
				fname=`echo "$fname" | sed -e 's/[_\$]$/'"$missingchar/" | sed -e 's/\.$//'`
				if [ "$fname" != "$1" ]; then
					echo "Extracting as $fname"
				fi
				;;
			*)
				;;
		esac
		#
		msexpand <"$1" >"$fname.expand.tmp" || return 1
		rm -v -- "$1" || return 1
		mv -v -- "$fname.expand.tmp" "$fname" || return 1
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

