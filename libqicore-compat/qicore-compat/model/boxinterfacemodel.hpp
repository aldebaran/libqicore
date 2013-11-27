/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOXINTERFACE_H_
#define BOXINTERFACE_H_

#include <string>
#include <list>
#include <map>
#include <boost/shared_ptr.hpp>

#include <qicore-compat/api.hpp>
#include <qicore-compat/model/inputmodel.hpp>

namespace qi
{
  class BoxInterfaceModelPrivate;
  class Bitmap;
  class ResourceModel;
  class ParameterModel;
  class OutputModel;
  class ContentsModel;
  class ContentModel;

  class QICORECOMPAT_API BoxInterfaceModel : private boost::noncopyable
  {
  public:
    typedef std::map<int, boost::shared_ptr<InputModel> > InputModelMap;
    typedef std::map<int, boost::shared_ptr<OutputModel> > OutputModelMap;

    BoxInterfaceModel(const std::string &path,
                      const std::string &uuid = "",
                      const std::string &boxVersion = "",
                      const std::string &name = "",
                      const std::string &tooltip = "",
                      const std::string &formatVersion = "4",
                      const std::string &plugin = "None",
                      const std::list<boost::shared_ptr<Bitmap> > &bitmaps = std::list<boost::shared_ptr<Bitmap> >(),
                      const std::list<boost::shared_ptr<ResourceModel> > &resources = std::list<boost::shared_ptr<ResourceModel> >(),
                      const std::list<boost::shared_ptr<ParameterModel> > &parameters = std::list<boost::shared_ptr<ParameterModel> >(),
                      const InputModelMap &inputs = InputModelMap(),
                      const OutputModelMap &outputs = OutputModelMap(),
                      const boost::shared_ptr<ContentsModel> &contents   = boost::shared_ptr<ContentsModel>()
        );

    virtual ~BoxInterfaceModel();

    std::string path() const;
    const std::string& uuid() const;
    const std::string& boxVersion() const;
    const std::string& name() const;
    const std::string& tooltip() const;
    const std::string& plugin() const;
    const std::string& formatVersion() const;
    const std::list<boost::shared_ptr<Bitmap> >& bitmaps() const;
    const std::list<boost::shared_ptr<ResourceModel> >& resources() const;
    const std::list<boost::shared_ptr<ParameterModel> >& parameters() const;
    const std::map<int, boost::shared_ptr<InputModel> >& inputs() const;
    const std::map<int, boost::shared_ptr<OutputModel> >& outputs() const;
    boost::shared_ptr<ContentsModel> contents() const;

    void setPath(const std::string& path);
    void setUuid(const std::string& uuid);
    void setBoxVersion(const std::string& box_version);
    void setName(const std::string& name);
    void setTooltip(const std::string& tooltip);
    void setPlugin(const std::string& plugin);
    void setFormatVersion(const std::string& format_version);

    bool addBitmap(boost::shared_ptr<Bitmap> bitmap);
    void addResource(boost::shared_ptr<ResourceModel> resource);
    bool addParameter(boost::shared_ptr<ParameterModel> parameter);
    void addInput(boost::shared_ptr<InputModel> input);
    void addOutput(boost::shared_ptr<OutputModel> output);
    void addContent(boost::shared_ptr<ContentModel> content);

    bool loadFromFile();

    bool hasResource() const;
    bool hasTimeline() const;
    bool hasFlowDiagram() const;

    boost::shared_ptr<ParameterModel> findParameter(unsigned int id) const;
    std::string findSignal(int id) const;
    std::string findMethod(int id) const;
    std::string findInput(InputModel::InputNature nature) const;

  private:
    BoxInterfaceModelPrivate *_p;
  };
  typedef boost::shared_ptr<BoxInterfaceModel> BoxInterfaceModelPtr;
}

#endif/* !BOXINTERFACE_H_ */
