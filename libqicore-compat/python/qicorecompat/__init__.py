from behavior import Behavior
from resourcemanager import ResourceManager
from timeline import Timeline
from framemanager import FrameManager
from controlflowdiagram import ControlFlowdiagram
import qi

global ALFrameManager
ALFrameManager = FrameManager()

qi.registerObjectFactory("ControlFlowdiagram", ControlFlowdiagram)
