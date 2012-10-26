/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef DIAGRAM_PRIVATE_H_
# define DIAGRAM_PRIVATE_H_

# include <set>

# include <newframemanager/box.h>

class Diagram;

class DiagramPrivate
{
  friend class Diagram;

  public:
    DiagramPrivate();
    ~DiagramPrivate();

    void addBox(Box* b);
    void removeBox(Box* b);

    void loadAll();
    void unloadAll();

    void loadFromDiagram(Diagram *d);
    void merge(Diagram* d);

  private:
    void loadBox(Box* b);
    void unloadBox(Box* b);

    std::set<Box*> _boxes;
};

#endif /* !DIAGRAM_H_ */
