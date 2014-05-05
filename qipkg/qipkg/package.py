## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

"""This package contains the PackageWorkTree object.
"""
import os
import qisys.qixml
from qisys import ui
import qibuild.parsers
import qibuild.deploy
import qilinguist.builder
import zipfile

class Package(object):
    """ A class representing a .pgk object """

    def __init__(self, pml_path):
        """ pml_path: the pml file used to build the package
            builders: a list of builders (cmake, crg, qidoc, ...)
        """
        self.pml_path = pml_path
        self.root = os.path.dirname(self.pml_path)
        self.manifest_xml = os.path.join(self.root, "manifest.xml")
        if not os.path.exists(self.manifest_xml):
            raise Exception("%s does not exist" % self.manifest_xml)
        self.name = None
        self.version = None
        pkg_name(self.manifest_xml)
        self.output = "%s-%s.pkg" % (self.name, self.version)

    def make(self, pml_builder, output=None):
        if not output:
            name = pkg_name(self.manifest_xml)
            output = os.path.join(os.getcwd(), name)
        # Step1: install everything in the stage path
        stage_path = pml_builder.stage_path
        manifest = pml_builder.install(stage_path)

        # Step2: zip the result with the manifest.xml inside
        archive = qisys.archive.compress(stage_path)
        qisys.sh.mv(archive, output)

        # add manifest.xml if it exists
        if os.path.exists(self.manifest_xml):
            zip = zipfile.ZipFile(output, "a")
            zip.write(self.manifest_xml, "manifest.xml")
        ui.info(ui.green, "Package generated in",
                ui.reset, ui.bold, output)
        return output

def pkg_name(manifest_xml):
    "Return a tuple name, version"
    root = qisys.qixml.read(manifest_xml).getroot()
    uuid = root.get("uuid")
    version = root.get("version")
    output_name = "%s-%s.pkg" % (uuid, version)
    return output_name

