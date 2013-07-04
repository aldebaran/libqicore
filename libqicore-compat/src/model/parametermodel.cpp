/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/choicemodel.hpp>
#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/parametervaluemodel.hpp>

#include "xmlutils.hpp"
#include "parametermodel_p.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.ParameterModel");


namespace qi
{
  static bool isIntegerOrDouble(AnyReference value)
  {

    Signature signature(value.signature());
    return signature.isConvertibleTo(Signature("i")) != 0.0 ||
           signature.isConvertibleTo(Signature("d")) != 0.0;
  }

  //--------------------------Private Class-------------------------------------//
  ParameterModelPrivate::ParameterModelPrivate(const std::string &name,
                                               AutoAnyReference defaultValue,
                                               bool inheritsFromParent,
                                               bool customChoice,
                                               bool password,
                                               const std::string &tooltip,
                                               unsigned int id,
                                               bool resource) :
    _metaProperty(id, name, defaultValue.signature()),
    _inheritsFromParent(inheritsFromParent),
    _customChoice(customChoice),
    _password(password),
    _tooltip(tooltip),
    _choices(),
    _isValid(true),
    _defaultValue(defaultValue.clone())
  {
    if(resource)
      _metaProperty = MetaProperty(id, name, ParameterModel::signatureRessource());

    Signature signature(defaultValue.signature());

    //If defaultValue is int or double min and max are not defined so this object is not valid
    if(isIntegerOrDouble(defaultValue))
    {
      qiLogError() << "Parameter.min and Parameter.max is not defined.";
      _isValid = false;
    }
  }

  ParameterModelPrivate::ParameterModelPrivate(const std::string &name,
                                               AutoAnyReference defaultValue,
                                               AutoAnyReference min,
                                               AutoAnyReference max,
                                               bool inheritsFromParent,
                                               bool customChoice,
                                               bool password,
                                               const std::string &tooltip,
                                               unsigned int id)
    :
      _metaProperty(id, name, defaultValue.signature()),
      _inheritsFromParent(inheritsFromParent),
      _customChoice(customChoice),
      _password(password),
      _tooltip(tooltip),
      _choices(),
      _isValid(true),
      _defaultValue(defaultValue.clone()),
      _min(min.clone()),
      _max(max.clone())
  {
    //If default value, max or min is'nt double or int this object is not valid
    if(!isIntegerOrDouble(defaultValue))
      _isValid = false;

    if(!isIntegerOrDouble(min))
      _isValid = false;

    if(!isIntegerOrDouble(max))
      _isValid = false;
  }

  ParameterModelPrivate::ParameterModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    std::string signature;
    std::string name;
    int id;
    elt->getAttribute("name", name);
    elt->getAttribute("type", signature);
    elt->getAttribute("id", id);
    _metaProperty = MetaProperty(id, name, signature);
    elt->getAttribute("inherits_from_parent", _inheritsFromParent);

    Signature sig = Signature(signature);

    _isValid = true;
    if(sig.isConvertibleTo(Signature::fromType(Signature::Type_Bool)) == 1.0f)
    {
      bool default_value_bool;
      elt->getAttribute("default_value",    default_value_bool);
      _defaultValue = AnyValue(default_value_bool).clone();
    }
    else if(sig.isConvertibleTo(Signature::fromType(Signature::Type_Int32)) == 1.0f)
    {
      int default_value_int, min_int, max_int;
      elt->getAttribute("default_value",    default_value_int);
      elt->getAttribute("min",              min_int, 0);
      elt->getAttribute("max",              max_int, 0);
      _defaultValue  = AnyValue(default_value_int).clone();
      _min           = AnyValue(min_int).clone();
      _max           = AnyValue(max_int).clone();
    }
    else if(sig.isConvertibleTo(Signature::fromType(Signature::Type_Double)) == 1.0f )
    {
      double default_value_double, min_double, max_double;
      elt->getAttribute("default_value",    default_value_double);
      elt->getAttribute("min",              min_double, 0.0);
      elt->getAttribute("max",              max_double, 0.0);
      _defaultValue  = AnyValue(default_value_double).clone();
      _min           = AnyValue(min_double).clone();
      _max           = AnyValue(max_double).clone();
    }
    else if(sig.isConvertibleTo(Signature::fromType(Signature::Type_String)) == 1.0f)
    {
      std::string default_value_string;
      elt->getAttribute("default_value", default_value_string, std::string(""));
      _defaultValue = AnyValue(default_value_string).clone();
    }
    else
    {
      _isValid = false;
    }

    elt->getAttribute("custom_choice",        _customChoice, false);
    elt->getAttribute("password",             _password, false);
    elt->getAttribute("tooltip",              _tooltip);

    AL::XmlElement::CList choices = elt->children("Choice", "");

