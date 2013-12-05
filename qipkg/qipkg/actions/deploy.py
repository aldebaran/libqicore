## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Deploy a complete package on the robot. This use rsync to be fast
"""

import os

from qisys import ui
import qisys.sh
import qibuild.parsers
import qipkg.package


def configure_parser(parser):
    """Configure parser for this action"""
    qibuild.parsers.build_parser(parser)
    group = parser.add_argument_group("qipkg options")
    group.add_argument("input", help=".pml package xml input file")
    qibuild.parsers.deploy_parser(parser)



def do(args):
    """Main entry point"""
    urls = qibuild.parsers.get_deploy_urls(args)
    pkg = qipkg.parsers.get_pkg_from_args(args)

    for url in urls:
        ui.info(ui.green, "Deploying package", ui.reset, pkg.name, ui.green, "into", ui.reset, url)
        pkg.deploy(url)
