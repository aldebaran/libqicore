/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef DIAGRAM_H_
# define DIAGRAM_H_

# include <qicore/api.hpp>
# include <qicore/box.hpp>

namespace qi
{

class DiagramPrivate;

class QICORE_API Diagram
{
  public:
    Diagram();
    ~Diagram();

    void setName(std::string name);
    std::string getName() const;

    void addBox(Box* b);
    void removeBox(Box* b);

    DiagramPrivate* _p;
};

};

#endif /* !DIAGRAM_H_ */
