#!/usr/bin/env python2
## Copyright (c) 2012, 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

from distutils.core import setup

setup(name="qipkg",
      version="0.0.1",
      description="qiPackage Generator",
      author="Aldebaran Robotics",
      author_email="cgestes@aldebaran-robotics.com",
      packages=["qipkg", "qipkg.actions"],
      license="BSD",
      scripts=['bin/qipkg'],
      requires=['qibuild']
)
