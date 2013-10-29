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

qiproject.xml:
<project version="3">
  # means you will have a qipackage inside that folder. (look for qipackage.xml)
  <package name="run_dialog"/>
</project>

qipackage.xml:
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
