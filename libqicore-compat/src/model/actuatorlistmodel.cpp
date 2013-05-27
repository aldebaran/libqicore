/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/


#include <qicore-compat/model/actuatorlistmodel.hpp>
#include "actuatorlistmodel_p.hpp"
#include "xmlutils.hpp"

namespace qi {

  ActuatorListModelPrivate::ActuatorListModelPrivate(const std::string &model, const std::list<ActuatorCurveModelPtr> &curves) :
    _isValid(true),
    _model(model),
    _actuatorsCurve(curves)
  {
  }

  ActuatorListModelPrivate::ActuatorListModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    elt->getAttribute("model", _model);

    AL::XmlElement::CList actuatorsCurve = elt->children("ActuatorCurve", "");
    _actuatorsCurve = XmlUtils::constructObjects<ActuatorCurveModel>(actuatorsCurve);

    _isValid = XmlUtils::verifyObjects<ActuatorCurveModel>(_actuatorsCurve);

    if(_model.empty())
      _model = "Nao";

  }

  ActuatorListModel::ActuatorListModel(const std::string &model, const std::list<boost::shared_ptr<ActuatorCurveModel> > &curves) :
    _p( new ActuatorListModelPrivate(model, curves))
  {
  }

  ActuatorListModel::ActuatorListModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new ActuatorListModelPrivate(elt))
  {
  }

  ActuatorListModel::~ActuatorListModel()
  {
    delete _p;
  }

  const std::string& ActuatorListModel::model() const
  {
    return _p->_model;
  }

  const std::list<ActuatorCurveModelPtr>& ActuatorListModel::actuatorsCurve() const
  {
    return _p->_actuatorsCurve;
  }

  bool ActuatorListModel::isValid() const
  {
    return _p->_isValid;
  }

  void ActuatorListModel::setModel(const std::string& model)
  {
    _p->_model = model;
  }

  void ActuatorListModel::addActuatorCurve(ActuatorCurveModelPtr actuatorCurve)
  {
    _p->_actuatorsCurve.push_front(actuatorCurve);
  }
}
