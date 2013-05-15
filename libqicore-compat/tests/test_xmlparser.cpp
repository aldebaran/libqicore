/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <gtest/gtest.h>

#include <qi/application.hpp>
#include <qi/log.hpp>

#include <qicore-compat/model/choregrapheprojectmodel.hpp>

#include <qicore-compat/model/boxinterfacemodel.hpp>
#include <qicore-compat/model/resourcemodel.hpp>
#include <qicore-compat/model/inputmodel.hpp>
#include <qicore-compat/model/outputmodel.hpp>
#include <qicore-compat/model/bitmapmodel.hpp>
#include <qicore-compat/model/choicemodel.hpp>
#include <qicore-compat/model/contentsmodel.hpp>
#include <qicore-compat/model/contentmodel.hpp>

#include <qicore-compat/model/animationmodel.hpp>
#include <qicore-compat/model/actuatorlistmodel.hpp>
#include <qicore-compat/model/actuatorcurvemodel.hpp>
#include <qicore-compat/model/keymodel.hpp>
#include <qicore-compat/model/tangentmodel.hpp>

#include <qicore-compat/model/behaviorsequencemodel.hpp>
#include <qicore-compat/model/behaviorlayermodel.hpp>
#include <qicore-compat/model/behaviorkeyframemodel.hpp>

#include <qicore-compat/model/flowdiagrammodel.hpp>
#include <qicore-compat/model/linkmodel.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/parametervaluemodel.hpp>

std::string valid_boxinterface;

