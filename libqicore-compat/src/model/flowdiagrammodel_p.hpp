/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef FLOWDIAGRAM_P_H_
#define FLOWDIAGRAM_P_H_

# include <qicore-compat/model/linkmodel.hpp>
# include <qicore-compat/model/boxinstancemodel.hpp>

namespace qi {
 class FlowDiagramModelPrivate
 {
   friend class FlowDiagramModel;
 public:
   FlowDiagramModelPrivate(const std::string &path,
                           boost::shared_ptr<qi::BoxInstanceModel> parent,
                           float scale,
                           const std::string &formatVersion,
                           const std::list<LinkModelPtr> &links,
                           const std::map<int, BoxInstanceModelPtr> &boxsInstance);

   bool loadFromFile();

 private:
   std::string _path;
   float _scale;
   std::string _formatVersion;
   std::list<LinkModelPtr> _links;
   std::map<int, BoxInstanceModelPtr> _boxsInstance;
   BoxInstanceModelPtr _parent;
 };
}

#endif /* !FLOWDIAGRAM_P_H_ */
