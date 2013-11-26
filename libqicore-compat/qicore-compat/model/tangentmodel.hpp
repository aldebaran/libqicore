/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TANGENT_H_
#define TANGENT_H_

#include <string>
#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>
#include <boost/noncopyable.hpp>

namespace qi {
  class TangentModelPrivate;

  class QICORECOMPAT_API TangentModel : private boost::noncopyable
  {
  public:
    enum Side
    {
      Side_Left  = 0,
      Side_Right = 1
    };

    enum InterpolationType {
      InterpolationType_Constant = 0,
      InterpolationType_Linear = 1,
      InterpolationType_Bezier = 2,
      InterpolationType_BezierAuto = 3
    };

    TangentModel(Side side,
                 InterpolationType interp = InterpolationType_BezierAuto,
                 float abscissa = 0.0f,
                 float ordinate = 0.0f);
    TangentModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~TangentModel();

    Side side() const;
    InterpolationType interpType() const;
    float abscissaParam() const;
    float ordinateParam() const;

    bool isValid() const;

    void setSide(Side side);
    void setInterpType(InterpolationType interpType);
    void setAbscissaParam(float abscissa);
    void setOrdinateParam(float ordinate);

  private:
    TangentModelPrivate* _p;
  };
  typedef boost::shared_ptr<TangentModel> TangentModelPtr;
}

#endif /* !TANGENT_H_ */
