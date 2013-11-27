## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Install a binary package on the robot

    This will copy the content of the package into the output folder
"""

from qisys import ui
import qipkg.parsers
import qibuild.parsers


def configure_parser(parser):
    """Configure parser for this action"""
    qibuild.parsers.build_parser(parser)
    group = parser.add_argument_group("Install options")
    group.add_argument("input", help=".pml package xml input file")
    group.set_defaults(listing=None)


def do(args):
    """Main entry point"""
    pkg = qipkg.parsers.get_pkg_from_args(args)

    ui.info(ui.green, "Building package", ui.reset, pkg.name)
    pkg.build()
