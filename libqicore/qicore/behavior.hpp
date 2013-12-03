/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/


#ifndef QICORE_BEHAVIOR_H_
# define QICORE_BEHAVIOR_H_

#include <qicore/api.hpp>
#include <qitype/anyvalue.hpp>

#ifdef interface
  #undef interface
#endif

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

    typedef std::map<std::string, Node> NodeMap;
    typedef std::map<std::string, Transition> TransitionMap;
    NodeMap       nodes;
    TransitionMap transitions;

    void load(std::istream& is);
    void clear();
  };
}

QI_TYPE_STRUCT(::qi::BehaviorModel, nodes, transitions);
QI_TYPE_STRUCT(::qi::BehaviorModel::Node, uid, interface, factory);
QI_TYPE_STRUCT(::qi::BehaviorModel::Transition, uid, src, dst, filter);

#endif
