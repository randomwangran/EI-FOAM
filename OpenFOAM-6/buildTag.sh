#!/bin/bash

## tweak: don't forget to manually creat a .tags file
##        within the same folder (bin, doc, etc...)
etagsCmd="/usr/local/bin/ctags -e --extra=+fq --file-scope=no --c-kinds=+p -o .tags/etags -L -"

find -H "/home/pi/OpenFOAM/OpenFOAM-6" \
     \( -name "*.[HC]" -o -name lnInclude -prune -o -name Doxygen -prune \) | \

    $etagsCmd
