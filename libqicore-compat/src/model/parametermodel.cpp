/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <sstream>
#include <qicore-compat/model/choicemodel.hpp>
#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/parametervaluemodel.hpp>

#include "xmlutils.hpp"
#include "parametermodel_p.hpp"

namespace qi
{
  //--------------------------Private Class-------------------------------------//
  ParameterModelPrivate::ParameterModelPrivate()
  {
    int default_value = 0;
    _defaultValue = GenericValue(default_value);
    _min = GenericValue(default_value);
    _max = GenericValue(default_value);
    _contentType = ParameterModel::ContentType_Error;
  }




  ParameterModelPrivate::ParameterModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    int contentType;
    elt->getAttribute("name",                 _name);
    elt->getAttribute("inherits_from_parent", _inheritsFromParent);
    elt->getAttribute("content_type",         contentType);

    switch(contentType)
    {
    case ParameterModel::ContentType_Bool:
      bool default_value_bool;

      _contentType = ParameterModel::ContentType_Bool;
      elt->getAttribute("default_value",    default_value_bool);
      _defaultValue = GenericValue(default_value_bool);
      break;

    case ParameterModel::ContentType_Int:
      int default_value_int, min_int, max_int;

      _contentType = ParameterModel::ContentType_Int;
      elt->getAttribute("default_value",    default_value_int);
      elt->getAttribute("min",              min_int, 0);
      elt->getAttribute("max",              max_int, 0);
      _defaultValue  = GenericValue(default_value_int);
      _min           = GenericValue(min_int);
      _max           = GenericValue(max_int);
      break;

    case ParameterModel::ContentType_Double:
      double default_value_double, min_double, max_double;

      _contentType = ParameterModel::ContentType_Double;
      elt->getAttribute("default_value",    default_value_double);
      elt->getAttribute("min",              min_double, 0.0);
      elt->getAttribute("max",              max_double, 0.0);
      _defaultValue  = GenericValue(default_value_double);
      _min           = GenericValue(min_double);
      _max           = GenericValue(max_double);
      break;

    case ParameterModel::ContentType_String:
    {
      std::string default_value_string;

      _contentType = ParameterModel::ContentType_String;
      elt->getAttribute("default_value",    default_value_string);
      _defaultValue = GenericValue(default_value_string);
      break;
    }

    case ParameterModel::ContentType_Ressource:
    {
      std::string default_value_ressource;

      _contentType = ParameterModel::ContentType_Ressource;
      elt->getAttribute("default_value",    default_value_ressource);
      _defaultValue = GenericValue(default_value_ressource);
      break;
    }

    default:
      _contentType = ParameterModel::ContentType_Error;
      break;
    }

    elt->getAttribute("custom_choice",        _customChoice, false);
    elt->getAttribute("password",             _password, false);
    elt->getAttribute("tooltip",              _tooltip);
    elt->getAttribute("id",                   _id);

    AL::XmlElement::CList choices = elt->children("Choice", "");

