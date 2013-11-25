<package name="test_package">

  <behavior src=".">
    <file src="manifest.xml"/>
    <file src="dummy"/>
  </behavior>

  <behavior src="subfolder">
    <file src="manifest.xml"/>
  </behavior>

  <resources>
    <file src="res1"/>
  </resources>

  <qibuild name="libqi"/>

</package>
