/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#include <boost/foreach.hpp>

#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/resourcemodel.hpp>
#include <qicore-compat/model/animationmodel.hpp>
#include <qicore-compat/model/behaviorsequencemodel.hpp>

#include <qi/log.hpp>

#include "pythonboxgenerator.hpp"
qiLogCategory("QiCore-Compat.PythonBoxGenerator");
#define foreach BOOST_FOREACH



namespace qi
{
  static std::string initInput(BoxInstanceModelPtr instance);
  static std::string initOutput(BoxInstanceModelPtr instance);
  static std::string initParameter(BoxInstanceModelPtr instance);
  static std::string internalInput(InputModelPtr inp, BoxInterfaceModelPtr interface);
  static std::string specialInputNatureAction(InputModel::InputNature nature, bool hasAnimation, bool hasResource);
  static std::string internalOutput(OutputModelPtr out, BoxInterfaceModelPtr interface);
  static std::string internalResource(BoxInterfaceModelPtr interface);
  static bool isSpecialChar(char c);
  static std::string pythonStringize(const std::string& str);

  std::string initInput(BoxInstanceModelPtr instance)
  {
    BoxInterfaceModelPtr interface = instance->interface();
    //Input signal
    InputModelMap inputs = interface->inputs();
    std::stringstream input;

    foreach(InputModelMap::value_type &inp, inputs)
    {
      if(inp.second->nature() == InputModel::InputNature_STMValue)
      {
        std::string name = inp.second->metaMethod().name();
        std::string sig = inp.second->metaMethod().parametersSignature().toString();
        if(sig.empty())
          sig = "(m)";
        if(inp.second->metaMethod().parametersSignature().isConvertibleTo("()"))
          sig = "(m)";

        input << "    self." << name << "Signal = qi.Signal('" << sig << "')\n";

        //Add input in self.stminput
        input << "    self.stminput = self." << name << "Signal\n";
      }
    }

    if(interface->hasTimeline() || interface->hasFlowDiagram())
    {
      foreach(InputModelMap::value_type &inp, inputs)
      {
        if(inp.second->nature() == InputModel::InputNature_OnLoad || inp.second->nature() == InputModel::InputNature_STMValue)
          continue;

        std::string name = inp.second->metaMethod().name();
        std::string sig = inp.second->metaMethod().parametersSignature().toString();


        if(sig.empty())
          sig = "(m)";

        if(inp.second->metaMethod().parametersSignature().isConvertibleTo("()"))
          sig = "(m)";

        input << "    self." << name << "Signal = qi.Signal('" << sig << "')\n";
      }
    }
    return input.str();
  }

  std::string initOutput(BoxInstanceModelPtr instance)
  {
    BoxInterfaceModelPtr interface = instance->interface();
    //Output signal
    OutputModelMap outputs = interface->outputs();
    std::stringstream output;
    foreach(OutputModelMap::value_type &out, outputs)
    {
      std::string name = out.second->metaSignal().name();
      std::string sig =  out.second->metaSignal().parametersSignature().toString();

      if(sig.empty())
        sig = "(m)";

      if(out.second->metaSignal().parametersSignature().isConvertibleTo("()"))
        sig = "(m)";

      output << "    self." << name << "Signal = qi.Signal('" << sig << "')\n";
    }
    return output.str();
  }

  std::string initParameter(BoxInstanceModelPtr instance)
  {
    BoxInterfaceModelPtr interface = instance->interface();
    std::list<ParameterModelPtr> parameters = interface->parameters();
    std::stringstream parameter;
    foreach (ParameterModelPtr param, parameters)
    {
      std::string name = param->metaProperty().name();
      std::string sig =  param->metaProperty().signature().toString();

      std::string param_name = "self.param_" + pythonStringize(name);
      parameter << "    " << param_name << " = qi.Property('" << sig << "')\n";
      parameter << "    " << param_name << ".__qi_name__ = '" << name << "'\n";
      parameter << "    self.parameters['" << name << "'] = " << param_name << "\n";
    }

    return parameter.str();
  }

