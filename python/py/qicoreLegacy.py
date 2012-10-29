import naoqi
import qicore

class BehaviorLegacy(qicore.Box, naoqi.ALBehavior):
  def __init__(self, param, autoBind):
    qicore.Box.__init__(self)
    naoqi.ALBehavior.__init__(self, param, autoBind)
