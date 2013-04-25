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
    ChoiceModelPrivate(const GenericValue &value, ParameterModel::ContentType type);
    ChoiceModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, ParameterModel::ContentType type);

  private:
    GenericValue _value;
    ParameterModel::ContentType _type;
  };
}

#endif/* !CHOICE_P_H_ */
