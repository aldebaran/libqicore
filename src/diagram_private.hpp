/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef DIAGRAM_PRIVATE_H_
# define DIAGRAM_PRIVATE_H_

# include <set>

# include <qicore/box.hpp>

namespace qi
{

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

  private:
    void loadBox(Box* b);
    void unloadBox(Box* b);

    std::string    _name;
    std::set<Box*> _boxes;
};

};

#endif /* !DIAGRAM_H_ */
