/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/behaviorlayermodel.hpp>
#include "behaviorlayermodel_p.hpp"
#include "xmlutils.hpp"

namespace qi {

  BehaviorLayerModelPrivate::BehaviorLayerModelPrivate() :
    _name(),
    _mute(),
    _behaviorsKeyFrame()
  {
  }

  BehaviorLayerModelPrivate::BehaviorLayerModelPrivate(const std::string &name,
                                             bool mute,
                                             const std::list<boost::shared_ptr<BehaviorKeyFrameModel> > &keys) :
    _name(name),
    _mute(mute),
    _behaviorsKeyFrame(keys)
  {
  }

  BehaviorLayerModelPrivate::BehaviorLayerModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir, boost::shared_ptr<BoxInstanceModel> parent)
  {
    elt->getAttribute("name", _name);
    elt->getAttribute("mute", _mute);

    AL::XmlElement::CList behaviorsKeyFrame = elt->children("BehaviorKeyframe", "");
    _behaviorsKeyFrame = XmlUtils::constructObjects<BehaviorKeyFrameModel>(behaviorsKeyFrame, dir, parent);
  }

  BehaviorLayerModel::BehaviorLayerModel(const std::string &name,
                                         bool mute,
                                         const std::list<BehaviorKeyFrameModelPtr> &keys) :
    _p( new BehaviorLayerModelPrivate(name, mute, keys))
  {
  }

  BehaviorLayerModel::BehaviorLayerModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir, boost::shared_ptr<BoxInstanceModel> parent) :
    _p( new BehaviorLayerModelPrivate(elt, dir, parent))
  {
  }

  BehaviorLayerModel::~BehaviorLayerModel()
  {
    delete _p;
  }

  const std::string& BehaviorLayerModel::name() const
  {
    return _p->_name;
  }

  bool BehaviorLayerModel::mute() const
  {
    return _p->_mute;
  }

  const std::list<BehaviorKeyFrameModelPtr>& BehaviorLayerModel::behaviorsKeyFrame() const
  {
    return _p->_behaviorsKeyFrame;
  }

  void BehaviorLayerModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void BehaviorLayerModel::setMute(bool mute)
  {
    _p->_mute = mute;
  }

  void BehaviorLayerModel::addBehaviorKeyFrame(BehaviorKeyFrameModelPtr behaviorKeyFrame)
  {
    _p->_behaviorsKeyFrame.push_front(behaviorKeyFrame);
  }
}
