/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

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
#include <qitype/objectfactory.hpp>
#include <qitype/dynamicobject.hpp>
#include "behaviorexecuter_p.hpp"

#include <boost/python.hpp>
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
    BehaviorExecuterPrivate::BehaviorExecuterPrivate(const std::string &dir, Session &session, bool debug) :
      _project(dir),
      _debug(debug)
    {
      std::stringstream p;
      p << session.url().port();

      _pythonCreator = PythonBoxLoader();
      _pythonCreator.initPython(session.url().host(), p.str(), dir);
      _behaviorService = session.service("BehaviorService").value()->call<qi::AnyObject>("create");
      _alresourcemanager = session.service("ALResourceManager");
      _almotion = session.service("ALMotion");
      _almemory = session.service("ALMemory");
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
          if(!loadFlowDiagram(key->diagram().get(), true, key->index()))
            return false;
        }
      }
      return true;
    }

    bool BehaviorExecuterPrivate::loadFlowDiagram(FlowDiagramModel *diagram, bool behaviorsequence, int index)
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
      foreach(BoxInstanceModelMap::value_type &instance, instances)
      {
        if(!declaredBox(instance.second))
          return false;

        qi::BehaviorModel::Node node;

        node.uid = instance.second->uid();
        node.factory = instance.second->uid();
        node.interface = "";//not use
        //TODO node.parameters

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
          return false;

        std::string method = dst->interface()->findMethod(methodid);
        if(method.empty())
          return false;

        //If srcid == 0 (srcid is the parent object) and this flowdiagram is part of behavior sequence
        //then connect this flowdiagram with ControlFlowdiagram
        if(srcid == 0 && behaviorsequence)
        {
          qi::BehaviorModel::Transition tStart;
          std::stringstream sindex;
          sindex << index;
          //Set transition for start flowdiagram in a behaviorsequence.
          tStart.src = BehaviorModel::Slot(src->uid() + "_controlflowdiagram_" + sindex.str(),
                                       "startFlowdiagram");
          tStart.dst = BehaviorModel::Slot(dst->uid(), method);


          std::string onStopMethod = dst->interface()->findInput(InputModel::InputNature_OnStop);
          qi::BehaviorModel::Transition tStop;
          tStop.src = BehaviorModel::Slot(src->uid() + "_controlflowdiagram_" + sindex.str(),
                                          "stopFlowdiagram");
          tStop.dst = BehaviorModel::Slot(dst->uid(), onStopMethod);

          //Set transition for stop flowdiagram in a behaviorsequence (if onStop input exist)
          if(!onStopMethod.empty())
            addTransition(tStop);
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

    void BehaviorExecuterPrivate::initialiseBox(BoxInstanceModelPtr instance)
    {
      //If FlowDiagram
      qi::AnyReference diagram = instance->content(ContentModel::ContentType_FlowDiagram);
      if(diagram.isValid())
        initialiseFlowDiagram(diagram.ptr<FlowDiagramModel>());

      std::map<std::string, int> frames;

      //If animation
      qi::AnyReference behaviorAnimation = instance->content(ContentModel::ContentType_Animation);
      if(behaviorAnimation.isValid())
      {
        qi::AnyObject timeline;
        qi::Timeline* t = new qi::Timeline(_almotion, _pythonCreator.getInterpreter());
        timeline = qi::AnyReference::fromPtr(t).toObject();
        timeline->call<void>("setAnimation",
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
        _behaviorService->call<void>("call", instance->uid(), "setParameter", param);
      }

      if(instance->interface()->hasResource())
      {
        std::list<ResourceModelPtr> resources = instance->interface()->resources();
        std::vector<std::string> resourcesName;

        foreach (ResourceModelPtr resource, resources) {
          resourcesName.push_back(resource->name());
        }
        _alresourcemanager->call<void>("createResource",
                                       _behaviorService->call<std::string>("call",
                                                                           instance->uid(),
                                                                           "getName",
                                                                           std::vector<qi::AnyValue>()).value(),
                                       "");
        _alresourcemanager->call<void>("createResourcesList",
                                       resourcesName,
                                       _behaviorService->call<std::string>("call",
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

        timeline->call<void>("setFrames", framesKey);
        _behaviorService->call<void>("call", instance->uid(), "setTimeline", args);
      }

      _behaviorService->call<void>("call", instance->uid(), "__onLoad__", std::vector<qi::AnyValue>());
    }

    void BehaviorExecuterPrivate::connect(qi::AnyObject src,
                                          qi::AnyObject dst,
                                          const std::string &signal,
                                          const std::string &method)
    {
      std::vector<qi::MetaMethod> methods = dst->metaObject().findMethod(method);
      const qi::MetaSignal* sig = src->metaObject().signal(signal);

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

      src->connect(signal, qi::SignalSubscriber(dst, methods[best].uid()));
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
        timeline = qi::AnyReference::fromPtr(new qi::Timeline(_almotion, _pythonCreator.getInterpreter())).toObject();
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
          ObjectMap objects = _behaviorService->call<ObjectMap>("objects");
          qi::AnyObject boxdelay = objects[uid];

          connect(timeline, boxdelay, "startFlowdiagram", "startDelay");
          connect(timeline, boxdelay, "stopFlowdiagram", "stop");

          //Load flowdiagram
          initialiseFlowDiagram(key->diagram().get());
        }
      }

      return ret;
    }

    void BehaviorExecuterPrivate::initialiseFlowDiagram(FlowDiagramModel* diagram)
    {
      BoxInstanceModelMap instances = diagram->boxsInstance();
      foreach(BoxInstanceModelMap::value_type &instance, instances)
      {
        initialiseBox(instance.second);
      }
    }

    BehaviorExecuter::BehaviorExecuter(const std::string &dir, Session &session, bool debug)
      : _p(new BehaviorExecuterPrivate(dir, session, debug))
    {

    }

    BehaviorExecuter::~BehaviorExecuter()
    {
      _p->_pythonCreator.terminate();
      delete _p;
    }

    void BehaviorExecuter::execute()
    {
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
      _p->_behaviorService->call<void>("call", root->uid(), inputStart, args);

      if(!_p->_timelines.empty())
      {
        foreach(TimlineMap::value_type &val, _p->_timelines)
        {
          val.second->call<void>("waitForTimelineCompletion");
        }
      }
      qiLogDebug() << "Stop Behavior " << root->behaviorPath();

      qiLogDebug() << "Unload box";
      _p->_pythonCreator.switchMainThread();
      ObjectMap objects = _p->_behaviorService->call<ObjectMap>("objects");
      foreach(ObjectMap::value_type const &val, objects)
      {
        qi::AnyObject o = val.second;
        o->call<void>("__onUnload__");
      }
    }

    bool BehaviorExecuter::load()
    {
      if(!_p->_project.loadFromFile())
        return false;

      BoxInstanceModelPtr root = _p->_project.rootBox();

      if(!root)
        return false;

      //Register root box
      BehaviorModel::Node node;
      node.uid = root->uid();
      node.factory = root->uid();
      node.interface = "";//not use

      _p->_model.nodes[node.uid] = node;

      if(!_p->declaredBox(root))
        return false;

      _p->_behaviorService->call<void>("setModel", _p->_model);
      _p->_behaviorService->call<void>("loadObjects", _p->_debug);
      _p->_behaviorService->call<void>("setTransitions", _p->_debug, (int)qi::MetaCallType_Direct);

      _p->initialiseBox(root);

      //Set ObjectThreadingModel to prevent deadlock.
      //Infact, A.method trigger A.signal (so A is locked)
      //        B.method is a callback of A.signal and B.method trigger B.signal
      //        A.method2 is a callback of B.signal but A is already lock so deadlock
      ObjectMap objects = _p->_behaviorService->call<ObjectMap>("objects");
      foreach(ObjectMap::value_type const &val, objects)
      {
        qi::AnyObject obj = val.second;
        DynamicObject* dynamic = reinterpret_cast<DynamicObject*>(obj->value);
        dynamic->setThreadingModel(ObjectThreadingModel_MultiThread);
      }

      return true;
    }
  }
}
