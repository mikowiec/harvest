#!/bin/sh

case `automake --version` in
 *\ 1.[0-4]*)    echo "automake version 1.5+ required!"
       exit 1;;
 *) ;;
esac
 
./gen_source_defs.pl

aclocal && \
autoheader && \
libtoolize --automake && \
automake --add-missing --copy --foreign && \
autoconf
