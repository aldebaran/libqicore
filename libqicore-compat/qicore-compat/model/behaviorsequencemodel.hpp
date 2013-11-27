/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BEHAVIORSEQUENCE_H_
# define BEHAVIORSEQUENCE_H_

# include <string>
# include <boost/shared_ptr.hpp>
# include <list>

# include <qicore-compat/api.hpp>

namespace qi {
  class BehaviorSequenceModelPrivate;
  class BehaviorLayerModel;
  class BoxInstanceModel;

  class QICORECOMPAT_API BehaviorSequenceModel : private boost::noncopyable {
  public:
    BehaviorSequenceModel(const std::string &path = "",
                          boost::shared_ptr<BoxInstanceModel> parent = boost::shared_ptr<BoxInstanceModel>(),
                          int fps = 25,
                          int startFrame = 0,
                          int endFrame = -1,
                          int size = 0,
                          const std::string &formatVersion = "4");
    virtual ~BehaviorSequenceModel();

    std::string path() const;
    int fps() const;
    int startFrame() const;
    int endFrame() const;
    int size() const;
    const std::string& formatVersion() const;
    const std::list<boost::shared_ptr<BehaviorLayerModel> >& behaviorsLayer() const;

    void setPath(const std::string& path);
    void setFPS(int fps);
    void setStartFrame(int startFrame);
    void setEndFrame(int endFrame);
    void setSize(int size);
    void setFormatVersion(const std::string& formatVersion);
    void addBehaviorLayer(boost::shared_ptr<BehaviorLayerModel> behaviorLayer);

    bool loadFromFile();

  private:
    BehaviorSequenceModelPrivate* _p;
  };
  typedef boost::shared_ptr<BehaviorSequenceModel> BehaviorSequenceModelPtr;
}

#endif /* !BEHAVIORSEQUENCE_H_ */
