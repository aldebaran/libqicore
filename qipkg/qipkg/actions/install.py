## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Install a binary package on the robot

    This will copy the content of the package into the output folder
"""

import os

from qisys import ui
import qisys.sh
import qisys.parsers
import qipkg.parsers
import qipkg.package
import qibuild.parsers

def _generate_listing(output_file, filelisting):
    with open(output_file, "w") as f:
        for fname in filelisting:
            f.write("%s\n" % fname)

def configure_parser(parser):
    """Configure parser for this action"""
    qibuild.parsers.build_parser(parser)
    group = parser.add_argument_group("Install options")
    group.add_argument("input", help=".pml package xml input file")
    group.add_argument("output", help="directory destination")
    group.add_argument("--listing", help="generate a listing of all installed files to the specified file")
    group.set_defaults(listing=None)

def do(args):
    """Main entry point"""

    pkg = qipkg.package.make(args.input, args)

    ui.info(ui.green, "Installing package", ui.reset, pkg.name, ui.green, "into", ui.reset, args.output)
    filelisting = pkg.install(args.output)
    if args.listing:
        _generate_listing(args.listing, filelisting)