    if(!choices.empty())
    {
      for (AL::XmlElement::CList::const_iterator it=choices.begin(), itEnd=choices.end(); it!=itEnd; ++it)
      {
        boost::shared_ptr<const AL::XmlElement> elt = *it;
        boost::shared_ptr<ChoiceModel> choice(new ChoiceModel(elt, Signature(_defaultValue.signature())));
        _choices.push_front(choice);
      }
    }
  }

  bool ParameterModelPrivate::inInterval(AnyReference value, AnyReference min, AnyReference max) const
  {
    Signature signature(value.signature());
    qiLogDebug() << "Enter in inInterval";
    qiLogDebug() << "Signature of value is : " << signature.toString();

    //if value is bool string or Resource return true
    if(signature.isConvertibleTo(Signature::fromType(Signature::Type_Bool)) == 1.0f ||
       signature.isConvertibleTo(Signature::fromType(Signature::Type_String)) == 1.0f ||
       signature.isConvertibleTo(ParameterModel::signatureRessource()) == 1.0f)
      return true;

    qiLogDebug() << "Type of value is double or integer";

    //if value is not in [min, max] return false
    if(value < min || max < value)
      return false;

    return true;
  }

  //------------------------------------Public Class-------------------------------//
  ParameterModel::ParameterModel(const std::string &name,
                                 AutoAnyReference defaultValue,
                                 bool inheritsFromParent,
                                 bool customChoice,
                                 bool password,
                                 const std::string &tooltip,
                                 int id,
                                 bool resource) :
    _p(new ParameterModelPrivate(name, defaultValue, inheritsFromParent, customChoice, password, tooltip, id, resource))
  {
  }

  ParameterModel::ParameterModel(const std::string &name,
                 AutoAnyReference defaultValue,
                 AutoAnyReference min,
                 AutoAnyReference max,
                 bool inheritsFromParent,
                 bool customChoice,
                 bool password,
                 const std::string &tooltip,
                 int id) :
    _p(new ParameterModelPrivate(name, defaultValue, min, max, inheritsFromParent, customChoice, password, tooltip, id))
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
  const MetaProperty& ParameterModel::metaProperty() const
  {
    return _p->_metaProperty;
  }

  bool ParameterModel::inheritsFromParent() const
  {
    return _p->_inheritsFromParent;
  }

  AnyReference ParameterModel::defaultValue() const
  {
    return _p->_defaultValue;
  }

  AnyReference ParameterModel::min() const
  {
    return _p->_min;
  }

  AnyReference ParameterModel::max() const
  {
    return _p->_max;
  }

  bool ParameterModel::customChoice() const
  {
    return _p->_customChoice;
  }

  bool ParameterModel::password() const
  {
    return _p->_password;
  }

  const std::string& ParameterModel::tooltip() const
  {
    return _p->_tooltip;
  }

  //-------------------------------Setter------------------------------------//
  void ParameterModel::setMetaProperty(unsigned int id, const std::string &name, const Signature &sig)
  {
    _p->_metaProperty = MetaProperty(id, name, sig.toString());
    _p->_isValid = false;
  }

  void ParameterModel::setInheritsFromParent(bool inherits_from_parent)
  {
    _p->_inheritsFromParent = inherits_from_parent;
  }

  bool ParameterModel::setValue(AutoAnyReference value)
  {
    Signature signature(value.signature());

    if(signature.isConvertibleTo(_p->_metaProperty.signature()) == 0.0)
      return false;

    if(!_p->inInterval(value, _p->_min, _p->_max))
      return false;

    _p->_defaultValue = value.clone();
    _p->_isValid = true;
    return true;
  }

  bool ParameterModel::setValue(AutoAnyReference value, AutoAnyReference min, AutoAnyReference max)
  {
    Signature signature(value.signature());

    if(signature.isConvertibleTo(_p->_metaProperty.signature()) == 0.0)
      return false;

    //value is not double or int
    if(signature.isConvertibleTo(Signature::fromType(Signature::Type_Double)) != 1.0f &&
       signature.isConvertibleTo(Signature::fromType(Signature::Type_Int32)) != 1.0f)
      return false;

    if(!_p->inInterval(value, min, max))
      return false;

    _p->_defaultValue = value.clone();
    _p->_min = min.clone();
    _p->_max = max.clone();
    _p->_isValid = true;
    return true;
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

  bool ParameterModel::addChoice(ChoiceModelPtr choice)
  {
    qiLogDebug() << "addChoice function" << std::endl;
    Signature signature(_p->_metaProperty.signature());

    //if false choice and parameter are the same type
    if(Signature(choice->value().signature()).isConvertibleTo(signature) < 1.0f )
    {
      qiLogWarning() << "choice.type (i.e "
                     << choice->value().signature().toString()
                     << ") != parameter.type (i.e "
                     << _p->_defaultValue.signature().toString()
                     <<")"
                     << std::endl;
      return false;
    }

    //If choice.value is not in [parameter.min, paramater.max] then the choice
    //is incorrect
    if(!_p->inInterval(choice->value(),
                       _p->_min,
                       _p->_max)
       )
    {
      qiLogInfo()    << "Choice : is not in interval"
                     << std::endl;
      return false;
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
    return _p->inInterval(value->value(), _p->_min, _p->_max);
  }

  bool ParameterModel::isValid() const
  {
    return _p->_isValid;
  }

  //Resource type
  const Signature& ParameterModel::signatureRessource()
  {
    static Signature signatureResource("s<Resource>");
    return signatureResource;
  }
}
