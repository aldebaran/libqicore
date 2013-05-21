/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <boost/filesystem.hpp>
#include <alserial/alserial.h>

#include <qicore-compat/model/flowdiagrammodel.hpp>
#include "flowdiagrammodel_p.hpp"
#include "xmlutils.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.FlowDiagramModel");

namespace qi {
  FlowDiagramModelPrivate::FlowDiagramModelPrivate(const std::string &path,
                                                   float scale, const std::string &formatVersion,
                                                   const std::list<LinkModelPtr> &links,
                                                   const std::list<BoxInstanceModelPtr> &boxsInstance) :
    _path(path),
    _scale(scale),
    _formatVersion(formatVersion),
    _links(links),
    _boxsInstance(boxsInstance)
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
    _boxsInstance = XmlUtils::constructObjects<BoxInstanceModel>(boxsInstance, boxPath.parent_path().string());

    bool isValid = true;
    isValid = XmlUtils::verifyObjects<BoxInstanceModel>(_boxsInstance);

    return isValid;
  }

  FlowDiagramModel::FlowDiagramModel(const std::string &path,
                           float scale,
                           const std::string &formatVersion,
                           const std::list<LinkModelPtr> &links,
                           const std::list<BoxInstanceModelPtr> &boxsInstance) :
    _p( new FlowDiagramModelPrivate(path,
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

  std::string FlowDiagramModel::getPath() const
  {
    boost::filesystem::path path(_p->_path);
    return path.filename().string();
  }

  float FlowDiagramModel::getScale() const
  {
    return _p->_scale;
  }

  const std::string& FlowDiagramModel::getFormatVersion() const
  {
    return _p->_formatVersion;
  }

  const std::list<LinkModelPtr>& FlowDiagramModel::getLinks() const
  {
    return _p->_links;
  }

  const std::list<BoxInstanceModelPtr>& FlowDiagramModel::getBoxsInstance() const
  {
    return _p->_boxsInstance;
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
    _p->_boxsInstance.push_front(boxInstance);
  }
}
