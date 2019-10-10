/**
 *  TUTORIAL 2: Utilities
 *  This file contains helper functions for Tutorial 2
 */
#include <iostream>
#include <libcellml/component.h>
#include <libcellml/model.h>
#include <libcellml/parser.h>
#include <libcellml/printer.h>
#include <libcellml/validator.h>
#include <libcellml/variable.h>

/**
 * @brief Prints the basic contents of a Model to the terminal
 */
void printModelToTerminal(libcellml::ModelPtr &model);

/**
 * @brief Prints the validation errors to the terminal
 */
void printValidationErrorsToTerminal(libcellml::ValidatorPtr &validator);

