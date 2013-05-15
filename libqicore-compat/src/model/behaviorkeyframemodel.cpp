/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <boost/filesystem.hpp>

#include <qicore-compat/model/flowdiagrammodel.hpp>
#include <qicore-compat/model/behaviorkeyframemodel.hpp>
#include "behaviorkeyframemodel_p.hpp"


namespace qi {
  BehaviorKeyFrameModelPrivate::BehaviorKeyFrameModelPrivate() :
    _name(),
    _index(),
    _bitmap(),
    _path()
  {
  }

  BehaviorKeyFrameModelPrivate::BehaviorKeyFrameModelPrivate(const std::string &name, int index, const std::string &bitmap, const std::string &path) :
    _name(name),
    _index(index),
    _bitmap(bitmap),
    _path(path)
  {
  }

  BehaviorKeyFrameModelPrivate::BehaviorKeyFrameModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir)
  {
    elt->getAttribute("name",   _name);
    elt->getAttribute("index",  _index);
    elt->getAttribute("bitmap", _bitmap);
    elt->getAttribute("path",   _path);
    _path = dir + "/" + _path;
  }

  BehaviorKeyFrameModel::BehaviorKeyFrameModel(const std::string &name, int index, const std::string &bitmap, const std::string &path) :
    _p(new BehaviorKeyFrameModelPrivate(name, index, bitmap, path))
  {
  }

  BehaviorKeyFrameModel::BehaviorKeyFrameModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir) :
    _p( new BehaviorKeyFrameModelPrivate(elt, dir) )
  {
  }

  BehaviorKeyFrameModel::~BehaviorKeyFrameModel()
  {
    delete _p;
  }

  const std::string& BehaviorKeyFrameModel::getName() const
  {
    return _p->_name;
  }

  int BehaviorKeyFrameModel::getIndex() const
  {
    return _p->_index;
  }

  const std::string& BehaviorKeyFrameModel::getBitmap() const
  {
    return _p->_bitmap;
  }

  std::string BehaviorKeyFrameModel::getPath() const
  {
    boost::filesystem::path path(_p->_path);
    return path.filename().string();
  }

  FlowDiagramModelPtr BehaviorKeyFrameModel::getDiagram() const
  {
    FlowDiagramModelPtr flowDiagram = FlowDiagramModelPtr(new FlowDiagramModel(_p->_path));

    if(!flowDiagram->loadFromFile())
      return FlowDiagramModelPtr();

    return flowDiagram;
  }

  void BehaviorKeyFrameModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void BehaviorKeyFrameModel::setIndex(int index)
  {
    _p->_index = index;
  }

  void BehaviorKeyFrameModel::setBitmap(const std::string& bitmap)
  {
    _p->_bitmap = bitmap;
  }

  void BehaviorKeyFrameModel::setPath(const std::string& path)
  {
    _p->_path = path;
  }
}
