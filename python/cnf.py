

class CNFTheory :

	def __init__(self,k) :
		self.clauses = {}
		self.k = k
		
	def add(self,neg,pos,w=0) :
		neg = tuple(neg)
		pos = tuple(pos)
	
		print "add: ", neg, "->", pos
		
		if len(neg) + len(pos) > self.k :
			return 0
		else :
			clauses = self.clauses
			if not neg in clauses :
				clauses[neg] = (0,{})
				clauses[neg][1][()] = 1
				clauses[neg][1][pos] = w
			#elif pos in clauses[neg] :
			#	clauses[neg][1][pos] += w
			elif pos not in clauses[neg] :
				clauses[neg][1][pos] = w
			return 1
		
		
	def find_fail(self,example,update=0) :
		clauses = self.clauses
		example = list(example)
		for neg,rest in pick_some(self.k,example) :
			if neg in clauses :
				if update :
					c,p = clauses[neg]
					clauses[neg] = c+1,p
				for pos in clauses[neg][1].keys() :
					if not intersect(pos,rest) :
						if update :
							clauses[neg][1][pos] += 1
						yield neg, pos, rest
			elif update :
				clauses[neg] = (1,{})
				clauses[neg][1][()] = 1
				
	def fix(self,neg,pos,add) :
		for x in add :
			p = list(pos) + [x]
			p.sort()
			#n = list(neg) + [x]
			#n.sort()
			#n = tuple(n)
			#if n in self.clauses :
			if len(p) == 1 :
				w = self.clauses[()][0] - 1
			else :
				w = 0
			self.add(neg,p,w)
		
def print_theory(clauses) :
	for neg in clauses :
		sn = ' and '.join([ str(x) for x in neg ])
		c,p = clauses[neg]
		for pos in p :
			sp = ' or '.join([ str(x) for x in pos ])
			print sn, '('+str(c)+') -> ', sp, '('+str(p[pos])+')'
		
	
		
def pick_one(list) :
	for i in range(0,len(list)) :
		yield list[i], list[i+1:], list[:i]
		
def pick(n,list,before) :
	if n == 0 :
		yield [], list, before
	else :
		for x,r,a in pick_one(list) :
			for l,s,y in pick(n-1,r,before+a) :
				yield [x] + l, s, y
				
def pick_some(n,list) :
	for i in range(0,n+1) :
		for l,x,y in pick(i,list,[]) :
			yield tuple(l), y+x
			
def intersect(a,b) :
	return len(set(a) & set(b))
	
	
def test() :
	c = CNFTheory(4)
	
	c.add([],[])
	
	print c.clauses
	
	examples = [ (1,2,3), (1,2,4), (1,3,4) ]
	
	for x in examples :
		print "=================="
		for f in c.find_fail(x,1) :
			c.fix(*f)
			
			print x,f
			print_theory(c.clauses)
			
	
	

	