#! /usr/bin/env python

def print_usage() :
	from sys import argv
	print "Usage:", argv[0], "[options] [filename]"
	print """ Description of options:
	-k <>	max length of rules learned by cnf miner (default: 3)
	-m <>	primary metric for reporting detection/restarting (default: 0)
	-o <>	base name for the outputfile (default: no detailed output)
			- main output		xxx.out
			- detection output	xxx.detect
			underscore (_) will write all output to stdout
	-r <>	restart mode (default: off)
			- off		do not restart learner
			- full		discard both test sets
			- partial	only discard test set 1
			- keep		keep both test sets
	-v  	verbose: output all detection events (default: off)
	-g <>	generate a graph (using GLE) with given filename
	-1 <>	size of test set 1 (50)
	-2 <>	size of test set 2 (50)
	-P <>	threshold on probability (0.001) """
	
def main() :
	
	from sys import argv, exit,stdout, stderr, stdin
	from getopt import GetoptError, gnu_getopt
	from termcolor import TerminalController
	try:
		opts, args = gnu_getopt(argv[1:], "k:m:o:r:vg:1:2:P:s", [ 'help' ])
	except GetoptError, err:
		print >> stderr, str(err)
		print_usage()
		exit(2)
		
	k = 3
	primary = 0
	single = 0
	restart = 'off' 
	verbose = 0
	outname = 'exp'
	outfile = None
	detfile = None
	graph = None
	n1 = 50
	n2 = 50
	P = 0.001
	single = 0
	for a,v in opts :
		if a == '-k' :
			k = int(v)
		elif a == '-m' :
			primary = int(v)
		elif a == '-o' :
			if v == '_' :
				outfile = stdout
				deffile = stdout
			else :
				outfile = open(v + '.out','w')
				detfile = open(v + '.detect','w')
				outname = v
		elif a == '-r' :
			restart = v
		elif a == '-v' :
			verbose = 1
		elif a == '-h' or a =='--help' :
			print_usage()
			exit(0)
		elif a == '-g' :
			graph = v
		elif a == '-1' :
			n1 = int(v)
		elif a == '-2' :
			n2 = int(v)
		elif a == '-P' :
			P = float(v)
		elif a == '-s' :
			single = 1
			
	


	if (len(args) == 0) :
		filename = "_"
	else :
		filename = args[0]
	
		
	from cnfalgo import CNFAlgo
	
	if filename == '_' :
		file = stdin
	else :
		file = open(filename)
	
	# dd = AlgoCaller(n1,n2,k,lambda x,y : prob_threshold(1,x,y) )
	
		
	prob_one = lambda x,y,_ : prob_threshold(1,x,y)
	prob_one.__doc__ = "bayesian (fixed 1)"
	
	#funcs = [prob_one,prob_min,prob_freq] 
	#funcs = [prob_freq]
	#funcs = [prob_min,prob_freq,manu]
	funcs = [manu,prob_freq_fixed,prob_freq]  # ,prob_freq_fixed]
	
	funcs = [funcs[primary]] + funcs[:primary] + funcs[primary+1:]
	
	if single :
		funcs = [funcs[0]]
		
#	for ex,p in run_experiment_nodetect(file,funcs,n1,n2,k,P) :
#		print ex, ' '.join([str(x) for x in p])


#	d = dict(run_experiment_nodetect(file,funcs,n1,n2,k,P))
#	print d
		
	run_experiment(file,funcs,n1,n2,k,P,restart,verbose,outfile,detfile)
	
	if graph != None :
		generate_graph(outname,P,graph,n1,n2,funcs)
		
		
def run_experiment_nodetect(file,funcs,n1,n2,k=2) :
	ac = AlgoCaller(n1,n2,k,funcs)
	
	for line in file :
		example = [int(x) for x in line.strip().split(" ")]
		
		(exid,probs) =  ac.push_example(example)
		
		if probs != None :
			yield exid, [p[0] for p in probs]
#		elif exid >= 0 :
#			yield exid, []
			
	return
		
		