  std::string internalInput(InputModelPtr inp, BoxInterfaceModelPtr interface)
  {
    std::stringstream input;
    input << "\n"
          << "  def onInput_" << inp->metaMethod().name() << "__" << "(self, p):\n";

    if(inp->nature() == InputModel::InputNature_OnStart)
    {
      if(interface->hasResource())
      {
        input << "    self.resource = True\n";
        if(interface->resources().back()->timeout() == -1)
        {
          input << "    try:\n"
                << "      self.waitResourceFree()\n"
                << "    expect:\n"
                << "      return\n";
        }

        input << "    try:\n"
              << "      self.waitResourcesCallback('__onResource__')\n"
              << "    except BaseException, errRes:\n"
              << "      try:\n"
              << "        self.onResourceError()\n"
              << "      except:\n"
              << "        try:\n"
              << "          self.onResourceError(None)\n"
              << "        except BaseException, errRes:\n"
              << "          try:\n"
              << "            self.onStopped()\n"
              << "          except:\n"
              << "            try:\n"
              << "              self.onStopped(None)\n"
              << "            except:\n"
              << "              pass\n"
              << "      self.releaseResource()\n"
              << "      return\n";
      }
    }

    if(interface->hasTimeline() || interface->hasResource())
      if(inp->nature() == InputModel::InputNature_OnStart || inp->nature() == InputModel::InputNature_OnStop)
        input << "    self._lastCommand" << " = "
              << (inp->nature() == InputModel::InputNature_OnStart ? 1 : 2)
              << "\n";

    //call method associated to this input
    std::string arg = "p";
    if(inp->metaMethod().parametersSignature().isConvertibleTo(qi::Signature("()")))
      arg = "None";

    input << "    if(not self._safeCallOfUserMethod('" << "onInput_"
          << inp->metaMethod().name() << "', " << arg << ")):\n";
    if(interface->hasResource())
      input << "      self.releaseResource()\n";
    input << "      return\n";

    if(interface->hasTimeline() || interface->hasFlowDiagram())
    {
      input << specialInputNatureAction(inp->nature(), interface->hasTimeline(), interface->hasResource())
            << "    self.stimulateIO('" << inp->metaMethod().name() << "', p)\n";
    }
    return input.str();
  }

  std::string specialInputNatureAction(InputModel::InputNature nature,
                                                           bool hasAnimation,
                                                           bool hasResource)
  {
    std::stringstream specialAction;
    switch(nature)
    {
    case InputModel::InputNature_OnStart:
      if(hasAnimation)
      {
        specialAction << "      if(self._lastCommand in [0, 1]):\n"
                      << "         self.getTimeline().play()\n";
      }
      break;

    case InputModel::InputNature_OnStop:
      if(hasAnimation || hasResource)
      {
        specialAction << "      if(self._lastCommand in [0, 2]):\n";

        if(hasAnimation)
          specialAction << "        self.getTimeline().stop()\n";

        if(hasResource)
          specialAction << "        self.releaseResource()\n";
      }
      break;

    default:
      break;
    }

    if(specialAction.str().empty())
      return std::string();

    std::stringstream ret;
    ret << "    try:\n";
    ret << specialAction.str();
    ret << "    except BaseException, err:\n"
        << "      self.logger.error(str(err))\n"
        << "      pass\n";

    return ret.str();
  }

  std::string internalOutput(OutputModelPtr out, BoxInterfaceModelPtr interface)
  {
    std::stringstream outputs;

    std::string arg = "p";
    if(out->metaSignal().parametersSignature().isConvertibleTo(Signature("()")))
    {
      arg = "p = None";
    }

    outputs << "\n"
            << "  def " << out->metaSignal().name() << "(self, " << arg << "):\n";
    if(out->nature() == OutputModel::OutputNature_Stopped)
    {
      std::stringstream output;

      if(interface->hasResource())
        output << "      self.releaseResource()\n";

      if(interface->hasTimeline())
        output << "      self.getTimeline().stop()\n";

      if(interface->hasTimeline() || interface->hasResource())
        output << "      self._lastCommand = " << out->nature() << "\n";

      if(!output.str().empty())
      {
        outputs << "    try:\n"
                << output.str()
                << "    except BaseException, err:\n"
                << "      self.logger.error(str(err))\n"
                << "      pass\n";
      }

      if(interface->hasTimeline())
        outputs << "    self.stimulateIO('" << out->metaSignal().name() << "', p)\n";
    }

    if(out->nature() == OutputModel::OutputNature_Stopped && interface->hasTimeline())
      outputs << "  def onStopped__(self):\n"
              << "    self.stimulateIO('" << out->metaSignal().name() << "', None)\n";
    else
      outputs << "    self.stimulateIO('" << out->metaSignal().name() << "', p)\n";

    return outputs.str();
  }

