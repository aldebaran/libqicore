## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

from qisys import ui
import qisys.parsers
from qipkg.worktree import PkgWorkTree, new_pkg_project

def get_pkg_worktree(args):
    worktree = qisys.parsers.get_worktree(args)
    pkg_worktree =  PkgWorkTree(worktree)
    ui.debug(ui.green, "Current worktree:", ui.reset,
             ui.bold, pkg_worktree.root)
    return pkg_worktree

def get_pkg_projects(pkg_worktree, args, default_all=False):
    parser = PackageProjectParser(pkg_worktree)
    return parser.parse_args(args, default_all=default_all)

class PackageProjectParser(qisys.parsers.AbstractProjectParser):
    """ Implements AbstractProjectParser for a PackageWorkTree """

    def __init__(self, pkg_worktree):
        self.pkg_worktree = pkg_worktree
        self.pkg_projects = pkg_worktree.packages

    def all_projects(self, args):
        return self.pkg_projects

    def parse_no_project(self, args):
        """ Try to find the closest worktree project that
        matches the current directory

        """
        worktree = self.pkg_worktree.worktree
        parser = qisys.parsers.WorkTreeProjectParser(worktree)
        worktree_projects = parser.parse_no_project(args)
        if not worktree_projects:
            raise CouldNotGuessProjectName()

        # WorkTreeProjectParser returns None or a list of one element
        worktree_project = worktree_projects[0]
        pkg_project = new_pkg_project(self.pkg_worktree, worktree_project)
        if not pkg_project:
            raise CouldNotGuessProjectName()

        return self.parse_one_project(args, pkg_project.name)

    def parse_one_project(self, args, project_arg):
        """ Get one pkg project given its name """

        project = self.pkg_worktree.get_package(project_arg, raises=True)
        return [project]

class CouldNotGuessProjectName(Exception):
    def __str__(self):
        return """
Could not guess package project name from current working directory
Please go inside a package project, or specify the project name
on the command line
"""