def run_experiment(file,funcs,n1,n2,k=2,P=0.001,restart='off',verbose=0,outfile=None,detfile=None) :
	from sys import stdout, stderr
	from termcolor import TerminalController

	errterm = TerminalController(stderr)
	outterm = TerminalController(stdout)

	#,lambda x,y : prob_one(y,x),lambda y,x : prob_min(x,y)]
	ac = AlgoCaller(n1,n2,k,funcs)
		
	dd = []
	for _ in funcs :
		dd.append(DriftDetector(P))
	
	for line in file :
		
		# parse the example from the file
		example = [int(x) for x in line.strip().split(" ")]
		
		# push the example to the learning algorithm
		(exid,probs) =  ac.push_example(example)
		# probs is a list of [( prob, threshold, error1, error2 )]
		
		if probs != None :
			if outfile != None :
					# print log string: exid, prob1, prob2, ...
					print >> outfile, exid, ' '.join([str(p[0]) for p in probs ])
		
			for i in range(0,len(probs)) :
				res = dd[i].push_probability(probs[i] + (exid, ))
				if (res != None) :
					if (res[1] != None or verbose) :
						
						if i == 0  :
							print outterm.BOLD, i,"==>", format_result(*res) , \
										'==>',funcs[i].__doc__, outterm.NORMAL
							if detfile != None and res[1] != None :	
#								print >> detfile, res[0][-1], 0.01
								print >> detfile, res[1][-1], 1, \
											res[0][-1], res[2][-1]
#								print >> detfile, res[2][-1], 0.01
							cnt = ac.restart_learner(restart)
							print outterm.render("${GREEN}restarting (" 
											+ restart 
											+ ") learner at example " 
											+ str(cnt) + "${NORMAL}")
						else :
							print "", i, "==>", format_result(*res), '==>', \
															funcs[i].__doc__
	#	else : # probs == none
	#		print >> outfile, exid, join(['*']*len(funcs))
		
	#print " -> final theory size", len(algo), "<- "
	file.close()
	if (outfile != None) :
		outfile.close()
	if (detfile != None) :
		detfile.close()

		
def format_result(start,min,end) :
	p_start, t_start, a_start, b_start, n_start = start
	p_min, t_min, a_min, b_min, n_min = min
	p_end, t_end, a_end, b_end, n_end = end
	
	return 'DETECT @ %(n_min)5i, p = %(p_min).4e, interval = [%(n_start)5i, %(n_end)5i], (%(a_min).4f, %(b_min).4f), threshold = %(t_min)i' % locals()
	
	
class AlgoCaller :
	
	def __init__(self,N1,N2,k,probf,cnt=0) :
		from cnfalgo import CNFAlgo
		self.algo = CNFAlgo(k)
		self.buffer = []
		self.N1 = N1
		self.N2 = N2
		self.count = cnt
		self.probf = probf
		
	def push_example(self,example) :
		self.buffer.append(example)
		self.count += 1
		if len(self.buffer) > self.N1 + self.N2 :
			example = self.buffer[0]
			self.buffer = self.buffer[1:]
			tf = self.algo.train(example)
			
			test1 = self.algo.test_set(self.buffer[0:self.N1])
#			test1 = []
#			for x in buffer[0:n1] :
#				test1.append(algo.test(x))
			
			test2 = self.algo.test_set(self.buffer[self.N1:])
#			test2 = []
#			for x in buffer[n1:] :
#				test2.append(algo.test(x))

			tsize = len(self.algo)
			
			return (self.count - self.N2, [f(test1,test2,tsize) 
													for f in self.probf])
		return (self.count-self.N2,None)
		
	def restart_learner(self,mode) :
		if mode == 'partial' :
			return self.restart_learner_partial()
		elif mode == 'full' :
			return self.restart_learner_full()
		elif mode == 'keep' :
			return self.restart_learner_keep()
		else :
			return None
		
	def restart_learner_full(self) :
		from cnfalgo import CNFAlgo
		self.algo = CNFAlgo(self.algo.k)
		self.buffer = []
		return self.count
		
	def restart_learner_partial(self) :
		from cnfalgo import CNFAlgo
		self.algo = CNFAlgo(self.algo.k)
		self.buffer = self.buffer[self.N1:]
		return self.count - self.N2
		
	def restart_learner_keep(self) :
		from cnfalgo import CNFAlgo
		self.algo = CNFAlgo(self.algo.k)
		return self.count - self.N1 - self.N2
		
