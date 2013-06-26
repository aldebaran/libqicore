/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#ifndef BEHAVIORLOADER_P_HPP_
#define BEHAVIORLOADER_P_HPP_

#include <qimessaging/session.hpp>
#include <qitype/anyobject.hpp>
#include <qicore/behavior.hpp>
#include <qicore-compat/model/flowdiagrammodel.hpp>
#include <qicore-compat/model/choregrapheprojectmodel.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include "python/pythonloader.hpp"
#include "asyncexecuter.hpp"

namespace qi
{
  namespace compat {
    class BehaviorExecuterPrivate
    {
      friend class BehaviorExecuter;
    public:
      BehaviorExecuterPrivate(const std::string &dir, qi::Session &session, bool debug);

      bool loadFlowDiagram(FlowDiagramModel *diagram);
      bool declaredBox(BoxInstanceModelPtr instance);
      bool declaredPythonBox(BoxInstanceModelPtr instance);
      bool initialiseBox(BoxInstanceModelPtr instance);
      bool initialiseFlowDiagram(FlowDiagramModel* diagram);

    private:
      ChoregrapheProjectModel _project;
      qi::BehaviorModel _model;
      qi::PythonBoxLoader _pythonCreator;
      qi::AnyObject _behaviorService;
      qi::AnyObject _alresourcemanager;
      boost::mutex _waiter;
      boost::condition_variable _waitcondition;
      bool _debug;
    };
  }
}

#endif
