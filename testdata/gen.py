#! /usr/bin/env python

def print_usage() :
	from sys import argv
	print "Usage:", argv[0], "[options] filename"
	print """ Description of options:
	-s <>	amount of overlap between concepts  (0)
	-o <>	base name for the output (.correct) (stderr)
	-c <>	id's of concepts separated by ','   (random)
	-n <>	number of examples per concept (see -c)
	-S <>	random seed to use (python default) """
	
def main() :
	
	# import some external functions
	from sys import argv, exit,stdout, stderr
	from getopt import GetoptError, gnu_getopt
	from termcolor import TerminalController
	from random import seed
	
	# read command line arguments
	try:
		opts, args = gnu_getopt(argv[1:], "s:o:c:n:S:")
	except GetoptError, err:
		print >> stderr, str(err)
		print_usage()
		exit(2)
		
	overlap = 0
	output = None
	concepts = None
	counts = None
	randomseed = None
	for a,v in opts :
		if a == '-s' :
			overlap = int(v)
		if a == '-o' :
			output = v
		if a == '-c' :
			concepts = v.split(',')
		if a == '-n' :
			counts = v.split(',')
		if a == '-S' : 
			randomseed = v
			
	if len(args) == 0 :
		print >> stderr, "missing command line argument <filename>"
		print_usage()
		exit(2)
		
	filename = args[0]
	
	if output == None or output == '_' :
		output = stderr
	else :
		output = open(output,'w')

	# seed the random generator
	seed(randomseed)

	# generate the file
	for c,x in Generator(DatasetSource(filename,wrap=Concept.WRAP_BREAK),
													overlap,output,concepts,counts) :
		print ' '.join(x)
	
	output.close()
	
	
def abstract():
    import inspect
    caller = inspect.getouterframes(inspect.currentframe())[1][3]
    raise NotImplementedError(caller + ' must be implemented in subclass')
	
class DataSource(object) :

	def __init__(self) :
		self._concepts = {}

	def add_concept(self,name,concept) :
		self._concepts[name] = concept
		
	def concept(self,name) :
		return self._concepts[name]
		
	def get_concepts(self) :
		return sorted(self._concepts.keys())

	def get_concept_count(self) :
		return len(self.concepts)
		
	def concept_size(self,concept) :
		return len(self._concepts[concept])
		
	concepts = property(get_concepts)
	concept_count = property(get_concept_count)
	
	def generate(self,concept=None,count=None) :
		# generate examples from a given concept
		# concept==None means random concept
		# count==None means all available data
		# count== -n means all available data but n
		if concept == None :
			from random import choice
			concept = choice(self.concepts)
			
		if not concept in self.concepts :
			raise LookupError, "concept " + str(concept) + " was not found"
		
		# concept is set to a valid concept
		source = self.concept(concept)
		if count == None :
			count = 0
			
		if count > 0 :
			for _ in range(0,count) :
				yield (concept,source.next())
		else :
			while source.remain >= -count :
				yield (concept,source.next())
				
		return
	
	
class Concept(object) :

	WRAP_NONE = 0			# no reuse of data
	WRAP_BREAK = 1			# restart reading data but pause with StopIteration
	WRAP_TRANSPARENT = 2	# restart reading data automatically
	WRAP_NOREMOVE = 3		# random sample data without removing

	def __init__(self,wrap) :
		self.count = 0
		self.wrapping = wrap
		
	def _remain(self) :
		from sys import maxint 
		if len(self) == maxint :
			return maxint
		else :
			return len(self) - self.count
	remain = property(_remain)
	
	def __iter__(self) :
		return self
	
class ListBasedConcept(Concept) :

	def __init__(self,data,wrap=Concept.WRAP_NONE) :
		Concept.__init__(self,wrap)
		self.data = data
		self.index = range(len(data))
		
	def __len__(self) :
		if self.wrapping == Concept.WRAP_NONE \
									or self.wrapping == Concept.WRAP_BREAK :
			return len(self.data)
		else :
			from sys import maxint
			return maxint		# infinity
		
	def next(self) :
		# print self.count, self.remain

		if len(self.index) == 0 :
			if self.wrapping != Concept.WRAP_NONE :
				self.index = range(len(self.data))
			if self.wrapping != Concept.WRAP_TRANSPARENT :
				self.count = 0
			#	print self.count, self.remain, "STOP"
				raise StopIteration
	
		self.count += 1
		if self.wrapping == Concept.WRAP_NOREMOVE :
			from random import choice
		#	print self.count, self.remain, "RDATA"

			return choice(self.data)
		else :
		#	print self.count, self.remain, "DATA"

			return self.data[choose_del(self.index)]
	
