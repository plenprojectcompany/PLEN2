# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import serial, serial.tools.list_ports


# Set global settings.
# ==============================================================================
ARDUINO = r'C:\workdir\app\Arduino\arduino.exe'
BOARD   = r'arduino:avr:micro'


# Application entry point.
# ==============================================================================
def main():
	for DEVICE in list(serial.tools.list_ports.comports()):
		if u'Arduino Micro' in DEVICE[1]:
			COM = DEVICE[0]

	if not 'COM' in locals():
		print 'PLEN has not been connecting with USB cable.'
		sys.exit()

	SKETCH = filter(lambda f:f.endswith('.ino'), os.listdir('./'))

	if len(SKETCH) == 0:
		print 'There is no sketch in the directory.'
		sys.exit()

	proc = subprocess.Popen(
		[
			ARDUINO,
			'--port',   COM,
			'--board',  BOARD,
			'--upload', os.path.realpath(SKETCH[0])
		],
		stdout = subprocess.PIPE,
		stderr = subprocess.STDOUT
	)

	proc.wait()

	return proc.stdout.read().decode('shift-jis').encode('utf-8')


# Purse command-line option(s).
# ==============================================================================
if __name__ == '__main__':
	print main()