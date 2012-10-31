/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef DIAGRAM_H_
# define DIAGRAM_H_

# include <qicore/api.h>
# include <qicore/box.h>

class DiagramPrivate;

class QICORE_API Diagram
{
  friend class DiagramPrivate;

  public:
    Diagram();
    ~Diagram();

    void setName(std::string name);
    std::string getName();

    void addBox(Box* b);
    void removeBox(Box* b);

    void loadAll();
    void unloadAll();

    void loadFromDiagram(Diagram* d);

  private:

    DiagramPrivate* _p;
};

#endif /* !DIAGRAM_H_ */
