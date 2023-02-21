#!/bin/sh
#for showing pnm images with the help of netpbm and farbfeld utils
pnmtopng $1 | png2ff
