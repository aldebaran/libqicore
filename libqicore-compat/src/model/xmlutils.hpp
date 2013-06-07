/**
 * @author Emmanuel Pot
 * Copyright (c) Aldebaran Robotics 2008 All Rights Reserved
 */

#pragma once

#ifndef XML_UTILS_HH
#define XML_UTILS_HH

#include <string>
#include <alserial/alserial.h>
#include <qicore-compat/model/boxinstancemodel.hpp>

namespace qi
{

  class XmlUtils
  {
  public:
    template <typename T>
    static std::list<boost::shared_ptr<T> > constructObjects(AL::XmlElement::CList list)
    {
      if(list.empty())
        return std::list<boost::shared_ptr<T> > ();

      std::list<boost::shared_ptr<T> >  returnList;
      for (AL::XmlElement::CList::const_iterator it=list.begin(), itEnd=list.end(); it!=itEnd; ++it)
      {
        boost::shared_ptr<const AL::XmlElement> elt = *it;
        T *object = new T (elt);
        boost::shared_ptr<T> pobject(object);

        returnList.push_front(pobject);

      }
      return returnList;
    }

    template <typename T>
    static std::list<boost::shared_ptr<T> > constructObjects(AL::XmlElement::CList list, std::string dir)
    {
      if(list.empty())
        return std::list<boost::shared_ptr<T> > ();

      std::list<boost::shared_ptr<T> >  returnList;
      for (AL::XmlElement::CList::const_iterator it=list.begin(), itEnd=list.end(); it!=itEnd; ++it)
      {
        boost::shared_ptr<const AL::XmlElement> elt = *it;
        T *object = new T (elt, dir);
        boost::shared_ptr<T> pobject(object);

        returnList.push_front(pobject);

      }
      return returnList;
    }

    template <typename T>
    static std::list<boost::shared_ptr<T> > constructObjects(AL::XmlElement::CList list, std::string dir, BoxInstanceModelPtr parent)
    {
      if(list.empty())
        return std::list<boost::shared_ptr<T> > ();

      std::list<boost::shared_ptr<T> >  returnList;
      for (AL::XmlElement::CList::const_iterator it=list.begin(), itEnd=list.end(); it!=itEnd; ++it)
      {
        boost::shared_ptr<const AL::XmlElement> elt = *it;
        T *object = new T (elt, dir, parent);
        boost::shared_ptr<T> pobject(object);

        returnList.push_front(pobject);

      }
      return returnList;
    }

    template <typename T>
    static bool verifyObjects(std::list<boost::shared_ptr<T> > list)
    {
      bool isValid = true;
      for(typename std::list<boost::shared_ptr<T> >::const_iterator it = list.begin(), itEnd = list.end();
          it != itEnd; ++it)
      {
        boost::shared_ptr<T> object = *it;
        isValid = isValid && object->isValid();
      }

      return isValid;
    }


  };

}

#endif // !XML_UTILS_HH
