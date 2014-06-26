/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOXINSTANCE_H_
#define BOXINSTANCE_H_

#include <boost/enable_shared_from_this.hpp>
#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>
#include <qi/anyvalue.hpp>
#include <qicore-compat/model/contentmodel.hpp>

#ifdef interface
  #undef interface
#endif

namespace qi {
  class BoxInstanceModelPrivate;
  class BoxInterfaceModel;
  class ParameterValueModel;

  class QICORECOMPAT_API BoxInstanceModel : public boost::enable_shared_from_this<BoxInstanceModel>, private boost::noncopyable
  {
  public:
    BoxInstanceModel(const std::string &name, int id, int x, int y, boost::shared_ptr<BoxInterfaceModel> interface);
    BoxInstanceModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir, boost::shared_ptr<BoxInstanceModel> parent);
    virtual ~BoxInstanceModel();

    const std::string& name() const;
    int id() const;
    std::string uid() const;
    int x() const;
    int y() const;
    std::string path() const;
    std::string behaviorPath() const;
    boost::shared_ptr<BoxInterfaceModel> interface() const;
    AnyReference parameter(int id);

    std::list<boost::shared_ptr<ParameterValueModel> > parametersValue() const;
    const std::string& plugin() const;
    boost::shared_ptr<BoxInstanceModel> parent();

    void setName(const std::string& name);
    void setId(int id);
    void setX(int x);
    void setY(int y);
    void setBoxInterface(boost::shared_ptr<BoxInterfaceModel> interface);
    bool addParameterValue(boost::shared_ptr<ParameterValueModel> value);
    AnyReference content(ContentModel::ContentType type);

    bool isValid() const;

  private:
    BoxInstanceModelPrivate *_p;
  };
  typedef boost::shared_ptr<BoxInstanceModel> BoxInstanceModelPtr;
  typedef std::map<int, BoxInstanceModelPtr> BoxInstanceModelMap;
}

#endif /* !BOXINSTANCE_H_ */
