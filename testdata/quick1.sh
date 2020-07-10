#! /bin/bash

FILE=$1

shift

rm graph-$FILE.out graph-$FILE.pdf > /dev/null

./mk_drift $FILE

./cnfalgo test cd $FILE.new $* > probs.out

gle -d pdf graph.gle

mv probs.out graph-$FILE.out
mv graph.pdf graph-$FILE.pdf

open graph-$FILE.pdf



