/**
 * @author Jerome Vuarand
 * Copyright (c) Aldebaran Robotics 2009 All Rights Reserved \n
 *
 */

#include "actuator_curve.h"
#include "xml_utils.h"
#include <alserial/alserial.h>
#include <alerror/alerror.h>
#include <almath/tools/altrigonometry.h>
#include <almathinternal/interpolations/alinterpolation.h>

#include "math.h"
#include <limits>

ActuatorCurve::ActuatorCurve():
  fCurveUnit(UNKNOWN_UNIT)
{
}

ActuatorCurve::~ActuatorCurve()
{
}

bool ActuatorCurve::loadFromXml(ActuatorCurve::Tangent& self, boost::shared_ptr<const AL::XmlElement> elt)
{
  AL::Math::Interpolation::InterpolationType type;
  float x;
  float y;

  if (!elt->getAttribute("interpType", type))
    return false;
  if (!elt->getAttribute("abscissaParam", x))
    return false;
  if (!elt->getAttribute("ordinateParam", y))
    return false;

  self.fType = type;
  self.fOffset = AL::Math::Position2D(x,y);

  return true;
}

bool ActuatorCurve::loadFromXml(ActuatorCurve::Key& self, boost::shared_ptr<const AL::XmlElement> elt)
{
  if (!elt->getAttribute("value", self.fValue))
    return false;

  AL::XmlElement::CList tangents = elt->children("Tangent", "Tangents");
  for (AL::XmlElement::CList::const_iterator it=tangents.begin(), itEnd=tangents.end(); it!=itEnd; ++it)
  {
    boost::shared_ptr<const AL::XmlElement> pair_elt = *it;
    Side side;
    Tangent* tangent = NULL;
    if (!pair_elt->getAttribute("side", side))
      return false;
    if (side==LEFT)
      tangent = &self.fLeftTangent;
    else if (side==RIGHT)
      tangent = &self.fRightTangent;
    if (!tangent || !loadFromXml(*tangent, pair_elt))
      return false;
  }
  return true;
}

int ActuatorCurve::loadFromXml(boost::shared_ptr<const AL::XmlElement> elt)
{
  if (!elt->getAttribute("actuator", fName))
    return -1;

  int unit;
  elt->getAttribute("unit", unit, -1);
  fCurveUnit = (CurveUnit)unit;

  int lastKeyframe = 0;

  AL::XmlElement::CList keys = elt->children("Key", "CurveKeys");
  for (AL::XmlElement::CList::const_iterator it=keys.begin(), itEnd=keys.end(); it!=itEnd; ++it)
  {
    boost::shared_ptr<const AL::XmlElement> pair_elt = *it;
    int frame;
    Key key;
    if (!pair_elt->getAttribute("frame", frame))
      return -1;
    if (!loadFromXml(key, pair_elt))
      return -1;
    fKeys[frame] = key;
    if (frame > lastKeyframe)
      lastKeyframe = frame;
  }
  rebuildBezierAutoTangents();
  return lastKeyframe;
}

static bool updateBezierAutoTangents(int currentIndex, AL::Math::Interpolation::Key* key, int leftIndex, const AL::Math::Interpolation::Key* lNeighbor, int rightIndex, const AL::Math::Interpolation::Key* rNeighbor)
{
  using AL::Math::Interpolation::Key;
  using AL::Math::Interpolation::BEZIER_AUTO;
  using AL::Math::Position2D;

  // :TODO: jvuarand 20100406: merge that function with the one in Choregraphe

  //Note JB Desmottes 19-05-09 : we now consider that whenever the method is
  //  called, tangent params will change, and thus we do not need to inform the
  //  caller whether they really changed or not. In some cases, this will lead to
  //  unecessary updates.
  //  Example : current key is a minimum of the curve, and neighbor value (not the
  //  index) has changed. In that case, params of current key do not change.

  if (key->fLeftTangent.fType==BEZIER_AUTO || key->fRightTangent.fType==BEZIER_AUTO)
  {
    float alpha = 1.0f/3.0f;
    float beta = 0.0f;

    if (lNeighbor && rNeighbor)
    {
      float value = key->fValue;
      float lvalue = lNeighbor->fValue;
      float rvalue = rNeighbor->fValue;
      if ((value < rvalue || value < lvalue)
        && (value > rvalue || value > lvalue))
      {
        if (currentIndex>=0 && leftIndex>=0 && rightIndex>=0)
        {
          beta = (rvalue - lvalue) / (rightIndex-leftIndex);

          // anti overshooting
          float tgtHeight = alpha * (rightIndex - currentIndex) * beta;
          if (fabs(tgtHeight) > fabs(rvalue - value))
          {
            beta *= (rvalue - value) / tgtHeight;
          }
          tgtHeight = alpha * (currentIndex - leftIndex) * beta;
          if (fabs(tgtHeight) > fabs(value - lvalue))
          {
            beta *= (value - lvalue) / tgtHeight;
          }
        }
      }
    }

    // set parameters into model
    if (key->fLeftTangent.fType==BEZIER_AUTO)
    {
      Position2D offset = Position2D(-alpha, -alpha*beta) * float(currentIndex - leftIndex);
      // :NOTE: for test purposes, you can force serialization of BEZIER_AUTO in Choregraphe, and enable assert below
      //assert(AL::Math::distanceSquared(key->fLeftTangent.fOffset, offset) < 0.01f);
      key->fLeftTangent.fOffset = offset;
    }
    if (key->fRightTangent.fType==BEZIER_AUTO)
    {
      Position2D offset = Position2D(alpha, alpha*beta) * float(rightIndex - currentIndex);
      // :NOTE: for test purposes, you can force serialization of BEZIER_AUTO in Choregraphe, and enable assert below
      //assert(AL::Math::distanceSquared(key->fRightTangent.fOffset, offset) < 0.01f);
      key->fRightTangent.fOffset = offset;
    }

    return true;
  }
  else
    return false;
}

