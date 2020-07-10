#!/usr/bin/env python

def main() :

	from sys import argv, exit
	from getopt import gnu_getopt, GetoptError

	try:
		opts, args = gnu_getopt(argv[1:], "k:1:2:t:")
	except GetoptError, err:
		print str(err)
		exit(2)

	k = 5
	t1 = 50
	t2 = 50
	th = 1
	for a,v in opts :
		if a == '-k' :
			k = v
		elif a == '-1' :
			t1 = v
		elif a == '-2' :
			t2 = v
		elif a == '-t' :
			th = int(v)
	
	cnf = open_pipe(k,t1+t2)
	  
	from sys import stdin
	
	i = 0
	
	min = 1.0
	minpos = -1
	for l in stdin :
		cnf.stdin.write(l)
		line = cnf.stdout.readline().strip().split(' ')
		k2 = len([x for x in line[2:t2+2] if int(x) >= th ])
		k1 = len([x for x in line[2+t2:] if int(x) >= th ])
		p = probability(k1,t1,k2,t2)
		print line[0], k1, k2, p
		if p < min and p > 0 :
			min = p
			minpos = line[0]
		#print line[0], [x for x in line[2:t1+2] if int(x) >= th ], [x for x in line[2+t1:] if int(x) >= th ]

	print min, minpos
	cnf.stdin.close()
	
def open_pipe(k,t) :	
	from subprocess import Popen, PIPE, STDOUT
	cnf = Popen(['./cnfalgo','online','_','-t', str(t), '-k', str(k)],stdin=PIPE,stdout=PIPE)
	
#	import fcntl, os
#	fd = cnf.stdout.fileno()
#	fl = fcntl.fcntl(fd,fcntl.F_GETFL)
#	fcntl.fcntl(fd,fcntl.F_SETFL, fl | os.O_NONBLOCK)

#	fd = cnf.stdin.fileno()
#	fl = fcntl.fcntl(fd,fcntl.F_GETFL)
#	fcntl.fcntl(fd,fcntl.F_SETFL, fl | os.O_NONBLOCK)
	return cnf

def probability(k1,N1,k2,N2) :
	from scipy.special import betaln
	from math import exp
	s = 0
	l1 = N1-k1
	for i in range(k2,N2) :
		l2 = N2-i
		s += exp(logchoose(N2,i)+betaln(1+k1+i,1+l1+l2)-betaln(1+k1,1+l1))
	return s
	

def logchoose(n, k):
	from scipy.special import gammaln

	lgn1 = gammaln(n+1)
	lgk1 = gammaln(k+1)
	lgnk1 = gammaln(n-k+1)
	return lgn1 - (lgnk1 + lgk1)

if __name__ == "__main__" :
	main()
