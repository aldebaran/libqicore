/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CHOICE_P_H_
#define CHOICE_P_H_

#include <qitype/genericvalue.hpp>
#include <qicore-compat/model/parametermodel.hpp>

namespace qi
{
  class ChoiceModelPrivate
  {
    friend class ChoiceModel;
  public:
    ChoiceModelPrivate(AutoGenericValuePtr value);
    ChoiceModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const Signature& type);

  private:
    GenericValuePtr _value;
    bool _isValid;
  };
}

#endif/* !CHOICE_P_H_ */
