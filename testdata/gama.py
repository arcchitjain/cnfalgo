#! /usr/bin/env python


class GamaDriftDetector :
	
	def __init__(self) :
		self.alfa = 3
		self.beta = 2
		self.minp = 1
		self.mins = 0
		self.testsize = 200
		self.classthresh = 1
		self.kw = None
		self.buffer = []
		self.wbuffer = []
		self.size = 0
		self.algo = None
		self.count = 0
	
	def push_example(self,example) :
		from cnfalgo import CNFAlgo
		self.buffer.append(example)
		self.count += 1
		self.size += 1
		if len(self.buffer) > self.testsize :
			example = self.buffer[0]
			self.buffer = self.buffer[1:]
			
			if self.algo == None :
				self.algo = CNFAlgo(2)
			
			tf = self.algo.train(example)
			
			p = float(len([x for x in self.algo.test_set(self.buffer) 
												if x >= self.classthresh ]))/len(self.buffer)
			
			from math import sqrt
			#s = sqrt(p*(1-p)/self.size)
			s = sqrt(p*(1-p)/len(self.buffer))
	
			#print self.count, p, s, self.size
	
			if p + s < self.minp + self.mins :
				self.minp, self.mins = p,s
	
			if p + s > self.minp + self.alfa * self.mins :
				print '=======> KD = ', self.count
				self.buffer = []
				self.algo = CNFAlgo(2)
				for x in self.wbuffer :
					self.algo.train(x)
				self.wbuffer = []
				self.kw = None
				self.size = 0 
				self.minp, self.mins = 1,0
				
			elif p + s > self.minp + self.beta * self.mins :
				if self.kw == None :
					self.kw = self.count	
					print '==> kw = ', self.count
				self.wbuffer.append(example)
			
			else :
				if self.kw != None :
					print "back to normal"
					self.kw = None
			
def main() :
	
	from sys import argv, stdin
	
	file = stdin
	
	g = GamaDriftDetector()
	
	for line in file :
		
		# parse the example from the file
		g.push_example([int(x) for x in line.strip().split(" ")])
		
	file.close()
	
if __name__ == "__main__" :
	main()