#! /usr/bin/env python



def main() :
	from sys import argv, exit
	from string import join
	from getopt import gnu_getopt, GetoptError

	if len(argv) < 2 :
		print "Please provide a execution mode: mkdrift or output!"
		exit(2)

	
	if argv[1] == 'mkdrift' :

		try:
			opts, args = gnu_getopt(argv[2:], "c:S")
		except GetoptError, err:
			print str(err)
			exit(2)

		if len(args) == 0 :
			print "Please provide a filename!"
			exit(2)
			
		filename = args[0]

		col = -1
		shc = 0
		for o,a in opts :
			if o == '-c' :
				col = int(a)
			elif o == '-S' :
				shc = 1
		print join([ str(a) for a in mk_drift(filename,shc,col)],',')
	
	elif argv[1] == 'output' :
	
		try:
			opts, args = gnu_getopt(argv[2:], "d:r:")
		except GetoptError, err:
			print str(err)
			exit(2)

		if len(args) == 0 :
			print "Please provide a filename!"
			exit(2)
			
		real = []
		detect = []
		for o,a in opts :
			if o == '-r' :
				real = a.split(',')
			elif o == '-d' :
				detect = a.split(',')

		filename = args[0]

		process_output(filename,real,detect)
	
def mk_drift(filename,shuffleclasses,col) :
	from string import join

	classes = {}
	classsz = {}
	s = 0
	
	f = open(filename,'r')
	for line in f :
		data = line.strip().split(' ')
		d,r = extract(data,col)
		if not d in classes :
			classes[d] = open(filename + '.new-' + str(d), 'w')
			classsz[d] = 0
		
		if s > 0 and s % 10000 == 0 :
			print s
			
		s+= 1
		classes[d].write(join(r) + '\n')
		classsz[d] += 1
		
	f.close()

	for c in classes :
		classes[c].close()
		
	from random import shuffle
	from os import remove
	classlist = [c for c in sorted(classes)]
	
	if shuffleclasses == 1 :
		shuffle(classlist)
	
	outf = open(filename + '.new', 'w')
	sum = 0
	clsz = []
	for c in classlist :
		inf = open(filename + '.new-' + str(c), 'r')
		outf.writelines(inf.readlines())
		inf.close()
		remove(filename + '.new-' + str(c))
		sum += classsz[c]
		clsz.append(sum)
	
	from sys import stderr
	print >> stderr, join(classlist,'|')
	
	return clsz

def process_output(output,real,detect) :	
	from string import join
	output = open(output,'r')
	for line in output :
		if line.strip()[0] != '!' :
			line = line.strip().split(' ')
			id = line[0]
			if id in real :
				line.append('1')
			else :
				line.append('*')
			if id in detect :
				line.append('1')
			else :
				line.append('*')
			print join(line,' ')
	output.close()

def extract(data,col) :
	if col == -1 :
		return data[0], data[1:]
	else :
	
		index = [ i for i in range(0,len(data)) if int(data[i]) / 1000 == col ]
		if index == [] :
			return -1, data
		else :
			index = index[0]
			return data[index], data[:index-1] + data[index+1:]

if __name__ == "__main__" :
	main()