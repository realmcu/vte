#! /bin/env python
#
# --------------------------------------------------------------------------------------
#		ihex2bin.py
#
#  Description: Convert ihex file to binary file.
#  Version: 1.00 DEMO
#  Date:    02-08-2010
#  Author:  Terry Lu <Terry.Lui@gmail.com>
#
#  History:
#  02-08-2010 1.00  Convert ihex file to binary file.
# --------------------------------------------------------------------------------------

import sys
import os
import string
import getopt

def Print_Help():
	'''  Print how to use this script, ^_^ '''
	# print '\n'
	print '+--------------------------------------------------------------+'
	print 'Usage:\n'
	print ' ihex2bin.py -i <in-file> [-o <out-file>]'
	print ' -i Input filename'
	print ' -o Output filename'
	print ' -v Version Info'
	print ' -h Help'
	print '+--------------------------------------------------------------+'

def Print_Version():
	''' Print the version info, ^_^ '''
	print '+--------------------------------------------------------------+'
	print '			ihex2bin.py\n'
	print '+Description: Convert ihex file to binary file.'
	print '+Version: 0.01 DEMO'
	print '+Date:    02-08-2010'
	print '+Author:  Terry Lu'
	print '+--------------------------------------------------------------+'

class data_rec:
	def __init__(self, addr, data):
		if type(data) != type(""):
			raise Exception("data is not a string")
		self.addr = addr
		self.data = data

	def __repr__(self):
		return "%08x: %x" % (self.addr, len(self.data))

class ihex:
	TYPE_DATA = 0
	TYPE_EOF = 1
	TYPE_EXTENDED_SEGMENT_ADDR = 2
	TYPE_START_SEGMENT_ADDR = 3
	TYPE_EXTENDED_LINEAR_ADDR = 4
	TYPE_START_LINEAR_ADDR = 5

	def __init__(self, filename):
		fd = open(filename, "r")

		self.data = []

		self.start_addr = None

		base_addr = 0

		line_no = 0
		for line in fd:
			l = string.strip(line)
			line_no += 1
			if l[0] != ":":
				raise Exception("invalid initial character '%s' line %d",
						(l[0], line_no))
			if len(l[1:]) % 2 != 0:
				raise Exception("line length not a multiple of two: line %d" %
						line_no)

			(l, addr, type, data) = self.__line_parse(l[1:], line_no)

			if l != len(data):
				print "data length does not match length field %d" % line_no

			if type == self.TYPE_EOF:
				return

			if type == self.TYPE_EXTENDED_SEGMENT_ADDR:
				base_addr = 0x10 * self.__multi_val(data)

			if type == self.TYPE_EXTENDED_LINEAR_ADDR:
				base_addr = 0x10000 * self.__multi_val(data)

			if type == self.TYPE_START_SEGMENT_ADDR or \
				type == self.TYPE_START_LINEAR_ADDR:
				if self.start_addr:
					print "start address set twice: line %d" % line_no
				self.start_addr = self.__multi_val(data)

			if type == self.TYPE_DATA:
				self.data.append(data_rec(addr + base_addr,
						self.__intlist_tostr(data)))
		fd.close()

	def __line_parse(self, hexstring, line_no):
		tmp_list = []
		cksum = 0
		for i in range(0, len(hexstring), 2):
			val = int(hexstring[i : i + 2], 16)
			tmp_list.append(val)
			cksum += val

		cksum %= 0x100
		if cksum != 0:
			raise Exception("invalid checksum: line %d" % line_no)

		return (tmp_list[0], self.__multi_val(tmp_list[1 : 3]),
			tmp_list[3], tmp_list[4 : -1])

	def __intlist_tostr(self, list):
		strdata = ""
		for c in list:
			strdata += chr(c)
		return strdata

	def __multi_val(self, data):
		sum = 0
		for val in data:
			sum = sum * 0x100 + val
		return sum

	def __dump_data(self):
		for d in self.data:
			print d

	def __padding(self, fill, len):
		s = ""
		for i in range(0, len):
			s += fill
		return s

	def flatten(self, fill = chr(0xff)):
		sort_list = []
		for d in self.data:
			sort_list.append((d.addr, d.data))
		sort_list.sort(lambda x, y: cmp(x[0], y[0]))
		data = ""
		last_addr = sort_list[0][0]
		start_addr = last_addr
		for e in sort_list:
			addr = e[0]
			l = len(e[1])
			pad = addr - last_addr
			if pad < 0:
				raise Exception("overlapping sections in file")
			if pad > 0:
				data += self.__padding(fill, pad)
				l += pad
			data += e[1]
			last_addr += l
		return (start_addr, data)

class wv_bin(ihex):
	HEADER_LEN = 48
	def __init__(self, filename):
		ihex.__init__(self, filename)
		(self.base_addr, self.byte_data) = ihex.flatten(self)

	def __parse_hdr(self):
		hdr_data = self.byte_data[: self.HEADER_LEN]
		self.trc = ord(hdr_data[38])

	def get_data_offset(self):
		self.__parse_hdr()
		return (self.HEADER_LEN + self.trc + 1 + 1)

	def parse_data(self):
		return self.byte_data[self.get_data_offset() :]

def Main_Run(argv_list):
	'''
	Main app to run like main() in C, ^_^
	Option included:
		-v : Prints the version info
		-h : Display help info
	'''

	# The number of args
	argc = len(argv_list)

	# Error string
	error_string = ''
    
	# Input & output filename
	strInFile  = ''
	strOutFile = ''

	try:
		opts, args = getopt.getopt(sys.argv[1:], 'vhi:o:')
	except getopt.GetoptError:
		Print_Help()
		return 2
	
	for o, a in opts:
		if o == '-h':
			Print_Help()
			return 0
		if o == '-v':
			Print_Version()
			return 0
		if o == '-i':
			strInFile = a;
		if o == '-o':
			strOutFile = a;

	if strInFile == '':
		print "No input file specified!"
		return 1
	if strOutFile == '':
		strOutFile = ''.join((strInFile, '.data'))
	
	# Check if the input file is valid
	if not os.path.exists(strInFile):
		error_string = ''.join(('Can\'t find file: ', strInFile))
		print error_string
		return 1

	if os.path.isdir(strInFile):
		error_string = ''.join(('Directory, not file: ', strInFile))
		print error_string
		return 1
	
	wv_obj = wv_bin(strInFile)
	wv_data = wv_obj.parse_data()

	out_file_obj = open(strOutFile, 'wb')
	out_file_obj.write(wv_data)
	out_file_obj.close

# Run script
Main_Run(sys.argv)

# For test
#if __name__ == '__main__':
#	pass