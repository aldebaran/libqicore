#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import os

import sax_parser
import newFormatGenerator

def parse(file):
  return sax_parser.parse(file)

def main():
  if len(sys.argv) != 2:
    print("Incorrect number of arguments")
    return
  root = parse(sys.argv[1])
  nfg = newFormatGenerator.newFormatGenerator()
  os.mkdir("objects")
  os.chdir("objects")
  nfg.visit(root)
  os.chdir("./..")

if __name__ == "__main__":
  main()
