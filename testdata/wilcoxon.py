#! /usr/bin/env python

import sys, getopt, re, math
from sets import Set

def usage() :
	print """  """


def main() :
		list1, list2 = read_arrays(sys.stdin)

		reject5 = [0,2,3,5,8,10,13,17,21,25,29,34,40,46,52,58,65,73,81,89,98,107,116,126,137]

		if (len(list1) > 5 and len(list1) == len(list2)) :
						rank1, rank2 = rank(list1,list2)

						n = len(list1)

						minrank = min(rank1,rank2)
						maxrank = max(rank1,rank2)
						
						z = (minrank - (0.25*n*(n+1))) / math.sqrt((n * (n+1)*(2*n+1))/24)
						print n, rank1, rank2, z

						if (n < 30) :
										if (minrank <= reject5[n-5]) :
														print "reject A=B"
										else :
														print "accept A=B"
						else :
							if rank1 < rank2 :
								print "P(M1 worse than M2) = ", ncdfint(z) 
							else :
								print "P(M1 worse than M2) = ", 1-ncdfint(z) 

def erfcc(x):
	"""Complementary error function."""
	z = abs(x)
	t = 1. / (1. + 0.5*z)
	r = t * math.exp(-z*z-1.26551223+t*(1.00002368+t*(.37409196+
		t*(.09678418+t*(-.18628806+t*(.27886807+
		t*(-1.13520398+t*(1.48851587+t*(-.82215223+
		t*.17087277)))))))))
	if (x >= 0.):
		return r
	else:
		return 2. - r


def ncdf(x):
	"""Cumulative normal dist'n."""
	rt2 = math.sqrt(2.)
	return 1. - 0.5*erfcc(x/rt2)

def ncdfint(x,y=None) :
	if y == None :
		y = -x
	return ncdf(y) - ncdf(x)

def read_arrays(f) :
	line = f.readline()

	first = []
	while (not line.startswith("--") and line != '') :
			first.append(float(line.strip()))
			line = f.readline()

	line = f.readline()

	second = []
	while (not line.startswith("--") and line != '') :
			second.append(float(line.strip()))
			line = f.readline()
	
	return first, second

def rank(l1,l2) :
	diff = []
	for i in range(1,len(l1)+1) :
					diff.append(l1[i-1]-l2[i-1])
	
	diff.sort(abs_cmp)

	r1 = 0.0
	r2 = 0.0
	currv = -1
	currnl = 0
	currnr = 0
	max = len(diff)

	n = 0.0
	i = 0
	while(i < max and diff[i] == 0) :
			n = n + 1
			i = i + 1
	
	if (n % 2 == 1) :
					n = n - 1
	
	if n > 0 :
					r1 = 1 + 1/n
					r2 = 1 + 1/n

	while (i < max) :
					n1 = 0.0
					n2 = 0.0
					val = abs(diff[i])
					while (i < max and abs(diff[i]) == val) :
									if (diff[i] > 0) :
													n1 = n1 + 1
									else :
													n2 = n2 + 1
									i = i + 1

				
					if (n1+n2) > 1 :
									rd = (i + (i-n1-n2+1)) / (n1+n2)
					elif (n1+n2) > 0 :
									rd = i
					r1 = r1 + n1*rd
					r2 = r2 + n2*rd

	
					
	return r1, r2

def abs_cmp(x,y) :
		if (abs(x) ==  abs(y)) :
						return 0
		elif (abs(x) > abs(y)) :
						return 1
		else :
						return -1

if __name__ == "__main__":
    main()

    
