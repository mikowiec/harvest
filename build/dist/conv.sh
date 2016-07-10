#!/bin/sh

TMP=/tmp/conv.tmp.$$

for d in "$@"
do
    if [ "$VER" = "linux" ]; then
        conv='s/\r//'
    else
        conv='s/\r//;s/$/\r/'
    fi
    perl -p -e $conv < "$d" >$TMP
    cat $TMP >"$d"
    rm $TMP
done
