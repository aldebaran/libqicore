import os
import qisys.script


def test_action_package(package_test, cd_pkg_dir, tmpdir):
    qisys.script.run_action("qipkg.actions.package", [tmpdir.strpath])
    result = os.path.join(tmpdir.strpath, "test_package.pkg")
    assert os.path.exists(result)
