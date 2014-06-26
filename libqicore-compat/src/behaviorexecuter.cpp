/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#include <boost/python.hpp>

#include <qicore-compat/model/boxinterfacemodel.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/linkmodel.hpp>
#include <qicore-compat/model/contentmodel.hpp>
#include <qicore-compat/model/contentsmodel.hpp>
#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/resourcemodel.hpp>
#include <qicore-compat/model/behaviorsequencemodel.hpp>
#include <qicore-compat/model/behaviorlayermodel.hpp>
#include <qicore-compat/model/behaviorkeyframemodel.hpp>
#include <qicore-compat/model/animationmodel.hpp>

#include <qicore-compat/behaviorexecuter.hpp>
#include <qicore-compat/timeline.hpp>
#include <qi/type/objectfactory.hpp>
#include <qi/type/dynamicobject.hpp>
#include <qipython/gil.hpp>
#include "behaviorexecuter_p.hpp"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.BehaviorExecuter");

namespace qi
{
  namespace compat
  {

    typedef std::map<std::string, int> FrameNameKey;
    typedef std::map<std::string, qi::AnyObject> ObjectMap;
    BehaviorExecuterPrivate::BehaviorExecuterPrivate(const std::string &dir, boost::shared_ptr<Session> session, bool debug) :
      _project(dir),
      _session(session),
      _debug(debug),
      _running(0)
    {
      std::stringstream p;
      p << _session->url().port();

      _pythonCreator = PythonBoxLoader();
      _pythonCreator.initPython(session->url().host(), p.str(), dir, _session);
      _behaviorService = session->service("BehaviorService").value().call<qi::AnyObject>("create");
      _behaviorServiceId = _session->registerService("Behavior", _behaviorService);
      _behaviorService.connect("onTaskError",
          boost::function<void(const std::string&, const std::string&)>(
            boost::bind(&BehaviorExecuterPrivate::onFailed, this, _1, _2)));
      _alresourcemanager = session->service("ALResourceManager");
      _almotion = session->service("ALMotion");
      _almemory = session->service("ALMemory");
    }

    BehaviorExecuterPrivate::~BehaviorExecuterPrivate()
    {
      _session->unregisterService(_behaviorServiceId);
      {
        py::GILScopedLock l;
        _timelines.clear();
        _behaviorService = qi::AnyObject();
        _alresourcemanager = qi::AnyObject();
        _almotion = qi::AnyObject();
        _almemory = qi::AnyObject();
      }
      _pythonCreator.terminate();
    }

    bool BehaviorExecuterPrivate::declaredBox(BoxInstanceModelPtr instance)
    {
      //If FlowDiagram
      qi::AnyReference diagram = instance->content(ContentModel::ContentType_FlowDiagram);
      if(diagram.isValid())
        if(!loadFlowDiagram(diagram.ptr<FlowDiagramModel>()))
          return false;

      //If behaviorSequence
      diagram = instance->content(ContentModel::ContentType_BehaviorSequence);
      if(diagram.isValid())
          if(!loadBehaviorSequence(diagram.ptr<BehaviorSequenceModel>()))
            return false;

      //Box no script or python box is same
      return declaredPythonBox(instance);

    }

    bool BehaviorExecuterPrivate::declaredPythonBox(BoxInstanceModelPtr instance)
    {
      return _pythonCreator.registerPythonClass(instance);
    }

    bool BehaviorExecuterPrivate::loadBehaviorSequence(BehaviorSequenceModel *behaviorSequence)
    {
      const std::list<BehaviorLayerModelPtr> &layers = behaviorSequence->behaviorsLayer();
      foreach(BehaviorLayerModelPtr layer, layers)
      {
        const std::list<BehaviorKeyFrameModelPtr> &keys = layer->behaviorsKeyFrame();
        foreach(BehaviorKeyFrameModelPtr key, keys)
        {
          if(!loadFlowDiagram(key->diagram().get(), true, key->index(), key->name()))
            return false;
        }
      }
      return true;
    }