  std::string internalResource(BoxInterfaceModelPtr interface)
  {
    std::stringstream resource;
    resource << "\n"
             << "  def __onResource__(self, resourceName):\n";

    ResourceModelPtr res = interface->resources().back();
    switch(res->lockType())
    {
    case ResourceModel::LockType_StopOnDemand:
      resource << "    bExists = True\n"
               << "    try:\n"
               << "      self.onResourceLost()\n"
               << "    expect:\n"
               << "      try:\n"
               << "        self.onResourceLost(None)\n"
               << "      expect:\n"
               << "        bExists = False\n";
      if(interface->hasTimeline())
        resource << "    self.getTimeline.stop()\n";

      if(interface->hasResource())
        resource << "    self.releaseResource()\n";

      resource << "    if(not bExists):\n"
               << "      try:\n"
               << "        self.onStopped()\n"
               << "      expect:\n"
               << "        try:\n"
               << "          self.onStopped(None)"
               << "        expect:\n"
               << "          pass\n";
      break;

    case ResourceModel::LockType_PauseOnDemand://BETA
      if(interface->hasTimeline())
        resource << "    self.getTimeline.pause()\n";

      if(interface->hasResource())
        resource << "    self.releaseResource()\n"
                 << "    self.waitResourceFree()\n"
                 << "    self.waitResources()\n";

      if(interface->hasTimeline())
        resource << "    self.getTimeline.play()\n";
      break;

    case ResourceModel::LockType_CallbackOnDemand:
      resource << "    self._safeCallOfUserMethod('onResource', resourceName)\n";
      break;

    default:
      resource << "    pass\n";
      break;
    }

    return resource.str();
  }

