#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import os

import sax_parser
import newFormatGenerator
import nameMapBuilder

def parse(file):
  return sax_parser.parse(file)

def main():
  if len(sys.argv) not in range(2,4):
    print("Incorrect number of arguments")
    return
  if (len(sys.argv) == 3):
    folder = sys.argv[2]
  else:
    folder = "objects"

  root = parse(sys.argv[1])
  if (root is None):
    print("Incorrect File format, file must be in valid XML, xar_version 3")
    sys.exit(6)
  nmb = nameMapBuilder.nameMapBuilder()
  nmb.visit(root)

  nameMap = nmb.getNameMap()
  nfg = newFormatGenerator.newFormatGenerator(nameMap)
  os.mkdir(folder)
  os.chdir(folder)
  nfg.visit(root)
  nfg.generateMain()
  os.chdir("./..")

if __name__ == "__main__":
  main()
