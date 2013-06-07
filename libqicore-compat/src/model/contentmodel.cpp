/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/contentmodel.hpp>
#include <qicore-compat/model/animationmodel.hpp>
#include <qicore-compat/model/behaviorsequencemodel.hpp>
#include <qicore-compat/model/flowdiagrammodel.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>

#include "contentmodel_p.hpp"

qiLogCategory("QiCore-Compat.ContentModel");

namespace qi
{
  //----------------------------Private Class-------------------------------//
  ContentModelPrivate::ContentModelPrivate() :
    _type(),
    _path(),
    _checksum()
  {
  }

  ContentModelPrivate::ContentModelPrivate(int type, const std::string &path, const std::string &checksum) :
    _type(type),
    _path(path),
    _checksum(checksum)
  {
  }

  ContentModelPrivate::ContentModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir)
  {
    elt->getAttribute("type",     _type);
    elt->getAttribute("path",     _path);
    elt->getAttribute("checksum", _checksum);
    _dir = dir;
  }

  ContentModelPrivate::~ContentModelPrivate()
  {
    if(_content.isValid())
    {
      switch(_type)
      {
      case ContentModel::ContentType_Animation:
        delete _content.ptr<AnimationModel>();
        break;

      case ContentModel::ContentType_BehaviorSequence:
        delete _content.ptr<BehaviorSequenceModel>();
        break;

      case ContentModel::ContentType_FlowDiagram:
        delete _content.ptr<FlowDiagramModel>();
        break;

      default:
        break;
      }
    }
  }

  //-----------------------------Public Class-------------------------------//

  ContentModel::ContentModel(ContentType type, const std::string &path, const std::string &checksum) :
    _p(new ContentModelPrivate(type, path, checksum))
  {
  }

  ContentModel::ContentModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir) :
    _p(new ContentModelPrivate(elt, dir))
  {
  }

  ContentModel::~ContentModel()
  {
    delete _p;
  }

  int ContentModel::type() const
  {
    return _p->_type;
  }

  const std::string& ContentModel::path() const
  {
    return _p->_path;
  }

  const std::string& ContentModel::checksum() const
  {
    return _p->_checksum;
  }

  template<class T>
  AnyReference loadContent(const std::string &path)
  {
    T *ptr = new T(path);


    if(!ptr->loadFromFile())
    {

      delete ptr;
      return AnyReference();
    }
    return AnyReference::fromPtr(ptr);
  }

  template<class T>
  AnyReference loadContent(const std::string &path, BoxInstanceModelPtr parent)
  {
    T *ptr = new T(path, parent);


    if(!ptr->loadFromFile())
    {

      delete ptr;
      return AnyReference();
    }
    return AnyReference::fromPtr(ptr);
  }

  AnyReference ContentModel::content(BoxInstanceModelPtr parent)
  {
    //if content is already loading return content
    if(_p->_content.isValid())
      return _p->_content;

    qiLogDebug() << "First load";

    //Load content
    AnyReference content;
    switch(_p->_type)
    {
    case ContentType_Animation:
      content = loadContent<AnimationModel>(_p->_dir + "/" + _p->_path);
      break;
    case ContentType_BehaviorSequence:
      content = loadContent<BehaviorSequenceModel>(_p->_dir + "/" + _p->_path, parent);
      break;
    case ContentType_FlowDiagram:
      content = loadContent<FlowDiagramModel>(_p->_dir + "/" + _p->_path, parent);
      break;
    default:
      return AnyReference();
      break;
    }

    _p->_content = content;
    return content;
  }

  void ContentModel::setType(ContentType type)
  {
    _p->_type = type;
  }

  void ContentModel::setPath(const std::string& path)
  {
    _p->_path = path;
  }

  void ContentModel::setChecksum(const std::string& checksum)
  {
    _p->_checksum = checksum;
  }
}
