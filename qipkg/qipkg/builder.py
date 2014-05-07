""" Builder for pml files """
import os

import qibuild.worktree

import qisys.qixml
import qipkg.package

from qipy.worktree import PythonWorkTree
from qipy.python_builder import PythonBuilder
from qibuild.worktree import BuildWorkTree
from qibuild.cmake_builder import CMakeBuilder

class PMLBuider(object):
    def __init__(self, worktree, pml_path, config="default"):
        self.worktree = worktree
        self.config = config
        self.pml_path = pml_path
        self.base_dir = os.path.dirname(self.pml_path)

        # used to prepare deploying files and making packages,
        # so it must always exist but also always start empty
        dot_qi = self.worktree.dot_qi
        self.stage_path = os.path.join(self, dot_qi, "staged", config)
        qisys.sh.rm(self.stage_path)
        qisys.sh.mkdir(self.stage_path, recursive=True)

        self.build_worktree = BuildWorkTree(self.worktree)
        self.cmake_builder = CMakeBuilder(self.build_worktree)
        self.cmake_builder.projects = list()
        self.cmake_builder.dep_types = ["runtime"]

        self.python_worktree = PythonWorkTree(self.worktree)
        self.python_builder = PythonBuilder(self.python_worktree,
                                            build_worktree=self.build_worktree)
        self.python_builder.projects = list()

        self.builders = [
                self.python_builder,
                self.cmake_builder
        ]
        self.file_list = list()


        self.load_pml(pml_path)
        dot_qi = self.worktree.dot_qi

    def load_pml(self, pml_path):
        tree= qisys.qixml.read(pml_path)
        root = tree.getroot()
        qibuild_elems = root.findall("qibuild")
        for qibuild_elem in qibuild_elems:
            name = qisys.qixml.parse_required_attr(qibuild_elem, "name", pml_path)
            project = self.build_worktree.get_build_project(name)
            self.cmake_builder.projects.append(project)

        qipython_elems = root.findall("qipython")
        for qipython_elem in qipython_elems:
            name = qisys.qixml.parse_required_attr(qipython_elem, "name", pml_path)
            project = self.python_worktree.get_python_project(name)
            self.python_builder.projects.append(project)

        # For top, ressource, dialog, behavior, add stuff to self.file_list
        behaviors = root.find("BehaviorDescriptions")
        if behaviors is not None:
            for child in behaviors.findall("BehaviorDescription"):
                src = child.get("src")
                full_src = os.path.join(src, "behavior.xar")
                self.file_list.append(full_src)

        # Dialog
        dialogs = root.find("Dialogs")
        if dialogs is not None:
            for child in dialogs.findall("Dialog"):
                src = child.get("src")
                self.file_list.append(src)

        # Resources
        resources = root.find("Resources")
        if resources is not None:
            for child in resources.findall("File"):
                src = child.get("src")
                self.file_list.append(src)

        # Topics
        topics = root.find("Topics")
        if topics is not None:
            for child in topics.findall("Topic"):
                src = child.get("src")
                self.file_list.append(src)

    def configure(self):
        for builder in self.builders:
            builder.configure()

    def build(self):
        for builder in self.builders:
            builder.build()

    def install(self, destination):
        # Use every available builder to install
        for builder in self.builders:
            if isinstance(builder, CMakeBuilder):
                builder.install(destination, components=["runtime"])
            else:
                builder.install(destination)
        # # Also use file from file_list
        for src in self.file_list:
            full_src = os.path.join(self.base_dir, src)
            qisys.sh.install(full_src, destination)

    def deploy(self, url):
        package = self.package(output=None)
        qisys.sh.mv(package, self.stage_path)
        qisys.remote.deploy(self.stage_path, url)


    def package(self, output=None):
        package = qipkg.package.Package(self.pml_path)
        return package.make(self, output=output)

