#!/bin/sh 
sed 's/,*//'  | awk -v "i=0"  'BEGIN {FS="=\|:";print "enum items {"} END{print "};"} NF {if (/IMG:/) { print toupper($2) , "=" , i ; i++ } else {if (/LBL:/) {} else {print toupper($0) , "=" , i , ","; i++} } }'

