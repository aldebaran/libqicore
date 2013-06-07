/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <boost/filesystem.hpp>
#include <alserial/alserial.h>

#include <qicore-compat/model/behaviorsequencemodel.hpp>
#include "behaviorsequencemodel_p.hpp"
#include "xmlutils.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.BehaviorSequenceModel");

namespace qi {

  // ------------------------------ Private Class -------------------------------------//
  BehaviorSequenceModelPrivate::BehaviorSequenceModelPrivate(const std::string &path,
                                                             boost::shared_ptr<BoxInstanceModel> parent,
                                                             int fps,
                                                             int startFrame,
                                                             int endFrame,
                                                             int size,
                                                             const std::string &formatVersion) :
    _path(path),
    _fps(fps),
    _startFrame(startFrame),
    _endFrame(endFrame),
    _size(size),
    _formatVersion(formatVersion),
    _parent(parent)
  {
  }

  bool BehaviorSequenceModelPrivate::loadFromFile()
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

    root->getAttribute("fps",            _fps);
    root->getAttribute("start_frame",    _startFrame);
    root->getAttribute("end_frame",      _endFrame);
    root->getAttribute("size",           _size);
    root->getAttribute("format_version", _formatVersion);
    boost::filesystem::path path(_path);
    AL::XmlElement::CList behaviorsLayer = root->children("BehaviorLayer", "");
    _behaviorsLayer = XmlUtils::constructObjects<BehaviorLayerModel>(behaviorsLayer, path.parent_path().string(), _parent);

    return true;
  }

  // -------------------------------- Public Class ----------------------------------//
  BehaviorSequenceModel::BehaviorSequenceModel(const std::string &path,
                                               boost::shared_ptr<BoxInstanceModel> parent,
                                               int fps,
                                               int startFrame,
                                               int endFrame,
                                               int size,
                                               const std::string &formatVersion) :
    _p( new BehaviorSequenceModelPrivate(path,
                                         parent,
                                         fps,
                                         startFrame,
                                         endFrame,
                                         size,
                                         formatVersion))
  {
  }

  BehaviorSequenceModel::~BehaviorSequenceModel()
  {
    delete _p;
  }

  std::string BehaviorSequenceModel::path() const
  {
    boost::filesystem::path path(_p->_path);
    return path.filename().string();
  }

  int BehaviorSequenceModel::fps() const
  {
    return _p->_fps;
  }

  int BehaviorSequenceModel::startFrame() const
  {
    return _p->_startFrame;
  }

  int BehaviorSequenceModel::endFrame() const
  {
    return _p->_endFrame;
  }

  int BehaviorSequenceModel::size() const
  {
    return _p->_size;
  }

  const std::string& BehaviorSequenceModel::formatVersion() const
  {
    return _p->_formatVersion;
  }

  const std::list<BehaviorLayerModelPtr>& BehaviorSequenceModel::behaviorsLayer() const
  {
    return _p->_behaviorsLayer;
  }

  void BehaviorSequenceModel::setPath(const std::string& path)
  {
    _p->_path = path;
  }

  void BehaviorSequenceModel::setFPS(int fps)
  {
    _p->_fps = fps;
  }

  void BehaviorSequenceModel::setStartFrame(int startFrame)
  {
    _p->_startFrame = startFrame;
  }

  void BehaviorSequenceModel::setEndFrame(int endFrame)
  {
    _p->_endFrame = endFrame;
  }

  void BehaviorSequenceModel::setSize(int size)
  {
    _p->_size = size;
  }

  void BehaviorSequenceModel::setFormatVersion(const std::string& formatVersion)
  {
    _p->_formatVersion = formatVersion;
  }

  void BehaviorSequenceModel::addBehaviorLayer(BehaviorLayerModelPtr behaviorLayer)
  {
    _p->_behaviorsLayer.push_front(behaviorLayer);
  }

  bool BehaviorSequenceModel::loadFromFile()
  {
    return _p->loadFromFile();
  }
}
