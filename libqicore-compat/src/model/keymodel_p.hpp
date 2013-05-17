/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef KEY_P_H_
# define KEY_P_H_

# include <alserial/alserial.h>
# include <qicore-compat/model/tangentmodel.hpp>

namespace qi
{
  class KeyModelPrivate
  {
    friend class KeyModel;
  public:
    KeyModelPrivate(int frame, float value, bool smooth, bool symmetrical);
    KeyModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);

  private:
    int _frame;
    float _value;
    bool _smooth;
    bool _symmetrical;
    TangentModelPtr _tangentLeft;
    TangentModelPtr _tangentRight;
    bool _isValid;
  };
}

#endif /* !KEY_P_H_ */
