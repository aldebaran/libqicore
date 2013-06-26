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

#include <qicore-compat/behaviorexecuter.hpp>
#include <qitype/objectfactory.hpp>
#include <qitype/dynamicobject.hpp>
#include "behaviorexecuter_p.hpp"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.BehaviorExecuter");

namespace qi
{
  namespace compat
  {
    typedef std::map<std::string, qi::AnyObject> ObjectMap;
    BehaviorExecuterPrivate::BehaviorExecuterPrivate(const std::string &dir, Session &session, bool debug) :
      _project(dir),
      _debug(debug)
    {
      std::stringstream p;
      p << session.url().port();

      _pythonCreator = PythonBoxLoader();
      _pythonCreator.initPython(session.url().host(), p.str());
      _behaviorService = session.service("BehaviorService").value()->call<qi::AnyObject>("create");
      _alresourcemanager = session.service("ALResourceManager");
    }

    bool BehaviorExecuterPrivate::declaredBox(BoxInstanceModelPtr instance)
    {
      //If FlowDiagram
      qi::AnyReference diagram = instance->content(ContentModel::ContentType_FlowDiagram);
      if(diagram.isValid())
        if(!loadFlowDiagram(diagram.ptr<FlowDiagramModel>()))
          return false;

      //TODO animation
      //TODO behaviorSequence

      //Box no script or python box is same
      return declaredPythonBox(instance);

    }

    bool BehaviorExecuterPrivate::declaredPythonBox(BoxInstanceModelPtr instance)
    {
      return _pythonCreator.registerPythonClass(instance);
    }

    bool BehaviorExecuterPrivate::loadFlowDiagram(FlowDiagramModel *diagram)
    {
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
      int i = 0;
      foreach(LinkModelPtr link, links)
      {
        qi::BehaviorModel::Transition t;
        int srcid = link->outputTowner();
        int signalid = link->indexOfOutput();
        int dstid = link->inputTowner();
        int methodid = link->indexOfInput();

        BoxInstanceModelPtr src = diagram->findInstance(srcid);
        BoxInstanceModelPtr dst = diagram->findInstance(dstid);

        std::string signal = src->interface()->findSignal(signalid);
        if(signal.empty())
          return false;

        t.src = BehaviorModel::Slot(src->uid(), signal);

        std::string method = dst->interface()->findMethod(methodid);
        if(method.empty())
          return false;

        t.dst = BehaviorModel::Slot(dst->uid(), method);

        std::stringstream ii;
        ii << i;
        t.uid = src->uid() + dst->uid() + ii.str();
        i++;
        _model.transitions[t.uid] = t;
        qiLogDebug() << t.toString();
      }

      return true;
    }

    bool BehaviorExecuterPrivate::initialiseBox(BoxInstanceModelPtr instance)
    {
      //If FlowDiagram
      qi::AnyReference diagram = instance->content(ContentModel::ContentType_FlowDiagram);
      if(diagram.isValid())
        initialiseFlowDiagram(diagram.ptr<FlowDiagramModel>());

      //TODO animation
      //TODO behaviorSequence

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

        _alresourcemanager->call<void>("createResourcesList" ,
                                       resourcesName,
                                       _behaviorService->call<std::string>("call", instance->uid(), "getName", std::vector<qi::AnyValue>()).value());
      }

      _behaviorService->call<void>("call", instance->uid(), "__onLoad__", std::vector<qi::AnyValue>());
      return true;
    }

    bool BehaviorExecuterPrivate::initialiseFlowDiagram(FlowDiagramModel* diagram)
    {
      BoxInstanceModelMap instances = diagram->boxsInstance();
      foreach(BoxInstanceModelMap::value_type &instance, instances)
      {
        if(!initialiseBox(instance.second))
          return false;
      }
      return true;
    }

    BehaviorExecuter::BehaviorExecuter(const std::string &dir, Session &session, bool debug)
      : _p(new BehaviorExecuterPrivate(dir, session, debug))
    {

    }

    BehaviorExecuter::~BehaviorExecuter()
    {
      delete _p;
    }

    void BehaviorExecuter::execute()
    {
      //Search start input
      int inputid = -1;
      float best = 0.0;
      BoxInstanceModelPtr root = _p->_project.rootBox();
      const InputModelMap& inputs = root->interface()->inputs();
      foreach(const InputModelMap::value_type &input, inputs)
      {
        float score = input.second->metaMethod().parametersSignature().isConvertibleTo(qi::Signature("()"));
        if(input.second->nature() == InputModel::InputNature_OnStart &&
           score > best)
        {
          best = score;
          inputid = input.first;
        }
      }

      if(inputid == -1 || best == 0.0f) {
        qiLogError() << "OnStart input not found, impossible to launch behavior.";
        return;
      }

      qiLogDebug() << "Start Behavior " << root->behaviorPath();
      std::vector<qi::AnyValue> args;
      args.push_back(qi::AnyValue("args"));
      _p->_behaviorService->call<void>("call", root->uid(), root->interface()->findMethod(inputid), args);
      qiLogDebug() << "Stop Behavior " << root->behaviorPath();

      qiLogDebug() << "Unload box";
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
      _p->_behaviorService->call<void>("loadObjects");
      _p->_behaviorService->call<void>("setTransitions", _p->_debug, qi::MetaCallType_Direct);

      if(!_p->initialiseBox(root))
        return false;

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
