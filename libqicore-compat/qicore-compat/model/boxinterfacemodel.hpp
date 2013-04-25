/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOXINTERFACE_H_
#define BOXINTERFACE_H_

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include <qicore-compat/api.hpp>

namespace qi
{
  class BoxInterfaceModelPrivate;
  class Bitmap;
  class ResourceModel;
  class ParameterModel;
  class OutputModel;
  class InputModel;
  class ContentsModel;
  class ContentModel;

  class QICORECOMPAT_API BoxInterfaceModel
  {
  public:
    BoxInterfaceModel(const std::string &path,
                      const std::string &uuid = "",
                      const std::string &boxVersion = "",
                      const std::string &name = "",
                      const std::string &tooltip = "",
                      const std::string &formatVersion = "4",
                      const std::string &plugin = "None",
                      const std::list<boost::shared_ptr<Bitmap> > &bitmaps    = std::list<boost::shared_ptr<Bitmap> >(),
                      const std::list<boost::shared_ptr<ResourceModel> > &resources  = std::list<boost::shared_ptr<ResourceModel> >(),
                      const std::list<boost::shared_ptr<ParameterModel> > &parameters = std::list<boost::shared_ptr<ParameterModel> >(),
                      const std::list<boost::shared_ptr<InputModel> > &inputs     = std::list<boost::shared_ptr<InputModel> >(),
                      const std::list<boost::shared_ptr<OutputModel> > &outputs    = std::list<boost::shared_ptr<OutputModel> >(),
                      const boost::shared_ptr<ContentsModel> &contents   = boost::shared_ptr<ContentsModel>()
        );

    virtual ~BoxInterfaceModel();

    std::string getPath() const;
    const std::string& getUuid() const;
    const std::string& getBoxVersion() const;
    const std::string& getName() const;
    const std::string& getTooltip() const;
    const std::string& getPlugin() const;
    const std::string& getFormatVersion() const;
    const std::list<boost::shared_ptr<Bitmap> >& getBitmaps() const;
    const std::list<boost::shared_ptr<ResourceModel> >& getResources() const;
    const std::list<boost::shared_ptr<ParameterModel> >& getParameters() const;
    const std::list<boost::shared_ptr<InputModel> >& getInputs() const;
    const std::list<boost::shared_ptr<OutputModel> >& getOutputs() const;
    boost::shared_ptr<ContentsModel> getContents() const;

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

    boost::shared_ptr<ParameterModel> findParameter(int id) const;

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(BoxInterfaceModel);
    BoxInterfaceModelPrivate *_p;
  };
  typedef boost::shared_ptr<BoxInterfaceModel> BoxInterfaceModelPtr;
}

#endif/* !BOXINTERFACE_H_ */
