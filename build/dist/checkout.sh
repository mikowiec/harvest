#!/bin/sh

TAG=${TAG:-HEAD}

. ./mk_init.sh

cd ${TMP}

cvs -z3 -d :pserver:anonymous@cvs.reaper3d.sf.net:/cvsroot/reaper3d export -r $TAG reaper
mv reaper reaper-$VERSION-clean

cvs -d :ext:peter@zarquon:/usr/share/cvsroot export -r HEAD spmake
(cd spmake; ghc --make Reaper -o Reaper -O2)

