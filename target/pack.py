#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import shutil
import json

from pack_misc import *


def read_partsfile():
	global parts

	fp = open(PARTS_FILE, 'r')
	try:
		parts = json.load(fp)
	except:
		print('error in part file')
		sys.exit(-1)
	finally:
		fp.close()


def dlink_pack(part_j, out_file):

	if p.get('source') == None:
		return

	attr_file = BINARIES_DIR + '/' + p.get('name') + '.attr'
	attrfp = open(attr_file, 'w+');

	if p.get('attr') != None:
		for a in p.get('attr'):
			attrfp.write(a + '\n')

	attrfp.close()

	in_file = BINARIES_DIR + '/' + p.get('source')

	# dlinkhdr
	os.system(HOST_DIR + '/usr/bin/dlinkhdr' + ' -i ' + in_file + ' -o ' + out_file + ' -a ' + attr_file);

	os.unlink(attr_file)


#**********************

read_partsfile()

ff_tmp_name = BINARIES_DIR + '/' + 'tmp_full.bin'
touch_empty(ff_tmp_name);

# первым обрабатываем arch прошивку
arch_name = ''
for p in parts:
	if p.get('arch') == True:
		# копируем arch прошивку в полный образ
		cat_file(BINARIES_DIR + '/' + FW_IMAGE_NAME, ff_tmp_name)
		arch_name = p.get('name')


# затем все dlinkhdr
dlinkhdr_cnt = 0
for p in parts:
	if p.get('arch') == True:
		continue

	part_name = BINARIES_DIR + '/' + get_img_name(p.get('name'))

	# создаем файл в формате dlinkhdr
	dlink_pack(p, part_name)

	# копируем созданный файл в полный образ
	cat_file(part_name, ff_tmp_name)

	# подписываем кусок в формате dlinkhdr с помощью d-link sign (0xfec0ffee)
	# после копирования, т.к. в ff образе не нужно подписывать каждую часть,
	# у ff есть общая подпись
	os.system(SIGN_PROGRAM + ' signfe ' + part_name + ' ' + SIGN_FILE)

	dlinkhdr_cnt = dlinkhdr_cnt + 1


# подписываем полный образ прошивки с помощью d-link sign (0x00c0ffee)
os.system(SIGN_PROGRAM + ' sign ' + ff_tmp_name + ' ' + SIGN_FILE)

if arch_name == '': # если в файле разделов не было информации об arch прошивке, то удаляем ее
	os.unlink(BINARIES_DIR + '/' + FW_IMAGE_NAME);

if dlinkhdr_cnt > 0:
	if arch_name != '': # переименовываем arch прошивку, если она была
		os.rename(BINARIES_DIR + '/' + FW_IMAGE_NAME, BINARIES_DIR + '/' + get_img_name(arch_name))
	os.rename(ff_tmp_name, BINARIES_DIR + '/' + FW_IMAGE_NAME)
else:
	os.unlink(ff_tmp_name)

sys.exit(0)

