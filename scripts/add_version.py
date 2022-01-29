#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys

if len(sys.argv) < 3:
	#raise Exception("Must be 2 argument")
	print("Must be 2 and only 2 argument!")
	sys.exit(1)

f1 = open(sys.argv[1], "r")

data = f1.read()

f2 = open(sys.argv[2], "a")
f2.write("DATAM")
f2.write(data.strip())
f2.write("DATAM")
f1.close()
f2.close()


sys.exit(0)
