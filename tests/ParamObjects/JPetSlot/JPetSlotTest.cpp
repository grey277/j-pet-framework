/**
 *  @copyright Copyright 2019 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @file JPetSlotTest.cpp
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE JPetSlotTest

#include <boost/test/unit_test.hpp>
#include "JPetSlotFactory.h"
#include "JPetSlot.h"

const float epsilon = 0.0001;

BOOST_AUTO_TEST_SUITE(FirstSuite)

BOOST_AUTO_TEST_CASE(default_constructor)
{
  JPetSlot slot;
  BOOST_REQUIRE_EQUAL(slot.getID(), -1);
  BOOST_REQUIRE_CLOSE(slot.getTheta(), -1., epsilon);
  BOOST_REQUIRE_EQUAL(slot.getType(), JPetSlot::Barrel);
}

BOOST_AUTO_TEST_CASE(constructor)
{
  JPetSlot slot(1, 5.5, JPetSlot::Module);
  BOOST_REQUIRE_EQUAL(slot.getID(), 1);
  BOOST_REQUIRE_CLOSE(slot.getTheta(), 5.5, epsilon);
  BOOST_REQUIRE_EQUAL(slot.getType(), JPetSlot::Module);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FactorySuite)

class TestParamGetter: public JPetParamGetter {

ParamObjectsDescriptions getAllBasicData(
  ParamObjectType type, const int runID
) {
  ParamObjectsDescriptions result;
  switch (type) {
    case ParamObjectType::kSlot:
      switch (runID) {
        // No layers
        case 0:
          break;
        // Single object
        case 1:
          result = { { 1, { {"id", "1"}, {"theta", "5.5"}, {"type", "module"}, {"layer_id", "1"} } } };
          break;
        // Two objects
        case 2:
          result = {
            { 1, { {"id", "1"}, {"theta", "5.5"}, {"type", "module"}, {"layer_id", "1"} } },
            { 5, { {"id", "5"}, {"theta", "15.5"}, {"type", "barrel"}, {"layer_id", "1"} } }
          };
          break;
        // Missing field
        case 3:
          result = { { 1, { {"id", "1"}, {"theta", "5.5"}, {"layer_id", "1"} } } };
          break;
        // Wrong field
        case 4:
          result = { { 1, { {"id", "1"}, {"theta", "too big"}, {"type", "module"}, {"layer_id", "1"} } } };
          break;
        // Wrong relation
        case 5:
          result = { { 1, { {"id", "1"}, {"theta", "5.5"}, {"type", "module"}, {"layer_id", "57"} } } };
          break;
      }
      break;
    case ParamObjectType::kLayer:
      result = { { 1, { {"id", "1"}, {"name", "sorbet"}, {"radius", "10.5"}, {"setup_id", "1"} } } };
      break;
    case ParamObjectType::kSetup:
      result = { { 1, { {"id", "1"}, {"description", "jpet"} } } };
      break;
    default:
      break;
  }
  return result;
}

ParamRelationalData getAllRelationalData(
  ParamObjectType type, ParamObjectType, const int runID) {
    ParamRelationalData result;
    switch (type) {
      case ParamObjectType::kSlot:
      switch (runID) {
        case 0:
          break;
        case 1:
          result = { {1, 1} };
          break;
        case 2:
          result = { {1, 1}, {5, 1} };
          break;
        case 5:
          result = { {1, 43} };
          break;
      }
      break;
      default:
        result = { {1, 1 } };
        break;
    }
    return result;
  }
};

TestParamGetter paramGetter;

BOOST_AUTO_TEST_CASE(no_barrelSlots)
{
  JPetSetupFactory setupFactory(paramGetter, 0);
  JPetLayerFactory layerFactory(paramGetter, 0, setupFactory);
  JPetSlotFactory slotfactory(paramGetter, 0, layerFactory);
  auto& slots = slotfactory.getSlots();
  BOOST_REQUIRE_EQUAL(slots.size(), 0u);
}

BOOST_AUTO_TEST_CASE(single_object)
{
  JPetSetupFactory setupFactory(paramGetter, 1);
  JPetLayerFactory layerFactory(paramGetter, 1, setupFactory);
  JPetSlotFactory slotfactory(paramGetter, 1, layerFactory);
  auto& slots = slotfactory.getSlots();
  BOOST_REQUIRE_EQUAL(slots.size(), 1u);
  auto slot = slots[1];
  BOOST_REQUIRE_EQUAL(slot->getID(), 1);
  BOOST_REQUIRE_CLOSE(slot->getTheta(), 5.5, epsilon);
  BOOST_REQUIRE_EQUAL(slot->getType(), JPetSlot::Module);
  BOOST_REQUIRE_EQUAL(slot->getLayer().getID(), layerFactory.getLayers().at(1)->getID());
}

BOOST_AUTO_TEST_CASE(two_objects)
{
  JPetSetupFactory setupFactory(paramGetter, 2);
  JPetLayerFactory layerFactory(paramGetter, 2, setupFactory);
  JPetSlotFactory slotfactory(paramGetter, 2, layerFactory);
  auto& slots = slotfactory.getSlots();
  BOOST_REQUIRE_EQUAL(slots.size(), 2u);
  auto slot = slots[1];
  BOOST_REQUIRE_EQUAL(slot->getID(), 1);
  BOOST_REQUIRE_CLOSE(slot->getTheta(), 5.5, epsilon);
  BOOST_REQUIRE_EQUAL(slot->getType(), JPetSlot::Module);
  BOOST_REQUIRE_EQUAL(slot->getLayer().getID(), layerFactory.getLayers().at(1)->getID());
  slot = slots[5];
  BOOST_REQUIRE_EQUAL(slot->getID(), 5);
  BOOST_REQUIRE_CLOSE(slot->getTheta(), 15.5, epsilon);
  BOOST_REQUIRE_EQUAL(slot->getType(), JPetSlot::Barrel);
  BOOST_REQUIRE_EQUAL(slot->getLayer().getID(), layerFactory.getLayers().at(1)->getID());
}

BOOST_AUTO_TEST_CASE(missing_field)
{
  JPetSetupFactory setupFactory(paramGetter, 3);
  JPetLayerFactory layerFactory(paramGetter, 3, setupFactory);
  JPetSlotFactory slotfactory(paramGetter, 3, layerFactory);
  BOOST_REQUIRE_THROW(slotfactory.getSlots(), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(wrong_field)
{
  JPetSetupFactory setupFactory(paramGetter, 4);
  JPetLayerFactory layerFactory(paramGetter, 4, setupFactory);
  JPetSlotFactory slotfactory(paramGetter, 4, layerFactory);
  BOOST_REQUIRE_THROW(slotfactory.getSlots(), std::bad_cast);
}

BOOST_AUTO_TEST_CASE(wrong_relation)
{
  JPetSetupFactory setupFactory(paramGetter, 5);
  JPetLayerFactory layerFactory(paramGetter, 5, setupFactory);
  JPetSlotFactory slotfactory(paramGetter, 5, layerFactory);
  BOOST_REQUIRE_THROW(slotfactory.getSlots(), std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()