    bool BehaviorExecuterPrivate::loadFlowDiagram(FlowDiagramModel *diagram,
                                                  bool behaviorsequence,
                                                  int index,
                                                  const std::string &prefix)
    {

      //Construct a ControlFlow
      if(behaviorsequence)
      {
        qi::BehaviorModel::Node n;
        std::stringstream sindex;
        sindex << index;
        n.uid = diagram->findInstance(0)->uid() + "_controlflowdiagram_" + sindex.str();
        n.factory = "ControlFlowdiagram";
        n.interface = "";//not use
        n.parameters["delay"] = qi::AnyValue(index);
        _model.nodes[n.uid] = n;
      }

      //Loading box
      BoxInstanceModelMap instances = diagram->boxsInstance();
      foreach(BoxInstanceModelMap::value_type &val, instances)
      {
        BoxInstanceModelPtr instance = val.second;

        if(!prefix.empty())
          instance->setName(prefix + "_" + instance->name());

        instance->setName(diagram->findInstance(0)->uid() + "_" + instance->name());

        if(!declaredBox(instance))
          return false;

        qi::BehaviorModel::Node node;

        node.uid = instance->uid();
        node.factory = instance->uid();
        node.interface = "";//not use
        foreach(const boost::shared_ptr<ParameterModel>& param, instance->interface()->parameters())
          node.parameters[param->metaProperty().name()] = param->defaultValue();

        _model.nodes[node.uid] = node;
      }

      //Linking box
      std::list<LinkModelPtr> links = diagram->links();
      foreach(LinkModelPtr link, links)
      {
        int srcid = link->outputTowner();
        int signalid = link->indexOfOutput();
        int dstid = link->inputTowner();
        int methodid = link->indexOfInput();

        BoxInstanceModelPtr src = diagram->findInstance(srcid);
        BoxInstanceModelPtr dst = diagram->findInstance(dstid);

        std::string signal = src->interface()->findSignal(signalid);
        if(signal.empty())
        {
          qiLogError() << "One of the output signal does not exist: " << signalid;
          return false;
        }

        std::string method = dst->interface()->findMethod(methodid);
        if(method.empty())
        {
          boost::shared_ptr<qi::ParameterModel> param = dst->interface()->findParameter(methodid);
          if (param)
          {
            qiLogDebug() << methodid << " recognized as a parameter";
            method = param->metaProperty().name();
          }
          else
          {
            qiLogError() << "One of the input method/property does not exist: " << methodid;
            return false;
          }
        }

        //If signalid is STM input
        const InputModelMap &inputs = src->interface()->inputs();
        InputModelMap::const_iterator it = inputs.find(methodid);
        if( it != inputs.end())
        {
          InputModelPtr input = it->second;
          if( input->nature() == InputModel::InputNature_STMValue)
          {
            //Connect QiCoreMemoryWatcher.almemoryEvent to srcid.handleSTM
            qi::BehaviorModel::Transition tEvent;
            tEvent.src = BehaviorModel::Slot("QiCoreMemoryWatcher", "almemoryEvent");
            tEvent.dst = BehaviorModel::Slot(src->uid(), "handleSTM");
            addTransition(tEvent);

            //add STMValue in the subscriber list
            _stmvalues.push_back(input->stmValueName());
          }
        }

        //If srcid == 0 (srcid is the parent object) and this flowdiagram is part of behavior sequence
        //then connect this flowdiagram with ControlFlowdiagram
        if(srcid == 0 && behaviorsequence)
        {
          qi::BehaviorModel::Transition tStart;
          std::stringstream sindex;
          sindex << index;
          tStart.src = BehaviorModel::Slot(src->uid() + "_controlflowdiagram_" + sindex.str(),
                                       "startFlowdiagram");
          tStart.dst = BehaviorModel::Slot(dst->uid(), method);
          addTransition(tStart);
        }
        else
        {
          qi::BehaviorModel::Transition t;
          t.src = BehaviorModel::Slot(src->uid(), signal);
          t.dst = BehaviorModel::Slot(dst->uid(), method);
          addTransition(t);
        }
      }

      return true;
    }