TEST(XmlParser, LoadXARFile)
{
  qi::ChoregrapheProjectModel xar(valid_boxinterface);
  //Parse XAR File
  ASSERT_TRUE(xar.loadFromFile());

  EXPECT_EQ(xar.getName(),          "behavior/xar/tests");
  EXPECT_EQ(xar.getFormatVersion(), "4");

  qi::BoxInstanceModelPtr rootBox = xar.getRootBox();
  ASSERT_TRUE(rootBox);

  EXPECT_EQ(rootBox->getName(), "root");
  EXPECT_EQ(rootBox->getId(),   -1);
  EXPECT_EQ(rootBox->getX(),     0);
  EXPECT_EQ(rootBox->getY(),     0);
  EXPECT_EQ(rootBox->getPath(), "test_boxinterface.xml");
  EXPECT_EQ(rootBox->getParametersValue().size(), 0);

  qi::BoxInterfaceModelPtr rootBoxInterface = rootBox->getInterface();
  ASSERT_TRUE(rootBoxInterface);
  EXPECT_EQ(rootBoxInterface->getPath(), rootBox->getPath());
  EXPECT_EQ(rootBoxInterface->getUuid(), "");
  EXPECT_EQ(rootBoxInterface->getBoxVersion(), "1.0.0.0");
  EXPECT_EQ(rootBoxInterface->getName(), "root");
  EXPECT_EQ(rootBoxInterface->getTooltip(), "Root box of Choregraphe's project. Highest level possible.");
  EXPECT_EQ(rootBoxInterface->getPlugin(), "None");
  EXPECT_EQ(rootBoxInterface->getFormatVersion(), "4");

  std::list<qi::BitmapModelPtr> bitmaps = rootBoxInterface->getBitmaps();
  ASSERT_EQ(bitmaps.size(), 1);

  qi::BitmapModelPtr bitmap = bitmaps.front();
  ASSERT_TRUE(bitmap);
  EXPECT_EQ(bitmap->getPath(), "media/images/box/root.png");

  std::list<qi::ResourceModelPtr> resources = rootBoxInterface->getResources();
  ASSERT_EQ(resources.size(), 2);

  qi::ResourceModelPtr resource = resources.front();
  ASSERT_TRUE(resource);
  EXPECT_EQ(resource->getName(), "Head2");
  EXPECT_EQ(resource->getLockTypeString(), "Lock");
  EXPECT_EQ(resource->getLockType(), qi::ResourceModel::LockType_Lock);
  EXPECT_EQ(resource->getTimeout(), 0);

  std::list<qi::ParameterModelPtr> parameters = rootBoxInterface->getParameters();
  ASSERT_EQ(parameters.size(), 2);

  qi::ParameterModelPtr parameter = parameters.front();
  ASSERT_TRUE(parameter);
  EXPECT_EQ(parameter->getName(), "Begin position (s)");
  EXPECT_EQ(parameter->getInheritsFromParent(), false);
  EXPECT_EQ(parameter->getContentType(), qi::ParameterModel::ContentType_Double);
  EXPECT_EQ(parameter->getDefaultValueDouble(), 0);
  EXPECT_EQ(parameter->getMinDouble(), 0);
  EXPECT_EQ(parameter->getMaxDouble(), 600);
  EXPECT_EQ(parameter->getCustomChoice(), false);
  EXPECT_EQ(parameter->getPassword(), false);
  EXPECT_EQ(parameter->getTooltip(),  "Position in seconds where the playing must start.");
  EXPECT_EQ(parameter->getId(), 6);

  std::list<qi::InputModelPtr> inputs = rootBoxInterface->getInputs();
  ASSERT_EQ(inputs.size(), 3);

  qi::InputModelPtr input = inputs.front();
  ASSERT_TRUE(input);
  EXPECT_EQ(input->getName(), "onStop");
  EXPECT_EQ(input->getType(), qi::InputModel::InputType_Bang);
  EXPECT_EQ(input->getTypeSize(), 1);
  EXPECT_EQ(input->getNature(), qi::InputModel::InputNature_OnStop);
  EXPECT_EQ(input->getSTMValueName(), "");
  EXPECT_EQ(input->getInner(), false);
  EXPECT_EQ(input->getTooltip(), "Box behavior stops when a signal is received on this input.");
  EXPECT_EQ(input->getId(), 3);


  std::list<qi::OutputModelPtr> outputs = rootBoxInterface->getOutputs();
  ASSERT_EQ(outputs.size(), 1);

  qi::OutputModelPtr output = outputs.front();
  ASSERT_TRUE(output);
  EXPECT_EQ(output->getName(), "onStopped");
  EXPECT_EQ(output->getType(), qi::OutputModel::OutputType_Bang);
  EXPECT_EQ(output->getTypeSize(), 1);
  EXPECT_EQ(output->getNature(), qi::OutputModel::OutputNature_Stopped);
  EXPECT_EQ(output->getInner(), false);
  EXPECT_EQ(output->getTooltip(), "Signal sent when box behavior is finished.");
  EXPECT_EQ(output->getId(), 4);

  qi::ContentsModelPtr contents = rootBoxInterface->getContents();
  ASSERT_TRUE(contents);
  qi::ContentModelPtr content = contents->getContents().front();
  ASSERT_TRUE(content);
  EXPECT_EQ(content->getChecksum(), "");
  EXPECT_EQ(content->getPath(), "test_bhs.bhs");
  EXPECT_EQ(content->getType(), qi::ContentModel::ContentType_BehaviorSequence);

  EXPECT_EQ(contents->getContents().size(), 3);
  ASSERT_EQ(contents->findContents(qi::ContentModel::ContentType_Animation).size(), 1);
  qi::ContentModelPtr contentAnim = contents->findContents(qi::ContentModel::ContentType_Animation).front();
  qi::AnimationModelPtr anim = contentAnim->getAnimationModel();
  ASSERT_TRUE(anim);
  EXPECT_EQ(anim->getPath(), contentAnim->getPath());
  EXPECT_EQ(anim->getFPS(), 25);
  EXPECT_EQ(anim->getStartFrame(), 0);
  EXPECT_EQ(anim->getEndFrame(), -1);
  EXPECT_EQ(anim->getSize(), 300);
  EXPECT_EQ(anim->getFormatVersion(), "4");

  qi::ActuatorListModelPtr actuatorList = anim->getActuatorList();
  ASSERT_TRUE(actuatorList);
  EXPECT_EQ(actuatorList->getModel(), "None");

  std::list<qi::ActuatorCurveModelPtr> actuatorsCurve = actuatorList->getActuatorsCurve();
  ASSERT_EQ(actuatorsCurve.size(), 25);
  qi::ActuatorCurveModelPtr actuatorCurve = actuatorsCurve.front();
  ASSERT_TRUE(actuatorCurve);
  EXPECT_EQ(actuatorCurve->getName(), "value");
  EXPECT_EQ(actuatorCurve->getActuator(), "RAnkleRoll");
  EXPECT_EQ(actuatorCurve->getRecordable(), true);
  EXPECT_EQ(actuatorCurve->getMute(), false);
  EXPECT_EQ(actuatorCurve->getUnit(), qi::ActuatorCurveModel::UnitType_Undefined);

  std::list<qi::KeyModelPtr> keys = actuatorCurve->getKeys();
  ASSERT_EQ(keys.size(), 2);
  qi::KeyModelPtr key = keys.front();
  ASSERT_TRUE(key);
  EXPECT_EQ(key->getFrame(), 23);
  EXPECT_EQ(key->getValue(), -1.4f);
  EXPECT_EQ(key->getSmooth(), false);
  EXPECT_EQ(key->getSymmetrical(), false);

  std::list<qi::TangentModelPtr> tangents = key->getTangents();
  ASSERT_EQ(tangents.size(), 2);
  qi::TangentModelPtr tangent = tangents.front();
  ASSERT_TRUE(tangent);
  EXPECT_EQ(tangent->getSide(), qi::TangentModel::Side_Right);
  EXPECT_EQ(tangent->getInterpType(), qi::TangentModel::InterpolationType_Bezier);
  EXPECT_EQ(tangent->getOrdinateParam(), 7.0f);
  EXPECT_EQ(tangent->getAbscissaParam(), 4.0f);

  ASSERT_EQ(contents->findContents(qi::ContentModel::ContentType_FlowDiagram).size(), 1);
  qi::ContentModelPtr contentFlowDiagram = contents->findContents(qi::ContentModel::ContentType_FlowDiagram).front();
  qi::FlowDiagramModelPtr flowdiagram = contentFlowDiagram->getFlowDiagramModel();
  ASSERT_TRUE(flowdiagram);
  EXPECT_EQ(flowdiagram->getPath(), contentFlowDiagram->getPath());
  EXPECT_EQ(flowdiagram->getScale(), 59.4604f);
  EXPECT_EQ(flowdiagram->getFormatVersion(), "4");

  std::list<qi::LinkModelPtr> links = flowdiagram->getLinks();
  ASSERT_EQ(links.size(), 38);
  qi::LinkModelPtr link = links.front();
  ASSERT_TRUE(link);
  EXPECT_EQ(link->getInputTowner(), 0);
  EXPECT_EQ(link->getIndexOfInput(), 4);
  EXPECT_EQ(link->getOutputTowner(), 26);
  EXPECT_EQ(link->getIndexOfOutput(), 4);

  std::list<qi::BoxInstanceModelPtr> boxs = flowdiagram->getBoxsInstance();
  EXPECT_EQ(boxs.size(), 2);

  ASSERT_EQ(contents->findContents(qi::ContentModel::ContentType_BehaviorSequence).size(), 1);
  qi::ContentModelPtr contentBehaviorSequence = contents->findContents(qi::ContentModel::ContentType_BehaviorSequence).front();
  qi::BehaviorSequenceModelPtr behaviorSequence = contentBehaviorSequence->getBehaviorSequenceModel();
  ASSERT_TRUE(behaviorSequence);
  EXPECT_EQ(behaviorSequence->getPath(), contentBehaviorSequence->getPath());
  EXPECT_EQ(behaviorSequence->getFPS(), 25);
  EXPECT_EQ(behaviorSequence->getStartFrame(), 0);
  EXPECT_EQ(behaviorSequence->getEndFrame(), -1);
  EXPECT_EQ(behaviorSequence->getSize(), 300);
  EXPECT_EQ(behaviorSequence->getFormatVersion(), "4");

  std::list<qi::BehaviorLayerModelPtr> layers = behaviorSequence->getBehaviorsLayer();
  ASSERT_EQ(layers.size(), 2);
  qi::BehaviorLayerModelPtr layer = layers.front();
  ASSERT_TRUE(layer);
  EXPECT_EQ(layer->getName(), "behavior_layer2");
  EXPECT_EQ(layer->getMute(), false);

  std::list<qi::BehaviorKeyFrameModelPtr> keysFrame = layer->getBehaviorsKeyFrame();
  ASSERT_EQ(keysFrame.size(), 1);
  qi::BehaviorKeyFrameModelPtr keyFrame = keysFrame.front();
  ASSERT_TRUE(keyFrame);
  EXPECT_EQ(keyFrame->getName(), "Say : Pssst");
  EXPECT_EQ(keyFrame->getIndex(), 76);
  EXPECT_EQ(keyFrame->getBitmap(), "");
  EXPECT_EQ(keyFrame->getPath(), "B2_1_Say_Pssst0.fld");
  ASSERT_TRUE(keyFrame->getDiagram());
}


