#! /usr/bin/env python

from sys import stdin

l = [ x.strip() for x in stdin ]
l2 = [ float(x) for x in l if x != 'None' ]

if not l2 :
	print 'NaN', len(l), len(l2)
else :
	print sum(l2)/len(l2), len(l), len(l2)
