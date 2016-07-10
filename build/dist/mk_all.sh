#!/bin/sh


#./checkout.sh
ls -l /tmp
export SYS=Linux
./mk_src.sh
ls -l /tmp
./mk_bin.sh
ls -l /tmp
./mk_data.sh
ls -l /tmp

export SYS=cross
./mk_src.sh
ls -l /tmp
./mk_bin.sh
ls -l /tmp
./mk_data.sh
ls -l /tmp



