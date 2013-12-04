## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Generate a binary package, ready to be used for a behavior """

import os
import qipkg.package
import qibuild.parsers
import qibuild.cmake_builder
import qilinguist.parsers


def get_pkg_from_args(args):
    build_worktree = qibuild.parsers.get_build_worktree(args)
    qilinguist_worktree = qilinguist.parsers.get_linguist_worktree(args)
    return qipkg.package.make(args.input, build_worktree, qilinguist_worktree)

