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

namespace qi {
  class TangentModelPrivate;

  class QICORECOMPAT_API TangentModel
  {
  public:
    enum Side
    {
      Side_Left  = 0,
      Side_Right = 1
    };

    enum InterpolationType {
      InterpolationType_Bezier   = 0,
      InterpolationType_Linear   = 1,
      InterpolationType_Constant = 3
    };
    TangentModel(Side side, InterpolationType interp, float abscissa, float ordinate);
    TangentModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~TangentModel();

    Side getSide() const;
    InterpolationType getInterpType() const;
    float getAbscissaParam() const;
    float getOrdinateParam() const;

    bool isValid() const;

    void setSide(Side side);
    void setInterpType(InterpolationType interpType);
    void setAbscissaParam(float abscissa);
    void setOrdinateParam(float ordinate);

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(TangentModel);
    TangentModelPrivate* _p;
  };
  typedef boost::shared_ptr<TangentModel> TangentModelPtr;
}

#endif /* !TANGENT_H_ */
