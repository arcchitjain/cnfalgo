#! /bin/bash

FILE=$1

shift

rm graph-$FILE.out graph-$FILE.pdf > /dev/null

REAL=`./mk_drift.py mkdrift $FILE -S`
echo $REAL

./cnfalgo test cd $FILE.new $* -M > graph-$FILE.out

echo " and now with threshold selection"
DETECT=`tail -n 1 graph-$FILE.out` 
DETECT=${DETECT:2}
echo $DETECT

./mk_drift.py output graph-$FILE.out -r $REAL -d $DETECT > probs.out

gle -d pdf graph.gle

mv probs.out graph-$FILE.out
mv graph.pdf graph-$FILE.pdf

open graph-$FILE.pdf



