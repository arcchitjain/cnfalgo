#! /bin/bash

rm failing-$1.out graph-$1.out graph-$1.pdf failing-$1.pdf

./mk_drift $1

./cnfalgo test cd $1.new > probs.out

gle -d pdf graph.gle

tail -n 1 probs.out | awk '{ print $3 }'
LSIZE=`tail -n 1 probs.out | awk '{ print $3 }'`
echo "lsize = $LSIZE"

mv probs.out graph-$1.out
mv graph.pdf graph-$1.pdf

open graph-$1.pdf


./cnfalgo test learner $1.new -L $LSIZE 2> failing.out

gle -d pdf failing.gle

mv failing.out failing-$1.out
mv failing.pdf failing-$1.pdf

open failing-$1.pdf

