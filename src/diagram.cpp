/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qi/log.hpp>

#include <qicore/diagram.hpp>
#include "diagram_private.hpp"

DiagramPrivate::DiagramPrivate()
  : _name ("Unnamed-Diagram"),
    _boxes()
{
}

DiagramPrivate::~DiagramPrivate()
{
}

void DiagramPrivate::addBox(Box *b)
{
  _boxes.insert(b);
}

void DiagramPrivate::removeBox(Box *b)
{
  _boxes.erase(b);
}

void DiagramPrivate::loadAll()
{
  for (std::set<Box*>::iterator it = _boxes.begin();
        it != _boxes.end(); it++)
  {
    loadBox(*it);
  }
}

void DiagramPrivate::unloadAll()
{
  for (std::set<Box*>::iterator it = _boxes.begin();
        it != _boxes.end(); it++)
  {
    unloadBox(*it);
  }
}

void DiagramPrivate::loadFromDiagram(Diagram* d)
{
  std::set<Box*>& l = d->_p->_boxes;
  std::set<Box*> toLoad = _boxes;

  for (std::set<Box*>::iterator it = l.begin();
        it != l.end(); it++)
  {
    std::set<Box*>::iterator loadIt = toLoad.find(*it);

    if (loadIt == toLoad.end())
      unloadBox(*it);
    else
    {
      qiLogDebug("qiCore.Diagram") << "Box is already loaded: " << (*it)->getName() << std::endl;
      toLoad.erase(loadIt);
    }
  }

  for (std::set<Box*>::iterator it = toLoad.begin();
        it != toLoad.end(); it++)
  {
    loadBox(*it);
  }
}

void DiagramPrivate::loadBox(Box* b)
{
  qiLogDebug("qiCore.Diagram") << "Load the box named: " << b->getName() << std::endl;
  b->load();
}

void DiagramPrivate::unloadBox(Box *b)
{
  qiLogDebug("qiCore.Diagram") << "Unload the box named: " << b->getName() << std::endl;
  b->unload();
}

/* Public */
Diagram::Diagram()
  : _p (new DiagramPrivate())
{
}

Diagram::~Diagram()
{
  delete _p;
}

void Diagram::addBox(Box *b)
{
  _p->addBox(b);
}

void Diagram::removeBox(Box *b)
{
  _p->removeBox(b);
}

void Diagram::loadAll()
{
  _p->loadAll();
}

void Diagram::unloadAll()
{
  _p->unloadAll();
}

void Diagram::loadFromDiagram(Diagram *d)
{
  _p->loadFromDiagram(d);
}

void Diagram::setName(std::string name)
{
  _p->_name = name;
}

std::string Diagram::getName()
{
  return _p->_name;
}
