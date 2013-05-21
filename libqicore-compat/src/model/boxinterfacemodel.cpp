/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <boost/filesystem.hpp>

#include <alserial/alserial.h>

#include <qicore-compat/model/boxinterfacemodel.hpp>
#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/contentmodel.hpp>
#include "boxinterfacemodel_p.hpp"

#include "xmlutils.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.BoxInterfaceModel");

namespace qi {
  //------------------------------------Private Class---------------------------------------//
  BoxInterfaceModelPrivate::BoxInterfaceModelPrivate(const std::string &path,
                                                     const std::string &uuid,
                                                     const std::string &boxVersion,
                                                     const std::string &name,
                                                     const std::string &tooltip,
                                                     const std::string &formatVersion,
                                                     const std::string &plugin,
                                                     const std::list<BitmapModelPtr> &bitmaps,
                                                     const std::list<ResourceModelPtr> &resources,
                                                     const std::list<ParameterModelPtr> &parameters,
                                                     const std::list<InputModelPtr> &inputs,
                                                     const std::list<OutputModelPtr> &outputs,
                                                     const ContentsModelPtr &contents) :
    _path(path),
    _uuid(uuid),
    _boxVersion(boxVersion),
    _name(name),
    _tooltip(tooltip),
    _plugin(plugin),
    _formatVersion(formatVersion),
    _bitmaps(bitmaps),
    _resources(resources),
    _parameters(parameters),
    _inputs(inputs),
    _outputs(outputs),
    _contents(contents)
  {
  }

  bool BoxInterfaceModelPrivate::loadFromFile()
  {
    //Try open the file given by this->_path
    std::string errorMsg;
    boost::shared_ptr<AL::XmlDocument> xmlFile = AL::XmlDocument::loadFromXmlFile(_path,
                                                                                  NULL,
                                                                                  &errorMsg);

    if (!xmlFile)
    {
      qiLogError() << "Failed to open the given file : "
                   << _path
                   << std::endl
                   << errorMsg
                   << std::endl;
      return false;
    }


    boost::shared_ptr<const AL::XmlElement> root = xmlFile->root();

    if (root == NULL)
    {
      qiLogError() << "No root element in the given file : "
                   << _path
                   << std::endl;
      return false;
    }

    root->getAttribute("uuid", _uuid);
    root->getAttribute("box_version", _boxVersion);
    root->getAttribute("name", _name);
    root->getAttribute("tooltip", _tooltip);
    root->getAttribute("plugin", _plugin);
    root->getAttribute("format_version", _formatVersion);

    AL::XmlElement::CList bitmaps   = root->children("Bitmap" , "");

    if (bitmaps.size() > 5)
    {
      qiLogError() << "Too many Bitmap (max 4) tag in file : "
                   << _path
                   << std::endl;
      return false;
    }

    _bitmaps = XmlUtils::constructObjects<Bitmap>(bitmaps);

    AL::XmlElement::CList resources = root->children("Resource", "");
    _resources = XmlUtils::constructObjects<ResourceModel>(resources);

    for(std::list<ResourceModelPtr>::const_iterator it = _resources.begin(),
        itEnd = _resources.end();
        it != itEnd; ++it)
    {
      if((*it)->getLockType() == ResourceModel::LockType_Error)
      {
        qiLogError() << "In file : "
                     << _path
                     << ". Invalid type in tag Resource.name = "
                     << (*it)->getName()
                     << std::endl;
        return false;
      }
    }

    AL::XmlElement::CList parameters = root->children("Parameter", "");
    _parameters = XmlUtils::constructObjects<ParameterModel>(parameters);

    for(std::list<ParameterModelPtr>::const_iterator it = _parameters.begin(),
        itEnd = _parameters.end();
        it != itEnd; ++it)
    {
      ParameterModelPtr parameter = *it;
      if(!parameter->isValid())
      {
        qiLogError() << "In file : "
                     << _path
                     << ". Invalid type in tag Parameter.name = "
                     << parameter->metaProperty().name()
                     << std::endl;
        return false;
      }
    }

    AL::XmlElement::CList inputs = root->children("Input", "");

    if (inputs.size() < 1)
    {
      qiLogError() << "At least one tag Input is required in file : "
                   << _path
                   << std::endl;
      return false;
    }
    _inputs = XmlUtils::constructObjects<InputModel>(inputs);

    AL::XmlElement::CList outputs = root->children("Output", "");
    _outputs = XmlUtils::constructObjects<OutputModel>(outputs);

    AL::XmlElement::CList contents = root->children("Contents", "");


    if(contents.empty() || contents.size() > 1)
    {
      qiLogError() << "Exactly one tag Contents is required in file"
                   << _path
                   << std::endl;
      return false;
    }

    boost::filesystem::path boxPath = boost::filesystem::path(_path);
    _contents = ContentsModelPtr(new ContentsModel(contents.front(), boxPath.parent_path().string()));

    return true;
  }