TEST(XmlParser, Bitmap)
{
  qi::Bitmap bitmap("box.png");

  ASSERT_EQ(bitmap.getPath(), "box.png");
  bitmap.setPath("b.png");
  ASSERT_EQ(bitmap.getPath(), "b.png");
}

TEST(XmlParser, Ressource)
{
  qi::ResourceModel resource("test", qi::ResourceModel::LockType_Lock, 0);

  ASSERT_EQ(resource.getName(), "test");
  resource.setName("test2");
  ASSERT_EQ(resource.getName(), "test2");

  ASSERT_EQ(resource.getLockType(), qi::ResourceModel::LockType_Lock);
  ASSERT_EQ(resource.getLockTypeString(), "Lock");

  resource.setLockType(qi::ResourceModel::LockType_StopOnDemand);
  ASSERT_EQ(resource.getLockType(), qi::ResourceModel::LockType_StopOnDemand);
  ASSERT_EQ(resource.getLockTypeString(), "Stop on demand");

  resource.setLockType(qi::ResourceModel::LockType_CallbackOnDemand);
  ASSERT_EQ(resource.getLockType(), qi::ResourceModel::LockType_CallbackOnDemand);
  ASSERT_EQ(resource.getLockTypeString(), "Callback on demand");

  resource.setLockType(qi::ResourceModel::LockType_PauseOnDemand);
  ASSERT_EQ(resource.getLockType(), qi::ResourceModel::LockType_PauseOnDemand);
  ASSERT_EQ(resource.getLockTypeString(), "Pause on demand");

  resource.setLockType(qi::ResourceModel::LockType_Error);
  ASSERT_EQ(resource.getLockType(), qi::ResourceModel::LockType_Error);
  ASSERT_EQ(resource.getLockTypeString(), "");

  ASSERT_EQ(resource.getTimeout(), 0);
  resource.setTimeout(42);
  ASSERT_EQ(resource.getTimeout(), 42);
}

