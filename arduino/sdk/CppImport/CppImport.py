# -*- coding: utf-8 -*-

import sys
import os
import shutil
import json


fin = open('dependencies.json', 'r')
dependency = json.load(fin)

if (len(sys.argv) == 2):
	if (sys.argv[1] == '-d'):
		for file in dependency['import']:
			if os.path.isfile(file + '.h'):
				os.remove(file + '.h')

			if os.path.isfile(file + '.cpp'):
				os.remove(file + '.cpp')

		fin.close()
		sys.exit()

for file in dependency['import']:
	if os.path.isfile(dependency['root'] + file + '.h'):
		shutil.copy2(dependency['root'] + file + '.h', './')

	if os.path.isfile(dependency['root'] + file + '.cpp'):
		shutil.copy2(dependency['root'] + file + '.cpp', './')

fin.close()