/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef FLOWDIAGRAM_H_
#define FLOWDIAGRAM_H_

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include <qicore-compat/api.hpp>

namespace qi {
  class FlowDiagramModelPrivate;
  class BoxInstanceModel;
  class LinkModel;

  class QICORECOMPAT_API FlowDiagramModel
  {
  public:
    FlowDiagramModel(const std::string &path = "",
                     float scale = 1.0,
                     const std::string &formatVersion = "4",
                     const std::list<boost::shared_ptr<LinkModel> > &links = std::list<boost::shared_ptr<LinkModel> >(),
                     const std::list<boost::shared_ptr<BoxInstanceModel> > &boxsInstance = std::list<boost::shared_ptr<BoxInstanceModel> >());

    virtual ~FlowDiagramModel();

    std::string path() const;
    float scale() const;
    const std::string& formatVersion() const;
    const std::list<boost::shared_ptr<LinkModel> >& links() const;
    const std::list<boost::shared_ptr<BoxInstanceModel> >& boxsInstance() const;

    void setPath(const std::string& path);
    void setScale(float scale);
    void setFormatVersion(const std::string& formatVersion);
    void addLink(boost::shared_ptr<LinkModel> link);
    void addBoxInstance(boost::shared_ptr<BoxInstanceModel> boxInstance);

    bool loadFromFile();

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(FlowDiagramModel);
    FlowDiagramModelPrivate* _p;
  };
  typedef boost::shared_ptr<FlowDiagramModel> FlowDiagramModelPtr;
}

#endif /* !FLOWDIAGRAM_H_ */