TEST(XmlParser, Parameter)
{

  qi::ParameterModel parameter;
  parameter.setDefaultValueDouble(0.0);
  parameter.setMinDouble(0.0);
  parameter.setMaxDouble(600.0);
  parameter.setName("Test");
  parameter.setInheritsFromParent(false);
  parameter.setCustomChoice(false);
  parameter.setPassword(false);
  parameter.setTooltip("tooltip");
  parameter.setId(1);

  ASSERT_EQ(parameter.getChoices().size(), 0);

  ASSERT_TRUE(parameter.addChoice(qi::ChoiceModelPtr(new qi::ChoiceModel(1.0))));
  ASSERT_FALSE(parameter.addChoice(qi::ChoiceModelPtr(new qi::ChoiceModel(false))));

  ASSERT_EQ(parameter.getChoices().size(), 1);

  qi::ChoiceModelPtr choice = parameter.getChoices().front();
  ASSERT_EQ(choice->getValueDouble(), 1.0);

  //Test double value
  ASSERT_EQ(parameter.getContentType(), qi::ParameterModel::ContentType_Double);
  ASSERT_EQ(parameter.getDefaultValueDouble(), 0.0);
  ASSERT_EQ(parameter.getMinDouble(), 0.0);
  ASSERT_EQ(parameter.getMaxDouble(), 600.0);

  ASSERT_FALSE(parameter.setMinDouble(1.0));
  ASSERT_EQ(parameter.getMinDouble(), 0.0);

  ASSERT_FALSE(parameter.setMaxDouble(-100.0));
  ASSERT_EQ(parameter.getMaxDouble(), 600.0);

  ASSERT_FALSE(parameter.setDefaultValueDouble(-0.4));
  ASSERT_EQ(parameter.getDefaultValueDouble(), 0.0);
  ASSERT_TRUE( parameter.setDefaultValueDouble(45.0));
  ASSERT_EQ(   parameter.getDefaultValueDouble(), 45.0);


  //Test integer value
  ASSERT_TRUE(parameter.setDefaultValueInt(42));
  ASSERT_EQ(parameter.getContentType(), qi::ParameterModel::ContentType_Int);
  ASSERT_EQ(  parameter.getDefaultValueInt(), 42);
  ASSERT_EQ(  parameter.getMinInt(),          42);
  ASSERT_EQ(  parameter.getMaxInt(),          42);

  //Test string value
  parameter.setDefaultValueString("Foo");
  ASSERT_EQ(parameter.getContentType(), qi::ParameterModel::ContentType_String);
  ASSERT_EQ(parameter.getDefaultValueString(), "Foo");

  //Test bool value
  parameter.setDefaultValueBool(true);
  ASSERT_EQ(parameter.getContentType(), qi::ParameterModel::ContentType_Bool);
  ASSERT_TRUE(parameter.getDefaultValueBool());

  //Test other attribute
  ASSERT_EQ(parameter.getName(),"Test");
  ASSERT_FALSE(parameter.getInheritsFromParent());
  ASSERT_FALSE(parameter.getCustomChoice());
  ASSERT_FALSE(parameter.getPassword());
  ASSERT_EQ(parameter.getTooltip(),"tooltip");
  ASSERT_EQ(parameter.getId(), 1);
  parameter.setName("Test1");
  parameter.setInheritsFromParent(true);
  parameter.setCustomChoice(true);
  parameter.setPassword(true);
  parameter.setTooltip("tooltip1");
  parameter.setId(2);
  ASSERT_EQ(parameter.getName(),"Test1");
  ASSERT_TRUE(parameter.getInheritsFromParent());
  ASSERT_TRUE(parameter.getCustomChoice());
  ASSERT_TRUE(parameter.getPassword());
  ASSERT_EQ(parameter.getTooltip(),"tooltip1");
  ASSERT_EQ(parameter.getId(), 2);
}


TEST(XmlParser, Choice)
{
  qi::ChoiceModel boolChoice(true);
  qi::ChoiceModel intChoice(42);
  qi::ChoiceModel doubleChoice(42.42);
  qi::ChoiceModel stringChoice(std::string("str"));

  EXPECT_EQ(boolChoice.getType(), qi::ParameterModel::ContentType_Bool);
  EXPECT_TRUE(boolChoice.getValueBool());

  EXPECT_EQ(intChoice.getType(), qi::ParameterModel::ContentType_Int);
  EXPECT_EQ(intChoice.getValueInt(), 42);

  EXPECT_EQ(doubleChoice.getType(), qi::ParameterModel::ContentType_Double);
  EXPECT_EQ(doubleChoice.getValueDouble(), 42.42);

  EXPECT_EQ(stringChoice.getType(), qi::ParameterModel::ContentType_String);
  EXPECT_EQ(stringChoice.getValueString(), "str");

  boolChoice.setValueInt(42);
  EXPECT_EQ(boolChoice.getType(), qi::ParameterModel::ContentType_Int);
  EXPECT_EQ(boolChoice.getValueDouble(), 42);

  boolChoice.setValueDouble(42.42);
  EXPECT_EQ(boolChoice.getType(), qi::ParameterModel::ContentType_Double);
  EXPECT_EQ(boolChoice.getValueDouble(), 42.42);

  boolChoice.setValueString("42.42");
  EXPECT_EQ(boolChoice.getType(), qi::ParameterModel::ContentType_String);
  EXPECT_EQ(boolChoice.getValueString(), "42.42");

  boolChoice.setValueBool(false);
  EXPECT_EQ(boolChoice.getType(), qi::ParameterModel::ContentType_Bool);
  EXPECT_FALSE(boolChoice.getValueBool());
}

