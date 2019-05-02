#!/bin/bash

etagsCmd="/home/superran/bin/bin/etags -e --extra=+fq --file-scope=no --c-kinds=+p -o .tags/etags -L -"

find -H "/home/superran/myGitHub/EI-FOAM/of-6/OpenFOAM-6-master" \
     \( -name "*.[HC]" -o -name lnInclude -prune -o -name Doxygen -prune \) | \

    $etagsCmd

