/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/linkmodel.hpp>
#include "linkmodel_p.hpp"

namespace qi {

  LinkModelPrivate::LinkModelPrivate(int inputtowner, int indexofinput, int outputtowner, int indexofoutput) :
    _inputTowner(inputtowner),
    _indexOfInput(indexofinput),
    _outputTowner(outputtowner),
    _indexOfOutput(indexofoutput)
  {
  }

  LinkModelPrivate::LinkModelPrivate() :
    _inputTowner(),
    _indexOfInput(),
    _outputTowner(),
    _indexOfOutput()
  {
  }

  LinkModelPrivate::LinkModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    elt->getAttribute("inputowner",    _inputTowner);
    elt->getAttribute("indexofinput",  _indexOfInput);
    elt->getAttribute("outputowner",   _outputTowner);
    elt->getAttribute("indexofoutput", _indexOfOutput);
  }

  LinkModel::LinkModel(int inputtowner, int indexofinput, int outputtowner, int indexofoutput) :
    _p(new LinkModelPrivate(inputtowner, indexofinput, outputtowner, indexofoutput))
  {
  }

  LinkModel::LinkModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new LinkModelPrivate(elt))
  {
  }

  LinkModel::~LinkModel()
  {
    delete _p;
  }

  int LinkModel::inputTowner() const
  {
    return _p->_inputTowner;
  }

  int LinkModel::indexOfInput() const
  {
    return _p->_indexOfInput;
  }

  int LinkModel::outputTowner() const
  {
    return _p->_outputTowner;
  }

  int LinkModel::indexOfOutput() const
  {
    return _p->_indexOfOutput;
  }

  void LinkModel::setInputTowner(int input)
  {
    _p->_inputTowner = input;
  }

  void LinkModel::setIndexOfInput(int index)
  {
    _p->_indexOfInput = index;
  }

  void LinkModel::setOutputTowner(int output)
  {
    _p->_outputTowner = output;
  }

  void LinkModel::setIndexOfOutput(int index)
  {
    _p->_indexOfOutput = index;
  }
}
