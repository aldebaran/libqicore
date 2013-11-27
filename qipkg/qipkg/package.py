## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

"""This package contains the PackageWorkTree object.
"""
import os
import qisys.qixml
from qisys import ui
import crgbuilder
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
        A package is a set of project, we use builders to build them.
        We use only one builder for each type of project.
    """

    def __init__(self, pml_path, build_config, name, builders):
        """ pml_path: the pml file used to build the package
            builders: a list of builders (cmake, crg, qidoc, ...)
        """
        self.pml_path       = pml_path
        self.pml_dir        = os.path.dirname(os.path.abspath(self.pml_path))
        self.name           = name
        self.builders       = builders
        self.build_config   = build_config

    def configure(self, *args, **kwargs):
        for builder in self.builders:
            builder.configure(*args, **kwargs)

    def build(self, *args, **kwargs):
        for builder in self.builders:
            builder.build(*args, **kwargs)

    def install(self, dest, listing=None):
        """ install the package's content into 'dest'
            return: the list of file installed
        """
        ui.debug("Installing inside:", dest)
        filelisting = list()
        for builder in self.builders:
            filelisting.extend(builder.install(dest))
        return filelisting

    def _cached_install(self):
        bdir = self.build_config.build_directory("build-pkg-%s" % self.name)
        install_dest = os.path.join(self.pml_dir, bdir)
        return (install_dest, self.install(install_dest))

    def package(self, dest):
        install_dest, files = self._cached_install()
        ui.info(ui.green, "Generating package:", ui.reset, dest)
        gen_package(dest, install_dest, files)

    def deploy(self, dest):
        install_dest, files = self._cached_install()
        ui.info(ui.green, "Deploying package to:", ui.reset, dest)
        raise NotImplementedError

def make(pmlfilename, cmake_builder):
    """ create Package from a PmlFile.
        Instanciate all Projects (crg, qibuild, ...) with correct arguments
    """
    builders = list()
    builders.append(crgbuilder.make(pmlfilename))
    builders.append(cmake_builder)

    if not os.path.isfile(pmlfilename):
        raise Exception("Package xml file not found: %s" % pmlfilename)
    pkg_path = os.path.dirname(pmlfilename)
    ui.debug("opening file: ", pmlfilename)
    root = qisys.qixml.read(pmlfilename).getroot()
    name = root.get("name")

    #populate cmake_builder
    qibuild_nodes = root.findall("qibuild")
    for qibuild_node in qibuild_nodes:
        qibname = qibuild_node.get("name")
        #create a cmake_builder for that project
        cmake_builder.add_project(qibname)

    return Package(pmlfilename, cmake_builder.build_worktree.build_config, name, builders)