  //-----------------------------------------Public Class------------------------------------//
  BoxInterfaceModel::BoxInterfaceModel(const std::string &path,
                                       const std::string &uuid,
                                       const std::string &boxVersion,
                                       const std::string &name,
                                       const std::string &tooltip,
                                       const std::string &formatVersion,
                                       const std::string &plugin,
                                       const std::list<BitmapModelPtr> &bitmaps,
                                       const std::list<ResourceModelPtr> &resources,
                                       const std::list<ParameterModelPtr> &parameters,
                                       const std::list<InputModelPtr> &inputs,
                                       const std::list<OutputModelPtr> &outputs,
                                       const ContentsModelPtr &contents) :
    _p(new BoxInterfaceModelPrivate(path,
                                    uuid,
                                    boxVersion,
                                    name,
                                    tooltip,
                                    formatVersion,
                                    plugin,
                                    bitmaps,
                                    resources,
                                    parameters,
                                    inputs,
                                    outputs,
                                    contents))
  {
  }

  BoxInterfaceModel::~BoxInterfaceModel()
  {
    delete _p;
  }

  bool BoxInterfaceModel::loadFromFile()
  {
    return _p->loadFromFile();
  }

  //--------------------------------------------Getter------------------------------------//
  std::string BoxInterfaceModel::getPath() const
  {
    boost::filesystem::path path(_p->_path);
    return path.filename().string();
  }

  const std::string& BoxInterfaceModel::getUuid() const
  {
    return _p->_uuid;
  }

  const std::string& BoxInterfaceModel::getBoxVersion() const
  {
    return _p->_boxVersion;
  }

  const std::string& BoxInterfaceModel::getName() const
  {
    return _p->_name;
  }

  const std::string& BoxInterfaceModel::getTooltip() const
  {
    return _p->_tooltip;
  }

  const std::string& BoxInterfaceModel::getPlugin() const
  {
    return _p->_plugin;
  }

  const std::string& BoxInterfaceModel::getFormatVersion() const
  {
    return _p->_formatVersion;
  }

  const std::list<BitmapModelPtr>& BoxInterfaceModel::getBitmaps() const
  {
    return _p->_bitmaps;
  }

  const std::list<ResourceModelPtr>& BoxInterfaceModel::getResources() const
  {
    return _p->_resources;
  }

  const std::list<ParameterModelPtr>& BoxInterfaceModel::getParameters() const
  {
    return _p->_parameters;
  }

  const std::list<InputModelPtr>& BoxInterfaceModel::getInputs() const
  {
    return _p->_inputs;
  }

  const std::list<OutputModelPtr>& BoxInterfaceModel::getOutputs() const
  {
    return _p->_outputs;
  }

  boost::shared_ptr<ContentsModel> BoxInterfaceModel::getContents() const
  {
    return _p->_contents;
  }

  //-----------------------------------------Setter------------------------------//
  void BoxInterfaceModel::setPath(const std::string& path)
  {
    _p->_path = path;
  }

  void BoxInterfaceModel::setUuid(const std::string& uuid)
  {
    _p->_uuid = uuid;
  }

  void BoxInterfaceModel::setBoxVersion(const std::string& box_version)
  {
    _p->_boxVersion = box_version;
  }

  void BoxInterfaceModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void BoxInterfaceModel::setTooltip(const std::string& tooltip)
  {
    _p->_tooltip = tooltip;
  }

  void BoxInterfaceModel::setPlugin(const std::string& plugin)
  {
    _p->_plugin = plugin;
  }

  void BoxInterfaceModel::setFormatVersion(const std::string& format_version)
  {
    _p->_formatVersion = format_version;
  }

  bool BoxInterfaceModel::addBitmap(BitmapModelPtr bitmap)
  {
    if(_p->_bitmaps.size() > 5)
      return false;

    _p->_bitmaps.push_front(bitmap);
    return true;
  }

  void BoxInterfaceModel::addResource(ResourceModelPtr ressource)
  {
    _p->_resources.push_front(ressource);
  }

  bool BoxInterfaceModel::addParameter(ParameterModelPtr parameter)
  {
    if(!parameter->isValid())
      return false;

    _p->_parameters.push_front(parameter);
    return true;
  }

  void BoxInterfaceModel::addInput(InputModelPtr input)
  {
    _p->_inputs.push_front(input);
  }

  void BoxInterfaceModel::addOutput(OutputModelPtr output)
  {
    _p->_outputs.push_front(output);
  }

  void BoxInterfaceModel::addContent(ContentModelPtr content)
  {
    if(!_p->_contents)
      _p->_contents = ContentsModelPtr(new ContentsModel());

    _p->_contents->addContent(content);
  }

  ParameterModelPtr BoxInterfaceModel::findParameter(unsigned int id) const
  {
    ParameterModelPtr rest = ParameterModelPtr();

    for(std::list<ParameterModelPtr>::const_iterator it = _p->_parameters.begin(),
        itEnd = _p->_parameters.end();
        it != itEnd; ++it)
    {
      ParameterModelPtr ptr = *it;

      if(ptr->metaProperty().uid() == id)
      {
        rest = ptr;
        break;
      }
    }

    return rest;
  }
}
