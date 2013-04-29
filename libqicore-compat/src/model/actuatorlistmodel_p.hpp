/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once


#ifndef ACTUATORLIST_P_H_
#define ACTUATORLIST_P_H_

#include <alserial/alserial.h>
#include <qicore-compat/model/actuatorcurvemodel.hpp>

namespace qi {
  class ActuatorListModelPrivate {
    friend class ActuatorListModel;
  public:
    ActuatorListModelPrivate(const std::string &model, const std::list<ActuatorCurveModelPtr> &curves);
    ActuatorListModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);

  private:
    bool _isValid;
    std::string _model;
    std::list<ActuatorCurveModelPtr> _actuatorsCurve;
  };
}

#endif /* !ACTUATORLIST_P_H_ */