void ActuatorCurve::rebuildBezierAutoTangents()
{
  using std::map;
  for (map<int, Key>::iterator it=fKeys.begin(), itEnd=fKeys.end(); it!=itEnd; ++it)
  {
    if (it->second.fLeftTangent.fType==AL::Math::Interpolation::BEZIER_AUTO
      || it->second.fRightTangent.fType==AL::Math::Interpolation::BEZIER_AUTO)
    {
      int currentIndex = it->first;
      Key* key = &it->second;
      // get left neighbor, if any
      int leftIndex = currentIndex;
      const Key* lNeighbor = 0;
      if (it!=fKeys.begin())
      {
        map<int, Key>::iterator left = it;
        --left;
        leftIndex = left->first;
        lNeighbor = &left->second;
      }
      // get right neighbor, if any
      int rightIndex = currentIndex;
      const Key* rNeighbor = 0;
      map<int, Key>::iterator right = it;
      ++right;
      if (right!=fKeys.end())
      {
        rightIndex = right->first;
        rNeighbor = &right->second;
      }
      // adjust this key
      updateBezierAutoTangents(currentIndex, key, leftIndex, lNeighbor, rightIndex, rNeighbor);
    }
  }
}

// Returns the nearest neighbor (right and left) key frames in the timeline for a given key frame
// if given key is before the first key, both neighbors are set to this first key
// if after the last key, both neighbors are set to this last key
void ActuatorCurve::getNeighborKeysOf(const int indexKey, int& indexLeftKey, Key& leftKey, int& indexRightKey, Key& rightKey) const
{
  if (fKeys.empty())
  {
    indexLeftKey = -1;
    indexRightKey = -1;
    return;
  }

  std::map<int, Key>::const_iterator it = fKeys.begin();

  indexLeftKey = it->first;
  leftKey = it->second;

  if (indexKey > indexLeftKey)
  {
    it++;
    while (it != fKeys.end())
    {
      indexRightKey = it->first;
      rightKey = it->second;
      if (indexKey < indexRightKey)
      {
        return;
      }
      indexLeftKey = indexRightKey;
      leftKey = rightKey;
      it++;
    }
  }
  else
  {
    // given key before first key => neighbors are both this first key
    indexRightKey = indexLeftKey;
    rightKey = leftKey;
  }
}

// Returns the value for a given frame based on interpolation on the two nearest frames
float ActuatorCurve::getInterpolatedValue(const int indexKey, float valueIncrementLimit) const
{
  AL::Math::Interpolation::ALInterpolationBezier interpolator;

  int indexLeftKey;
  AL::Math::Interpolation::Key leftKey;
  int indexRightKey;
  AL::Math::Interpolation::Key rightKey;

  getNeighborKeysOf(indexKey, indexLeftKey, leftKey, indexRightKey, rightKey);

  int keyIntervalSize = indexRightKey - indexLeftKey + 1;

  if (keyIntervalSize > 1)
  {
    // FIXME : startValue, minValue, maxValue
    float startValue = 0.0f;
    float minValue = -std::numeric_limits<float>::max();
    float maxValue = std::numeric_limits<float>::max();

    std::vector<float> actuatorValues = interpolator.interpolate(keyIntervalSize, leftKey, rightKey,
      startValue, minValue, maxValue, valueIncrementLimit, 1);

    return actuatorValues[indexKey - indexLeftKey];
  }
  else
  {
    return leftKey.fValue;
  }
}

float ActuatorCurve::getMotionValue(float value) const
{
  float result = value;
  switch(fCurveUnit)
  {
  case DEGREE_UNIT:
    result *= AL::Math::TO_RAD;
    break;

  case PERCENT_UNIT:
    break;

  case UNKNOWN_UNIT:
  default:
    if(fName != "LHand" && fName != "RHand")
    {
      result *= AL::Math::TO_RAD;
      break;
    }
  }

  return result;
}
