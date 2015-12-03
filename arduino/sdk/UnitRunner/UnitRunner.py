# -*- coding: utf-8 -*-

import json, locale
from datetime import datetime
from argparse import ArgumentParser


# Set global settings.
# ==============================================================================
locale.setlocale(locale.LC_ALL, 'jpn')


# Application entry point.
# ==============================================================================
def main(args):
	with open('status.json', 'w') as fout:
		today     = datetime.today()
		today_str = '%04d/%02d/%02d' % (today.year, today.month, today.day)

		status = {}

		status['test'] = {
			'status': args.test,
			'last': today_str if args.test else None
		}

		status['build'] = {
			'status': args.build,
			'last': today_str if args.build else None
		}

		json.dump(status, fout, sort_keys = True, indent = 4)


# Purse command-line option(s).
# ==============================================================================
if __name__ == '__main__':
	arg_parser = ArgumentParser()

	arg_parser.add_argument(
		'-b', '--build',
		action  = 'store_true',
		dest    = 'build',
		default = False,
		help    = 'Set to build "True".'
	)

	arg_parser.add_argument(
		'-t', '--test',
		action  = 'store_true',
		dest    = 'test',
		default = False,
		help    = 'Set to test "True".'
	)

	args = arg_parser.parse_args()
	main(args)