class FileBasedConcept(Concept) :

	def __init__(self,source,size=None,wrap=Concept.WRAP_NONE) :
		Concept.__init__(self,wrap)
		if self.wrapping == Concept.WRAP_NOREMOVE :
			raise ValueError, "wrap mode not supported for this concept type"
		self.source = source
		self.openfile = None
		self.size = size
		self.calc_size()
	
	def __len__(self) :
		return self.size
	
	def calc_size(self) :
		if self.size == None :
			f = open(self.source)
			self.size = len(f)
			f.close()
			
	def next(self) :
		if self.openfile == None :
			self.openfile = open(self.source)
			
		line = self.openfile.readline()
		if line == '' :
			if self.wrapping != Concept.WRAP_NONE :
				self.openfile.close()
				self.openfile = None
		
			if self.wrapping != Concept.WRAP_TRANSPARENT :
				raise StopIteration 
			else :
				line = self.openfile.readline()
			
		return line
		
def choose_del(list) :
	from random import randrange
	
	index = randrange(len(list))
	
	result = list[index] 
	list[:] =  list[:index]+list[index+1:]
	return result

class DatasetSource(DataSource) :

	def __init__(self,sourcefile,destfile=None,wrap=Concept.WRAP_NONE) :
		DataSource.__init__(self)
		self.source = sourcefile
#		if destfile == None :
#			destfile = sourcefile+'.new'
		self.destination = destfile
		self.class_column = -1
		self.wrapping = wrap
		self.parse()

	def __del__(self) :
		if self.destination != None :
			from os import remove
			for cl in self.concepts :
				remove(self.get_filename(cl))

	def get_filename(self,concept) :
		return self.destination + '-' + str(concept)

	def parse(self) :
		from string import join
		from random import choice, shuffle

		cl_data = {}
#		cl_files = {}	# this will hold a file handle for each class
		cl_sizes = {}	# this will hold the number of elements in each class
		
		# open the source file
		f = open(self.source,'r')
		
		# read file and sort each line into the right file for the class
		for line in f :
			# parse line
			data = line.strip().split(' ')
			
			# extract the class column from the data
			cl,rest = self.extract_class(data)
			
			# if new class, create class
			if not cl in cl_sizes :
				cl_data[cl] = []
#				cl_files[cl] = open(self.get_filename(cl), 'w')
				cl_sizes[cl] = 0
				
			# write data to appropriate file
#			cl_files[cl].write(join(rest) + '\n')
			cl_data[cl].append(rest)
			cl_sizes[cl] += 1
			
		# close files
		f.close()
		
		# write output files
		for cl in cl_sizes :
			if self.destination == None :
			#	shuffle(cl_data[cl])
				self.add_concept(cl,
							ListBasedConcept(cl_data[cl],self.wrapping))
			else :
				fn = self.get_filename(cl)
				f = open(fn,'w') 
				while cl_data[cl] :
					line = choice(cl_data[cl])
					f.write(join(rest) + '\n')
					cl_data[cl].remove(line)
				f.close()
				self.add_concept(cl,
							FileBasedConcept(fn,cl_sizes[cl],self.wrapping))
			
	def extract_class(self,data) :
		if self.class_column == -1 :
			return data[0], data[1:]
		else :
			index = [ i for i in range(0,len(data)) 
								if int(data[i]) / 1000 == self.class_column ]
			if index == [] :
				return -1, data
			else :
				index = index[0]
				return data[index], data[:index-1] + data[index+1:]


class Generator(object) :

	def __init__(self,source,overlap=0,out=None,concepts=None,counts=None) :
		self.source = source
		self.overlap = overlap
		self.output = out
		self.concepts = concepts
		self.counts = counts
	
	def __iter__(self) :
		count = 0
		from random import shuffle
		if self.concepts == None :
			concepts = self.source.concepts
			shuffle(concepts)
		else :
			concepts = self.concepts
			
	
		overlap = self.overlap / 2 
	
		
	
		oc = None
		for c in concepts :
			start_c = count
			from random import random
			
			if (oc != None) :
				oc_values = [ x for x in self.source.generate(oc) ]
				for i in range(1,overlap*2) :
					
					if random() < (0.5*i)/(overlap) :
						value = self.source.generate(c,1).next()
					else :
						if not oc_values :
							break
						else :
							value = oc_values[0]
							oc_values = oc_values[1:]
					count += 1
					yield value
			mid_c = count
			oc = c 
			for d in self.source.generate(c,-overlap) :
				count += 1
				yield d
			
			if self.output != None and start_c != 0 :
				#for x in range(start_c,mid_c) :
				#	print >> self.output, x, 1
				#for x in range(mid_c,count) :
				#	print >> self.output, x, 0
				print >> self.output, (start_c + mid_c)/2, 1, \
												start_c, mid_c, count,  c
				
		return

if __name__ == "__main__" :
	main()