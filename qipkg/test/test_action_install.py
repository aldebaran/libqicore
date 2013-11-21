import os
import qisys.script


def test_action_install(package_test, tmpdir):
    pkgin  = os.path.join(package_test, "pkgtest.pml")
    result = os.path.join(tmpdir.strpath)
    qisys.script.run_action("qipkg.actions.install", [pkgin, result])

    assert tmpdir.join("manifest.xml").check(file=True)
    assert tmpdir.join("behavior.xar").check(file=True)
    assert tmpdir.join("dummy").check(file=True)
    assert tmpdir.join("res1").check(file=True)
    assert tmpdir.join("subfolder/manifest.xml").check(file=True)
    assert tmpdir.join("subfolder/behavior.xar").check(file=True)
