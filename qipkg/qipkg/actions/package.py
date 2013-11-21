## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Generate a binary package, ready to be used for a behavior """

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
    group = parser.add_argument_group("Package build options")
    group.add_argument("input", help=".pml package xml input file")
    group.add_argument("output", help=".pkg package output file")

def do(args):
    """Main entry point"""

    pkg = qipkg.package.make(args.input)

    ui.info(ui.green, "Generating package for:", ui.reset, pkg.name)
    pkg.do_package(args.output)
