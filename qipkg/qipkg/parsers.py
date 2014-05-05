## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Generate a binary package, ready to be used for a behavior """

import qisys.parsers
import qipkg.builder

def pml_parser(parser):
    qisys.parsers.worktree_parser(parser)
    parser.add_argument("pml_path")

def get_pml_builder(args):
    pml_path = args.pml_path
    worktree = qisys.parsers.get_worktree(args)
    return qipkg.builder.PMLBuider(worktree, pml_path)

