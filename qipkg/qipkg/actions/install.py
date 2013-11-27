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
import qibuild.parsers
import qipkg.worktree


def configure_parser(parser):
    """Configure parser for this action"""
    qisys.parsers.worktree_parser(parser)
    qibuild.parsers.project_parser(parser)
    group = parser.add_argument_group("Install options")
    group.add_argument("output", help="directory destination")


def do(args):
    """Main entry point"""

    if not os.path.isdir(args.output):
        raise Exception("Destination is not a folder: %s" % args.output)
    pkg_worktree = qipkg.parsers.get_pkg_worktree(args)

    topackage = qipkg.parsers.get_pkg_projects(pkg_worktree, args)

    for tp in topackage:
        ui.info(ui.green, "Generating package for:", ui.reset, tp.name)
        tp.do_install(args.output)
