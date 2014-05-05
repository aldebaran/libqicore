## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Deploy a complete package on the robot. This use rsync to be fast
"""

import os

from qisys import ui
import qisys.sh
import qisys.parsers
import qipkg.package


def configure_parser(parser):
    """Configure parser for this action"""
    qisys.parsers.deploy_parser(parser)



def do(args):
    """Main entry point"""
    urls = qisys.parsers.get_deploy_urls(args)
