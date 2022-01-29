import os
import shutil

FILE_CHUNK = 4 * 1024

os_environments = [
	'TARGET_ARCH_DIR',
	'BINARIES_DIR',
	'PROFILE_DIR',
	'HOST_DIR',
	'PROFILE_FILE',
	'PARTS_FILE',
	'LINUX_CONFIG',
	'LINUX26_IMAGE_NAME',
	'FW_IMAGE_NAME',
	'SIGN_PROGRAM',
	'SIGN_FILE',
]

def import_environments():
	for e in os_environments:
		try:
			globals()[''.join(e)] = os.environ[e]
		except:
			print('can\'t load \"' + e + '\" env')


# round file to block_size boundary minus tail
def round_file(path, block_size, tail):
	size = os.stat(path).st_size

	fp = open(path, 'r+b')

	nblock = size / block_size

	if size % block_size != 0:
		nblock = nblock + 1;

	if nblock * block_size - size < tail:
		nblock = nblock + 1;

	target_size = nblock * block_size - tail;

	if target_size - size > 0:
		fp.seek(target_size - 1)
		fp.write('\0');

	fp.close()


def read_config(path):
	fp = open(path, 'r')

	lines = fp.read().splitlines()
	for line in lines:
		line.strip()

	d = {}

	for line in lines:
		if len(line) != 0 and line[0] != '#':
			name = line[0 : line.find('=')]
			val = line[line.find('=') + 1 : len(line)]

			d.update({name: val.strip('"')})

	fp.close

	return d


def get_img_name(name):
	fw_image_prefix = FW_IMAGE_NAME

	if FW_IMAGE_NAME[len(FW_IMAGE_NAME) - 4 : len(FW_IMAGE_NAME)] == '.bin':
		fw_image_prefix = FW_IMAGE_NAME[0 : len(FW_IMAGE_NAME) - 4]

	return fw_image_prefix + '[' + name + ']' + '.bin'


def touch_empty(name):
	f = open(name, 'w+')
	f.close()


def cat_file(src, dst):
	fsrc = open(src, 'rb')
	fdst = open(dst, 'ab')

	shutil.copyfileobj(fsrc, fdst, FILE_CHUNK)

	fsrc.close()
	fdst.close()

import_environments()
dev_profile = read_config(PROFILE_FILE)
krn_config = read_config(LINUX_CONFIG)
