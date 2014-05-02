#!/usr/bin/env python2
## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

from setuptools import setup, find_packages

setup(name="qipkg",
      version="0.1",
      description="qiPackage Generator",
      author="Aldebaran Robotics",
      author_email="contact@aldebaran-robotics.com",
      packages=find_packages(),
      license="BSD",
      requires=['qibuild'],
      entry_points = {
          "console_scripts" : [
              "qipkg = qisys.main:main"
              ]
          }
)
