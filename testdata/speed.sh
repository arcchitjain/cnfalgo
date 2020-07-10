./generate.py speed $* > speed
./cnfalgo test cd speed -k 2 -P 0.001 -1 50 -2 50 -M  > probs.out
gle -d pdf graph.gle
open graph.pdf
