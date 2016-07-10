#!/bin/sh

. ./mk_init.sh

dist_path=`pwd`

cd $TMP

rm -rf ${VER}-data
mkdir ${VER}-data
cd ${VER}-data
mkdir ${DEST}

cp -R ${DIR}/data ${DEST}

cp ${DIR}/LICENSE ${DEST}
cp ${DIR}/README ${DEST}


rm -f ../${DEST}-data.zip
for lst in min_data tiny level ; do
    (cd ${DEST}; cat ${dist_path}/${lst}.list | zip -q -r -@ ../../${DEST}-${lst}.zip)
done

rm -r ${DEST}
cd ..
rmdir ${VER}-data

