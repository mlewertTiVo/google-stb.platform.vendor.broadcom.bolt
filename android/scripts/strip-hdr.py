#!/usr/bin/env python
# this file is used to strip out all registers definitions except
# the ones we specified in this file.
import os, sys, re
from subprocess import call, check_output, check_call, STDOUT

# Tips: set to '1' to debug this script
verbose=0

# Tips: Update the dictionary as we need to include additional RDB header files
rdb_hdr_file_dict = {}
rdb_hdr_file_dict["bchp_common.h"] = {"BCHP_PHYSICAL_OFFSET","BCHP_WKTMR_REG_START"}
rdb_hdr_file_dict["bchp_aon_ctrl.h"] = {"BCHP_AON_CTRL_SYSTEM_DATA_RAMi_ARRAY_BASE"}

h_file_iterator_pattern = r"""
	(?P<file_hdr_content>
		/\*{20,}		#C comment start
		(?P<lic_content>.+?
		)
		\*{20,}/		#C comment end
	)
	(?:\n{1,})			#Empty line right after file header
	(?P<start_marker>^\#ifndef[ ]+[\w*]+\n\#define[ ]+[\w*]+)
	(?P<const_content>.*)
#	(?P<end_marker>^\#endif.*?\n{1,}.*End of File.*?$)
	"""

h_file_iterator_regex = re.compile(h_file_iterator_pattern, re.DOTALL | re.MULTILINE | re.VERBOSE)


def strip_hdr_usage():
	print 'usage: strip-hdr.py <inc-path-to-rdb-headers>'
	print 'inc-path-to-rdb-headers - relative path to RDB generated header files'
	print ' e.g. strip-hdr.py android/boltinc/include/7445d0'
	sys.exit(0)

input = len(sys.argv)
if input <= 1:
	strip_hdr_usage()

hdr_file_path = sys.argv[1]

# In the dictionary, key == header-file, values == register-list-to-keep
for key, value in rdb_hdr_file_dict.items():
	infilename = os.path.join(hdr_file_path, key)
	in_fd = open(infilename, 'r')
	outfilename = os.path.join(hdr_file_path, key + '.tmp.out')
	out_fd = open(outfilename, 'w+')
	if verbose:
		print 'open input file: %s' % infilename
		print 'open output file: %s' % outfilename
		print 'registers to include: %s' % list(value)

	content = in_fd.read()
	in_fd.close()

	# start to search input file to create output file content
	new_content = h_file_iterator_regex.search(content)

	out_fd.write(new_content.group('file_hdr_content'))
	out_fd.write("\n")
	out_fd.write(new_content.group('start_marker'))
	out_fd.write("\n\n")

	for reg in value:
		reg_pattern = r"""
				(?P<reg_define>^\#define[ ]%s.*?$)
				""" % reg
		if verbose:
			print reg_pattern
		reg_regex = re.compile(reg_pattern, re.DOTALL | re.MULTILINE | re.VERBOSE)
		reg_content = reg_regex.search(new_content.group('const_content'))
		if reg_content != None:
			out_fd.write(reg_content.group('reg_define') + "\n")
			out_fd.write("\n")
		else:
			print "Register %s could not be found in file %s" % (reg, infilename)

	out_fd.write("#endif\n/* End of File */\n")
	out_fd.close()

	# remove origin file and use the new output file
	rm_org_file_cmd = 'bash -c "rm %s"' % infilename
	if verbose:
		print rm_org_file_cmd
	check_call(rm_org_file_cmd, shell=True)

	mv_new_file_cmd = 'bash -c "mv %s %s"' % (outfilename, infilename)
	if verbose:
		print mv_new_file_cmd
	check_call(mv_new_file_cmd, shell=True)

