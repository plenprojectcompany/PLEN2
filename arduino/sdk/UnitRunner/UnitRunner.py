# -*- coding: utf-8 -*-

import json
import locale
import time
import subprocess
from datetime import datetime
from argparse import ArgumentParser

import serial
import serial.tools.list_ports


# Set global settings.
# =============================================================================
locale.setlocale(locale.LC_ALL, 'jpn')


# Set up and build *.ino.
# =============================================================================
def build():
    setup_proc = subprocess.Popen(
        [
            'CppImport'
        ]
    )
    setup_proc.wait()

    build_proc = subprocess.Popen(
        [
            'Builder'
        ],
        stdout = subprocess.PIPE,
        stderr = subprocess.STDOUT
    )
    build_proc.wait()

    teardown_proc = subprocess.Popen(
        [
            'CppImport', '-d'
        ]
    )
    teardown_proc.wait()

    result = build_proc.stdout.read()

    return ('compilation terminated.' not in result)


# Run and assert *.ino.
# =============================================================================
def test():
    for DEVICE in list(serial.tools.list_ports.comports()):
        if 'Arduino Micro' in DEVICE[1]:
            COM = DEVICE[0]

    if not 'COM' in locals():
        return False

    usb_serial = serial.Serial(port=COM, baudrate=2000000, timeout=1)

    result = ''

    while (True):
        if (usb_serial.in_waiting > 0):
            result += usb_serial.read(usb_serial.in_waiting)

            if ('Test summary:' in result):
                while (result[-1] != '\n'):
                    result += usb_serial.read(usb_serial.in_waiting)

                    time.sleep(0.01)

                break

        time.sleep(0.01)

    print result

    return ('0 failed' in result)


# Application entry point.
# =============================================================================
def main(args):
    with open('status.json', 'w') as fout:
        today     = datetime.today()
        today_str = '%04d/%02d/%02d' % (today.year, today.month, today.day)
        status    = {}

        build_status = build()
        test_status  = False

        status['build'] = {
            'status': build_status,
            'last': today_str if build_status else None
        }

        if (build_status and not args.buildonly):
            test_status = test()

        status['test'] = {
            'status': test_status,
            'last': today_str if test_status else None
        }

        print(status)

        json.dump(status, fout, sort_keys = True, indent = 4)


# Purse command-line option(s).
# =============================================================================
if __name__ == '__main__':
    arg_parser = ArgumentParser()

    arg_parser.add_argument(
        '-b', '--buildonly',
        action  = 'store_true',
        dest    = 'buildonly',
        default = False,
        help    = 'Run the application as build only mode.'
    )

    args = arg_parser.parse_args()
    main(args)
