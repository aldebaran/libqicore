## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import qisys.qixml
from qisys import ui
import qisys.sh
import os
import zipfile
import shutil

class CrgProject(object):
    def __init__(self, path, name, manifest, behaviors, resources):
        self.path = path
        self.name = name
        self.behaviors = behaviors
        self.resources = resources
        self.resources.append(manifest)
        for f in self.resources:
            if not os.path.isfile(f):
                raise Exception("Missing file: %s" % f)

    def configure(self):
        pass

    def make(self):
        pass

    def package(self, output_file):
        outdir = os.path.dirname(os.path.abspath(output_file))
        if not os.path.isdir(outdir):
            raise Exception("Destination folder do not exists: " % outdir)

        archive = zipfile.ZipFile(output_file, "w", zipfile.ZIP_DEFLATED)

        for f in self.resources:
            arcname = os.path.relpath(f, self.path)
            archive.write(f, arcname)
        for b in self.behaviors:
            for f in b.files:
                arcname = os.path.relpath(f, self.path)
                archive.write(f, arcname)
        ui.info(ui.green, "Generated package:", ui.reset, output_file)
        archive.close()

    def install(self, output_dir):
        for f in self.resources:
            arcname = os.path.relpath(f, self.path)
            dest = os.path.join(output_dir, arcname)
            qisys.sh.mkdir(os.path.dirname(dest), recursive=True)
            shutil.copy(f, dest)
        for b in self.behaviors:
            for f in b.files:
                arcname = os.path.relpath(f, self.path)
                dest = os.path.join(output_dir, arcname)
                qisys.sh.mkdir(os.path.dirname(dest), recursive=True)
                shutil.copy(f, dest)

class Behavior(object):
    def __init__(self, path, files):
        ui.debug("Adding behavior:", path)
        self.path = path
        if not os.path.isfile(os.path.join(self.path, "behavior.xar")):
            raise Exception("No behavior.xar found inside: %s" % self.path)
        self.files = files
        self.files.append(os.path.join(self.path, "behavior.xar"))
        for f in self.files:
            if not os.path.isfile(f):
                raise Exception("Missing file: %s" % f)


def _populate_files(xmlnode, path):
    files = list()
    for f in xmlnode.findall("file"):
        files.append(os.path.join(path, f.get("src")))
    return files

def make(fullpath):
    if not os.path.isfile(fullpath):
        raise Exception("Package xml file not found: %s" % fullpath)
    pkg_path = os.path.dirname(fullpath)
    ui.debug("opening file: ", fullpath)
    root = qisys.qixml.read(fullpath).getroot()
    name = root.get("name")
    manifests = root.findall("manifest")
    if len(manifests) > 1:
        raise Exception("Invalid package file: Only one manifest field allowed")
    if len(manifests):
        manifest = manifests[0].get("src")
    else:
        manifest = "manifest.xml"
    manifest = os.path.join(pkg_path, manifest)

    resources = root.findall("resources")
    if len(resources) > 1:
        raise Exception("Invalid package file: Only one global resources field allowed")
    if len(resources):
        pkg_resources = _populate_files(resources[0], pkg_path)
    else:
        pkg_resources = list()

    behaviors = root.findall("behavior")
    pkg_behaviors = list()
    for behavior in behaviors:
        src = behavior.get("src", "")
        path = os.path.join(pkg_path, src)
        files = _populate_files(behavior, path)
        behavior_toadd = Behavior(path, files)
        pkg_behaviors.append(behavior_toadd)
    return CrgProject(pkg_path, name, manifest, pkg_behaviors, pkg_resources)
