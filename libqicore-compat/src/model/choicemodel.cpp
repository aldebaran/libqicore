/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/choicemodel.hpp>
#include "choicemodel_p.hpp"

namespace qi {

  //----------------------------------------Private Class------------------------------------------//

  ChoiceModelPrivate::ChoiceModelPrivate(const GenericValue &value, ParameterModel::ContentType type)
  {
    _type = type;
    _value = value;
  }

  ChoiceModelPrivate::ChoiceModelPrivate(boost::shared_ptr<const AL::XmlElement> elt,
                                         ParameterModel::ContentType type) :
    _value(),
    _type(type)
  {
    switch(_type)
    {
    case ParameterModel::ContentType_Bool:
      bool valueBool;
      elt->getAttribute("value", valueBool);
      _value = GenericValue(valueBool);
      break;

    case ParameterModel::ContentType_Int:
      int valueInt;
      elt->getAttribute("value", valueInt);
      _value = GenericValue(valueInt);
      break;

    case ParameterModel::ContentType_Double:
      double valueDouble;
      elt->getAttribute("value", valueDouble);
      _value = GenericValue(valueDouble);
      break;

    case ParameterModel::ContentType_String:
    case ParameterModel::ContentType_Ressource:
    {
      std::string valueString;
      elt->getAttribute("value", valueString);
      _value = GenericValue(valueString);
      break;
    }

    case ParameterModel::ContentType_Error:
      break;
    }
  }

  //-----------------------------------------Public Class------------------------------------------//

  ChoiceModel::ChoiceModel(bool value) :
    _p(new ChoiceModelPrivate(GenericValue(value), ParameterModel::ContentType_Bool))
  {
  }

  ChoiceModel::ChoiceModel(int value) :
    _p(new ChoiceModelPrivate(GenericValue(value), ParameterModel::ContentType_Int))
  {
  }

  ChoiceModel::ChoiceModel(double value) :
    _p(new ChoiceModelPrivate(GenericValue(value), ParameterModel::ContentType_Double))
  {
    qiLogDebug("QICore") << "Double Choice" << std::endl;
  }

  ChoiceModel::ChoiceModel(const std::string &value) :
    _p(new ChoiceModelPrivate(GenericValue(value), ParameterModel::ContentType_String))
  {
  }

  ChoiceModel::ChoiceModel(boost::shared_ptr<const AL::XmlElement> elt, ParameterModel::ContentType type) :
    _p(new ChoiceModelPrivate(elt, type))
  {
  }

  ChoiceModel::~ChoiceModel()
  {
    delete _p;
  }

  bool ChoiceModel::getValueBool()
  {
    return _p->_value.to<bool>();
  }

  int ChoiceModel::getValueInt()
  {
    return _p->_value.toInt();
  }

  double ChoiceModel::getValueDouble()
  {
    return _p->_value.toDouble();
  }

  std::string ChoiceModel::getValueString()
  {
    return _p->_value.toString();
  }

  int ChoiceModel::getType()
  {
    return _p->_type;
  }

  void ChoiceModel::setValueBool(bool value)
  {
    _p->_type  = ParameterModel::ContentType_Bool;
    _p->_value = GenericValue(value);
  }

  void ChoiceModel::setValueInt(int value)
  {
    _p->_type  = ParameterModel::ContentType_Int;
    _p->_value = GenericValue(value);
  }

  void ChoiceModel::setValueDouble(double value)
  {
    _p->_type  = ParameterModel::ContentType_Double;
    _p->_value = GenericValue(value);
  }

  void ChoiceModel::setValueString(const std::string& value)
  {
    _p->_type  = ParameterModel::ContentType_String;
    _p->_value = GenericValue(value);
  }

}
