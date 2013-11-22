## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

"""This package contains the PackageWorkTree object.
"""
import os
import qisys.qixml
from qisys import ui
import crgproject

class Package(object):
    """ Wrap operation related to a Package.
        A package is a set of project, just call the action
        on the subproject as appropriated
    """

    def __init__(self, pml_path, name, projects):
        """ pml_path: the pml file used to build the package
            projects: a list of projects
        """
        self.pml_path = pml_path
        self.name     = name
        self.projects = projects

    def configure(self):
        raise NotImplementedError

    def make(self):
        raise NotImplementedError

    def install(self, dest):
        for p in self.projects:
            p.install(dest)

    def package(self, dest):
        for p in self.projects:
            p.package(dest)

class DummyProject(object):
    """ the interface of a project
    """
    def __init__(self, name):
        self.name = name

    def configure(self):
        raise NotImplementedError

    def make(self):
        raise NotImplementedError

    def install(self, dest):
        raise NotImplementedError

    def package(self, dest):
        raise NotImplementedError

def make(pmlfilename):
    projects = list()
    projects.append(crgproject.make(pmlfilename))

    if not os.path.isfile(pmlfilename):
        raise Exception("Package xml file not found: %s" % pmlfilename)
    pkg_path = os.path.dirname(pmlfilename)
    ui.debug("opening file: ", pmlfilename)
    root = qisys.qixml.read(pmlfilename).getroot()
    name = root.get("name")

    return Package(pmlfilename, name, projects)
