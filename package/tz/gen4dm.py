#!/usr/bin/env python
# -*- coding: utf-8 -*-
# билд не имеет на борту python3


from __future__ import print_function

import sys
#from enum import Enum
import re
import json

#class LineType(Enum):
class LineType:
	Empty = 0
	Comment = 1
	ZoneHeader = 2
	Rule = 3
	Link = 4
	Other = 99

def GetLineType(line):
	rip = line.lstrip()
	if len(rip)==0:
		return LineType.Empty # вот такого быть не должно в *.zi
	if rip[0]=='#':
		return LineType.Comment
	token = rip.split()[0]
	if token == "Z":
		return LineType.ZoneHeader
	if token == "R":
		return LineType.Rule
	if token == "Li":
		return LineType.Link
	if token == "L":
		return LineType.Link
	return LineType.Other


def dlog(*msg):
	print(*msg, file=sys.stderr)
def info(*msg):
	dlog(*msg)
def dbg(*msg):
	dlog(*msg)
def fatal(*msg):
	dlog("FATAL:", *msg)
	raise

def out_zone(Names, Data, header, last):
	hws = header.split()
	name = hws[1]
	gmtoff = hws[2] if header == last else last.split()[0]

	#print("name = %s, offset=%s"%(name, gmtoff))
	if re.match('^[-+]?[0-9]{1,2}$', gmtoff): # просто час, типа "9" или "-14"
		off_in_seconds = int(gmtoff) * 3600
	elif re.match('^[-+]?[0-9]{1,2}:[0-9]{2}$', gmtoff): # напр. "-3:30"
		gmtoff = gmtoff.split(':')
		off_in_seconds = int(gmtoff[0]) * 3600
		if off_in_seconds >= 0:
			off_in_seconds += int(gmtoff[1]) * 60
		else:
			off_in_seconds -= int(gmtoff[1]) * 60
	else:
		fatal("GMT offset %s was not parsed for %s"%(gmtoff, name))
		raise
	Names.append(name)
#	Data[name] = {"Offset": off_in_seconds, "Links": []}
	Data[name] = {"Offset": off_in_seconds}

def out_link(Links, Data, line):
	line = line.split()
	zone = line[1]
	link = line[2]
	#print(Data[zone])
	if "Links" not in Data[zone]:
		Data[zone]["Links"] = []
	Data[zone]["Links"].append(link)
	Links.append(link)

def process_zone_file(Names, Links, Data, filename):
	info("Processing %s..."%filename)
	#with open(filename) as f:
	f = open(filename);
	in_zone = False
	zone_header = ''
	prevline = ''
	for line in f:
		lt = GetLineType(line)
		if lt == LineType.Comment:
			continue # просто не замечаем комментариев
		if in_zone and lt != LineType.Other:
			# пред. строка была последней в текущей зоне
			out_zone(Names, Data, zone_header, prevline)
			in_zone = False
		if lt == LineType.ZoneHeader:
			zone_header = line
			in_zone = True
#		if lt == LineType.Link:
#			out_link(Links, Data, line)
		prevline = line

	f.seek(0)
	for line in f:
		lt = GetLineType(line)
		if lt == LineType.Link:
			out_link(Links, Data, line)
		prevline = line
	f.close()

def collect_zones(files):
	data = {}
	names = []
	links = []
	for filename in files:
		process_zone_file(names, links, data, filename)
	names.sort()
	links.sort()
	return {"Zones": names, "Links": links, "Data": data}

json.dump(collect_zones(sys.argv[1:]), sys.stdout, sort_keys=True, indent=4)
