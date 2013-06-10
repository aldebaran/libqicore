/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOXINTERFACE_P_H_
# define BOXINTERFACE_P_H_

#include <string>
#include <list>

#include <qicore-compat/model/bitmapmodel.hpp>
#include <qicore-compat/model/resourcemodel.hpp>
#include <qicore-compat/model/inputmodel.hpp>
#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/outputmodel.hpp>
#include <qicore-compat/model/contentsmodel.hpp>

namespace qi
{
  class ParameterModel;
  class BoxInterfaceModelPrivate
  {
    friend class BoxInterfaceModel;

  public:
    BoxInterfaceModelPrivate(const std::string &path,
                             const std::string &uuid,
                             const std::string &boxVersion,
                             const std::string &name,
                             const std::string &tooltip,
                             const std::string &formatVersion,
                             const std::string &plugin,
                             const std::list<BitmapModelPtr> &bitmaps,
                             const std::list<ResourceModelPtr> &resources,
                             const std::list<ParameterModelPtr> &parameters,
                             const std::map<int, InputModelPtr> &inputs,
                             const std::map<int, OutputModelPtr> &outputs,
                             const ContentsModelPtr &contents);

    bool loadFromFile();

  private:
    std::string _path;
    std::string _uuid;
    std::string _boxVersion;
    std::string _name;
    std::string _tooltip;
    std::string _plugin;
    std::string _formatVersion;
    std::list<BitmapModelPtr> _bitmaps;
    std::list<ResourceModelPtr> _resources;
    std::list<ParameterModelPtr> _parameters;
    std::map<int, InputModelPtr> _inputs;
    std::map<int, OutputModelPtr> _outputs;
    ContentsModelPtr _contents;
  };
}

#endif/* !BOXINTERFACE_P_H_ */
