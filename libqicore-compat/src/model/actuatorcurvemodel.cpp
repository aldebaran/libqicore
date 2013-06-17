/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/actuatorcurvemodel.hpp>
#include "xmlutils.hpp"
#include "actuatorcurvemodel_p.hpp"

#include "math.h"
#include <limits>

namespace qi
{
  ActuatorCurveModelPrivate::ActuatorCurveModelPrivate(const std::string &name, const std::string &actuator, bool recordable, bool mute, int unit, const KeyModelMap &keys) :
    _name(name),
    _actuator(actuator),
    _recordable(recordable),
    _mute(mute),
    _unit(unit),
    _keys(keys),
    _isValid(true)
  {
    _lastKeyFrame = -1;
    for(std::map<int, KeyModelPtr>::const_iterator it = _keys.begin(), itEnd = _keys.end(); it != itEnd; ++it)
    {
      if(it->first > _lastKeyFrame)
        _lastKeyFrame = it->first;
    }
  }

  ActuatorCurveModelPrivate::ActuatorCurveModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    std::string unit;
    elt->getAttribute("name",       _name);
    elt->getAttribute("actuator",   _actuator);
    elt->getAttribute("recordable", _recordable);
    elt->getAttribute("mute",       _mute);
    elt->getAttribute("unit",       unit);

    if(unit == "None")
    {
      _unit = ActuatorCurveModel::UnitType_Undefined;
    }
    else
    {
      elt->getAttribute("unit",       _unit);
    }

    AL::XmlElement::CList keys = elt->children("Key", "");

    _isValid = true;
    _lastKeyFrame = -1;
    for(AL::XmlElement::CList::const_iterator it = keys.begin(), itEnd = keys.end(); it != itEnd; ++it)
    {
      KeyModelPtr key = KeyModelPtr(new KeyModel(*it));

      _isValid = key->isValid();
      if(!_isValid)
        break;

      int keyFrame = key->frame();
      _keys.insert(std::pair<int, KeyModelPtr>(keyFrame, key));

      if(keyFrame > _lastKeyFrame)
        _lastKeyFrame = keyFrame;
    }
  }

  ActuatorCurveModel::ActuatorCurveModel(const std::string &name, const std::string &actuator, bool recordable, bool mute, UnitType unit, const std::map<int, KeyModelPtr> &keys) :
    _p(new ActuatorCurveModelPrivate(name, actuator, recordable, mute, unit, keys))
  {

  }

  ActuatorCurveModel::ActuatorCurveModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new ActuatorCurveModelPrivate(elt))
  {
  }

  ActuatorCurveModel::~ActuatorCurveModel()
  {
    delete _p;
  }

  const std::string& ActuatorCurveModel::name() const
  {
    return _p->_name;
  }

  const std::string& ActuatorCurveModel::actuator() const
  {
    return _p->_actuator;
  }

  bool ActuatorCurveModel::recordable() const
  {
    return _p->_recordable;
  }

  bool ActuatorCurveModel::mute() const
  {
    return _p->_mute;
  }

  ActuatorCurveModel::UnitType ActuatorCurveModel::unit() const
  {
    return static_cast<ActuatorCurveModel::UnitType>( _p->_unit );
  }

  const std::map<int, KeyModelPtr>& ActuatorCurveModel::keys() const
  {
    return _p->_keys;
  }

  int ActuatorCurveModel::lastKeyFrame() const
  {
    return _p->_lastKeyFrame;
  }

  bool ActuatorCurveModel::isValid() const
  {
    return _p->_isValid;
  }

  void ActuatorCurveModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void ActuatorCurveModel::setActuator(const std::string& actuator)
  {
    _p->_actuator = actuator;
  }

  void ActuatorCurveModel::setRecordable(bool recordable)
  {
    _p->_recordable = recordable;
  }

  void ActuatorCurveModel::setMute(bool mute)
  {
    _p->_mute = mute;
  }

  void ActuatorCurveModel::setUnit(UnitType unit)
  {
    _p->_unit = unit;
  }

  void ActuatorCurveModel::addKey(KeyModelPtr key)
  {
    if(!key->isValid())
      return;

    _p->_keys.insert(std::pair<int, KeyModelPtr>(key->frame(), key));

    if(key->frame() > _p->_lastKeyFrame)
      _p->_lastKeyFrame = key->frame();
  }
}
