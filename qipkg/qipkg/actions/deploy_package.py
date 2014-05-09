""" Deploy and install a package to a target

"""

import os

from qisys import ui

import qisys.command
import qisys.parsers

def configure_parser(parser):
    qisys.parsers.default_parser(parser)
    qisys.parsers.deploy_parser(parser)
    parser.add_argument("pkg_path")
    parser.add_argument("pkg_name")

def do(args):
    urls = qisys.parsers.get_deploy_urls(args)
    for url in urls:
        pkg_path = args.pkg_path
        pkg_name = args.pkg_name

        scp_cmd = ["scp",
                   pkg_path,
                   "%s@%s:" % (url.user, url.host)]
        qisys.command.call(scp_cmd)

        try:
            _install_package(url, pkg_path, pkg_name)
        except Exception as e:
            ui.error("Unable to install package on target")
            ui.error("Error was: ", e)

def _install_package(url, pkg_path, pkg_name):
    import qi
    session = qi.Session()
    session.connect("tcp://%s:9559" % url.host)
    package_manager = session.service("PackageManager")
    package_manager.install("/home/nao/%s" % os.path.basename(pkg_path), pkg_name)