TEST(XmlParser, Input)
{
  qi::InputModel input("Foo",
                  qi::InputModel::InputType_Number,
                  1,
                  qi::InputModel::InputNature_OnLoad,
                  false,
                  "tooltip",
                  42);

  qi::InputModel inputstm("Foo2",
                     qi::InputModel::InputType_Bang,
                     1,
                     "stm_value_name",
                     false,
                     "tooltip",
                     42);

  EXPECT_EQ(input.getName(), "Foo");
  EXPECT_EQ(input.getType(), qi::InputModel::InputType_Number);
  EXPECT_EQ(input.getTypeSize(), 1);
  EXPECT_EQ(input.getNature(), qi::InputModel::InputNature_OnLoad);
  EXPECT_EQ(input.getSTMValueName(), "");
  EXPECT_EQ(input.getInner(), false);
  EXPECT_EQ(input.getTooltip(), "tooltip");
  EXPECT_EQ(input.getId(), 42);

  EXPECT_EQ(inputstm.getName(), "Foo2");
  EXPECT_EQ(inputstm.getType(), qi::InputModel::InputType_Bang);
  EXPECT_EQ(inputstm.getTypeSize(), 1);
  EXPECT_EQ(inputstm.getNature(), qi::InputModel::InputNature_STMValue);
  EXPECT_EQ(inputstm.getSTMValueName(), "stm_value_name");
  EXPECT_EQ(inputstm.getInner(), false);
  EXPECT_EQ(inputstm.getTooltip(), "tooltip");
  EXPECT_EQ(inputstm.getId(), 42);

  input.setName("Fa");
  EXPECT_EQ(input.getName(), "Fa");

  input.setType(qi::InputModel::InputType_Dynamic);
  EXPECT_EQ(input.getType(), qi::InputModel::InputType_Dynamic);

  input.setTypeSize(2);
  EXPECT_EQ(input.getTypeSize(), 2);

  input.setNature(qi::InputModel::InputNature_Event);
  EXPECT_EQ(input.getNature(), qi::InputModel::InputNature_Event);

  input.setSTMValueName("grep");
  EXPECT_EQ(input.getNature(), qi::InputModel::InputNature_STMValue);
  EXPECT_EQ(input.getSTMValueName(), "grep");

  input.setInner(true);
  EXPECT_EQ(input.getInner(), true);

  input.setTooltip("Tooltip2");
  EXPECT_EQ(input.getTooltip(), "Tooltip2");

  input.setId(314159);
  EXPECT_EQ(input.getId(), 314159);
}

TEST(XmlParser, Output)
{
  qi::OutputModel output("Foo",
                  qi::OutputModel::OutputType_Number,
                  1,
                  qi::OutputModel::OutputNature_Punctual,
                  false,
                  "tooltip",
                  42);

  EXPECT_EQ(output.getName(), "Foo");
  EXPECT_EQ(output.getType(), qi::OutputModel::OutputType_Number);
  EXPECT_EQ(output.getTypeSize(), 1);
  EXPECT_EQ(output.getNature(), qi::OutputModel::OutputNature_Punctual);
  EXPECT_EQ(output.getInner(), false);
  EXPECT_EQ(output.getTooltip(), "tooltip");
  EXPECT_EQ(output.getId(), 42);

  output.setName("Fa");
  EXPECT_EQ(output.getName(), "Fa");

  output.setType(qi::OutputModel::OutputType_Dynamic);
  EXPECT_EQ(output.getType(), qi::OutputModel::OutputType_Dynamic);

  output.setTypeSize(2);
  EXPECT_EQ(output.getTypeSize(), 2);

  output.setNature(qi::OutputModel::OutputNature_Stopped);
  EXPECT_EQ(output.getNature(), qi::OutputModel::OutputNature_Stopped);

  output.setInner(true);
  EXPECT_EQ(output.getInner(), true);

  output.setTooltip("Tooltip2");
  EXPECT_EQ(output.getTooltip(), "Tooltip2");

  output.setId(314159);
  EXPECT_EQ(output.getId(), 314159);
}

TEST(XmlParse, Content)
{
  qi::ContentModel content(qi::ContentModel::ContentType_PythonScript, "main.py", "");
  EXPECT_EQ(content.getType(), qi::ContentModel::ContentType_PythonScript);
  EXPECT_EQ(content.getPath(), "main.py");
  EXPECT_EQ(content.getChecksum(), "");

  content.setType(qi::ContentModel::ContentType_Animation);
  EXPECT_EQ(content.getType(), qi::ContentModel::ContentType_Animation);

  content.setPath("main.anim");
  EXPECT_EQ(content.getPath(), "main.anim");

  content.setChecksum("1234");
  EXPECT_EQ(content.getChecksum(), "1234");

}

TEST(XmlParse, BoxInterface)
{
  qi::BoxInterfaceModel interface("box.xml",
                             "1",
                             "1",
                             "box",
                             "tooltip",
                             "45");

  EXPECT_EQ(interface.getPath(), "box.xml");
  EXPECT_EQ(interface.getUuid(), "1");
  EXPECT_EQ(interface.getBoxVersion(), "1");
  EXPECT_EQ(interface.getName(), "box");
  EXPECT_EQ(interface.getTooltip(), "tooltip");
  EXPECT_EQ(interface.getFormatVersion(), "45");
  EXPECT_EQ(interface.getPlugin(), "None");
  EXPECT_EQ(interface.getBitmaps().size(), 0);
  EXPECT_EQ(interface.getResources().size(), 0);
  EXPECT_EQ(interface.getParameters().size(), 0);
  EXPECT_EQ(interface.getInputs().size(), 0);
  EXPECT_EQ(interface.getOutputs().size(), 0);
  EXPECT_FALSE(interface.getContents());

  interface.setPath("box2.xml");
  EXPECT_EQ(interface.getPath(), "box2.xml");

  interface.setUuid("2");
  EXPECT_EQ(interface.getUuid(), "2");

  interface.setBoxVersion("2");
  EXPECT_EQ(interface.getBoxVersion(), "2");

  interface.setName("box2");
  EXPECT_EQ(interface.getName(), "box2");

  interface.setTooltip("tooltip2");
  EXPECT_EQ(interface.getTooltip(), "tooltip2");

  interface.setFormatVersion("452");
  EXPECT_EQ(interface.getFormatVersion(), "452");

  interface.setPlugin("None2");
  EXPECT_EQ(interface.getPlugin(), "None2");

  interface.addBitmap(qi::BitmapModelPtr(new qi::Bitmap("toto")));
  EXPECT_EQ(interface.getBitmaps().size(), 1);

  interface.addResource(qi::ResourceModelPtr(new qi::ResourceModel("toto", qi::ResourceModel::LockType_CallbackOnDemand, 0)));
  EXPECT_EQ(interface.getResources().size(), 1);

  qi::ParameterModelPtr parameter(new qi::ParameterModel());
  parameter->setDefaultValueDouble(0.0);
  parameter->setMinDouble(0.0);
  parameter->setMaxDouble(600.0);
  parameter->setName("Test");
  parameter->setInheritsFromParent(false);
  parameter->setCustomChoice(false);
  parameter->setPassword(false);
  parameter->setTooltip("tooltip");
  parameter->setId(1);

  interface.addParameter(parameter);
  EXPECT_EQ(interface.getParameters().size(), 1);

  interface.addInput(qi::InputModelPtr(new qi::InputModel("Foo",
                                                   qi::InputModel::InputType_Number,
                                                   1,
                                                   qi::InputModel::InputNature_OnLoad,
                                                   false,
                                                   "tooltip",
                                                   42)));
  EXPECT_EQ(interface.getInputs().size(), 1);

  interface.addOutput(qi::OutputModelPtr(new qi::OutputModel("Foo",
                                           qi::OutputModel::OutputType_Number,
                                           1,
                                           qi::OutputModel::OutputNature_Punctual,
                                           false,
                                           "tooltip",
                                           42)));
  EXPECT_EQ(interface.getOutputs().size(), 1);

  interface.addContent(qi::ContentModelPtr(new qi::ContentModel(qi::ContentModel::ContentType_PythonScript, "main.py", "")));
  EXPECT_TRUE(interface.getContents());
  EXPECT_EQ(interface.getContents()->getContents().size(), 1);

}