  std::string generatedClass(BoxInstanceModelPtr instance)
  {
    //load needed module
    BoxInterfaceModelPtr interface = instance->interface();

    std::stringstream generatedClass;
    generatedClass << "\n"
                   << "class GeneratedClass_" << instance->uid() << "(Behavior):\n"
                   << "  def __init__(self):\n"
                   << "    Behavior.__init__(self, '" << instance->uid() << "')\n"
                   << "    self._lastCommand = 0\n"
                   << "    self.id = '" << instance->uid() << "'\n"
                   << "    self.boxName = '" << instance->name() << "'\n"
                   << "    self.behaviorPath = '" << instance->behaviorPath() << "'\n"
                   << "    self.behaviorId = ''\n"//compat
                   << initOutput(instance) << "\n"
                   << initParameter(instance) << "\n"
                   << initInput(instance) << "\n";

    if(instance->parent())
      generatedClass << "    self.parentBoxName = '"
                     << instance->parent()->uid() << "'\n";


    if(interface->hasTimeline())
    {
      int fps;
      qi::AnyReference animation = instance->content(ContentModel::ContentType_Animation);
      if(animation.isValid())
        fps = animation.ptr<AnimationModel>()->fps();
      else
      {
        qi::AnyReference behaviorSequence = instance->content(ContentModel::ContentType_BehaviorSequence);
        fps = behaviorSequence.ptr<BehaviorSequenceModel>()->fps();
      }
      generatedClass << "    self.timeline = Timeline(" << fps << ")\n"
                     << "    ALFrameManager.addTimeline(self.name, " << fps << ", self.timeline)\n"
                     << "    self.getTimeline().onTimelineFinished.connect(self.onStopped__)\n";
    }

    if(interface->hasResource())
    {
      generatedClass << "    self.resource = True\n";
      generatedClass << "    owner = '" << instance->name() << "_" << instance->id() << "'\n";
      generatedClass << "    resources = [";
      std::list<ResourceModelPtr> resources = interface->resources();
      foreach(ResourceModelPtr resource, resources) {
        generatedClass << "'" << resource->name() << "', ";
      }
      generatedClass << "]\n";
      generatedClass << "    timeout = " << interface->resources().front()->timeout() << "\n";
      generatedClass << "    self.resourceManager = ResourceManager(resources, owner, timeout)\n";
    }

    // internal resource callback
    if(interface->hasResource())
      generatedClass << internalResource(interface);

    //internal methods for inputs
    InputModelMap inputs = interface->inputs();
    foreach(InputModelMap::value_type inp, inputs)
    {
      if(inp.second->nature() == InputModel::InputNature_OnLoad || inp.second->nature() == InputModel::InputNature_STMValue)
        continue;

      generatedClass << internalInput(inp.second, interface);
    }

    //internal methods for outputs
    OutputModelMap outputs = interface->outputs();
    foreach(OutputModelMap::value_type &out, outputs)
    {
      generatedClass << internalOutput(out.second, interface);
    }

    // deprecated method since 1.14 to control the associated timeline
    if(interface->hasTimeline())
    {
      generatedClass << "\n"
                     << "  def playTimeline(self):\n"
                     << "    self.getTimeline().play()\n"
                     << "\n"
                     << "  def stopTimeline(self):\n"
                     << "    self.getTimeline.pause()\n"
                     << "\n"
                     << "  def exitBehavior(self):\n"
                     << "    self.getTimeline().stop()\n"
                     << "\n"
                     << "  def gotoAndStop(self, frame):\n"
                     << "    self.getTimeline().gotoAndStop(frame)\n"
                     << "\n"
                     << "  def gotoAndPlay(self, frame):\n"
                     << "    self.getTimeline().gotoAndPlay(frame)\n"
                        //Def timeline setter
                     << "\n"
                     << "  def setTimeline(self, timeline, frames):\n"
                     << "    self.timeline.setTimeline(timeline, frames)\n"
                        //Def timeline getter
                     << "\n"
                     << "  def getTimeline(self):\n"
                     << "    return self.timeline\n";
    }

    if(instance->parent() && instance->parent()->interface()->hasTimeline())
    {
      generatedClass << "\n"
                     << "  def setTimelineParent(self, timeline, frames):\n"
                     << "    self.parentTimeline.setTimeline(timeline, frames)\n"
                     << "\n"
                     << "  def getTimelineParent(self, timeline, frames):\n"
                     << "    return self.parentTimeline\n"
                        // deprectated method since 1.14 to control the parent timeline
                     << "\n"
                     << "  def playTimelineParent(self):\n"
                     << "    self.parentTimeline.play()\n"
                     << "\n"
                     << "  def stopTimelineParent(self):\n"
                     << "    self.parentTimeline.pause()\n"
                     << "\n"
                     << "  def exitBehaviorParent(self):\n"
                     << "    self.parentTimeline.stop()\n"
                     << "\n"
                     << "  def gotoAndStopParent(self, frame):\n"
                     << "    self.parentTimeline.gotoAndStop(frame)\n"
                     << "\n"
                     << "  def gotoAndPlayParent(self, frame):\n"
                     << "    self.parentTimeline.gotoAndPlay(frame)\n";
    }

    if(interface->hasResource())
    {
      generatedClass << "\n"
                     << "  def releaseResource(self):\n"
                     << "    self.resourceManager.releaseResource()\n"
                     << "\n"
                     << "  def waitResourcesCallback(self, callbackname):\n"
                     << "    self.resourceManager.waitResourcesCallback(callbackname)\n"
                     << "\n"
                     << "  def waitResourceFree(self):\n"
                     << "    self.resourceManager.waitResourceFree()\n"
                     << "\n"
                     << "  def waitResources(self):\n"
                     << "    self.resourceManager.waitResources()\n";
    }

    return generatedClass.str();
  }

  bool isSpecialChar(char c)
  {
    return !(
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') ||
        c == '_');
  }

  std::string pythonStringize(const std::string& str)
  {
    std::string out = str;
    std::replace_if(out.begin(), out.end(), isSpecialChar, '_');
    return out;
  }
}
