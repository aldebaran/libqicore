/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETER_P_H_
# define PARAMETER_P_H_

#include <qicore-compat/model/choicemodel.hpp>
#include <qitype/genericvalue.hpp>
#include <qitype/signature.hpp>
#include <qitype/metaproperty.hpp>
#include <alserial/alserial.h>

namespace qi
{
  class ParameterModelPrivate
  {
    friend class ParameterModel;
  public:
    ParameterModelPrivate(const std::string &name, AutoGenericValuePtr defaultValue, bool inheritsFromParent, bool customChoice, bool password, const std::string &tooltip, unsigned int id, bool resource);
    ParameterModelPrivate(const std::string &name, AutoGenericValuePtr defaultValue, AutoGenericValuePtr min, AutoGenericValuePtr max, bool inheritsFromParent, bool customChoice, bool password, const std::string &tooltip, unsigned int id);

    ParameterModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);


    bool inInterval(GenericValuePtr value, GenericValuePtr min, GenericValuePtr max) const;

  private:
    MetaProperty _metaProperty;
    bool _inheritsFromParent;
    bool _customChoice;
    bool _password;
    std::string _tooltip;
    std::list<boost::shared_ptr<ChoiceModel> > _choices;
    bool _isValid;
    GenericValuePtr _defaultValue;
    GenericValuePtr _min;
    GenericValuePtr _max;
  };
}
#endif /* !PARAMETER_P_H_ */
