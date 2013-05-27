/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/contentsmodel.hpp>
#include "xmlutils.hpp"
#include "contentsmodel_p.hpp"
#include <qi/log.hpp>

namespace qi
{
  ContentsModelPrivate::ContentsModelPrivate() :
    _contents()
  {
  }

  ContentsModelPrivate::ContentsModelPrivate(std::list<ContentModelPtr> contents) :
    _contents(contents)
  {
  }

  ContentsModelPrivate::ContentsModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir)
  {
    AL::XmlElement::CList contents = elt->children("Content", "");

    AL::XmlElement::CList::const_iterator it = contents.begin(), itEnd = contents.end();
    for(; it != itEnd; ++it)
    {
      _contents.push_front(ContentModelPtr(new ContentModel(*it, dir)));
    }
  }

  ContentsModel::ContentsModel(const std::list<ContentModelPtr> &contents) :
    _p(new ContentsModelPrivate(contents))
  {
  }

  ContentsModel::ContentsModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir) :
    _p(new ContentsModelPrivate(elt, dir))
  {
  }

  ContentsModel::~ContentsModel()
  {
    delete _p;
  }

  const std::list<ContentModelPtr>& ContentsModel::contents() const
  {
    return _p->_contents;
  }

  void ContentsModel::addContent(ContentModelPtr content)
  {
    _p->_contents.push_front(content);
  }

  std::list<ContentModelPtr> ContentsModel::findContents(int type) const
  {
    std::list<ContentModelPtr> result;
    std::list<ContentModelPtr>::const_iterator it = _p->_contents.begin();
    std::list<ContentModelPtr>::const_iterator itEnd = _p->_contents.end();

    for(; it != itEnd; ++it)
    {
      if((*it)->type() == type)
        result.push_front(*it);
    }

    return result;
  }
}
