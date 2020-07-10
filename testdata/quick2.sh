#! /bin/bash

FILE=$1

shift

rm graph-$FILE.out graph-$FILE.pdf > /dev/null

./mk_drift.py mkdrift $FILE 

./cnfalgo test cd $FILE.new $* > probs.out

echo " and now with threshold selection"
./cnfalgo test cd $FILE.new $* -M > /dev/null

gle -d pdf graph.gle

mv probs.out graph-$FILE.out
mv graph.pdf graph-$FILE.pdf

open graph-$FILE.pdf



