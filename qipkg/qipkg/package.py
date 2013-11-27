## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

"""This package contains the PackageWorkTree object.
"""
import qisys.qixml
from qisys import ui
import qisys.sh
import os
import zipfile
import shutil


class Package(object):
    def __init__(self, path, name, behaviors):
        self.path = path
        self.name = name
        self.behaviors = behaviors

    def do_package(self, output_dir):
        #todo: be smart here (read the manifest)
        output_file = os.path.join(output_dir, self.name + ".pkg")
        archive = zipfile.ZipFile(output_file, "w", zipfile.ZIP_DEFLATED)

        for b in self.behaviors:
            for f in b.files:
                arcname = os.path.relpath(f, self.path)
                archive.write(f, arcname)
        ui.info(ui.green, "Generated package:", ui.reset, output_file)
        archive.close()

    def do_install(self, output_dir):
        for b in self.behaviors:
            for f in b.files:
                arcname = os.path.relpath(f, self.path)
                dest = os.path.join(output_dir, arcname)
                qisys.sh.mkdir(os.path.dirname(dest), recursive=True)
                shutil.copy(f, dest)

class Behavior(object):
    def __init__(self, path, name, files):
        ui.debug("Adding behavior:", name)
        self.path = path
        self.name = name
        self.files = files


def make(pkg_path, packagexml="qipackage.xml"):
    fullpath = os.path.join(pkg_path, packagexml)
    ui.debug("opening file: ", fullpath)
    root = qisys.qixml.read(fullpath).getroot()
    name = root.get("name")
    behaviors = root.findall("behavior")
    pkg_behaviors = list()
    for behavior in behaviors:
        src = behavior.get("src", "")
        if src and src != ".":
            bname = name + "/" + src
        else:
            bname = name
        path = os.path.join(pkg_path, src)
        files = list()
        for f in behavior.findall("file"):
            files.append(os.path.join(path, f.get("src")))
        behavior_toadd = Behavior(path, bname, files)
        pkg_behaviors.append(behavior_toadd)
    return Package(pkg_path, name, pkg_behaviors)
