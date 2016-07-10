#!/bin/sh

. ./mk_init.sh

cd $TMP


rm -rf ${VER}-src
mkdir ${VER}-src
cd ${VER}-src
mkdir ${DEST}

cp -R ${DIR}/src ${DEST}
cp -R ${DIR}/build ${DEST}
mkdir -p ${DEST}/plugins/win32

cp ${DIR}/LICENSE ${DEST}
cp ${DIR}/README ${DEST}
cp ${DIR}/INSTALL ${DEST}
cp ${DIR}/ChangeLog ${DEST}

(cd ${DEST}/build/ac ; ./bootstrap.sh; touch stamp-h.in)

 
find ${DEST} \( -name '*.h' -or -name '*.c' -or -name '*.cpp' \
                -or -name 'read*' -or -name 'READ*' \
                -or -name 'INSTA*' -or -name 'LICENS*' \) -print0 \
        | xargs -0 conv.sh

if [ "$VER" = "linux" ]; then
    tar zcf ../${DEST}-src.tar.gz ${DEST}
else
    rm -f ../${DEST}-src.zip
    zip -q -r ../${DEST}-src.zip ${DEST}
fi

rm -r ${DEST}
cd ..
rmdir ${VER}-src

