/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETER_P_H_
# define PARAMETER_P_H_

#include <qicore-compat/model/choicemodel.hpp>
#include <qi/anyvalue.hpp>
#include <qi/signature.hpp>
#include <qi/type/metaproperty.hpp>
#include <alserial/alserial.h>

namespace qi
{
  class ParameterModelPrivate
  {
    friend class ParameterModel;
  public:
    ParameterModelPrivate(const std::string &name, AutoAnyReference defaultValue, bool inheritsFromParent, bool customChoice, bool password, const std::string &tooltip, unsigned int id, bool resource);
    ParameterModelPrivate(const std::string &name, AutoAnyReference defaultValue, AutoAnyReference min, AutoAnyReference max, bool inheritsFromParent, bool customChoice, bool password, const std::string &tooltip, unsigned int id);

    ParameterModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);


    bool inInterval(AnyReference value, AnyReference min, AnyReference max) const;

  private:
    MetaProperty _metaProperty;
    bool _inheritsFromParent;
    bool _customChoice;
    bool _password;
    std::string _tooltip;
    std::list<boost::shared_ptr<ChoiceModel> > _choices;
    bool _isValid;
    AnyReference _defaultValue;
    AnyReference _min;
    AnyReference _max;
  };
}
#endif /* !PARAMETER_P_H_ */
