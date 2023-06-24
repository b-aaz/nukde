#!/bin/sh
# An example of a "extension" for showing contents of a pnm file in its desktop icon .
# This is just a PoC and the idea can be expanded further .
# You need to install the `farbfeld` package for `png2ff` .
# You need to install the `netpbm` package for `pnmtopng` .
pnmtopng $1 | png2ff
