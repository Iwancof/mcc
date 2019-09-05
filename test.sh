#!/bin/bash
try() {
	expected="$1"
	input="$2"

	./mcc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

try 1 1
try 100 100

echo OK