class DriftDetector :
	
	def __init__(self,P) :
		self.start_threshold = P
		self.end_threshold = P
		self.detect_start = None
		self.detect_min = None
		self.log = None
		
	def push_probability(self,prob) :
		if prob == None:
			return None
		if prob < (self.start_threshold, ) :
			if self.detect_start == None :
				self.detect_start = prob
				self.detect_min = prob
				return self.detect_start, None, None
			elif prob < self.detect_min :
				self.detect_min = prob
				return None
		elif prob > (self.end_threshold, ) and self.detect_start != None :
		#	if (prob[-1] - self.detect_start[-1] <= 5) :
		#		result = self.detect_start, None, prob
		#	else :
			result = self.detect_start, self.detect_min, prob
			
			self.detect_start = None
			self.detect_min = None
			return result
		return None
		
#	def _write_log(self,line) :
#		from string import join
#		if self.log != None :
#			print >> self.log, join([str(x) for x in line])
	
		
	
	

def generate_graph(data,threshold,oname,n1,n2,funcs) :
	
	outname = oname + '.gle'
	out = open(outname,'w')
	
	f = open('exp.detect')
	det = 0
	for x in f :
		det = 1
		break
	f.close()
	
	print >> out, """
size 16 12
begin graph
	scale 0.8 0.8
"""
	if det :
		print >> out ,"""data "exp.detect" d1=c1,c2"""
	else :
		print >> out, """data "exp.correct" d1=c1,c2"""

	print >> out, """
	data "exp.correct" d2=c1,c2
	xtitle "example between test sets"
	data "exp.out"
   x2axis off
   y2axis off
   yaxis log max 1
   ytitle "ln p"
   key position br offset 0.4 0.4
"""

	if det :
		print >> out, """d1 line impulses color darkgreen lwidth 0.02 lstyle 9 key "concept drift detected" """

	print >> out, """
	d2 line impulses color black lwidth 0.02 lstyle 2 key "class transition"
	
	let d7 = 1
"""
	colors = ['red','blue','black','green']
	
	for i,f in enumerate(funcs) :
		print >> out, 'd' + str(i+3), 'line color', colors[i], 'lwidth 0.02 key "' + f.__doc__ + '"'
   
	print >> out, "let d8 =", threshold
	print >> out, "d8 line color black lwidth 0.01 lstyle 0"
	f = open(data + '.correct') 
	for line in f :
		d = line.strip().split(' ')
		if (d[3] > d[2]) :
			print >> out, "fill d8,d7 color gray10 xmin", d[2], "xmax", d[3]
	f.close()
	
	f = open(data + '.detect') 
	for line in f :
		d = line.strip().split(' ')
		print >> out, "fill x1,d8 color lightgreen xmin", d[2], "xmax", d[3]
	f.close()
	
	
	print >> out, "end graph"
	
	out.close()
	
	from os import popen
	
	pipe = popen('gle -d pdf ' + outname)
	
	for _ in pipe :
		pass
	
	pipe.close()
	
	pipe = popen('open ' + oname + '.pdf')
	
	pipe.close()
	
	
	
#def prob_one(test1,test2) :
#	return prob_threshold(1,test1,test2)
	
def prob_threshold(t,test1,test2) :
	k1 = sum((1 for x in test1 if x >= t))
	k2 = sum((1 for x in test2 if x >= t))
	return (probability_bayes(k1,len(test1),k2,len(test2)), t, k1, k2)
	
def prob_min(test1, test2,_) :
	"""bayesian (dynamic)"""
	th = set(test1) | set(test2)
	
	minp = (1,)
	for t in th :
		prob = prob_threshold(t,test1,test2)
		if prob < minp :
			minp = prob
	return minp

def probability_bayes(k1,N1,k2,N2) :
	from scipy.special import betaln
	from math import exp
	s = 0
	l1 = N1-k1
	for i in range(k2,N2+1) :
		l2 = N2-i
		s += exp(logchoose(N2,i)+betaln(1+k1+i,1+l1+l2)-betaln(1+k1,1+l1))
	return s
	
def probability_freq(k1,N1,k2,N2) :
	from math import exp
	a = abs(k1-k2)
	return exp(-2*a*a*N1*N2/(N1+N2))

def single_test_error(t,test1,test2,tsize) :
	return len([ 1 for x in test1 + test2 if x >= t ])


