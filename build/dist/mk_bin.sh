#!/bin/sh

. ./mk_init.sh


cd $TMP

rm -rf ${VER}-bin
mkdir ${VER}-bin
cd ${VER}-bin
mkdir ${DEST}

cp -R ${DIR}/data ${DEST}

cp ${DIR}/LICENSE ${DEST}
cp ${DIR}/README ${DEST}

find ${DEST} -name CVS -or -name .foo -or -name .fool_cvs | xargs rm -rf

if [ "$VER" = "win32" ]; then
    cp ${DIR}/build/win32-support/lib/OpenAL32.dll ${DEST}
fi

if [ "$VER" = "linux" ]; then
    (cd ../spmake; ./Reaper${EXT} ${TG} all -c topdir=../${VER}-bin/${DEST} srcdir=${DIR}/src dstdir=${DIR}/../spmake/build)
    (cd ../spmake; ./Reaper${EXT} ${TG} all -ov topdir=../${VER}-bin/${DEST} srcdir=${DIR}/src dstdir=${DIR}/../spmake/build $MKFLAGS1 "$MKFLAGS2")
    cp ../spmake/bin/prototype${AEXT} ${DEST}/reaper${AEXT}
    #rm -f ${DEST}/plugins/${VER}/*.lib
    #rm -f ${DEST}/plugins/${VER}/*.exp
fi


echo "Binary: ${VER} (${DEST})"
read


#rm -r ${DEST}/build

for d in config scenario levels objectdata objects shaders terrains
do
    find ${DEST}/data/$d -type f -print0 | xargs -0 conv.sh
done
find ${DEST} \( -name 'INST*' -or -name 'readm*' -or -name 'READM*' -or -name 'LICEN*' \) -print0 | xargs -0 conv.sh

if [ "$VER" = "win32" ]; then
    rm -f ../${DEST}-${VER}.zip
    zip -q -r ../${DEST}-${VER}.zip ${DEST}
else
    tar zcf ../${DEST}-${VER}.tar.gz ${DEST}
fi

rm -r ${DEST}
cd ..
rmdir ${VER}-bin

