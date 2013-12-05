## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

"""This package contains the PackageWorkTree object.
"""
import os
import qisys.qixml
from qisys import ui
from . import crgbuilder
import qibuild.parsers
import qibuild.deploy
import qilinguist.builder
import zipfile

def gen_package(output_file, basedir, files):
    ui.info(ui.green, "Creating package", ui.reset, output_file)
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

    def __init__(self, pml_path, build_config, name, builders, install_in_subdir=False):
        """ pml_path: the pml file used to build the package
            builders: a list of builders (cmake, crg, qidoc, ...)
        """
        self.pml_path       = pml_path
        self.pml_dir        = os.path.dirname(os.path.abspath(self.pml_path))
        self.name           = name
        self.builders       = builders
        self.build_config   = build_config
        self.install_in_subdir = install_in_subdir

    def configure(self, *args, **kwargs):
        for builder in self.builders:
            builder.configure(*args, **kwargs)

    def build(self, *args, **kwargs):
        for builder in self.builders:
            builder.build(*args, **kwargs)

    def install(self, dest, *args, **kwargs):
        """ install the package's content into 'dest'
            return: the list of file installed
        """
        if self.install_in_subdir:
            dest = os.path.join(dest, self.name)
        ui.debug("Installing inside:", dest)
        filelisting = list()
        for builder in self.builders:
            flisting = builder.install(dest, *args, **kwargs)
            if self.install_in_subdir:
                flisting.extend([os.path.join(self.name, x) for x in flisting])
            else:
                filelisting.extend(flisting)
        return filelisting

    def _cached_install(self):
        bdir = self.build_config.build_directory("build-pkg-%s" % self.name)
        install_dest = os.path.join(self.pml_dir, bdir, "sdk")
        return (install_dest, self.install(install_dest))

    def package(self, dest):
        install_dest, files = self._cached_install()
        gen_package(dest, install_dest, files)

    def deploy(self, url):
        install_dest, files = self._cached_install()
        bdir = self.build_config.build_directory("build-pkg-%s" % self.name)
        install_man = os.path.join(self.pml_dir, bdir, "install_manifest.txt")
        with open(install_man, "w") as f:
            f.writelines([ "%s\n" % x for x in files])
        qibuild.deploy.deploy(install_dest, url, filelist=install_man)

#bah oui!
MetaPackage = Package

def _parse_metapackage(pmlfilename, build_worktree, linguist_worktree):
    packages = list()

    if not os.path.isfile(pmlfilename):
        raise Exception("Package xml file not found: %s" % pmlfilename)
    pkg_path = os.path.dirname(pmlfilename)
    ui.debug("opening file: ", pmlfilename)
    root = qisys.qixml.read(pmlfilename).getroot()
    name = root.get("name")
    pml_nodes = root.findall("pml")
    for pml in pml_nodes:
        pkg = make(pml.get("src"), build_worktree, linguist_worktree)
        pkg.install_in_subdir = True
        packages.append(pkg)
    return MetaPackage(pmlfilename, build_worktree.build_config, name, packages)

def make(pmlfilename, build_worktree, linguist_worktree):
    """ create Package from a PmlFile.
        Instanciate all Projects (crg, qibuild, ...) with correct arguments
    """
    builders = list()

    if not os.path.isfile(pmlfilename):
        raise Exception("Package xml file not found: %s" % pmlfilename)
    pkg_path = os.path.dirname(pmlfilename)
    ui.debug("opening file: ", pmlfilename)
    root = qisys.qixml.read(pmlfilename).getroot()
    if root.tag == "metapml":
        return _parse_metapackage(pmlfilename, build_worktree, linguist_worktree)

    name = root.get("name")
    builders.append(crgbuilder.make(pmlfilename))

    #populate cmake_builder
    qibuild_nodes = root.findall("qibuild")
    if len(qibuild_nodes) > 0:
        cmake_builder = qibuild.cmake_builder.CMakeBuilder(build_worktree)
        cmake_builder.dep_types = ["runtime"]
        builders.append(cmake_builder)

    for qibuild_node in qibuild_nodes:
        qibname = qibuild_node.get("name")
        #create a cmake_builder for that project
        cmake_builder.add_project(qibname)

    #populate qilinguist_builder
    qilinguist_nodes = root.findall("qilinguist")
    if len(qilinguist_nodes) > 0:
        qilinguist_builder = qilinguist.builder.QiLinguistBuilder(linguist_worktree)
        builders.append(qilinguist_builder)

    for qilinguist_node in qilinguist_nodes:
        qilname = qilinguist_node.get("name")
        qilinguist_builder.add_project(qilname)

    return Package(pmlfilename, build_worktree.build_config, name, builders)