TEST(XmlParse, Contents)
{
  qi::ContentsModel contents;

  EXPECT_EQ(contents.getContents().size(), 0);
  contents.addContent(qi::ContentModelPtr(new qi::ContentModel(qi::ContentModel::ContentType_PythonScript, "main.py", "")));
  EXPECT_EQ(contents.getContents().size(), 1);
}

TEST(XmlParse, Animation)
{

  qi::AnimationModel anim("anim.anim", 25, 0, -1, 0, "4");

  EXPECT_EQ(anim.getPath(), "anim.anim");
  EXPECT_EQ(anim.getFPS(), 25);
  EXPECT_EQ(anim.getStartFrame(), 0);
  EXPECT_EQ(anim.getEndFrame(), -1);
  EXPECT_EQ(anim.getSize(), 0);
  EXPECT_EQ(anim.getFormatVersion(), "4");
  EXPECT_FALSE(anim.getActuatorList());

  anim.setPath("anim.a");
  EXPECT_EQ(anim.getPath(), "anim.a");

  anim.setFPS(20);
  EXPECT_EQ(anim.getFPS(), 20);

  anim.setStartFrame(1);
  EXPECT_EQ(anim.getStartFrame(), 1);

  anim.setEndFrame(2);
  EXPECT_EQ(anim.getEndFrame(), 2);

  anim.setSize(24);
  EXPECT_EQ(anim.getSize(), 24);

  anim.setFormatVersion("6");
  EXPECT_EQ(anim.getFormatVersion(), "6");

  anim.setActuatorList(qi::ActuatorListModelPtr( new qi::ActuatorListModel("Blitz")));
  EXPECT_TRUE(anim.getActuatorList());
}

TEST(XmlParse, ActuatorList)
{
  qi::ActuatorListModel actuatorList("Loup");

  ASSERT_EQ(actuatorList.getModel(), "Loup");
  ASSERT_EQ(actuatorList.getActuatorsCurve().size(), 0);

  actuatorList.setModel("Vayne");
  ASSERT_EQ(actuatorList.getModel(), "Vayne");

  actuatorList.addActuatorCurve(qi::ActuatorCurveModelPtr(new qi::ActuatorCurveModel("Grab", "Grab", true, true, qi::ActuatorCurveModel::UnitType_Degree)));
  ASSERT_EQ(actuatorList.getActuatorsCurve().size(), 1);
}

TEST(XmlParser, ActuatorCurve)
{
  qi::ActuatorCurveModel curve("Grab", "Grab", true, false, qi::ActuatorCurveModel::UnitType_Degree);

  ASSERT_EQ(curve.getName(), "Grab");
  ASSERT_EQ(curve.getActuator(), "Grab");
  ASSERT_EQ(curve.getRecordable(), true);
  ASSERT_EQ(curve.getMute(), false);
  ASSERT_EQ(curve.getKeys().size(), 0);

  curve.setName("Grab2");
  ASSERT_EQ(curve.getName(), "Grab2");

  curve.setActuator("Grab1");
  ASSERT_EQ(curve.getActuator(), "Grab1");

  curve.setRecordable(false);
  ASSERT_EQ(curve.getRecordable(), false);

  curve.setMute(true);
  ASSERT_EQ(curve.getMute(), true);

  curve.addKey(qi::KeyModelPtr(new qi::KeyModel(1, 3.14159f, true, true)));
  ASSERT_EQ(curve.getKeys().size(), 1);

}

