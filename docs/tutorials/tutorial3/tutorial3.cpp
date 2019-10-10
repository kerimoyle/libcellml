/**
 *  TUTORIAL 3: MODEL CREATION THROUGH THE API
 *
 *  By the time you have worked through Tutorial 3 you will be able to:
 *    - create a new model and its child entities from scratch using the API
 *    - define custom combinations of built-in units
 *    - define your own custom units independent from the built-in units
 *
 *  This tutorial assumes that you are comfortable with:
 *    - accessing and adjusting names of items inside a model hierarchy (T2)
 *    - creating a validator and using it to check a model for errors (T2)
 *    - accessing the errors produced by a validator and using them to correct
 *      the model (T2)
 *    - serialising and printing a model to a CellML file (T1)
 */

#include <fstream>
#include <iostream>
#include <sstream>

#include <libcellml/component.h>
#include <libcellml/model.h>
#include <libcellml/parser.h>
#include <libcellml/printer.h>
#include <libcellml/units.h>
#include <libcellml/validator.h>
#include <libcellml/variable.h>

#include "tutorial_utilities.h"

int main() {
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "   TUTORIAL 3: CREATE A MODEL USING THE API" << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  // ---------------------------------------------------------------------------
  //  STEP 1: Create the model instance
  //
  //  1.a   Allocate the ModelPtr
  //  TODO Need some more specific info on how to think about smart pointers
  //  here
  libcellml::ModelPtr model = std::make_shared<libcellml::Model>();

  //  1.b   Set the attributes of the model.
  model->setName("tutorial_3_model");
  model->setId("tutorial_3_model_id");

  //  Check that it worked
  std::cout << "Model has name: '" << model->name() << "'" << std::endl;
  std::cout << "Model has id: '" << model->id() << "'" << std::endl;

  //  1.c   Create a component to use as an integrator, set its attributes and
  //        add it to the model
  libcellml::ComponentPtr distance_finder =
      std::make_shared<libcellml::Component>();
  distance_finder->setName("distance_finder");
  distance_finder->setId("c1");
  model->addComponent(distance_finder);

  //  Check that it worked
  std::cout << "Model has " << model->componentCount()
            << " components:" << std::endl;
  for (size_t c = 0; c < model->componentCount(); ++c) {
    std::cout << "  Component [" << c << "] has name: '"
              << model->component(c)->name() << "'" << std::endl;
    std::cout << "  Component [" << c << "] has id: '"
              << model->component(c)->id() << "'" << std::endl;
  }

  //  1.d   Create some variables and add them to the component
  libcellml::VariablePtr speed = std::make_shared<libcellml::Variable>();
  libcellml::VariablePtr time = std::make_shared<libcellml::Variable>();
  libcellml::VariablePtr distance = std::make_shared<libcellml::Variable>();

  speed->setName("speed");
  time->setName("time");
  distance->setName("distance");

  distance_finder->addVariable(speed);
  distance_finder->addVariable(time);
  distance_finder->addVariable(distance);

  //  1.e   Assign units to the variables
  speed->setUnits("metres_per_second");
  time->setUnits("millisecond");
  distance->setUnits("league");

  //  Check that it all worked
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "         Printing the model at Step 1" << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  printModelToTerminal(model);

  //  1.f   Try validating the model at this stage
  libcellml::ValidatorPtr validator = std::make_shared<libcellml::Validator>();
  validator->validateModel(model);
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "  Printing the validation errors after Step 1" << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  printValidationErrorsToTerminal(validator);

  // ---------------------------------------------------------------------------
  //  STEP 2: Create the user-defined units
  //
  //  From the validation errors printed above you'll see that none of the three
  //  units we need are built-in. The good news is that we can create the ones
  //  we need from the set of built-in units, we just need to define the
  //  relationship.  NB: Even though units are used by Variables, which sit
  //  'inside' Components, Units sit inside the Model itself.  This helps you to
  //  reuse Units when you have more than one component (more on that in
  //  Tutorial TODO)

  //  2.a   Define the relationship between our custom units and the built-in
  //        units. There is a list of built-in units and their definitions
  //        available here TODO
  //        The argument list for the addUnit function is overloaded, and is:

  libcellml::UnitsPtr metres_per_second = std::make_shared<libcellml::Units>();
  metres_per_second->setName("metres_per_second");
  metres_per_second->addUnit(
      "metre"); // metre is a built-in unit, used inside metres_per_second with
                // no exponent, multiplier or prefix
  metres_per_second->addUnit("second",
                            -1.0); // second is a built-in unit, used inside
                                   // meter_per_second with an exponent of -1

  libcellml::UnitsPtr millisecond = std::make_shared<libcellml::Units>();
  millisecond->setName("millisecond");
  millisecond->addUnit(
      "second",
      "milli"); // second is a built-in unit, used inside millisecond with the
                // prefix "milli".  NB this is equivalent to specifying a prefix
                // integer value of -3, corresponding to the power of 10 by
                // which the base is multiplied.

  libcellml::UnitsPtr league = std::make_shared<libcellml::Units>();
  league->setName("league");
  league->addUnit(
      "metre", 3, 1.0,
      5.556); // metre is a built-in unit.  A league is equal to 5556m, but here
              // we illustrate the overloaded function by passing a prefix of 3
              // (indicating a factor of 10^3), an exponent of 1, and a
              // multiplier of 5.556.

  //  2.b   Add the units to the model
  model->addUnits(metres_per_second);
  model->addUnits(millisecond);
  model->addUnits(league);

  //  2.c   Validate the model again
  validator->validateModel(model);
  std::cout << "-----------------------------------------------" << std::endl;
  std::cout << "  Printing the validation errors after Step 2" << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;
  printValidationErrorsToTerminal(validator);
}