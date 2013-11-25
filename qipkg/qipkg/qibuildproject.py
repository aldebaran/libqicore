## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

from qisys import ui
import qisys.sh

class QiBuildProject(object):
    def __init__(self, name, cmake_builder):
        self.name = name
        self.cmake_builder = cmake_builder


    def configure(self):
        raise NotImplementedError

    def make(self):
        raise NotImplementedError

    def install(self, dest):
        dest_dir = qisys.sh.to_native_path(dest)
        ui.info(ui.green, "Installing qibuild project:", ui.reset, self.name)
        return self.cmake_builder.install(dest_dir)
        #, prefix=args.prefix, split_debug=args.split_debug, components=components)


def make(name, cmake_builder):
    return QiBuildProject(name, cmake_builder)
