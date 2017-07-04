import sys
import os
from collections import OrderedDict

args = sys.argv

script_path = os.path.dirname(args[0])
os.chdir(script_path)

project_root_path = '../'
os.chdir(project_root_path)

ignore_list = ['build', 'Makefile', 'Makefile.config', 'README.md', '.git', '.gitignore']

line_count_dict = {}

def count_code_line_recursively(current_dir):
	for d in os.listdir(current_dir):
		full_dir = os.path.join(current_dir, d)
		if full_dir not in map(lambda k: os.path.join('.', k), ignore_list):
			if os.path.isfile(full_dir):
				with open(full_dir, 'r') as f:
					for l in f:
						if len(l) > 1:
							if full_dir not in line_count_dict:
								line_count_dict[full_dir] = 0
							line_count_dict[full_dir] += 1
			elif os.path.isdir(full_dir):
				count_code_line_recursively(full_dir)
			else:
				raise ValueError('dir? file? %s' % full_dir)

count_code_line_recursively('.')

total_line_number = 0
for k in OrderedDict(sorted(line_count_dict.items(), key=lambda x: x[0])):
	total_line_number += line_count_dict[k]
	print "{0:40}: {1:5}".format(k[2:], line_count_dict[k])
print "================================================="
print "Total code line number: %s" % total_line_number

