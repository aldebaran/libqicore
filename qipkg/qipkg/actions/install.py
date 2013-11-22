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


def configure_parser(parser):
    """Configure parser for this action"""
    qisys.parsers.worktree_parser(parser)
    #qibuild.parsers.project_parser(parser)
    group = parser.add_argument_group("Install options")
    group.add_argument("input", help=".pml package xml input file")
    group.add_argument("output", help="directory destination")


def do(args):
    """Main entry point"""

    pkg = qipkg.package.make(args.input)

    ui.info(ui.green, "Installing package", ui.reset, pkg.name, ui.green, "into", ui.reset, args.output)
    pkg.install(args.output)
