/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/


#ifndef QICORE_BEHAVIOR_H_
# define QICORE_BEHAVIOR_H_

#include <qicore/api.hpp>
#include <qi/anyvalue.hpp>
#include <qi/anyobject.hpp>
#include <qi/session.hpp>
#include <qi/type/objecttypebuilder.hpp>
#include <qi/anymodule.hpp>

#ifdef interface
  #undef interface
#endif
namespace qilang {
  class Node;
  typedef boost::shared_ptr<Node> NodePtr;
  typedef std::vector<NodePtr>    NodePtrVector;
}

namespace qi {

  struct QICORE_API BehaviorModel
  {
    typedef std::pair<std::string, std::string> Slot; // objectUid.methodName
    typedef std::map<std::string, qi::AnyValue> ParameterMap;

    struct QICORE_API Node
    {
      std::string  uid;
      std::string  interface;
      std::string  factory;
      ParameterMap parameters;
    };

    struct QICORE_API Transition
    {
      std::string uid;
      Slot src; // uid.slot
      Slot dst; // uid.slot
      AnyValue filter; // will probably change
      std::string toString()
      {
        return src.first + '.' + src.second + " -> " + dst.first + '.' + dst.second;
      }
    };

    typedef std::map<std::string, Node>       NodeMap;
    typedef std::map<std::string, Transition> TransitionMap;
    NodeMap       nodes;
    TransitionMap transitions;
    std::string   name;
    void load(std::istream& is);
    void clear();
  };

  QICORE_API BehaviorModel loadBehaviorModel(const qilang::NodePtrVector& nodes);
  QICORE_API void displayModel(const BehaviorModel& model);

}

QI_TYPE_STRUCT(::qi::BehaviorModel, nodes, transitions, name);
QI_TYPE_STRUCT(::qi::BehaviorModel::Node, uid, interface, factory);
QI_TYPE_STRUCT(::qi::BehaviorModel::Transition, uid, src, dst, filter);


namespace qi {

  class QICORE_API Behavior
  {
  public:
    typedef std::map<std::string, AnyObject> ObjectMap;
    /* Two transision modes:
     * - Direct: link is a link to a connect(source, target, targetMethod)
     * - Intercept: debug mode, link is a connect(source, &Behavior::transition)
     */
    struct TransitionPtr
    {
      AnyObject source;
      AnyObject target;
      SignalLink link;
      std::string property; // target property name
      unsigned int targetMethod; // or target method id
      bool debug;
    };
    typedef std::map<std::string, TransitionPtr> TransitionMap;

  public:
    Behavior(qi::SessionPtr session): _session(session) {}
    ~Behavior() { }
    AnyObject makeObject(const std::string& model, const std::string& factory, const qi::BehaviorModel::ParameterMap& params);
    void loadObjects(bool debugmode);
    void unloadObjects();
    void setTransitions(bool debugmode, qi::MetaCallType type = qi::MetaCallType_Auto);
    void removeTransitions();
    void loadFile(const std::string& path);
    void loadString(const std::string& data);
    AnyObject object(const std::string& name);
    qi::AnyValue call(const std::string& objUid, const std::string& fun, std::vector<qi::AnyValue> args);

    /// Triggered when a transition occurrs, if transitions were set in debug mode.
    qi::Signal<const std::string&, qi::AnyValue> onTransition;
    /// Triggered when one of the tasks enter/leave an error state
    qi::Signal<const std::string&, const std::string&> onTaskError;
    /// Triggered when one of the tasks's running property changes state
    qi::Signal<const std::string&, bool> onTaskRunning;

  public:
    void setModel(const qi::BehaviorModel &model) { _model = model; }
    qi::SessionPtr     session()     { return _session;};
    qi::BehaviorModel& model()       { return _model; };
    ObjectMap&         objects()     { return _objects; };
    TransitionMap&     transitions() { return _transitions; };
    //TODO: DOES NOT BUILD WITH BOTH const and non const
    //const BehaviorModel&   model() const       { return _model; };
    //const ObjectMap&       objects() const     { return _objects; };
    //const TransitionMap&   transitions() const { return _transitions; };

  public:
    void start() { onStart(); }
    void stop() {}

    qi::Signal<void> onStart;
    qi::Signal<void> onStop;

  private:
    void transition(qi::AnyValue argument, const std::string& transitionId);

  private:
    typedef qi::SignalLink Link;
    // hooks on running/error
    typedef std::map<std::string, std::pair<SignalLink, SignalLink> > ObjectLinks;
    ObjectLinks       _objectLinks;
    qi::BehaviorModel _model;
    ObjectMap         _objects;
    TransitionMap     _transitions;
    qi::SessionPtr    _session;
  };

  QI_REGISTER_OBJECT(Behavior, loadObjects, unloadObjects, setTransitions, removeTransitions, loadFile, loadString, call, onTransition, model, objects, transitions, setModel, onTaskRunning, onTaskError, object, start, stop, onStart, onStop);

}



#endif
