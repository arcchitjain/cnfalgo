#! /bin/bash

rm failingr-$1.out graphr-$1.out graphr-$1.pdf failingr-$1.pdf

./mk_drift $1 r

./cnfalgo test cd $1.new > probs.out

gle -d pdf graph.gle

tail -n 1 probs.out | awk '{ print $3 }'
LSIZE=`tail -n 1 probs.out | awk '{ print $3 }'`
echo "lsize = $LSIZE"

mv probs.out graphr-$1.out
mv graph.pdf graphr-$1.pdf

open graph-$1.pdf


./cnfalgo test learner $1.new -L $LSIZE 2> failing.out

gle -d pdf failing.gle

mv failing.out failingr-$1.out
mv failing.pdf failingr-$1.pdf

open failingr-$1.pdf