    if(!choices.empty())
    {
      for (AL::XmlElement::CList::const_iterator it=choices.begin(), itEnd=choices.end(); it!=itEnd; ++it)
      {
        boost::shared_ptr<const AL::XmlElement> elt = *it;
        boost::shared_ptr<ChoiceModel> choice(new ChoiceModel(elt, _contentType));
        _choices.push_front(choice);
      }
    }



  }

  //------------------------------------Public Class-------------------------------//
  ParameterModel::ParameterModel() :
    _p(new ParameterModelPrivate())
  {
  }

  ParameterModel::ParameterModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new ParameterModelPrivate(elt))
  {
  }

  ParameterModel::~ParameterModel()
  {
    delete _p;
  }

  //-------------------------------------Getter----------------------------------//
  const std::string& ParameterModel::getName() const
  {
    return _p->_name;
  }

  bool ParameterModel::getInheritsFromParent() const
  {
    return _p->_inheritsFromParent;
  }

  ParameterModel::ContentType ParameterModel::getContentType() const
  {
    return _p->_contentType;
  }

  bool ParameterModel::getDefaultValueBool() const
  {
    return _p->_defaultValue.to<bool>();
  }

  int ParameterModel::getDefaultValueInt() const
  {
    return _p->_defaultValue.toInt();
  }

  int ParameterModel::getMinInt() const
  {
    return _p->_min.toInt();
  }

  int ParameterModel::getMaxInt() const
  {
    return _p->_max.toInt();
  }

  double ParameterModel::getDefaultValueDouble() const
  {
    return _p->_defaultValue.toDouble();
  }

  double ParameterModel::getMinDouble() const
  {
    return _p->_min.toDouble();
  }

  double ParameterModel::getMaxDouble() const
  {
    return _p->_max.toDouble();
  }

  std::string ParameterModel::getDefaultValueString() const
  {
    return _p->_defaultValue.toString();
  }

  std::string ParameterModel::getDefaultValueRessource() const
  {
    return _p->_defaultValue.toString();
  }

  bool ParameterModel::getCustomChoice() const
  {
    return _p->_customChoice;
  }

  bool ParameterModel::getPassword() const
  {
    return _p->_password;
  }

  const std::string& ParameterModel::getTooltip() const
  {
    return _p->_tooltip;
  }

  int ParameterModel::getId() const
  {
    return _p->_id;
  }

  //-------------------------------Setter------------------------------------//
  void ParameterModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void ParameterModel::setInheritsFromParent(bool inherits_from_parent)
  {
    _p->_inheritsFromParent = inherits_from_parent;
  }

  void ParameterModel::setDefaultValueBool(bool default_value)
  {
    _p->_contentType  = ContentType_Bool;
    _p->_defaultValue = GenericValue(default_value);
    _p->_choices.clear();
  }

  bool ParameterModel::setDefaultValueInt(int default_value)
  {
    if(_p->inInterval<int>(default_value, _p->_min.toInt(), _p->_max.toInt()) &&
       _p->_contentType == ContentType_Int )
    {
      _p->_contentType  = ContentType_Int;
      _p->_defaultValue = GenericValue(default_value);
      _p->_choices.clear();
      return true;
    }
    else if(_p->_contentType != ContentType_Int)
    {
      _p->_contentType  = ContentType_Int;
      _p->_defaultValue = GenericValue(default_value);
      _p->_choices.clear();
      setMaxInt(default_value);
      setMinInt(default_value);
      return true;
    }
    return false;
  }

  bool ParameterModel::setMinInt(int min)
  {
    if(_p->_contentType != ParameterModel::ContentType_Int)
      return false;

    if(!_p->inInterval<int>(_p->_defaultValue.toInt(), min, _p->_max.toInt()))
      return false;

    _p->_min = GenericValue(min);
    return true;
  }

  bool ParameterModel::setMaxInt(int max)
  {
    if(_p->_contentType != ParameterModel::ContentType_Int)
      return false;

    if(!_p->inInterval<int>(_p->_defaultValue.toInt(), _p->_min.toInt(), max))
      return false;

    _p->_max = GenericValue(max);
    return true;
  }

  bool ParameterModel::setDefaultValueDouble(double default_value)
  {
    if(_p->inInterval<double>(default_value, _p->_min.toDouble(), _p->_max.toDouble()) &&
       _p->_contentType == ContentType_Double)
    {
      _p->_contentType  = ContentType_Double;
      _p->_defaultValue = GenericValue(default_value);
      _p->_choices.clear();
      return true;
    }
    else if(_p->_contentType != ParameterModel::ContentType_Double)
    {
      _p->_contentType  = ContentType_Double;
      _p->_defaultValue = GenericValue(default_value);
      _p->_choices.clear();
      setMaxDouble(default_value);
      setMinDouble(default_value);
    }
    return false;
  }

  bool ParameterModel::setMinDouble(double min)
  {
    if(_p->_contentType != ParameterModel::ContentType_Double)
      return false;

    if(!_p->inInterval<double>(_p->_defaultValue.toDouble(), min, _p->_max.toDouble()))
      return false;

    _p->_min = GenericValue(min);
    return true;
  }

  bool ParameterModel::setMaxDouble(double max)
  {
    if(_p->_contentType != ParameterModel::ContentType_Double)
      return false;

    if(!_p->inInterval<double>(_p->_defaultValue.toDouble(), _p->_min.toDouble(), max))
      return false;

    _p->_max = GenericValue(max);
    return true;
  }

  void ParameterModel::setDefaultValueString(const std::string& default_value)
  {
    _p->_contentType  = ContentType_String;
    _p->_defaultValue = GenericValue(default_value);
    _p->_choices.clear();
  }

  void ParameterModel::setDefaultValueRessource(const std::string& default_value)
  {
    _p->_contentType  = ContentType_Ressource;
    _p->_defaultValue = GenericValue(default_value);
    _p->_choices.clear();
  }

  void ParameterModel::setCustomChoice(bool custom_choice)
  {
    _p->_customChoice = custom_choice;
  }

  void ParameterModel::setPassword(bool password)
  {
    _p->_password = password;
  }

  void ParameterModel::setTooltip(const std::string& tooltip)
  {
    _p->_tooltip = tooltip;
  }

  void ParameterModel::setId(int id)
  {
    _p->_id = id;
  }

  bool ParameterModel::addChoice(ChoiceModelPtr choice)
  {
    qiLogDebug("QICore") << "addChoice function" << std::endl;
    if(choice->getType() != _p->_contentType)
    {
      qiLogWarning("QICore") << "choice.type(i.e "
                             << choice->getType()
                             << ") != parameter.type("
                             << _p->_contentType
                             <<")"
                            << std::endl;
      return false;
    }

    if(_p->_contentType == ContentType_Double)
    {
      //If choice.value is not in [parameter.min, paramater.max] then the choice
      //is incorrect


      if(!_p->inInterval<double>(choice->getValueDouble(),
                                 _p->_min.to<double>(),
                                 _p->_max.to<double>())
         )
      {
        qiLogInfo("QICore")    << "Choice.value : "
                               << choice->getValueDouble()
                               << " not in ["
                               << _p->_min.toDouble()
                               << ", "
                               << _p->_max.toDouble()
                               << std::endl;
        return false;
      }
    }

    if(_p->_contentType == ContentType_Int)
    {
      //If choice.value is not in [parameter.min, paramater.max] then the choice
      //is incorrect
      if(!_p->inInterval<int>(choice->getValueInt(),
                              _p->_min.to<int>(),
                              _p->_max.to<int>())
         )
      {
        qiLogInfo("QICore")    << "Choice.value : "
                               << choice->getValueInt()
                               << " not in ["
                               << _p->_min.toDouble()
                               << ", "
                               << _p->_max.toDouble()
                               << std::endl;
        return false;
      }
    }

    _p->_choices.push_front(choice);
    return true;
  }

  const std::list<ChoiceModelPtr>& ParameterModel::getChoices() const
  {
    return _p->_choices;
  }

  bool ParameterModel::checkInterval(ParameterValueModelPtr value) const
  {
    switch(_p->_contentType)
    {
    case ParameterModel::ContentType_Bool:
    case ParameterModel::ContentType_Ressource:
    case ParameterModel::ContentType_String:
      return true;
      break;

    case ParameterModel::ContentType_Double:
      return _p->inInterval<double>(value->getValueDouble(),
                                    _p->_min.toDouble(),
                                    _p->_max.toDouble());
      break;
    case ParameterModel::ContentType_Int:
      return _p->inInterval<int>(value->getValueInt(),
                                 _p->_min.toInt(),
                                 _p->_max.toInt());
      break;

    default:
      return false;
      break;
    }
  }
}
