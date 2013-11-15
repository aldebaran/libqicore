import os
import qisys.script


def test_action_install(package_test, cd_pkg_dir, tmpdir):
    qisys.script.run_action("qipkg.actions.install", [tmpdir.strpath])

    assert tmpdir.join("manifest.xml").check(file=True)
    assert tmpdir.join("dummy").check(file=True)
    assert tmpdir.join("subfolder/manifest.xml").check(file=True)
    assert tmpdir.join("subfolder/super.xar").check(file=True)