TEST(XmlParser, Key)
{
  qi::KeyModel key(1, 3.14159f, true, true);

  ASSERT_EQ(key.getFrame(), 1);
  ASSERT_EQ(key.getValue(), 3.14159f);
  ASSERT_EQ(key.getSmooth(), true);
  ASSERT_EQ(key.getSymmetrical(), true);
  ASSERT_EQ(key.getTangents().size(), 0);

  key.setFrame(2);
  ASSERT_EQ(key.getFrame(), 2);

  key.setValue(42.42f);
  ASSERT_EQ(key.getValue(), 42.42f);

  key.setSmooth(false);
  ASSERT_EQ(key.getSmooth(), false);

  key.setSymmetrical(false);
  ASSERT_EQ(key.getSymmetrical(), false);

  qi::TangentModelPtr t1(new qi::TangentModel(qi::TangentModel::Side_Left, qi::TangentModel::InterpolationType_Bezier, 3.0, 1.0));
  qi::TangentModelPtr t2(new qi::TangentModel(qi::TangentModel::Side_Right, qi::TangentModel::InterpolationType_Bezier, 3.0, 1.0));

  key.setTangents(t1, t2);
  ASSERT_EQ(key.getTangents().size(), 2);
}

TEST(XmlParser, Tangent)
{
  qi::TangentModel tangent(qi::TangentModel::Side_Right, qi::TangentModel::InterpolationType_Bezier, 3.0, 1.0);

  ASSERT_EQ(tangent.getSide(), qi::TangentModel::Side_Right);
  ASSERT_EQ(tangent.getInterpType(), qi::TangentModel::InterpolationType_Bezier);
  ASSERT_EQ(tangent.getAbscissaParam(), 3.0f);
  ASSERT_EQ(tangent.getOrdinateParam(), 1.0f);

  tangent.setSide(qi::TangentModel::Side_Left);
  ASSERT_EQ(tangent.getSide(), qi::TangentModel::Side_Left);

  tangent.setInterpType(qi::TangentModel::InterpolationType_Constant);
  ASSERT_EQ(tangent.getInterpType(), qi::TangentModel::InterpolationType_Constant);

  tangent.setAbscissaParam(2.0);
  ASSERT_EQ(tangent.getAbscissaParam(), 2.0f);

  tangent.setOrdinateParam(4.0);
  ASSERT_EQ(tangent.getOrdinateParam(), 4.0f);
}

TEST(XmlParser, BehaviorSequence)
{
  qi::BehaviorSequenceModel seq("bhs.bhs", 25, 0, 61, 3000, "42");

  ASSERT_EQ(seq.getPath(), "bhs.bhs");
  ASSERT_EQ(seq.getFPS(), 25);
  ASSERT_EQ(seq.getStartFrame(), 0);
  ASSERT_EQ(seq.getEndFrame(), 61);
  ASSERT_EQ(seq.getSize(), 3000);
  ASSERT_EQ(seq.getFormatVersion(), "42");
  ASSERT_EQ(seq.getBehaviorsLayer().size(), 0);

  seq.setPath("bhs2.bhs");
  ASSERT_EQ(seq.getPath(), "bhs2.bhs");

  seq.setFPS(27);
  ASSERT_EQ(seq.getFPS(), 27);

  seq.setStartFrame(1);
  ASSERT_EQ(seq.getStartFrame(), 1);

  seq.setEndFrame(60);
  ASSERT_EQ(seq.getEndFrame(), 60);

  seq.setSize(3001);
  ASSERT_EQ(seq.getSize(), 3001);

  seq.setFormatVersion("43");
  ASSERT_EQ(seq.getFormatVersion(), "43");

  seq.addBehaviorLayer(qi::BehaviorLayerModelPtr(new qi::BehaviorLayerModel("toto", true)));
  ASSERT_EQ(seq.getBehaviorsLayer().size(), 1);


}

TEST(XmlParser, BehaviorLayer)
{
  qi::BehaviorLayerModel layer("toto", true);

  ASSERT_EQ(layer.getName(), "toto");
  ASSERT_EQ(layer.getMute(), true);
  ASSERT_EQ(layer.getBehaviorsKeyFrame().size(), 0);

  layer.setName("aaa");
  ASSERT_EQ(layer.getName(), "aaa");

  layer.setMute(false);
  ASSERT_EQ(layer.getMute(), false);

  layer.addBehaviorKeyFrame(qi::BehaviorKeyFrameModelPtr( new qi::BehaviorKeyFrameModel("titi", 1, "png", "fld")));
  ASSERT_EQ(layer.getBehaviorsKeyFrame().size(), 1);
}

TEST(XmlParser, BehaviorKeyFrame)
{
  qi::BehaviorKeyFrameModel keyFrame("titi", 1, "png", "fld");

  ASSERT_EQ(keyFrame.getName(), "titi");
  ASSERT_EQ(keyFrame.getIndex(), 1);
  ASSERT_EQ(keyFrame.getBitmap(), "png");
  ASSERT_EQ(keyFrame.getPath(), "fld");

  keyFrame.setName("bb");
  ASSERT_EQ(keyFrame.getName(), "bb");

  keyFrame.setIndex(2);
  ASSERT_EQ(keyFrame.getIndex(), 2);

  keyFrame.setBitmap("jpg");
  ASSERT_EQ(keyFrame.getBitmap(), "jpg");

  keyFrame.setPath(".fld");
  ASSERT_EQ(keyFrame.getPath(), ".fld");
}

