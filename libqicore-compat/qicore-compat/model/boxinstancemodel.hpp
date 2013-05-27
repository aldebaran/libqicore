/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOXINSTANCE_H_
# define BOXINSTANCE_H_

# include <alserial/alserial.h>
# include <qicore-compat/api.hpp>

namespace qi {
  class BoxInstanceModelPrivate;
  class BoxInterfaceModel;
  class ParameterValueModel;

  class QICORECOMPAT_API BoxInstanceModel
  {
  public:
    BoxInstanceModel(const std::string &name, int id, int x, int y, boost::shared_ptr<BoxInterfaceModel> interface);
    BoxInstanceModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir);
    virtual ~BoxInstanceModel();

    const std::string& name() const;
    int id() const;
    int x() const;
    int y() const;
    const std::string& path() const;
    boost::shared_ptr<BoxInterfaceModel> interface() const;

    std::list<boost::shared_ptr<ParameterValueModel> > parametersValue() const;
    const std::string& plugin() const;

    void setName(const std::string& name);
    void setId(int id);
    void setX(int x);
    void setY(int y);
    void setBoxInterface(boost::shared_ptr<BoxInterfaceModel> interface);
    bool addParameterValue(boost::shared_ptr<ParameterValueModel> value);

    bool isValid() const;

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(BoxInstanceModel);
    BoxInstanceModelPrivate *_p;
  };
  typedef boost::shared_ptr<BoxInstanceModel> BoxInstanceModelPtr;
}

#endif /* !BOXINSTANCE_H_ */