    void BehaviorExecuterPrivate::addTransition(BehaviorModel::Transition &t)
    {
      static int i = 0;
      std::stringstream ii;
      ii << i;
      t.uid = t.src.first + t.dst.first + ii.str();
      i++;
      _model.transitions[t.uid] = t;
      qiLogDebug() << t.toString();
    }

    void BehaviorExecuterPrivate::initialiseBox(BoxInstanceModelPtr instance, qi::AnyObject timeline, bool rootBox)
    {
      //If FlowDiagram
      qi::AnyReference diagram = instance->content(ContentModel::ContentType_FlowDiagram);
      if(diagram.isValid())
        initialiseFlowDiagram(diagram.ptr<FlowDiagramModel>(), timeline);

      std::map<std::string, int> frames;

      //If animation
      qi::AnyReference behaviorAnimation = instance->content(ContentModel::ContentType_Animation);
      if(behaviorAnimation.isValid())
      {
        qi::AnyObject timeline;
        qi::Timeline* t = new qi::Timeline(_almotion);
        timeline = qi::AnyReference::fromPtr(t).toObject();
        timeline.call<void>("setAnimation",
                             behaviorAnimation.ptr<AnimationModel>());
        _timelines[instance->uid()] = timeline;
      }

      //If behaviorSequence
      qi::AnyReference behaviorSequence = instance->content(ContentModel::ContentType_BehaviorSequence);
      if(behaviorSequence.isValid())
      {
        frames = initialiseBehaviorSequence(behaviorSequence.ptr<BehaviorSequenceModel>(), instance->uid());
      }

      //Set Parameter (todo parameter -> property)
      std::list<ParameterModelPtr> parameters = instance->interface()->parameters();

      foreach (ParameterModelPtr parameter, parameters) {
        std::string name = parameter->metaProperty().name();
        qi::AnyReference value = instance->parameter(parameter->metaProperty().uid());

        std::vector<qi::AnyValue> param;
        param.push_back(qi::AnyValue(name));
        param.push_back(qi::AnyValue(value));
        _behaviorService.call<void>("call", instance->uid(), "setParameter", param);
      }

      if(instance->interface()->hasResource())
      {
        std::list<ResourceModelPtr> resources = instance->interface()->resources();
        std::vector<std::string> resourcesName;

        foreach (ResourceModelPtr resource, resources) {
          resourcesName.push_back(resource->name());
        }
        _alresourcemanager.call<void>("createResource",
                                       _behaviorService.call<std::string>("call",
                                                                           instance->uid(),
                                                                           "getName",
                                                                           std::vector<qi::AnyValue>()).value(),
                                       "");
        _alresourcemanager.call<void>("createResourcesList",
                                       resourcesName,
                                       _behaviorService.call<std::string>("call",
                                                                           instance->uid(),
                                                                           "getName",
                                                                           std::vector<qi::AnyValue>()).value());
      }

      if(instance->interface()->hasTimeline())
      {
        qi::AnyObject timeline = _timelines[instance->uid()];
        std::vector<qi::AnyValue> args;
        args.push_back(qi::AnyValue(timeline));
        args.push_back(qi::AnyValue(frames));

        std::map<int, std::string> framesKey;
        if(!frames.empty())
        {
          foreach(FrameNameKey::value_type &val, frames)
          {
            framesKey[val.second] = val.first;
          }
        }

        timeline.call<void>("setFrames", framesKey);
        timeline.call<void>("setFrameNames", frames);
        _behaviorService.call<void>("call", instance->uid(), "setTimeline", args);
      }

      if(timeline)
      {
        std::vector<qi::AnyObject> args;
        args.push_back(timeline);
        _behaviorService.call<void>("call", instance->uid(), "setParentTimeline", args);
      }

      if (!_behaviorService.call<bool>("call", instance->uid(), "__onLoad__", std::vector<qi::AnyValue>()))
        throw std::runtime_error("A box failed onLoad");

      // connect to end output to know when the behavior is finished
      if (rootBox)
      {
        boost::shared_ptr<BoxInterfaceModel> interface = instance->interface();
        const std::map<int, boost::shared_ptr<OutputModel> >& outputs = interface->outputs();
        for (std::map<int, boost::shared_ptr<OutputModel> >::const_iterator
              iter = outputs.begin();
            iter != outputs.end();
            ++iter)
        {
          // We suppose the object returned by behaviorService is always local
          _behaviorService.call<AnyObject>("object", instance->uid()).value().connect(iter->second->metaSignal().name() + "Signal", boost::function<void(AnyValue)>(boost::bind(&BehaviorExecuterPrivate::onFinished, this, _1)));
        }
      }
    }

