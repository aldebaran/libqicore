#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import sys
import os

import parse_folder

def parseFolder(folder):
  if (folder[len(folder) - 1] != os.sep):
    folder = folder + os.sep
  output = open("behavior.py", encoding='utf-8', mode='w')
  parser = parse_folder.behaviorParser(folder, output)
  parser.generatePython()
  output.close()

def main():
  if len(sys.argv) != 2:
    print("Incorrect number of arguments")
    return
  parseFolder(sys.argv[1])

if __name__ == "__main__":
  main()
