/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef KEY_H_
#define KEY_H_

#include <map>
#include <qicore-compat/api.hpp>
#include <alserial/alserial.h>
#include <boost/noncopyable.hpp>

namespace qi
{
  class KeyModelPrivate;
  class TangentModel;

  class QICORECOMPAT_API KeyModel : private boost::noncopyable
  {
  public:
    KeyModel(int frame, float value, bool smooth, bool symmetrical);
    KeyModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~KeyModel();

    int   frame()       const;
    float value()       const;
    bool  smooth()      const;
    bool  symmetrical() const;

    boost::shared_ptr<TangentModel>& leftTangent() const;
    boost::shared_ptr<TangentModel>& rightTangent() const;

    bool isValid() const;

    void setFrame(int frame);
    void setValue(float value);
    void setSmooth(bool smooth);
    void setSymmetrical(bool symmetrical);

    bool setTangents(boost::shared_ptr<TangentModel> left, boost::shared_ptr<TangentModel> right);

  private:
    KeyModelPrivate* _p;
  };
  typedef boost::shared_ptr<KeyModel> KeyModelPtr;
  typedef boost::shared_ptr<const KeyModel> CKeyModelPtr;
  typedef std::map<int, KeyModelPtr> KeyModelMap;
}

#endif /* !KEY_H_ */
