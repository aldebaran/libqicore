/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TANGENT_P_H_
#define TANGENT_P_H_

#include <alserial/alserial.h>
#include <qicore-compat/model/tangentmodel.hpp>

namespace qi
{
  class TangentModelPrivate
  {
    friend class TangentModel;
  public:
    TangentModelPrivate();
    TangentModelPrivate(TangentModel::Side side, TangentModel::InterpolationType interp, float abscissa, float ordinate);
    TangentModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);

  private:
    TangentModel::Side _side;
    TangentModel::InterpolationType _interType;
    float _abscissaParam;
    float _ordinateParam;
    bool _isValid;
  };
}

#endif /* TANGENT_P_H_ */