    void BehaviorExecuterPrivate::onFinished(AnyValue v)
    {
      _finished.setValue(0);
    }

    void BehaviorExecuterPrivate::onFailed(const std::string& box, const std::string& error)
    {
      _finished.setValue(0);
    }

    void BehaviorExecuterPrivate::connect(qi::AnyObject src,
                                          qi::AnyObject dst,
                                          const std::string &signal,
                                          const std::string &method)
    {
      std::vector<qi::MetaMethod> methods = dst.metaObject().findMethod(method);
      const qi::MetaSignal* sig = src.metaObject().signal(signal);

      qi::Signature sigS = sig->parametersSignature();
      float bestScore = 0.0;
      unsigned best;
      for(unsigned d = 0; d<methods.size(); d++)
      {
        qi::Signature sigD = methods[d].parametersSignature();
        float score = sigS.isConvertibleTo(sigD);

        if(score > bestScore)
        {
          bestScore = score;
          best = d;
        }
      }

      if(bestScore == 0)
        throw std::runtime_error("Could not connect src." + signal + "with dst." + method);

      src.connect(signal, qi::SignalSubscriber(dst, methods[best].uid()));
    }

    std::map<std::string, int> BehaviorExecuterPrivate::initialiseBehaviorSequence(BehaviorSequenceModel* seq,
                                                                                   const std::string &uid)
    {
      std::map<std::string, qi::AnyObject>::iterator it = _timelines.find(uid);
      qi::AnyObject timeline;

      if( it != _timelines.end())
        timeline = it->second;
      else
      {
        //create timeline
        timeline = qi::AnyReference::fromPtr(new qi::Timeline(_almotion)).toObject();
        _timelines[uid] = timeline;
      }
      std::map<std::string, int> ret;
      const std::list<BehaviorLayerModelPtr> &layers = seq->behaviorsLayer();
      foreach(BehaviorLayerModelPtr layer, layers)
      {
        const std::list<BehaviorKeyFrameModelPtr> &frames = layer->behaviorsKeyFrame();
        foreach(BehaviorKeyFrameModelPtr key, frames)
        {
          ret[key->name()] = key->index();

          //Connect timeline with ControlFlowdiagram
          BoxInstanceModelPtr parent = key->diagram()->findInstance(0);
          std::stringstream sindex;
          sindex << key->index();
          std::string uid = parent->uid() + "_controlflowdiagram_" + sindex.str();
          ObjectMap objects = _behaviorService.call<ObjectMap>("objects");
          qi::AnyObject boxdelay = objects[uid];

          connect(timeline, boxdelay, "startFlowdiagram", "startDelay");
          connect(timeline, boxdelay, "stopFlowdiagram", "stop");

          //Load flowdiagram
          initialiseFlowDiagram(key->diagram().get(), timeline);
        }
      }

      return ret;
    }

    void BehaviorExecuterPrivate::initialiseFlowDiagram(FlowDiagramModel* diagram, qi::AnyObject timeline)
    {
      BoxInstanceModelMap instances = diagram->boxsInstance();
      foreach(BoxInstanceModelMap::value_type &instance, instances)
      {
        initialiseBox(instance.second, timeline);
      }
    }

    BehaviorExecuter::BehaviorExecuter(const std::string &dir, boost::shared_ptr<Session> session, bool debug)
      : _p(new BehaviorExecuterPrivate(dir, session, debug))
    {

    }

