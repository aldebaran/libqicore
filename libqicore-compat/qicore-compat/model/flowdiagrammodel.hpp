/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef FLOWDIAGRAM_H_
#define FLOWDIAGRAM_H_

#include <string>
#include <list>
#include <map>
#include <boost/shared_ptr.hpp>

#include <qicore-compat/api.hpp>

namespace qi {
  class FlowDiagramModelPrivate;
  class BoxInstanceModel;
  class LinkModel;


  class QICORECOMPAT_API FlowDiagramModel : private boost::noncopyable
  {
  public:
    typedef std::map<int, boost::shared_ptr<BoxInstanceModel> > BoxInstanceModelMap;

    FlowDiagramModel(const std::string &path = "",
                     boost::shared_ptr<BoxInstanceModel> parent = boost::shared_ptr<BoxInstanceModel>(),
                     float scale = 1.0,
                     const std::string &formatVersion = "4",
                     const std::list<boost::shared_ptr<LinkModel> > &links = std::list<boost::shared_ptr<LinkModel> >(),
                     const BoxInstanceModelMap &boxsInstance = BoxInstanceModelMap());

    virtual ~FlowDiagramModel();

    std::string path() const;
    float scale() const;
    const std::string& formatVersion() const;
    const std::list<boost::shared_ptr<LinkModel> >& links() const;
    const std::map<int, boost::shared_ptr<BoxInstanceModel> >& boxsInstance() const;

    boost::shared_ptr<BoxInstanceModel> findInstance(int id) const;

    void setPath(const std::string& path);
    void setScale(float scale);
    void setFormatVersion(const std::string& formatVersion);
    void addLink(boost::shared_ptr<LinkModel> link);
    void addBoxInstance(boost::shared_ptr<BoxInstanceModel> boxInstance);

    bool loadFromFile();

  private:
    FlowDiagramModelPrivate* _p;
  };
  typedef boost::shared_ptr<FlowDiagramModel> FlowDiagramModelPtr;
}

#endif /* !FLOWDIAGRAM_H_ */