def prob_freq(test1,test2,tsize) :
	"""frequentist (dynamic)"""
	N1 = float(len(test1))
	N2 = float(len(test2))
	
	a,t = margin_threshold(test1,test2)
	from math import exp
	
	k1 = float(sum((1 for x in test1 if x < t)))/len(test1)
	k2 = float(sum((1 for x in test2 if x < t)))/len(test2)

	#x,y = margin_cnf(test1,tsize), margin_cnf(test2,tsize)
	
	#print '-->',k1,k2,probability_freq(k1,N1,k2,N2),t
	prob = probability_freq(k1,N1,k2,N2)
	#print a,t,prob,k1,k2
	return (prob,t,k1,k2)
	
def prob_freq_fixed(test1,test2,tsize) :
	"""frequentist (fixed 1)"""
	N1 = float(len(test1))
	N2 = float(len(test2))
	
	#a,t = margin_threshold(test1,test2)
	t = 5
	
	from math import exp
	
	k1 = float(sum((1 for x in test1 if x < t)))/len(test1)
	k2 = float(sum((1 for x in test2 if x < t)))/len(test2)

	#x,y = margin_cnf(test1,tsize), margin_cnf(test2,tsize)
	
	#print '-->',k1,k2,probability_freq(k1,N1,k2,N2),t
	prob = probability_freq(k1,N1,k2,N2)
	#print a,t,prob,k1,k2
	return (prob,t,k1,k2)
	
def manu_direct(s1,s2) :
	cnt = lambda a,b : 0.5*sign(a-b) + 0.5 
	return min(sum(( sum(( cnt(x,y) for y in s2)) for x in s1) ),
							sum(( sum(( cnt(x,y) for y in s2)) for x in s1) ))
							
def manu(s1,s2,_) :
	"""statistical (Mann-Whitney U)"""
	from scipy.stats import mannwhitneyu, ranksums
	
	
	try :
	#	if len(s1) > 20 or len(s2) > 20 :
	#		u,p = ranksums(s1,s2)
	#	else :
		u,p = mannwhitneyu(s1,s2)
	except ValueError, err :
		u,p = 0,1
	
	#print s1,s2, u,p
	
	return (p,0,u,len(s1)*len(s2)-u)

def ks(s1,s2,_) :
	"""statistical (Kolmogorov-Smirnov)"""
	from scipy.stats import ks_2samp
	
	
	try :
	#	if len(s1) > 20 or len(s2) > 20 :
	#		u,p = ranksums(s1,s2)
	#	else :
		u,p = ks_2samp(s1,s2)
	except ValueError, err :
		u,p = 0,1
	
	#print s1,s2, u,p
	
	return (p,0,u,len(s1)*len(s2)-u)
	

	
def ranksum(s1,s2,_) :
	"""statistical (ranksum)"""
	from scipy.stats import mannwhitneyu, ranksums
	
	try :
	#	if len(s1) > 20 or len(s2) > 20 :
		D,p = ranksums(s1,s2)
		
	#	else :
	#		u,p = mannwhitneyu(s1,s2)
	except ValueError, err :
		D,p = 0,1
	return (p,0,D,0)
	
	
def sign(s) :
	if s == 0 :
		return 0
	elif s < 0 :
		return -1
	else :
		return 1

def margin_cnf(test,tsize) :
	return 1 - sum((x/tsize for x in test))/len(test)
		
def margin_threshold(t1,t2) :
	f = lambda t,t1,t2 : abs(float(sum((1 for x in t1 if x < t)))/len(t1) \
				- float(sum((1 for x in t2 if x < t)))/len(t2))
	th = set(t1) | set(t2)
	t,p = argmax(th,f,t1,t2)
	return p,t
	
def argmax(values,function,*args) :
	maxf = None
	maxv = None
	for v in values :
		f = function(v,*args)
	#	print f,v
		if f > maxf :
			maxf,maxv = f,v
	
	return maxv,maxf

def logchoose(n, k):
	from scipy.special import gammaln

	lgn1 = gammaln(n+1)
	lgk1 = gammaln(k+1)
	lgnk1 = gammaln(n-k+1)
	return lgn1 - (lgnk1 + lgk1)

if __name__ == "__main__" :
	main()
