import qipkg.package


def test_make(package_test):
    package = qipkg.package.make(package_test)

    assert package.name == "test_package"
    names = list()
    for behavior in package.behaviors:
        names.append(behavior.name)
    assert "test_package" in names
    assert "test_package/subfolder" in names