TEST(XmlParser, FlowDiagram)
{
  qi::FlowDiagramModel flowDiagram("d.fld", 12.0, "53");

  ASSERT_EQ(flowDiagram.getPath(), "d.fld");
  ASSERT_EQ(flowDiagram.getScale(), 12.0);
  ASSERT_EQ(flowDiagram.getFormatVersion(), "53");
  ASSERT_EQ(flowDiagram.getLinks().size(), 0);
  ASSERT_EQ(flowDiagram.getBoxsInstance().size(), 0);

  flowDiagram.setPath("c.fld");
  ASSERT_EQ(flowDiagram.getPath(), "c.fld");

  flowDiagram.setScale(1.0);
  ASSERT_EQ(flowDiagram.getScale(), 1.0);

  flowDiagram.setFormatVersion("4");
  ASSERT_EQ(flowDiagram.getFormatVersion(), "4");

  flowDiagram.addLink(qi::LinkModelPtr(new qi::LinkModel(1, 2, 3, 4)));
  ASSERT_EQ(flowDiagram.getLinks().size(), 1);

  qi::BoxInterfaceModelPtr interface(new qi::BoxInterfaceModel("box.xml", "1", "1","box", "tooltip", "45"));

  flowDiagram.addBoxInstance(qi::BoxInstanceModelPtr(new qi::BoxInstanceModel("root", 1, 1, 1, interface)));
  ASSERT_EQ(flowDiagram.getBoxsInstance().size(), 1);
}

TEST(XmlParser, Link)
{
  qi::LinkModel link(1, 2, 3, 4);

  ASSERT_EQ(link.getInputTowner(), 1);
  ASSERT_EQ(link.getIndexOfInput(), 2);
  ASSERT_EQ(link.getOutputTowner(), 3);
  ASSERT_EQ(link.getIndexOfOutput(), 4);

  link.setInputTowner(5);
  ASSERT_EQ(link.getInputTowner(), 5);

  link.setIndexOfInput(6);
  ASSERT_EQ(link.getIndexOfInput(), 6);

  link.setOutputTowner(7);
  ASSERT_EQ(link.getOutputTowner(), 7);

  link.setIndexOfOutput(8);
  ASSERT_EQ(link.getIndexOfOutput(), 8);
}

TEST(XmlParser, BoxInstance)
{
  qi::BoxInterfaceModelPtr interface(new qi::BoxInterfaceModel("box.xml", "1", "1","box", "tooltip", "45"));
  qi::ParameterModelPtr parameter(new qi::ParameterModel());
  parameter->setDefaultValueDouble(0.0);
  parameter->setMinDouble(0.0);
  parameter->setMaxDouble(600.0);
  parameter->setName("Test");
  parameter->setInheritsFromParent(false);
  parameter->setCustomChoice(false);
  parameter->setPassword(false);
  parameter->setTooltip("tooltip");
  parameter->setId(0);

  interface->addParameter(parameter);

  qi::BoxInstanceModel box("root", 1, 2, 3, interface);

  EXPECT_EQ(box.getName(), "root");
  EXPECT_EQ(box.getId(), 1);
  EXPECT_EQ(box.getX(), 2);
  EXPECT_EQ(box.getY(), 3);
  EXPECT_EQ(box.getParametersValue().size(), 0);
  EXPECT_EQ(box.getPath(), "box.xml");
  EXPECT_EQ(box.getInterface(), interface);


  box.setName("ra");
  EXPECT_EQ(box.getName(), "ra");

  box.setId(2);
  EXPECT_EQ(box.getId(), 2);

  box.setX(3);
  EXPECT_EQ(box.getX(), 3);

  box.setY(4);
  EXPECT_EQ(box.getY(), 4);

  qi::ParameterValueModelPtr param(new qi::ParameterValueModel());
  EXPECT_FALSE(box.addParameterValue(param));

  param->setValueDouble(12.5);
  param->setId(0);
  EXPECT_TRUE(box.addParameterValue(param));
  EXPECT_EQ(box.getParametersValue().size(), 1);

  qi::BoxInterfaceModelPtr interface2(new qi::BoxInterfaceModel("ra.xml", "1", "1","box", "tooltip", "45"));
  box.setBoxInterface(interface2);
  EXPECT_EQ(box.getParametersValue().size(), 0);
  EXPECT_EQ(box.getInterface(), interface2);
  EXPECT_EQ(box.getPath(), "ra.xml");
}

TEST(XmlParser, ParameterValue)
{
  qi::ParameterValueModel value;
  value.setValueDouble(12.5);
  value.setId(0);

  EXPECT_EQ(value.getId(), 0);
  value.setId(1);
  EXPECT_EQ(value.getId(), 1);

  EXPECT_EQ(value.getType(), qi::ParameterModel::ContentType_Double);
  EXPECT_EQ(value.getValueDouble(), 12.5);

  value.setValueBool(true);
  EXPECT_EQ(value.getType(), qi::ParameterModel::ContentType_Bool);
  EXPECT_EQ(value.getValueInt(), true);

  value.setValueInt(1);
  EXPECT_EQ(value.getType(), qi::ParameterModel::ContentType_Int);
  EXPECT_EQ(value.getValueInt(), 1);

  value.setValueString("test");
  EXPECT_EQ(value.getType(), qi::ParameterModel::ContentType_String);
  EXPECT_EQ(value.getValueString(), "test");
}

int main(int argc, char **argv)
{
  qi::Application(argc, argv);
  qi::log::init(qi::log::info, 6);
  ::testing::InitGoogleTest(&argc, argv);

  if(argc < 2)
  {
    std::cerr << "Usage: test_xmlparser /path/to/valid_box_interface_format.xml"
              << std::endl;

    return 2;
  }

  valid_boxinterface = std::string(argv[1]);

  return RUN_ALL_TESTS();
}
