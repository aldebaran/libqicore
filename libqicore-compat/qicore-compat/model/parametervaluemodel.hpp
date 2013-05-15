/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETERVALUE_H_
# define PARAMETERVALUE_H_

# include <qicore-compat/api.hpp>
# include <alserial/alserial.h>

namespace qi {
  class ParameterValueModelPrivate;
  class BoxInterfaceModel;

  class QICORECOMPAT_API ParameterValueModel {
  public:
    ParameterValueModel();
    ParameterValueModel(boost::shared_ptr<const AL::XmlElement> elt, boost::shared_ptr<BoxInterfaceModel> interface);

    virtual ~ParameterValueModel();

    int getId()                  const;
    int getType()                const;
    bool getValueBool()          const;
    int getValueInt()            const;
    double getValueDouble()      const;
    std::string getValueString() const;

    void setId(int id);
    void setValueBool(bool value);
    void setValueInt(int value);
    void setValueDouble(double value);
    void setValueString(const std::string& value);

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(ParameterValueModel);
    ParameterValueModelPrivate* _p;
  };
  typedef boost::shared_ptr<ParameterValueModel> ParameterValueModelPtr;
}

#endif /* !PARAMETERVALUE_H_ */