    BehaviorExecuter::~BehaviorExecuter()
    {
      delete _p;
    }

    void BehaviorExecuter::execute()
    {
      if (!_p->_running.setIfEquals(0, 1))
      {
        qiLogError() << "Cannot execute behavior, already running";
        return;
      }

      //Search start input
      BoxInstanceModelPtr root = _p->_project.rootBox();

      std::string inputStart = root->interface()->findInput(InputModel::InputNature_OnStart);
      if(inputStart.empty()) {
        qiLogError() << "OnStart input not found, impossible to launch behavior.";
        return;
      }

      qiLogDebug() << "Start Behavior " << root->behaviorPath();
      std::vector<qi::AnyValue> args;
      args.push_back(qi::AnyValue("args"));

      _p->_finished = qi::Promise<void>();

      _p->_behaviorService.call<void>("call", root->uid(), inputStart, args);

      if(!_p->_timelines.empty())
      {
        foreach(TimlineMap::value_type &val, _p->_timelines)
        {
          val.second.call<void>("waitForTimelineCompletion");
        }
      }

      _p->_finished.future().wait();
      qiLogDebug() << "Behavior " << root->behaviorPath() << " finished";
      ObjectMap objects = _p->_behaviorService.call<ObjectMap>("objects");
      foreach(ObjectMap::value_type const &val, objects)
      {
        qi::AnyObject o = val.second;
        o.call<void>("__onUnload__");
      }

      _p->_running = 0;
    }

    bool BehaviorExecuter::load()
    {
      if(!_p->_project.loadFromFile())
      {
        qiLogError() << "Failed to load project from file";
        return false;
      }

      BoxInstanceModelPtr root = _p->_project.rootBox();

      if(!root)
      {
        qiLogError() << "No root box";
        return false;
      }

      //Register root box
      BehaviorModel::Node node;
      node.uid = root->uid();
      node.factory = root->uid();
      node.interface = "";//not use
      _p->_model.nodes[node.uid] = node;

      //Register QiCoreMemoryWatcher
      BehaviorModel::Node nodeal;
      nodeal.uid = "QiCoreMemoryWatcher";
      nodeal.factory = "QiCoreMemoryWatcher";
      nodeal.interface = "";
      _p->_model.nodes[nodeal.uid] = nodeal;

      if(!_p->declaredBox(root))
      {
        qiLogError() << "Failed to declare root box";
        return false;
      }

      try {
        _p->_behaviorService.call<void>("setModel", _p->_model);
        _p->_behaviorService.call<void>("loadObjects", _p->_debug);
        _p->_behaviorService.call<void>("setTransitions", _p->_debug, (int)qi::MetaCallType_Direct);
      } catch (std::exception& e) {
        qiLogError() << "A box failed to load: " << e.what();
        return false;
      }

      //QiCoreMemoryWatcher set watching value
      ObjectMap objects = _p->_behaviorService.call<ObjectMap>("objects");
      qi::AnyObject qiCoreMemoryWatcher = objects["QiCoreMemoryWatcher"];
      foreach(std::string const &s, _p->_stmvalues)
      {
        qiCoreMemoryWatcher.call<void>("subscribe", s);
      }

      try {
        _p->initialiseBox(root, qi::AnyObject(), true);
      } catch (std::exception& e) {
        qiLogError() << "Failed to initialise boxes: " << e.what();
        return false;
      }

      //Set ObjectThreadingModel to prevent deadlock.
      //Infact, A.method trigger A.signal (so A is locked)
      //        B.method is a callback of A.signal and B.method trigger B.signal
      //        A.method2 is a callback of B.signal but A is already lock so deadlock
      foreach(ObjectMap::value_type const &val, objects)
      {
        qi::AnyObject obj = val.second;
        DynamicObject* dynamic = reinterpret_cast<DynamicObject*>(obj.asGenericObject()->value);
        dynamic->setThreadingModel(ObjectThreadingModel_MultiThread);
      }

      return true;
    }
  }
}
