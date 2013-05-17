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
    _tangentLeft(new TangentModel(TangentModel::Side_Left)),
    _tangentRight(new TangentModel(TangentModel::Side_Right)),
    _isValid(true)
  {
  }

  KeyModelPrivate::KeyModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    elt->getAttribute("frame",       _frame);
    elt->getAttribute("value",       _value);
    elt->getAttribute("smooth",      _smooth);
    elt->getAttribute("symmetrical", _symmetrical);

    AL::XmlElement::CList tangents = elt->children("Tangent", "");

    _isValid = true;
    if(tangents.size() == 2)
    {
      for(AL::XmlElement::CList::const_iterator it = tangents.begin(), itEnd = tangents.end(); it != itEnd; ++it)
      {
        boost::shared_ptr<const AL::XmlElement> elt = *it;
        TangentModelPtr tan = TangentModelPtr(new TangentModel(elt));

        if(tan->side() == TangentModel::Side_Left)
          _tangentLeft = tan;

        if(tan->side() == TangentModel::Side_Right)
          _tangentRight = tan;
      }

      if(!_tangentLeft || !_tangentRight)
        _isValid = false;
    }
    else if(tangents.size() == 0)
    {
      _tangentLeft = TangentModelPtr(new TangentModel(TangentModel::Side_Left));
      _tangentRight = TangentModelPtr(new TangentModel(TangentModel::Side_Right));
    }
    else
    {
      qiLogError() << "Invalid number of tag Tangent"
                           << std::endl;
      _isValid = false;
    }


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

  int KeyModel::frame() const
  {
    return _p->_frame;
  }

  float KeyModel::value() const
  {
    return _p->_value;
  }

  bool KeyModel::smooth() const
  {
    return _p->_smooth;
  }

  bool KeyModel::symmetrical() const
  {
    return _p->_symmetrical;
  }

  boost::shared_ptr<TangentModel>& KeyModel::leftTangent() const
  {
    return _p->_tangentLeft;
  }

  boost::shared_ptr<TangentModel> &KeyModel::rightTangent() const
  {
    return _p->_tangentRight;
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

  bool KeyModel::setTangents(TangentModelPtr left, TangentModelPtr right)
  {
    if(!(left->side() == TangentModel::Side_Left && right->side() == TangentModel::Side_Right))
       return false;

    _p->_tangentLeft = left;
    _p->_tangentRight = right;
    return true;
  }
}
