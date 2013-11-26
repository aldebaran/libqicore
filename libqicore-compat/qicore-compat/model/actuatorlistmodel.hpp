/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef ACTUATORLIST_H_
#define ACTUATORLIST_H_

#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>
#include <boost/noncopyable.hpp>

namespace qi {
  class ActuatorListModelPrivate;
  class ActuatorCurveModel;

  class QICORECOMPAT_API ActuatorListModel : private boost::noncopyable {
  public:
    ActuatorListModel(const std::string &model,
                      const std::list<boost::shared_ptr<ActuatorCurveModel> > &curves = std::list<boost::shared_ptr<ActuatorCurveModel> >());

    ActuatorListModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~ActuatorListModel();

    const std::string& model() const;
    const std::list<boost::shared_ptr<ActuatorCurveModel> >& actuatorsCurve() const;

    void setModel(const std::string &model);
    void addActuatorCurve(boost::shared_ptr<ActuatorCurveModel> actuatorCurve);

    bool isValid() const;

  private:
    ActuatorListModelPrivate* _p;

  };
  typedef boost::shared_ptr<ActuatorListModel> ActuatorListModelPtr;
}

#endif /* !ACTUATORLIST_H_ */
