/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/tangentmodel.hpp>
#include "tangentmodel_p.hpp"

namespace qi {
  TangentModelPrivate::TangentModelPrivate() :
    _side(),
    _interType(),
    _abscissaParam(),
    _ordinateParam(),
    _isValid(false)
  {
  }

  TangentModelPrivate::TangentModelPrivate(TangentModel::Side side, TangentModel::InterpolationType interp, float abscissa, float ordinate) :
    _side(side),
    _interType(interp),
    _abscissaParam(abscissa),
    _ordinateParam(ordinate),
    _isValid(true)
  {

  }

  TangentModelPrivate::TangentModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    std::string side;
    std::string interpType;

    elt->getAttribute("side",          side);
    elt->getAttribute("interpType",    interpType);
    elt->getAttribute("abscissaParam", _abscissaParam);
    elt->getAttribute("ordinateParam", _ordinateParam);

    _isValid = true;

    if(side == "left")
      _side = TangentModel::Side_Left;
    else if(side == "right")
      _side = TangentModel::Side_Right;
    else
      _isValid = false;

    if(interpType == "bezier")
      _interType = TangentModel::InterpolationType_Bezier;
    else if(interpType == "linear")
      _interType = TangentModel::InterpolationType_Linear;
    else if(interpType == "constant")
      _interType = TangentModel::InterpolationType_Constant;
    else
      _isValid = false;
  }

  TangentModel::TangentModel(Side side, InterpolationType interp, float abscissa, float ordinate) :
    _p( new TangentModelPrivate(side, interp, abscissa, ordinate))
  {
  }

  TangentModel::TangentModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new TangentModelPrivate(elt))
  {
  }

  TangentModel::~TangentModel()
  {
    delete _p;
  }

  TangentModel::Side TangentModel::side() const
  {
    return _p->_side;
  }

  TangentModel::InterpolationType TangentModel::interpType() const
  {
    return _p->_interType;
  }

  float TangentModel::abscissaParam() const
  {
    return _p->_abscissaParam;
  }

  float TangentModel::ordinateParam() const
  {
    return _p->_ordinateParam;
  }

  bool TangentModel::isValid() const
  {
    return _p->_isValid;
  }

  void TangentModel::setSide(Side side)
  {
    _p->_side = side;
  }

  void TangentModel::setInterpType(InterpolationType interpType)
  {
    _p->_interType = interpType;
  }

  void TangentModel::setAbscissaParam(float abscissa)
  {
    _p->_abscissaParam = abscissa;
  }

  void TangentModel::setOrdinateParam(float ordinate)
  {
    _p->_ordinateParam = ordinate;
  }

}
