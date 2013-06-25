/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <boost/filesystem.hpp>
#include <alserial/alserial.h>
#include <boost/foreach.hpp>

#include <qicore-compat/model/flowdiagrammodel.hpp>
#include "flowdiagrammodel_p.hpp"
#include "xmlutils.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.FlowDiagramModel");

#define foreach BOOST_FOREACH

namespace qi {
  FlowDiagramModelPrivate::FlowDiagramModelPrivate(const std::string &path,
                                                   boost::shared_ptr<BoxInstanceModel> parent,
                                                   float scale, const std::string &formatVersion,
                                                   const std::list<LinkModelPtr> &links,
                                                   const std::map<int, BoxInstanceModelPtr> &boxsInstance) :
    _path(path),
    _scale(scale),
    _formatVersion(formatVersion),
    _links(links),
    _boxsInstance(boxsInstance),
    _parent(parent)
  {
  }

  bool FlowDiagramModelPrivate::loadFromFile()
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

    root->getAttribute("scale", _scale);
    root->getAttribute("format_version", _formatVersion);

    AL::XmlElement::CList links = root->children("Link", "");
    _links = XmlUtils::constructObjects<LinkModel>(links);

    AL::XmlElement::CList boxsInstance = root->children("BoxInstance", "");

    boost::filesystem::path boxPath = boost::filesystem::path(_path);
    std::list<BoxInstanceModelPtr> instances = XmlUtils::constructObjects<BoxInstanceModel>(boxsInstance, boxPath.parent_path().string(), _parent);

    bool isValid = true;
    isValid = XmlUtils::verifyObjects<BoxInstanceModel>(instances);

    foreach(BoxInstanceModelPtr instance, instances)
    {
      _boxsInstance[instance->id()] = instance;
    }

    return isValid;
  }

  FlowDiagramModel::FlowDiagramModel(const std::string &path,
                                     boost::shared_ptr<BoxInstanceModel> parent,
                                     float scale,
                                     const std::string &formatVersion,
                                     const std::list<LinkModelPtr> &links,
                                     const std::map<int, BoxInstanceModelPtr> &boxsInstance) :
    _p( new FlowDiagramModelPrivate(path,
                                    parent,
                                    scale,
                                    formatVersion,
                                    links,
                                    boxsInstance))
  {
  }

  FlowDiagramModel::~FlowDiagramModel()
  {
    delete _p;
  }

  bool FlowDiagramModel::loadFromFile()
  {
    return _p->loadFromFile();
  }

  std::string FlowDiagramModel::path() const
  {
    boost::filesystem::path path(_p->_path);
    return path.filename().string();
  }

  float FlowDiagramModel::scale() const
  {
    return _p->_scale;
  }

  const std::string& FlowDiagramModel::formatVersion() const
  {
    return _p->_formatVersion;
  }

  const std::list<LinkModelPtr>& FlowDiagramModel::links() const
  {
    return _p->_links;
  }

  const std::map<int, BoxInstanceModelPtr>& FlowDiagramModel::boxsInstance() const
  {
    return _p->_boxsInstance;
  }

  BoxInstanceModelPtr FlowDiagramModel::findInstance(int id) const
  {
    if(id == 0)
      return _p->_parent;

    BoxInstanceModelMap::const_iterator it = _p->_boxsInstance.find(id);

    if(it == _p->_boxsInstance.end())
      return BoxInstanceModelPtr();

    return it->second;
  }

  void FlowDiagramModel::setPath(const std::string& path)
  {
    _p->_path = path;
  }

  void FlowDiagramModel::setScale(float scale)
  {
    _p->_scale = scale;
  }

  void FlowDiagramModel::setFormatVersion(const std::string& formatVersion)
  {
    _p->_formatVersion = formatVersion;
  }

  void FlowDiagramModel::addLink(LinkModelPtr link)
  {
    _p->_links.push_front(link);
  }

  void FlowDiagramModel::addBoxInstance(BoxInstanceModelPtr boxInstance)
  {
    _p->_boxsInstance[boxInstance->id()] = boxInstance;
  }
}
