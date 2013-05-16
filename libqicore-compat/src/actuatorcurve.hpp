/**
 * @author Jerome Vuarand
 * Copyright (c) Aldebaran Robotics 2009 All Rights Reserved \n
 *
 */

#ifndef ACTUATORCURVE_H
#define ACTUATORCURVE_H

#include <boost/shared_ptr.hpp>
#include <almathinternal/interpolations/alinterpolation.h>
#include <map>
#include <string>

namespace AL
{
  class XmlElement;
}

namespace qi
{

class ActuatorCurve
{
public:

  enum Side
  {
    LEFT,
    RIGHT,
  };

  enum CurveUnit
  {
    UNKNOWN_UNIT = -1,
    DEGREE_UNIT,
    PERCENT_UNIT
  };

  typedef AL::Math::Interpolation::Tangent Tangent;
  typedef AL::Math::Interpolation::Key Key;

  static bool loadFromXml(Tangent& self, boost::shared_ptr<const AL::XmlElement> elt);
  static bool loadFromXml(Key& self, boost::shared_ptr<const AL::XmlElement> elt);

  ActuatorCurve();

  virtual ~ActuatorCurve();

  virtual int loadFromXml(boost::shared_ptr<const AL::XmlElement> elt);

  const std::string& name() const { return fName; }
  const std::map<int, Key>& keys() const { return fKeys; }
  CurveUnit curveUnit() const { return fCurveUnit; }

  float getInterpolatedValue(const int indexKey, float valueIncrementLimit) const;
  float getMotionValue(float value) const;

private:
  void rebuildBezierAutoTangents();

  void getNeighborKeysOf(const int indexKey, int& indexLeftKey, Key& leftKey,
                         int& indexRightKey, Key& rightKey) const;


  std::string fName;
  std::map<int, Key> fKeys;
  CurveUnit fCurveUnit;
};

};


#endif // !ACTUATORCURVE_H
