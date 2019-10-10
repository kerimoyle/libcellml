
/**
 *  TUTORIAL 2: Utilities
 *  This file contains helper functions for Tutorial 2
 */

#include "tutorial_utilities.h"

void printModelToTerminal(libcellml::ModelPtr &model) {
  std::cout << "The model name is: '" << model->name() << "'" << std::endl;
  std::cout << "The model id is: '" << model->id() << "'" << std::endl;

  // 2.a    Print any custom units of the model
  std::cout << "The model defines " << model->unitsCount()
            << " custom units:" << std::endl;
  for (size_t u = 0; u < model->unitsCount(); ++u) {
    std::cout << "  Units[" << u << "] is '" << model->units(u) << "'"
              << std::endl;
  }

  // 2.b    Print the components of the model
  std::cout << "The model has " << model->componentCount()
            << " components:" << std::endl;
  for (size_t c = 0; c < model->componentCount(); ++c) {
    // 2.c  Printing the attributes of the component
    std::cout << "  Component[" << c << "] has name: '"
              << model->component(c)->name() << "'" << std::endl;
    std::cout << "  Component[" << c << "] has id: '"
              << model->component(c)->id() << "'" << std::endl;
    std::cout << "  Component[" << c << "] has "
              << model->component(c)->variableCount()
              << " variables:" << std::endl;
    //  2.d  Printing the variables within the component
    for (size_t v = 0; v < model->component(c)->variableCount(); v++) {
      std::cout << "    Variable[" << v << "] has name: '"
                << model->component(c)->variable(v)->name() << "'" << std::endl;
      std::cout << "    Variable[" << v << "] has initial_value: '"
                << model->component(c)->variable(v)->initialValue() << "'"
                << std::endl;
      std::cout << "    Variable[" << v << "] has units: '"
                << model->component(c)->variable(v)->units() << "'"
                << std::endl;
    }
    // 2.c   Print the maths within the component
    std::cout << "    Maths in the component is:" << std::endl;
    std::cout << model->component(c)->math() << std::endl;
  }
}

void printValidationErrorsToTerminal(libcellml::ValidatorPtr &validator) {
  //  2.b   Check whether there were errors returned from the validator
  int numberOfValidationErrors = validator->errorCount();

  std::cout << "The validator has found " << numberOfValidationErrors
            << " errors!" << std::endl;
  if (numberOfValidationErrors != 0) {

    // 2.c  Retrieve the errors, and print their description and specification
    //      reference to the terminal
    for (size_t e = 0; e < numberOfValidationErrors; ++e) {
      libcellml::ErrorPtr validatorError = validator->error(e);
      std::string errorSpecificationReference =
          validatorError->specificationHeading();

      std::cout << "  Validator error[" << e << "]:" << std::endl;
      std::cout << "     Description: " << validatorError->description()
                << std::endl;
      std::cout << "     Kind: " << (int)validatorError->kind() << std::endl;
      if (errorSpecificationReference != "") {
        std::cout << "    See section " << errorSpecificationReference
                  << " in the CellML specification." << std::endl;
      }
    }
  }
}