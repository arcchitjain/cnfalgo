#! /usr/bin/env python

def print_usage() :
	from sys import argv
	print "Usage:", argv[0], "[options]"
#	print """ Description of options: """

def main() :
	
	from sys import argv, exit,stdout, stderr, stdin
	from getopt import GetoptError, gnu_getopt
	from termcolor import TerminalController
	try:
		opts, args = gnu_getopt(argv[1:], "k:m:o:r:vg:1:2:P:", [ 'help' ])
	except GetoptError, err:
		print >> stderr, str(err)
		print_usage()
		exit(2)
		
	n1 = 50
	n2 = 50
	for a,v in opts :
		if a == '-1' :
			n1 = int(v)
		elif a == '-2' :
			n2 = int(v)
		
	
	if args :
		fn = args[0]
		fn2 = args[1]
	else :
		fn = 'exp'
		fn2 = 'exp'
	
	f = open(fn + '.correct')
	correct = [ int(x.strip().split(' ')[0]) for x in f ]
	f.close()
	
	f = open(fn2 + '.detect')
	detect = [ [int(a) for a in x.strip().split(' ')] for x in f ]
	f.close()
	
		
	if detect :
		detect_min, _, detect_left, detect_right = zip(*detect)
	else :
		detect_min = detect_left = detect_right = []
	
	#print correct,detect_min, detect_left
	
	a,b =  match_drift(correct,detect_min,n1,n2)
	
	if a :
		precision = float(len([x for x in a if x]))/len(a)
	else :
		precision = 1
	
	if b :
		recall = float(len([x for x in b if x]))/len(b)
	else :
		recall = None
	
	d = distance(correct,detect_min,b)
	if d :
		dist =  float(sum(d))/len(d)
	else :
		dist = None
	
	s = distance([x-n2 for x in correct],detect_left,b)
	if s :
		speed = float(sum(s))/len(s)
	else :
		speed = None

	print "SCORE", len(correct),len(detect_min), precision,recall,speed,dist,n1,n2
	
	
	
def match_drift(correct,detect,n1,n2) :
	return [[i for i,c in enumerate(correct) if d >= c - n2 and d <= c + n1 ] 
		for d in detect ], [[i for i,d in enumerate(detect) 
				if d >= c - n2 and d <= c + n1 ] for c in correct]
	
def distance(correct,detect,match) :
	return [ min([abs(detect[x]-c) for x in match[i]])	
									for i,c in enumerate(correct) if match[i] ]

def speed(correct,match) :
	pass
	
	
def precision_recall(correct,detect) :
	# correct = list of correct values
	# detect = list of detection intervals
	
	# easy case: correct are single point

	ds = set([])
	false_negative = 0.0
	true_positive = 0.0
	for c in correct :
		d = [ (x,y) for (x,y) in detect 
									if int(x) <= int(c) and int(c) <= int(y) ]
		# d contains 0 or 1 element
		if not d :
			# current change not detected
			false_negative += 1
		else :
			true_positive += 1
#			ds.update(d)
						
	all_positive = float(len(detect))

#	true_positive = float(len(ds))
	#false_positive = all_positive - true_positive
							
	precision = true_positive / all_positive
	recall = true_positive / float(len(correct))
	
	return precision,recall
	
if __name__ == "__main__" :
	main()