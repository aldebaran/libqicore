/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <alserial/alserial.h>
#include <boost/filesystem.hpp>

#include <qicore-compat/model/animationmodel.hpp>
#include "animationmodel_p.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.AnimationModel");

namespace qi
{
  //-------------------------Private Class--------------------------------//

  AnimationModelPrivate::AnimationModelPrivate(const std::string &path,
                                               int fps,
                                               int startFrame,
                                               int endFrame,
                                               AnimationModel::MotionResourcesHandler resources,
                                               int size,
                                               const std::string &formatVersion,
                                               ActuatorListModelPtr actuatorList) :
    _path(path),
    _fps(fps),
    _startFrame(startFrame),
    _endFrame(endFrame),
    _resourcesAcuqisition(resources),
    _size(size),
    _formatVersion(formatVersion),
    _actuatorList(actuatorList)
  {
  }

  bool AnimationModelPrivate::loadFromFile()
  {
    std::string errorMsg;
    boost::shared_ptr<AL::XmlDocument> xmlFile = AL::XmlDocument::loadFromXmlFile(_path,
                                                                                  NULL,
                                                                                  &errorMsg);

    if(!xmlFile)
    {
      qiLogError() << "Failed to open the given file : "
                   << _path
                   << std::endl
                   << errorMsg
                   << std::endl;
      return false;
    }

    boost::shared_ptr<const AL::XmlElement> root = xmlFile->root();

    if(root == NULL)
    {
      qiLogError() << "No root element in the given file : "
                   << _path
                   << std::endl;
      return false;
    }

    int resourcesAcquisition;
    root->getAttribute("fps",            _fps);
    root->getAttribute("start_frame",    _startFrame);
    root->getAttribute("end_frame",      _endFrame);
    root->getAttribute("resources_acquisition", resourcesAcquisition);
    root->getAttribute("size",           _size);
    root->getAttribute("format_version", _formatVersion);

    AL::XmlElement::CList actuator_list = root->children("ActuatorList", "");

    if( actuator_list.size() != 1 )
    {
      qiLogError() << "Exactly one tag \"ActuatorList\" is required in file : "
                   << _path
                   << std::endl;
      return false;
    }

    _actuatorList = ActuatorListModelPtr(new ActuatorListModel(actuator_list.front()));

    return _actuatorList->isValid();
  }

  AnimationModel::AnimationModel(const std::string &path,
                                 int fps,
                                 int startFrame,
                                 int endFrame,
                                 AnimationModel::MotionResourcesHandler resources,
                                 int size,
                                 const std::string &formatVersion,
                                 ActuatorListModelPtr actuatorList) :
    _p(new AnimationModelPrivate(path, fps, startFrame, endFrame, resources, size, formatVersion, actuatorList))
  {
  }

  AnimationModel::~AnimationModel()
  {
    delete _p;
  }

  std::string AnimationModel::path() const
  {
    boost::filesystem::path path(_p->_path);
    return path.filename().string();
  }

  int AnimationModel::fps() const
  {
    return _p->_fps;
  }

  int AnimationModel::startFrame() const
  {
    return _p->_startFrame;
  }

  int AnimationModel::endFrame() const
  {
    return _p->_endFrame;
  }

  AnimationModel::MotionResourcesHandler AnimationModel::resourcesAcquisition() const
  {
    return _p->_resourcesAcuqisition;
  }

  int AnimationModel::size() const
  {
    return _p->_size;
  }

  const std::string& AnimationModel::formatVersion() const
  {
    return _p->_formatVersion;
  }

  ActuatorListModelPtr AnimationModel::actuatorList() const
  {
    return _p->_actuatorList;
  }

  void AnimationModel::setPath(const std::string &path)
  {
    _p->_path = path;
  }

  void AnimationModel::setFPS(int fps)
  {
    _p->_fps = fps;
  }

  void AnimationModel::setStartFrame(int start_frame)
  {
    _p->_startFrame = start_frame;
  }

  void AnimationModel::setEndFrame(int end_frame)
  {
    _p->_endFrame = end_frame;
  }

  void AnimationModel::setResourcesAcquisition(MotionResourcesHandler resources)
  {
    _p->_resourcesAcuqisition = resources;
  }

  void AnimationModel::setSize(int size)
  {
    _p->_size = size;
  }

  void AnimationModel::setFormatVersion(const std::string &format_version)
  {
    _p->_formatVersion = format_version;
  }

  void AnimationModel::setActuatorList(ActuatorListModelPtr actuator_list)
  {
    _p->_actuatorList = actuator_list;
  }

  bool AnimationModel::loadFromFile()
  {
    return _p->loadFromFile();
  }
}
