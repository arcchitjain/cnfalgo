#! /usr/bin/env python

def run(file,correct,minN1=0,maxN1=50,maxN2=50) :
	from cnfalgo import CNFAlgo

	k = 2

	cnf = CNFAlgo(k)


	testset = []

	i = 0
	for line in file :
		example = [int(x) for x in line.strip().split(" ")]
		i += 1
		
		if i < correct - maxN1 :
			cnf.train(example)
		elif i < correct + maxN2 :
			testset.append(example)
		elif i == correct + maxN2 :
			for j in range(0,maxN1) :
				if maxN1-j >= minN1 :
					yield maxN1-j,cnf.test_set(testset[j:])
				else :
					return
				cnf.train(testset[j])
		else :
			return
			
		
def main() :

	import exp
	import sys

	filename = sys.argv[1]

	file = open(filename + '.correct')
	correct = [ int(x.strip().split(' ')[0]) for x in file ]
	file.close() 

	print correct


	k = 2 
	n1 = 50
	funcs = [exp.manu, exp.prob_freq, exp.prob_min]

	for n1 in [50,45,40,35,30,25,20,15,10,5] :
		for n2 in [50,45,40,35,30,25,20,15,10,5,3,1] :
			file = open(filename + '.data')
			pvalues = ([y for x,y in exp.run_experiment_nodetect(file,funcs,n1,n2,k) if x in correct][0])
			file.close()
			print n1, n2, ' '.join([ str(p) for p in pvalues])
		
		
def main2() :
	import sys
	import exp
	
	filename = sys.argv[1]
	
	file = open(filename + '.correct')
	correct = [ int(x.strip().split(' ')[0]) for x in file ]
	file.close() 
	
	file = open(filename + '.data')
	
	d = {}
	for x,s in run(file,correct[0]) :
		t1,t2 = s[:x],s[x+1:]
		d[x] = {}
		for n in range(1,len(t2)) :
			t2n = t2[:n]
			d[x][len(t2n)] = exp.manu(t1,t2n,0)[0], exp.prob_min(t1,t2n,0)[0], exp.prob_freq(t1,t2n,0)[0]
		
	f = open('X','w')
	print >> f, '\n'.join([' '.join([str(x) for x in d[y]]) for y in d ])
	f.close()	

	f = open('Y','w')
	print >> f, '\n'.join([ ' '.join([str(y)] * len(d[y])) for y in d ])
	f.close()						
	
	f = open('A','w')
	print >> f, '\n'.join([ ' '.join([str(d[y][x][0]) for x in d[y]]) for y in d ])
	f.close()
	
	f = open('B','w')
	print >> f, '\n'.join([ ' '.join([str(d[y][x][1]) for x in d[y]]) for y in d ])
	f.close()	
	
	f = open('C','w')
	print >> f, '\n'.join([ ' '.join([str(d[y][x][2]) for x in d[y]]) for y in d ])
	f.close()
	
	file.close()

def main3() :
	import sys
	import exp
	
	filename = sys.argv[1]
	
	if filename.endswith('.data') :
		 filename = filename[:-5]
	
	if len(sys.argv) < 3 :
		N1 = 100
	else :
		N1 = int(sys.argv[2])
	
	file = open(filename + '.correct')
	correct = [ int(x.strip().split(' ')[0]) for x in file ]
	file.close() 
	
	fs = [exp.ks, exp.prob_min, exp.prob_freq, exp.manu]
	
	file = open(filename + '.data')
	
	
	pvalues = []
	

	N2 = 50
	
	for x,s in run(file,correct[0],N1,N1,N2) :
		t1,t2 = s[:x],s[x+1:]
		for n in range(1,len(t2)) :
			t2n = t2[:n]
			pvalues.append((x,n, [ f(t1,t2n,0)[0] for f in fs ]))
		
	thresh = [1e-4, 1e-6, 1e-8, 1e-10, 1e-12, 1e-14, 1e-16, 1e-18, 1e-20]
		
	#print ', '.join(['thresh'] + [f.__doc__ for f in fs])
	
	for t in thresh :
		res = []
		for i in range(0,len(fs)) :
			try :
				r = ( y for x,y,p in pvalues if p[i] <= t ).next()
			except StopIteration :
				r = None
			res.append(r)
		if None in res :
			print t,N1,None,None,None,' '.join([str(x) for x in res])
		else :
			print t,N1,' '.join([str(x) for x in res]),' '.join([str(x) for x in res])


if __name__ == "__main__" :
	main3()