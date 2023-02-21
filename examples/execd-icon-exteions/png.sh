#!/bin/sh
# example of a "extension" for showing contents of a png file in its desktop icon
# this is just a PoC and the idea can be expanded further  
# install the `farbfeld` package for png2ff
cat $1 | png2ff
