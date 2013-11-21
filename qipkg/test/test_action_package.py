import os
import qisys.script


def test_action_package(package_test, tmpdir):
    pkgin  = os.path.join(package_test, "pkgtest.pml")
    result = os.path.join(tmpdir.strpath, "test_package.pkg")
    qisys.script.run_action("qipkg.actions.package", [pkgin, result])
    assert os.path.exists(result)
