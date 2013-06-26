/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#ifndef PYTHONBOXGENERATOR_HPP_
#define PYTHONBOXGENERATOR_HPP_

#include <string>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/inputmodel.hpp>
#include <qicore-compat/model/outputmodel.hpp>
#include <qicore-compat/model/boxinterfacemodel.hpp>

namespace qi
{
  std::string generatedClass(BoxInstanceModelPtr instance);
}

#endif /* !PYTHONBOXGENERATOR_HPP_ */
