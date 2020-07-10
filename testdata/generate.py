#!/usr/bin/env python


# Concept Drift Dataset Generator


def generate_stagger(concept, posonly = 0) :
	from random import randint
	value = 0
	
	while value == 0 :
		size = randint(1,3)
		color = randint(1,3)
		shape = randint(1,3)
	
		if concept == 1 and size == 1 and color == 1 :
			value = 1
		elif concept == 2 and (color == 2 or shape == 1) :
			value = 1
		elif concept == 3 and (size == 2 or size == 3) :
			value = 1
			
		if posonly == 0 :
			break
	
	
	return (str(value*2+1),str(2*(size+2)-1),str(2*(color+5)-1),str(2*(shape+8)-1))
	
	
def generate_speed(concept, posonly = 0) :
	from random import randint, random
	value = 0
	
	while value == 0 :
		a = [randint(0,1) for _ in range(0,6)]
		c = int(random() <= concept)
		
		if c == 0 and a[0] and a[1] :
			value = 1
		elif c == 1 and ((a[2] and a[3]) or (a[4] and a[5])) :
			value = 1 
		
		if posonly == 0 :
			break
#	print concept, c, a
	return [str(value*2+1)] + [str((1+a[i]+2*(i+1))*2 - 1) for i in range(0,6)]
	
def generate_extent(concept, posonly = 0) :
	from random import randint, random
	value = 0
	
	while value == 0 :
		a = [randint(0,1) for _ in range(0,6)]
		
		if concept == 0 and a[0] and not a[1] :
			value = 1
		elif concept == 1 and ((a[0] and not a[1] and (a[2] or a[3])) or (not a[0] and a[1] and a[2] and a[3])) :
			value = 1 
		elif concept == 2 and (a[0] and not a[1] and a[2] or (not a[0] and a[1] and a[2])) :
			value = 1
		elif concept == 3 and ((a[0] and not a[1] and a[2] and a[3]) or (not a[0] and a[1] and (a[2] or a[3]))) :
			value = 1
		elif concept == 4 and not a[0] and a[1] :
			value = 1
		
		if posonly == 0 :
			break
#	print concept, c, a
	return [str(value*2+1)] + [str((1+a[i]+2*(i+1))*2 - 1) for i in range(0,6)]
	
	
	
def main() :
	from string import join
	
	from sys import argv
	
	if argv[1] == "stagger" :
		
		for i in range(0,500) :
			print join(generate_stagger(1,1))
			
		for i in range(0,500) :
			print join(generate_stagger(2,1))
		
		for i in range(0,500) :
			print join(generate_stagger(3,1))
			
		for i in range(0,500) :
			print join(generate_stagger(1,1))
			
		for i in range(0,500) :
			print join(generate_stagger(2,1))
			
		for i in range(0,500) :
			print join(generate_stagger(3,1))
		
	elif argv[1] == "speed" :
		a = int(argv[2])
		b = int(argv[3])
		c = int(argv[4])
	
		for i in range(0,a) :
			print join(generate_speed(0,1))
			
		
		for i in range(0,b) :
			print join(generate_speed(float(i)/float(b),1))
			
		for i in range(0,c) :
			print join(generate_speed(1,1))
	elif argv[1] == "extent" :
	
		a = 100
		b = 100
		for i in range(0,a) :
			print join(generate_extent(0,1))
			
		for i in range(0,a) :
			print join(generate_extent(1,1))
		

if __name__ == "__main__" :
	main()