#!/usr/bin/env python
"""
randomize.py - Randomize a File
Author: Sean B. Palmer, inamidst.com
"""

import sys, random
from optparse import OptionParser

def randomize(input, output): 
   if input in ('-', '/dev/stdin'): 
      i = sys.stdin
   else: i = open(input)
   lines = list(i)
   if i is not sys.stdin: 
      i.close()

   if output in ('-', '/dev/stdout'): 
      o = sys.stdout
   else: o = open(output, 'w')
   while lines: 
      line = random.choice(lines)
      o.write(line)
      lines.remove(line)
   if o is not sys.stdout: 
      o.close()

def main(): 
   parser = OptionParser(usage="%prog [options] <input>")
   parser.add_option("-i", "--inplace", dest="inplace", action="store_true", 
                     default=False, help="randomize file in-place")
   parser.add_option("-o", "--output", dest="output", default=False, 
                     help="output filename", metavar="FN")
   options, args = parser.parse_args()

   if options.inplace and options.output: 
      parser.error("-i and -o are mutually exclusive")

   if len(args) > 1: 
      parser.error("Maximum of one input files")

   if args: 
      input = args[0]
   else: input = '-'

   if options.output: 
      output = options.output
   elif options.inplace: 
      if input in ('-', '/dev/stdin'): 
         parser.error("Can't use in-place with stdin")
      output = input
   else: output = '-'

   randomize(input, output)

if __name__=="__main__": 
   main()
