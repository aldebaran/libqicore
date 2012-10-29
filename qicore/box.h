/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOX_H_
# define BOX_H_

# include <albehavior/albehavior.h>

/* REMOVE ME PLEASE */
# include <qicore/newinaoqi.h>

class BoxPrivate;
class Timeline;
class StateMachine;

class Box
{
  public:
    Box(boost::shared_ptr<AL::ALBroker> broker, std::string name);
    ~Box();

    /* --- Behavior Compatibility --- */
    void waitResourcesCallback(const char *pCallback);
    // true if resource free
    bool isResourceFree(const std::vector<std::string>& pList);
    // we know the resources and wait it but not take it
    void waitResourceFree();
    // we know the resources and wait it or take it if possible
    void waitResources();
    void releaseResource();
    bool addInput(char *pInputName);
    bool addOutput(char *pOutputName, bool pIsBang );
    void addParameter(char * pParameterName, PyObject* pValue, bool pInheritFromParent);
    AL::ALValue getParameter(char * pParameterName);
    AL::ALValue getParametersList();
    void setParameter(char * pParameterName, PyObject* pValue);
    void connectInput(char* pInputName, char* pALMemoryValueName, bool enabled);
    void connectOutput(char* pInputName, char* pALMemoryValueName, bool enabled);
    void connectParameter(char* pInputName, char* pALMemoryValueName, bool enabled);
    void stimulateIO(char *pIOName, PyObject* pValue);
    // we have to redefine all basemodule methods because python seems to be stupid and is not able to call them otherwise.
    void BIND_PYTHON(char * module, char *method);
    std::string getName();
    std::string _methodMissing();

    /* --- Behavior inheritance compatibility --- */
    void functionName(char *method, char *module, char *description);
    void addParam(char *pParam);
    void _bindWithParam(char * module, char *method, int paramNumber );

    int getTimelineFps(const std::string& pId);
    void setTimelineFps(const std::string& pId, const int& pFPS);

    Timeline* getTimeline();
    void setTimeline(Timeline*);
    bool hasTimeline() const;

    StateMachine* getStateMachine();
    void setStateMachine(StateMachine*);
    bool hasStateMachine() const;

    void executePython(const std::string& pMethod, const AL::ALValue& pParams, AL::ALValue& pResult);

  private:
    BoxPrivate*           _p;
};

#endif /* !BOX_H_ */
