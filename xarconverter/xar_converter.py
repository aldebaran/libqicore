#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Entry point of the converter
.. module:: converter
"""

import sys
import os
import distutils.dir_util

import converter.xar_parser as xar_parser
import converter.new_format_generator as nfgenerator
import converter.name_map_builder as nmbuilder
import converter.node.box


def convert_behavior(filepath, output, name):
    """ converts an old behavior (xar_version 3) into a newer format (format_version 4)
        The entry point of the behavior is still a behavior.xar file but this one
        only leads to the interface of the root box of the behavior
    """
    root = xar_parser.generate_tree_from_filename(filepath)
    if not root:
        sys.stderr.write("Incorrect format, file must be in xar_version 3"
                         + os.linesep)
        sys.exit(6)

    nmb = nmbuilder.NameMapBuilder()
    nmb.visit(root)

    name_map = nmb.get_name_map()

    nfg = nfgenerator.NewFormatGenerator(name_map)

    distutils.dir_util.mkpath(output)
    nfg.visit(root, output)
    nfg.generate_entry_point(root, name)


def convert_boxlibrary(filepath, output, name):
    """ converts an old box (xar_version 3) into a newer format (format_version 4)
        There are two different cases to manage:
         - simple box, represented by a box interface and its implementation
         - behavior box, represented as simple box but with a behavior entry point
    """
    root = xar_parser.generate_tree_from_filename(filepath)
    if not root:
        sys.stderr.write("Incorrect format, file must be in xar_version 3"
                         + os.linesep)
        sys.exit(6)

    # root box is a simple box, it should have a name
    if isinstance(root, converter.node.box.Box):
        root.name = name
        root.id = "-1"

    nmb = nmbuilder.NameMapBuilder()
    nmb.visit(root)
    msg = "{} {} {} {} {}".format(root.name,
                                  root.id,
                                  root.node_type,
                                  root.parent_path,
                                  root.node_path)
    print msg

    name_map = nmb.get_name_map()

    nfg = nfgenerator.NewFormatGenerator(name_map)

    distutils.dir_util.mkpath(output)
    nfg.visit(root, output)
    if filepath.find("behavior.xar") != -1:
        nfg.generate_entry_point(root, name)


def main():
    """ Entry point of the xar converter
    """
    param = []
    if len(sys.argv) not in range(2, 4):
        sys.stderr.write("Incorrect number of arguments" + os.linesep)
        sys.exit(1)
    param.append(sys.argv[1])
    if (len(sys.argv) == 3):
        param.append(sys.argv[2])
    else:
        param.append("objects")

    abspath = os.path.abspath(param[0])
    dest_dir = os.path.abspath(param[1])

    folders = dest_dir.split("\\")
    convert_behavior(abspath, dest_dir, folders[len(folders) - 1])

if __name__ == "__main__":
    main()
