/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/keymodel.hpp>
#include <qicore-compat/model/tangentmodel.hpp>
#include "keymodel_p.hpp"
#include "xmlutils.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.KeyModel");

namespace qi
{
  KeyModelPrivate::KeyModelPrivate(int frame, float value, bool smooth, bool symmetrical) :
    _frame(frame),
    _value(value),
    _smooth(smooth),
    _symmetrical(symmetrical),
    _tangents()
  {

  }

  KeyModelPrivate::KeyModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    elt->getAttribute("frame",       _frame);
    elt->getAttribute("value",       _value);
    elt->getAttribute("smooth",      _smooth);
    elt->getAttribute("symmetrical", _symmetrical);

    AL::XmlElement::CList tangents = elt->children("Tangent", "");

    _tangents = XmlUtils::constructObjects<TangentModel>(tangents);

    _isValid = true;

    //Quantity tangents is 0 or 2
    qiLogDebug("QICore") << "Tangents size : "
                         << _tangents.size()
                         << std::endl;
    if(_tangents.size() != 2 && _tangents.size() != 0)
    {
      qiLogError() << "Invalid number of tag Tangent"
                           << std::endl;
      _isValid = false;
    }

    _isValid = _isValid && XmlUtils::verifyObjects<TangentModel>(_tangents);
  }

  KeyModel::KeyModel(int frame, float value, bool smooth, bool symmetrical) :
    _p( new KeyModelPrivate(frame, value, smooth, symmetrical))
  {

  }

  KeyModel::KeyModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new KeyModelPrivate(elt))
  {
  }

  KeyModel::~KeyModel()
  {
    delete _p;
  }

  int KeyModel::getFrame() const
  {
    return _p->_frame;
  }

  float KeyModel::getValue() const
  {
    return _p->_value;
  }

  bool KeyModel::getSmooth() const
  {
    return _p->_smooth;
  }

  bool KeyModel::getSymmetrical() const
  {
    return _p->_symmetrical;
  }

  const std::list<TangentModelPtr>& KeyModel::getTangents() const
  {
    return _p->_tangents;
  }

  bool KeyModel::isValid() const
  {
    return _p->_isValid;
  }

  void KeyModel::setFrame(int frame)
  {
    _p->_frame = frame;
  }

  void KeyModel::setValue(float value)
  {
    _p->_value = value;
  }

  void KeyModel::setSmooth(bool smooth)
  {
    _p->_smooth = smooth;
  }

  void KeyModel::setSymmetrical(bool symmetrical)
  {
    _p->_symmetrical = symmetrical;
  }

  void KeyModel::setTangents(TangentModelPtr tangent1, TangentModelPtr tangent2)
  {
    _p->_tangents.clear();
    _p->_tangents.push_front(tangent1);
    _p->_tangents.push_front(tangent2);
  }
}
