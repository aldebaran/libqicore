## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

"""This package contains the PackageWorkTree object.
"""
import os
import qisys.qixml
from qisys import ui
import crgproject
import qibuildproject
import qibuild.parsers
import zipfile

def gen_package(output_file, basedir, files):
    outdir = os.path.dirname(os.path.abspath(output_file))
    if not os.path.isdir(outdir):
        raise Exception("Destination folder do not exists: " % outdir)

    archive = zipfile.ZipFile(output_file, "w", zipfile.ZIP_DEFLATED)

    for f in files:
        arcname = os.path.relpath(f, basedir)
        archive.write(f, arcname)
    archive.close()


class Package(object):
    """ Wrap operation related to a Package.
        A package is a set of project, just call the action
        on the subproject as appropriated
    """

    def __init__(self, pml_path, build_worktree, name, projects):
        """ pml_path: the pml file used to build the package
            projects: a list of projects
        """
        self.pml_path       = pml_path
        self.pml_dir        = os.path.dirname(os.path.abspath(self.pml_path))
        self.name           = name
        self.projects       = projects
        self.build_worktree = build_worktree

    def configure(self):
        raise NotImplementedError

    def make(self):
        raise NotImplementedError

    def install(self, dest, listing=None):
        """ install the package's content into 'dest'
            return: the list of file installed
        """
        filelisting = list()
        for p in self.projects:
            filelisting.extend(p.install(dest))
        return filelisting

    def get_build_dir(self, ):
        pass

    def package(self, dest):
        bdir = self.build_worktree.build_config.build_directory("build-pkg-%s" % self.name)
        install_dest = os.path.join(self.pml_dir, bdir)
        print "Install inside:", install_dest

        files = list()
        for p in self.projects:
            files.extend(p.install(install_dest))
        ui.info(ui.green, "Generating package:", ui.reset, dest)
        gen_package(dest, install_dest, files)

    def deploy(self, dest):
        raise NotImplementedError

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


def make(pmlfilename, args):
    """ create Package from a PmlFile.
        Instanciate all Projects (crg, qibuild, ...) with correct arguments
    """
    projects = list()
    projects.append(crgproject.make(pmlfilename))

    if not os.path.isfile(pmlfilename):
        raise Exception("Package xml file not found: %s" % pmlfilename)
    pkg_path = os.path.dirname(pmlfilename)
    ui.debug("opening file: ", pmlfilename)
    root = qisys.qixml.read(pmlfilename).getroot()
    name = root.get("name")

    #needed for qibuild projects
    build_worktree = qibuild.parsers.get_build_worktree(args)

    qibuild_nodes = root.findall("qibuild")
    for qibuild_node in qibuild_nodes:
        qibname = qibuild_node.get("name")
        #create a cmake_builder for that project
        build_projects = [ build_worktree.get_build_project(qibname) ]
        cmake_builder = qibuild.cmake_builder.CMakeBuilder(build_worktree, build_projects)
        projects.append(qibuildproject.make(qibname, cmake_builder))

    return Package(pmlfilename, build_worktree, name, projects)
