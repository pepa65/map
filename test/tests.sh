#!/usr/bin/env bash

assert_equal(){
	[[ $1 = $2 ]] || printf "F: \"%s\" != \"%s\"\n" $1 $2
}

! type -p map >/dev/null && echo "F: map is not installed yet" && exit 1
! type -p mapf >/dev/null && echo "F: map is not installed yet" && exit 1
assert_equal "foofoo" $(printf foo |map -m f 'printf $f$f')
assert_equal "foofoo" $(printf foo |map 'printf $m$m')
assert_equal "foobarbaz" $(cat ./test/input | map -m f 'printf "$f"')
assert_equal "foobarbaz" $(cat ./test/input | map 'printf "$m"')
