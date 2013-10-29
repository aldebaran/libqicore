import os
import pytest


@pytest.fixture
def package_test():
    this_dir = os.path.dirname(__file__)
    package_path = os.path.join(this_dir, "pkgtest")
    return package_path


@pytest.fixture
def cd_pkg_dir(monkeypatch, package_test):
    monkeypatch.chdir(package_test)
    return package_test
