## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import os

import qisys.worktree
import qisys.qixml
import qipkg.package


class PkgWorkTree(qisys.worktree.WorkTreeObserver):
    def __init__(self, worktree):
        self.worktree = worktree
        self.root = worktree.root
        self.packages = list()
        self._load_pkg_projects()
        worktree.register(self)

    def _load_pkg_projects(self):
        self.pkg_projects = list()
        for worktree_project in self.worktree.projects:
            pkg_project = new_pkg_project(self, worktree_project)
            if pkg_project:
                self.check_unique_name(pkg_project)
                self.packages.append(pkg_project)

    def on_project_added(self, project):
        """ Called when a new project has been registered """
        raise NotImplementedError

    def on_project_removed(self, project):
        """ Called when a build project has been removed """
        raise NotImplementedError

    def on_project_moved(self, project):
        """ Called when a build project has been moved """
        raise NotImplementedError

    def get_package(self, name, raises=False):
        for pkg in self.packages:
            if pkg.name == name:
                return pkg
        if raises:
            raise Exception("No such package project: %s" % name)
        else:
            return None

    def check_unique_name(self, new_package):
        project_with_same_name = self.get_package(new_package.name,
                                                  raises=False)
        if project_with_same_name:
            raise Exception("""\
Found two projects with the same name ({0})
In:
* {1}
* {2}
""".format(new_package.name,
               project_with_same_name.path,
               new_package.path))


def new_pkg_project(pkg_worktree, project):
    if not os.path.exists(project.qiproject_xml):
        return None
    tree = qisys.qixml.read(project.qiproject_xml)
    root = tree.getroot()
    if root.get("version") != "3":
        return None
    pkg_elem = root.find("package")
    if pkg_elem is None:
        return None
    name = pkg_elem.get("name")
    if not name:
        raise BadProjectConfig(project.qiproject_xml,
                               "Expecting a 'name' attribute")
    if not os.path.exists(os.path.join(project.path, "qipackage.xml")):
        return None
    return qipkg.package.make(project.path, "qipackage.xml")

class BadProjectConfig(Exception):
    def __str__(self):
        return """
Incorrect configuration detected for project in {0}
{1}
""".format(*self.args)
