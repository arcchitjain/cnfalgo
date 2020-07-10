#! /usr/bin/env python


def main() :
	
	from sys import argv, stderr
	import os.path
	from os import listdir

	if len(argv) < 2 :
		sys.exit(2)
	
	fn = argv[1]
	
	if not os.path.exists(fn) :
		sys.exit(2)
	
	if os.path.isdir(fn) :
		files = [ os.path.join(fn,x) for x in listdir(fn) if x.endswith(".cpp") or x.endswith(".h") ]
	else :
		files = [fn]
	
	dict = {}
	for f in files :
		dict = get_deps_recursive(f,dict)
		
	for x in sorted(dict.keys())  :
		if not x in files :
			print x

def get_deps_recursive(filename,dict = {}) :
	from os.path import exists
	if not exists(filename) :
		return dict
	
	if filename.endswith('.h'):
		dict = get_deps_recursive(filename[:-2] + '.cpp',dict)

	if not filename in dict :
		dict[filename] = get_deps(filename)
		for f in dict[filename] :
			dict = get_deps_recursive(f,dict)
				
	return dict

def get_deps(filename) :
	from os.path import join, dirname, normpath
	from sys import stderr
	try :
		f = open(filename)
		deps = [ normpath(join(dirname(filename),l.strip()[10:-1])) for l in f if l.strip().startswith('#include "') ] 
		f.close()
		return deps
	except IOError :
		print >> stderr, "Error reading file ", filename
		return []
	


if __name__ == "__main__" :
	main()