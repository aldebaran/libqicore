How to install for developers?
==============================

cd tools/qibuild

# -e mean editable  (ie works with sources)
# --user to install for the user only
pip install -e . --user


cd qicore/qipkg
pip install -e . --user


That's all. Then you can edit your sources and your change will be taken into account.


File Format
===========

qiproject.xml::

  <project version="3">
    <!-- means you will have a qipackage inside that folder. (look for qipackage.xml) -->
    <package name="run_dialog"/>
  </project>

qipackage.pml::

<package name="run_dialog">

  <behavior src=".">
    <file src="manifest.xml"/>
    <file src="behavior.xar"/>
    <file src="icon.png"/>
  </behavior>

  <behavior src="aisystem">
    <file src="manifest.xml"/>
    <file src="behavior.xar"/>
    <file src="icon.png"/>
  </behavior>

  <behavior src="init">
    <file src="behavior.xar"/>
    <file src="icon.png"/>
    <file src="manifest.xml"/>
    <file src="SLMJapanese.fcf"/>
    <file src="strategies_onepass.ini"/>
  </behavior>
</package>

You can add qibuild or python project to the list directly, like so::

  <package>

    <qibuild name="foo" />
    <qipy name="pyfoo" />

  </package>

The projects will be installed inside the package

Usage
=====

A  ``manifest.xml`` should exist next to the pml file and should at least
contain ::

  <package version="0.0.1" uuid="test_package">

Those are the id and the version number used by the PackageManager on the robot

After that, just use:

qipkg make-package my-behavior.pml

to generate a package, or even (if qimessaging python bindings are available)

qipkg deploy-package my-behavior.pml --url nao@nao.local

to call PackageManager.install()
