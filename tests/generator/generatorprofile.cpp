/*
Copyright libCellML Contributors

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifdef _WIN32
#    define _USE_MATH_DEFINES
#endif

#include "test_utils.h"

#include "gtest/gtest.h"

#include <libcellml>

#include <cmath>

std::string convertDoubleToString(double value)
{
    std::ostringstream strs;
    strs << std::setprecision(std::numeric_limits<double>::digits10) << value;
    return strs.str();
}

TEST(GeneratorProfile, defaultGeneralValues)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    EXPECT_EQ(libcellml::GeneratorProfile::Profile::C, generatorProfile->profile());

    EXPECT_EQ(true, generatorProfile->hasInterface());
}

TEST(GeneratorProfile, defaultRelationalAndLogicalOperatorValues)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    EXPECT_EQ(" = ", generatorProfile->assignmentString());
    EXPECT_EQ(" == ", generatorProfile->eqString());
    EXPECT_EQ(" != ", generatorProfile->neqString());
    EXPECT_EQ(" < ", generatorProfile->ltString());
    EXPECT_EQ(" <= ", generatorProfile->leqString());
    EXPECT_EQ(" > ", generatorProfile->gtString());
    EXPECT_EQ(" >= ", generatorProfile->geqString());
    EXPECT_EQ(" && ", generatorProfile->andString());
    EXPECT_EQ(" || ", generatorProfile->orString());
    EXPECT_EQ("xor", generatorProfile->xorString());
    EXPECT_EQ("!", generatorProfile->notString());

    EXPECT_EQ(true, generatorProfile->hasEqOperator());
    EXPECT_EQ(true, generatorProfile->hasNeqOperator());
    EXPECT_EQ(true, generatorProfile->hasLtOperator());
    EXPECT_EQ(true, generatorProfile->hasLeqOperator());
    EXPECT_EQ(true, generatorProfile->hasGtOperator());
    EXPECT_EQ(true, generatorProfile->hasGeqOperator());
    EXPECT_EQ(true, generatorProfile->hasAndOperator());
    EXPECT_EQ(true, generatorProfile->hasOrOperator());
    EXPECT_EQ(false, generatorProfile->hasXorOperator());
    EXPECT_EQ(true, generatorProfile->hasNotOperator());
}

TEST(GeneratorProfile, defaultArithmeticOperatorValues)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    EXPECT_EQ("+", generatorProfile->plusString());
    EXPECT_EQ("-", generatorProfile->minusString());
    EXPECT_EQ("*", generatorProfile->timesString());
    EXPECT_EQ("/", generatorProfile->divideString());
    EXPECT_EQ("pow", generatorProfile->powerString());
    EXPECT_EQ("sqrt", generatorProfile->squareRootString());
    EXPECT_EQ("", generatorProfile->squareString());
    EXPECT_EQ("fabs", generatorProfile->absoluteValueString());
    EXPECT_EQ("exp", generatorProfile->exponentialString());
    EXPECT_EQ("log", generatorProfile->napierianLogarithmString());
    EXPECT_EQ("log10", generatorProfile->commonLogarithmString());
    EXPECT_EQ("ceil", generatorProfile->ceilingString());
    EXPECT_EQ("floor", generatorProfile->floorString());
    EXPECT_EQ("min", generatorProfile->minString());
    EXPECT_EQ("max", generatorProfile->maxString());
    EXPECT_EQ("fmod", generatorProfile->remString());

    EXPECT_EQ(false, generatorProfile->hasPowerOperator());
}

TEST(GeneratorProfile, defaultTrigonometricOperatorValues)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    EXPECT_EQ("sin", generatorProfile->sinString());
    EXPECT_EQ("cos", generatorProfile->cosString());
    EXPECT_EQ("tan", generatorProfile->tanString());
    EXPECT_EQ("sec", generatorProfile->secString());
    EXPECT_EQ("csc", generatorProfile->cscString());
    EXPECT_EQ("cot", generatorProfile->cotString());
    EXPECT_EQ("sinh", generatorProfile->sinhString());
    EXPECT_EQ("cosh", generatorProfile->coshString());
    EXPECT_EQ("tanh", generatorProfile->tanhString());
    EXPECT_EQ("sech", generatorProfile->sechString());
    EXPECT_EQ("csch", generatorProfile->cschString());
    EXPECT_EQ("coth", generatorProfile->cothString());
    EXPECT_EQ("asin", generatorProfile->asinString());
    EXPECT_EQ("acos", generatorProfile->acosString());
    EXPECT_EQ("atan", generatorProfile->atanString());
    EXPECT_EQ("asec", generatorProfile->asecString());
    EXPECT_EQ("acsc", generatorProfile->acscString());
    EXPECT_EQ("acot", generatorProfile->acotString());
    EXPECT_EQ("asinh", generatorProfile->asinhString());
    EXPECT_EQ("acosh", generatorProfile->acoshString());
    EXPECT_EQ("atanh", generatorProfile->atanhString());
    EXPECT_EQ("asech", generatorProfile->asechString());
    EXPECT_EQ("acsch", generatorProfile->acschString());
    EXPECT_EQ("acoth", generatorProfile->acothString());
}

TEST(GeneratorProfile, defaultPiecewiseStatementValues)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    EXPECT_EQ("(<CONDITION>)?<IF_STATEMENT>", generatorProfile->conditionalOperatorIfString());
    EXPECT_EQ(":<ELSE_STATEMENT>", generatorProfile->conditionalOperatorElseString());
    EXPECT_EQ("", generatorProfile->piecewiseIfString());
    EXPECT_EQ("", generatorProfile->piecewiseElseString());

    EXPECT_EQ(true, generatorProfile->hasConditionalOperator());
}

TEST(GeneratorProfile, defaultConstantValues)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    EXPECT_EQ("1.0", generatorProfile->trueString());
    EXPECT_EQ("0.0", generatorProfile->falseString());
    EXPECT_EQ(convertDoubleToString(exp(1.0)), generatorProfile->eString());
    EXPECT_EQ(convertDoubleToString(M_PI), generatorProfile->piString());
    EXPECT_EQ("1.0/0.0", generatorProfile->infString());
    EXPECT_EQ("sqrt(-1.0)", generatorProfile->nanString());
}

TEST(GeneratorProfile, defaultMathematicalFunctionValues)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    EXPECT_EQ("", generatorProfile->interfaceEqFunctionString());
    EXPECT_EQ("", generatorProfile->implementationEqFunctionString());

    EXPECT_EQ("", generatorProfile->interfaceNeqFunctionString());
    EXPECT_EQ("", generatorProfile->implementationNeqFunctionString());

    EXPECT_EQ("", generatorProfile->interfaceLtFunctionString());
    EXPECT_EQ("", generatorProfile->implementationLtFunctionString());

    EXPECT_EQ("", generatorProfile->interfaceLeqFunctionString());
    EXPECT_EQ("", generatorProfile->implementationLeqFunctionString());

    EXPECT_EQ("", generatorProfile->interfaceGtFunctionString());
    EXPECT_EQ("", generatorProfile->implementationGtFunctionString());

    EXPECT_EQ("", generatorProfile->interfaceGeqFunctionString());
    EXPECT_EQ("", generatorProfile->implementationGeqFunctionString());

    EXPECT_EQ("", generatorProfile->interfaceAndFunctionString());
    EXPECT_EQ("", generatorProfile->implementationAndFunctionString());

    EXPECT_EQ("", generatorProfile->interfaceOrFunctionString());
    EXPECT_EQ("", generatorProfile->implementationOrFunctionString());

    EXPECT_EQ("extern double xor(double x, double y);\n",
              generatorProfile->interfaceXorFunctionString());
    EXPECT_EQ("double xor(double x, double y)\n"
              "{\n"
              "    return (x != 0.0) ^ (y != 0.0);\n"
              "}\n",
              generatorProfile->implementationXorFunctionString());

    EXPECT_EQ("", generatorProfile->interfaceNotFunctionString());
    EXPECT_EQ("", generatorProfile->implementationNotFunctionString());

    EXPECT_EQ("extern double min(double x, double y);\n",
              generatorProfile->interfaceMinFunctionString());
    EXPECT_EQ("double min(double x, double y)\n"
              "{\n"
              "    return (x < y)?x:y;\n"
              "}\n",
              generatorProfile->implementationMinFunctionString());

    EXPECT_EQ("extern double max(double x, double y);\n",
              generatorProfile->interfaceMaxFunctionString());
    EXPECT_EQ("double max(double x, double y)\n"
              "{\n"
              "    return (x > y)?x:y;\n"
              "}\n",
              generatorProfile->implementationMaxFunctionString());

    EXPECT_EQ("extern double sec(double x);\n",
              generatorProfile->interfaceSecFunctionString());
    EXPECT_EQ("double sec(double x)\n"
              "{\n"
              "    return 1.0/cos(x);\n"
              "}\n",
              generatorProfile->implementationSecFunctionString());

    EXPECT_EQ("extern double csc(double x);\n",
              generatorProfile->interfaceCscFunctionString());
    EXPECT_EQ("double csc(double x)\n"
              "{\n"
              "    return 1.0/sin(x);\n"
              "}\n",
              generatorProfile->implementationCscFunctionString());

    EXPECT_EQ("extern double cot(double x);\n",
              generatorProfile->interfaceCotFunctionString());
    EXPECT_EQ("double cot(double x)\n"
              "{\n"
              "    return 1.0/tan(x);\n"
              "}\n",
              generatorProfile->implementationCotFunctionString());

    EXPECT_EQ("extern double sech(double x);\n",
              generatorProfile->interfaceSechFunctionString());
    EXPECT_EQ("double sech(double x)\n"
              "{\n"
              "    return 1.0/cosh(x);\n"
              "}\n",
              generatorProfile->implementationSechFunctionString());

    EXPECT_EQ("extern double csch(double x);\n",
              generatorProfile->interfaceCschFunctionString());
    EXPECT_EQ("double csch(double x)\n"
              "{\n"
              "    return 1.0/sinh(x);\n"
              "}\n",
              generatorProfile->implementationCschFunctionString());

    EXPECT_EQ("extern double coth(double x);\n",
              generatorProfile->interfaceCothFunctionString());
    EXPECT_EQ("double coth(double x)\n"
              "{\n"
              "    return 1.0/tanh(x);\n"
              "}\n",
              generatorProfile->implementationCothFunctionString());

    EXPECT_EQ("extern double asec(double x);\n",
              generatorProfile->interfaceAsecFunctionString());
    EXPECT_EQ("double asec(double x)\n"
              "{\n"
              "    return acos(1.0/x);\n"
              "}\n",
              generatorProfile->implementationAsecFunctionString());

    EXPECT_EQ("extern double acsc(double x);\n",
              generatorProfile->interfaceAcscFunctionString());
    EXPECT_EQ("double acsc(double x)\n"
              "{\n"
              "    return asin(1.0/x);\n"
              "}\n",
              generatorProfile->implementationAcscFunctionString());

    EXPECT_EQ("extern double acot(double x);\n",
              generatorProfile->interfaceAcotFunctionString());
    EXPECT_EQ("double acot(double x)\n"
              "{\n"
              "    return atan(1.0/x);\n"
              "}\n",
              generatorProfile->implementationAcotFunctionString());

    EXPECT_EQ("extern double asech(double x);\n",
              generatorProfile->interfaceAsechFunctionString());
    EXPECT_EQ("double asech(double x)\n"
              "{\n"
              "    double oneOverX = 1.0/x;\n"
              "\n"
              "    return log(oneOverX+sqrt(oneOverX*oneOverX-1.0));\n"
              "}\n",
              generatorProfile->implementationAsechFunctionString());

    EXPECT_EQ("extern double acsch(double x);\n",
              generatorProfile->interfaceAcschFunctionString());
    EXPECT_EQ("double acsch(double x)\n"
              "{\n"
              "    double oneOverX = 1.0/x;\n"
              "\n"
              "    return log(oneOverX+sqrt(oneOverX*oneOverX+1.0));\n"
              "}\n",
              generatorProfile->implementationAcschFunctionString());

    EXPECT_EQ("extern double acoth(double x);\n",
              generatorProfile->interfaceAcothFunctionString());
    EXPECT_EQ("double acoth(double x)\n"
              "{\n"
              "    double oneOverX = 1.0/x;\n"
              "\n"
              "    return 0.5*log((1.0+oneOverX)/(1.0-oneOverX));\n"
              "}\n",
              generatorProfile->implementationAcothFunctionString());
}

TEST(GeneratorProfile, defaultMiscellaneousValues)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    EXPECT_EQ("/* <CODE> */\n", generatorProfile->commentString());
    EXPECT_EQ("The content of this file was generated using <PROFILE_INFORMATION> libCellML <LIBCELLML_VERSION>.", generatorProfile->originCommentString());

    EXPECT_EQ("#pragma once\n"
              "\n"
              "#include <stddef.h>\n",
              generatorProfile->interfaceHeaderString());
    EXPECT_EQ("#include \"model.h\"\n"
              "\n"
              "#include <math.h>\n"
              "#include <stdlib.h>\n",
              generatorProfile->implementationHeaderString());

    EXPECT_EQ("extern const char LIBCELLML_VERSION[];\n", generatorProfile->interfaceLibcellmlVersionString());
    EXPECT_EQ("const char LIBCELLML_VERSION[] = \"<LIBCELLML_VERSION>\";\n", generatorProfile->implementationLibcellmlVersionString());

    EXPECT_EQ("extern const size_t STATE_COUNT;\n", generatorProfile->interfaceStateCountString());
    EXPECT_EQ("const size_t STATE_COUNT = <STATE_COUNT>;\n", generatorProfile->implementationStateCountString());

    EXPECT_EQ("extern const size_t VARIABLE_COUNT;\n", generatorProfile->interfaceVariableCountString());
    EXPECT_EQ("const size_t VARIABLE_COUNT = <VARIABLE_COUNT>;\n", generatorProfile->implementationVariableCountString());

    EXPECT_EQ("typedef enum {\n"
              "    CONSTANT,\n"
              "    COMPUTED_CONSTANT,\n"
              "    ALGEBRAIC\n"
              "} VariableType;\n",
              generatorProfile->variableTypeObjectString());

    EXPECT_EQ("CONSTANT", generatorProfile->constantVariableTypeString());
    EXPECT_EQ("COMPUTED_CONSTANT", generatorProfile->computedConstantVariableTypeString());
    EXPECT_EQ("ALGEBRAIC", generatorProfile->algebraicVariableTypeString());

    EXPECT_EQ("typedef struct {\n"
              "    char name[<NAME_SIZE>];\n"
              "    char units[<UNITS_SIZE>];\n"
              "    char component[<COMPONENT_SIZE>];\n"
              "} VariableInfo;\n",
              generatorProfile->variableInfoObjectString());
    EXPECT_EQ("typedef struct {\n"
              "    char name[<NAME_SIZE>];\n"
              "    char units[<UNITS_SIZE>];\n"
              "    char component[<COMPONENT_SIZE>];\n"
              "    VariableType type;\n"
              "} VariableInfoWithType;\n",
              generatorProfile->variableInfoWithTypeObjectString());

    EXPECT_EQ("extern const VariableInfo VOI_INFO;\n", generatorProfile->interfaceVoiInfoString());
    EXPECT_EQ("const VariableInfo VOI_INFO = <CODE>;\n", generatorProfile->implementationVoiInfoString());

    EXPECT_EQ("extern const VariableInfo STATE_INFO[];\n", generatorProfile->interfaceStateInfoString());
    EXPECT_EQ("const VariableInfo STATE_INFO[] = {\n"
              "<CODE>"
              "};\n",
              generatorProfile->implementationStateInfoString());

    EXPECT_EQ("extern const VariableInfoWithType VARIABLE_INFO[];\n", generatorProfile->interfaceVariableInfoString());
    EXPECT_EQ("const VariableInfoWithType VARIABLE_INFO[] = {\n"
              "<CODE>"
              "};\n",
              generatorProfile->implementationVariableInfoString());

    EXPECT_EQ("{\"<NAME>\", \"<UNITS>\", \"<COMPONENT>\"}", generatorProfile->variableInfoEntryString());
    EXPECT_EQ("{\"<NAME>\", \"<UNITS>\", \"<COMPONENT>\", <TYPE>}", generatorProfile->variableInfoWithTypeEntryString());

    EXPECT_EQ("voi", generatorProfile->voiString());

    EXPECT_EQ("states", generatorProfile->statesArrayString());
    EXPECT_EQ("rates", generatorProfile->ratesArrayString());
    EXPECT_EQ("variables", generatorProfile->variablesArrayString());

    EXPECT_EQ("return (double *) malloc(<ARRAY_SIZE>*sizeof(double));\n",
              generatorProfile->returnCreatedArrayString());

    EXPECT_EQ("double * createStatesArray();\n",
              generatorProfile->interfaceCreateStatesArrayMethodString());
    EXPECT_EQ("double * createStatesArray()\n"
              "{\n"
              "<CODE>"
              "}\n",
              generatorProfile->implementationCreateStatesArrayMethodString());

    EXPECT_EQ("double * createVariablesArray();\n",
              generatorProfile->interfaceCreateVariablesArrayMethodString());
    EXPECT_EQ("double * createVariablesArray()\n"
              "{\n"
              "<CODE>"
              "}\n",
              generatorProfile->implementationCreateVariablesArrayMethodString());

    EXPECT_EQ("void deleteArray(double *array);\n",
              generatorProfile->interfaceDeleteArrayMethodString());
    EXPECT_EQ("void deleteArray(double *array)\n"
              "{\n"
              "    free(array);\n"
              "}\n",
              generatorProfile->implementationDeleteArrayMethodString());

    EXPECT_EQ("void initializeStatesAndConstants(double *states, double *variables);\n",
              generatorProfile->interfaceInitializeStatesAndConstantsMethodString());
    EXPECT_EQ("void initializeStatesAndConstants(double *states, double *variables)\n"
              "{\n"
              "<CODE>"
              "}\n",
              generatorProfile->implementationInitializeStatesAndConstantsMethodString());

    EXPECT_EQ("void computeComputedConstants(double *variables);\n",
              generatorProfile->interfaceComputeComputedConstantsMethodString());
    EXPECT_EQ("void computeComputedConstants(double *variables)\n"
              "{\n"
              "<CODE>"
              "}\n",
              generatorProfile->implementationComputeComputedConstantsMethodString());

    EXPECT_EQ("void computeRates(double voi, double *states, double *rates, double *variables);\n",
              generatorProfile->interfaceComputeRatesMethodString());
    EXPECT_EQ("void computeRates(double voi, double *states, double *rates, double *variables)\n"
              "{\n"
              "<CODE>"
              "}\n",
              generatorProfile->implementationComputeRatesMethodString());

    EXPECT_EQ("void computeVariables(double voi, double *states, double *rates, double *variables);\n",
              generatorProfile->interfaceComputeVariablesMethodString());
    EXPECT_EQ("void computeVariables(double voi, double *states, double *rates, double *variables)\n"
              "{\n"
              "<CODE>"
              "}\n",
              generatorProfile->implementationComputeVariablesMethodString());

    EXPECT_EQ("", generatorProfile->emptyMethodString());

    EXPECT_EQ("    ", generatorProfile->indentString());

    EXPECT_EQ("{", generatorProfile->openArrayInitializerString());
    EXPECT_EQ("}", generatorProfile->closeArrayInitializerString());

    EXPECT_EQ("[", generatorProfile->openArrayString());
    EXPECT_EQ("]", generatorProfile->closeArrayString());

    EXPECT_EQ(",", generatorProfile->arrayElementSeparatorString());

    EXPECT_EQ("\"", generatorProfile->stringDelimiterString());

    EXPECT_EQ(";", generatorProfile->commandSeparatorString());
}

TEST(GeneratorProfile, generalSettings)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    const libcellml::GeneratorProfile::Profile profile = libcellml::GeneratorProfile::Profile::PYTHON;
    const bool falseValue = false;

    generatorProfile->setProfile(profile);

    generatorProfile->setHasInterface(falseValue);

    EXPECT_EQ(profile, generatorProfile->profile());

    EXPECT_EQ(falseValue, generatorProfile->hasInterface());
}

TEST(GeneratorProfile, relationalAndLogicalOperators)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    const std::string value = "value";
    const bool trueValue = true;
    const bool falseValue = false;

    generatorProfile->setAssignmentString(value);
    generatorProfile->setEqString(value);
    generatorProfile->setNeqString(value);
    generatorProfile->setLtString(value);
    generatorProfile->setLeqString(value);
    generatorProfile->setGtString(value);
    generatorProfile->setGeqString(value);
    generatorProfile->setAndString(value);
    generatorProfile->setOrString(value);
    generatorProfile->setXorString(value);
    generatorProfile->setNotString(value);

    generatorProfile->setHasEqOperator(falseValue);
    generatorProfile->setHasNeqOperator(falseValue);
    generatorProfile->setHasLtOperator(falseValue);
    generatorProfile->setHasLeqOperator(falseValue);
    generatorProfile->setHasGtOperator(falseValue);
    generatorProfile->setHasGeqOperator(falseValue);
    generatorProfile->setHasAndOperator(falseValue);
    generatorProfile->setHasOrOperator(falseValue);
    generatorProfile->setHasXorOperator(trueValue);
    generatorProfile->setHasNotOperator(falseValue);

    EXPECT_EQ(value, generatorProfile->assignmentString());
    EXPECT_EQ(value, generatorProfile->eqString());
    EXPECT_EQ(value, generatorProfile->neqString());
    EXPECT_EQ(value, generatorProfile->ltString());
    EXPECT_EQ(value, generatorProfile->leqString());
    EXPECT_EQ(value, generatorProfile->gtString());
    EXPECT_EQ(value, generatorProfile->geqString());
    EXPECT_EQ(value, generatorProfile->andString());
    EXPECT_EQ(value, generatorProfile->orString());
    EXPECT_EQ(value, generatorProfile->xorString());
    EXPECT_EQ(value, generatorProfile->notString());

    EXPECT_EQ(falseValue, generatorProfile->hasEqOperator());
    EXPECT_EQ(falseValue, generatorProfile->hasNeqOperator());
    EXPECT_EQ(falseValue, generatorProfile->hasLtOperator());
    EXPECT_EQ(falseValue, generatorProfile->hasLeqOperator());
    EXPECT_EQ(falseValue, generatorProfile->hasGtOperator());
    EXPECT_EQ(falseValue, generatorProfile->hasGeqOperator());
    EXPECT_EQ(falseValue, generatorProfile->hasAndOperator());
    EXPECT_EQ(falseValue, generatorProfile->hasOrOperator());
    EXPECT_EQ(trueValue, generatorProfile->hasXorOperator());
    EXPECT_EQ(falseValue, generatorProfile->hasNotOperator());
}

TEST(GeneratorProfile, arithmeticOperators)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    const std::string value = "value";
    const bool trueValue = true;

    generatorProfile->setPlusString(value);
    generatorProfile->setMinusString(value);
    generatorProfile->setTimesString(value);
    generatorProfile->setDivideString(value);
    generatorProfile->setPowerString(value);
    generatorProfile->setSquareRootString(value);
    generatorProfile->setSquareString(value);
    generatorProfile->setAbsoluteValueString(value);
    generatorProfile->setExponentialString(value);
    generatorProfile->setNapierianLogarithmString(value);
    generatorProfile->setCommonLogarithmString(value);
    generatorProfile->setCeilingString(value);
    generatorProfile->setFloorString(value);
    generatorProfile->setMinString(value);
    generatorProfile->setMaxString(value);
    generatorProfile->setRemString(value);

    generatorProfile->setHasPowerOperator(trueValue);

    EXPECT_EQ(value, generatorProfile->plusString());
    EXPECT_EQ(value, generatorProfile->minusString());
    EXPECT_EQ(value, generatorProfile->timesString());
    EXPECT_EQ(value, generatorProfile->divideString());
    EXPECT_EQ(value, generatorProfile->powerString());
    EXPECT_EQ(value, generatorProfile->squareRootString());
    EXPECT_EQ(value, generatorProfile->squareString());
    EXPECT_EQ(value, generatorProfile->absoluteValueString());
    EXPECT_EQ(value, generatorProfile->exponentialString());
    EXPECT_EQ(value, generatorProfile->napierianLogarithmString());
    EXPECT_EQ(value, generatorProfile->commonLogarithmString());
    EXPECT_EQ(value, generatorProfile->ceilingString());
    EXPECT_EQ(value, generatorProfile->floorString());
    EXPECT_EQ(value, generatorProfile->minString());
    EXPECT_EQ(value, generatorProfile->maxString());
    EXPECT_EQ(value, generatorProfile->remString());

    EXPECT_EQ(trueValue, generatorProfile->hasPowerOperator());
}

TEST(GeneratorProfile, trigonometricOperators)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    const std::string value = "value";

    generatorProfile->setSinString(value);
    generatorProfile->setCosString(value);
    generatorProfile->setTanString(value);
    generatorProfile->setSecString(value);
    generatorProfile->setCscString(value);
    generatorProfile->setCotString(value);
    generatorProfile->setSinhString(value);
    generatorProfile->setCoshString(value);
    generatorProfile->setTanhString(value);
    generatorProfile->setSechString(value);
    generatorProfile->setCschString(value);
    generatorProfile->setCothString(value);
    generatorProfile->setAsinString(value);
    generatorProfile->setAcosString(value);
    generatorProfile->setAtanString(value);
    generatorProfile->setAsecString(value);
    generatorProfile->setAcscString(value);
    generatorProfile->setAcotString(value);
    generatorProfile->setAsinhString(value);
    generatorProfile->setAcoshString(value);
    generatorProfile->setAtanhString(value);
    generatorProfile->setAsechString(value);
    generatorProfile->setAcschString(value);
    generatorProfile->setAcothString(value);

    EXPECT_EQ(value, generatorProfile->sinString());
    EXPECT_EQ(value, generatorProfile->cosString());
    EXPECT_EQ(value, generatorProfile->tanString());
    EXPECT_EQ(value, generatorProfile->secString());
    EXPECT_EQ(value, generatorProfile->cscString());
    EXPECT_EQ(value, generatorProfile->cotString());
    EXPECT_EQ(value, generatorProfile->sinhString());
    EXPECT_EQ(value, generatorProfile->coshString());
    EXPECT_EQ(value, generatorProfile->tanhString());
    EXPECT_EQ(value, generatorProfile->sechString());
    EXPECT_EQ(value, generatorProfile->cschString());
    EXPECT_EQ(value, generatorProfile->cothString());
    EXPECT_EQ(value, generatorProfile->asinString());
    EXPECT_EQ(value, generatorProfile->acosString());
    EXPECT_EQ(value, generatorProfile->atanString());
    EXPECT_EQ(value, generatorProfile->asecString());
    EXPECT_EQ(value, generatorProfile->acscString());
    EXPECT_EQ(value, generatorProfile->acotString());
    EXPECT_EQ(value, generatorProfile->asinhString());
    EXPECT_EQ(value, generatorProfile->acoshString());
    EXPECT_EQ(value, generatorProfile->atanhString());
    EXPECT_EQ(value, generatorProfile->asechString());
    EXPECT_EQ(value, generatorProfile->acschString());
    EXPECT_EQ(value, generatorProfile->acothString());
}

TEST(GeneratorProfile, piecewiseStatement)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    const std::string value = "value";
    const bool falseValue = true;

    generatorProfile->setConditionalOperatorIfString(value);
    generatorProfile->setConditionalOperatorElseString(value);
    generatorProfile->setPiecewiseIfString(value);
    generatorProfile->setPiecewiseElseString(value);

    generatorProfile->setHasConditionalOperator(falseValue);

    EXPECT_EQ(value, generatorProfile->conditionalOperatorIfString());
    EXPECT_EQ(value, generatorProfile->conditionalOperatorElseString());
    EXPECT_EQ(value, generatorProfile->piecewiseIfString());
    EXPECT_EQ(value, generatorProfile->piecewiseElseString());

    EXPECT_EQ(falseValue, generatorProfile->hasConditionalOperator());
}

TEST(GeneratorProfile, constants)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    const std::string value = "value";

    generatorProfile->setTrueString(value);
    generatorProfile->setFalseString(value);
    generatorProfile->setEString(value);
    generatorProfile->setPiString(value);
    generatorProfile->setInfString(value);
    generatorProfile->setNanString(value);

    EXPECT_EQ(value, generatorProfile->trueString());
    EXPECT_EQ(value, generatorProfile->falseString());
    EXPECT_EQ(value, generatorProfile->eString());
    EXPECT_EQ(value, generatorProfile->piString());
    EXPECT_EQ(value, generatorProfile->infString());
    EXPECT_EQ(value, generatorProfile->nanString());
}

TEST(GeneratorProfile, mathematicalFunctions)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    const std::string value = "value";

    generatorProfile->setInterfaceEqFunctionString(value);
    generatorProfile->setImplementationEqFunctionString(value);

    generatorProfile->setInterfaceNeqFunctionString(value);
    generatorProfile->setImplementationNeqFunctionString(value);

    generatorProfile->setInterfaceLtFunctionString(value);
    generatorProfile->setImplementationLtFunctionString(value);

    generatorProfile->setInterfaceLeqFunctionString(value);
    generatorProfile->setImplementationLeqFunctionString(value);

    generatorProfile->setInterfaceGtFunctionString(value);
    generatorProfile->setImplementationGtFunctionString(value);

    generatorProfile->setInterfaceGeqFunctionString(value);
    generatorProfile->setImplementationGeqFunctionString(value);

    generatorProfile->setInterfaceAndFunctionString(value);
    generatorProfile->setImplementationAndFunctionString(value);

    generatorProfile->setInterfaceOrFunctionString(value);
    generatorProfile->setImplementationOrFunctionString(value);

    generatorProfile->setInterfaceXorFunctionString(value);
    generatorProfile->setImplementationXorFunctionString(value);

    generatorProfile->setInterfaceNotFunctionString(value);
    generatorProfile->setImplementationNotFunctionString(value);

    generatorProfile->setInterfaceMinFunctionString(value);
    generatorProfile->setImplementationMinFunctionString(value);

    generatorProfile->setInterfaceMaxFunctionString(value);
    generatorProfile->setImplementationMaxFunctionString(value);

    generatorProfile->setInterfaceSecFunctionString(value);
    generatorProfile->setImplementationSecFunctionString(value);

    generatorProfile->setInterfaceCscFunctionString(value);
    generatorProfile->setImplementationCscFunctionString(value);

    generatorProfile->setInterfaceCotFunctionString(value);
    generatorProfile->setImplementationCotFunctionString(value);

    generatorProfile->setInterfaceSechFunctionString(value);
    generatorProfile->setImplementationSechFunctionString(value);

    generatorProfile->setInterfaceCschFunctionString(value);
    generatorProfile->setImplementationCschFunctionString(value);

    generatorProfile->setInterfaceCothFunctionString(value);
    generatorProfile->setImplementationCothFunctionString(value);

    generatorProfile->setInterfaceAsecFunctionString(value);
    generatorProfile->setImplementationAsecFunctionString(value);

    generatorProfile->setInterfaceAcscFunctionString(value);
    generatorProfile->setImplementationAcscFunctionString(value);

    generatorProfile->setInterfaceAcotFunctionString(value);
    generatorProfile->setImplementationAcotFunctionString(value);

    generatorProfile->setInterfaceAsechFunctionString(value);
    generatorProfile->setImplementationAsechFunctionString(value);

    generatorProfile->setInterfaceAcschFunctionString(value);
    generatorProfile->setImplementationAcschFunctionString(value);

    generatorProfile->setInterfaceAcothFunctionString(value);
    generatorProfile->setImplementationAcothFunctionString(value);

    EXPECT_EQ(value, generatorProfile->interfaceEqFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationEqFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceNeqFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationNeqFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceLtFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationLtFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceLeqFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationLeqFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceGtFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationGtFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceGeqFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationGeqFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceAndFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationAndFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceOrFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationOrFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceXorFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationXorFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceNotFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationNotFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceMinFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationMinFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceMaxFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationMaxFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceSecFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationSecFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceCscFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationCscFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceCotFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationCotFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceSechFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationSechFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceCschFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationCschFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceCothFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationCothFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceAsecFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationAsecFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceAcscFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationAcscFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceAcotFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationAcotFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceAsechFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationAsechFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceAcschFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationAcschFunctionString());

    EXPECT_EQ(value, generatorProfile->interfaceAcothFunctionString());
    EXPECT_EQ(value, generatorProfile->implementationAcothFunctionString());
}

TEST(GeneratorProfile, miscellaneous)
{
    libcellml::GeneratorProfilePtr generatorProfile = std::make_shared<libcellml::GeneratorProfile>();

    const std::string value = "value";

    generatorProfile->setCommentString(value);
    generatorProfile->setOriginCommentString(value);

    generatorProfile->setInterfaceHeaderString(value);
    generatorProfile->setImplementationHeaderString(value);

    generatorProfile->setInterfaceLibcellmlVersionString(value);
    generatorProfile->setImplementationLibcellmlVersionString(value);

    generatorProfile->setInterfaceStateCountString(value);
    generatorProfile->setImplementationStateCountString(value);

    generatorProfile->setInterfaceVariableCountString(value);
    generatorProfile->setImplementationVariableCountString(value);

    generatorProfile->setVariableTypeObjectString(value);

    generatorProfile->setConstantVariableTypeString(value);
    generatorProfile->setComputedConstantVariableTypeString(value);
    generatorProfile->setAlgebraicVariableTypeString(value);

    generatorProfile->setVariableInfoObjectString(value);
    generatorProfile->setVariableInfoWithTypeObjectString(value);

    generatorProfile->setInterfaceVoiInfoString(value);
    generatorProfile->setImplementationVoiInfoString(value);

    generatorProfile->setInterfaceStateInfoString(value);
    generatorProfile->setImplementationStateInfoString(value);

    generatorProfile->setInterfaceVariableInfoString(value);
    generatorProfile->setImplementationVariableInfoString(value);

    generatorProfile->setVariableInfoEntryString(value);
    generatorProfile->setVariableInfoWithTypeEntryString(value);

    generatorProfile->setVoiString(value);

    generatorProfile->setStatesArrayString(value);
    generatorProfile->setRatesArrayString(value);
    generatorProfile->setVariablesArrayString(value);

    generatorProfile->setReturnCreatedArrayString(value);

    generatorProfile->setInterfaceCreateStatesArrayMethodString(value);
    generatorProfile->setImplementationCreateStatesArrayMethodString(value);

    generatorProfile->setInterfaceCreateVariablesArrayMethodString(value);
    generatorProfile->setImplementationCreateVariablesArrayMethodString(value);

    generatorProfile->setInterfaceDeleteArrayMethodString(value);
    generatorProfile->setImplementationDeleteArrayMethodString(value);

    generatorProfile->setInterfaceInitializeStatesAndConstantsMethodString(value);
    generatorProfile->setImplementationInitializeStatesAndConstantsMethodString(value);

    generatorProfile->setInterfaceComputeComputedConstantsMethodString(value);
    generatorProfile->setImplementationComputeComputedConstantsMethodString(value);

    generatorProfile->setInterfaceComputeRatesMethodString(value);
    generatorProfile->setImplementationComputeRatesMethodString(value);

    generatorProfile->setInterfaceComputeVariablesMethodString(value);
    generatorProfile->setImplementationComputeVariablesMethodString(value);

    generatorProfile->setEmptyMethodString(value);

    generatorProfile->setIndentString(value);

    generatorProfile->setOpenArrayInitializerString(value);
    generatorProfile->setCloseArrayInitializerString(value);

    generatorProfile->setOpenArrayString(value);
    generatorProfile->setCloseArrayString(value);

    generatorProfile->setArrayElementSeparatorString(value);

    generatorProfile->setStringDelimiterString(value);

    generatorProfile->setCommandSeparatorString(value);

    EXPECT_EQ(value, generatorProfile->commentString());
    EXPECT_EQ(value, generatorProfile->originCommentString());

    EXPECT_EQ(value, generatorProfile->interfaceHeaderString());
    EXPECT_EQ(value, generatorProfile->implementationHeaderString());

    EXPECT_EQ(value, generatorProfile->interfaceLibcellmlVersionString());
    EXPECT_EQ(value, generatorProfile->implementationLibcellmlVersionString());

    EXPECT_EQ(value, generatorProfile->interfaceStateCountString());
    EXPECT_EQ(value, generatorProfile->implementationStateCountString());

    EXPECT_EQ(value, generatorProfile->interfaceVariableCountString());
    EXPECT_EQ(value, generatorProfile->implementationVariableCountString());

    EXPECT_EQ(value, generatorProfile->variableTypeObjectString());

    EXPECT_EQ(value, generatorProfile->constantVariableTypeString());
    EXPECT_EQ(value, generatorProfile->computedConstantVariableTypeString());
    EXPECT_EQ(value, generatorProfile->algebraicVariableTypeString());

    EXPECT_EQ(value, generatorProfile->variableInfoObjectString());
    EXPECT_EQ(value, generatorProfile->variableInfoWithTypeObjectString());

    EXPECT_EQ(value, generatorProfile->interfaceVoiInfoString());
    EXPECT_EQ(value, generatorProfile->implementationVoiInfoString());

    EXPECT_EQ(value, generatorProfile->interfaceStateInfoString());
    EXPECT_EQ(value, generatorProfile->implementationStateInfoString());

    EXPECT_EQ(value, generatorProfile->interfaceVariableInfoString());
    EXPECT_EQ(value, generatorProfile->implementationVariableInfoString());

    EXPECT_EQ(value, generatorProfile->variableInfoEntryString());
    EXPECT_EQ(value, generatorProfile->variableInfoWithTypeEntryString());

    EXPECT_EQ(value, generatorProfile->voiString());

    EXPECT_EQ(value, generatorProfile->statesArrayString());
    EXPECT_EQ(value, generatorProfile->ratesArrayString());
    EXPECT_EQ(value, generatorProfile->variablesArrayString());

    EXPECT_EQ(value, generatorProfile->returnCreatedArrayString());

    EXPECT_EQ(value, generatorProfile->interfaceCreateStatesArrayMethodString());
    EXPECT_EQ(value, generatorProfile->implementationCreateStatesArrayMethodString());

    EXPECT_EQ(value, generatorProfile->interfaceCreateVariablesArrayMethodString());
    EXPECT_EQ(value, generatorProfile->implementationCreateVariablesArrayMethodString());

    EXPECT_EQ(value, generatorProfile->interfaceDeleteArrayMethodString());
    EXPECT_EQ(value, generatorProfile->implementationDeleteArrayMethodString());

    EXPECT_EQ(value, generatorProfile->interfaceInitializeStatesAndConstantsMethodString());
    EXPECT_EQ(value, generatorProfile->implementationInitializeStatesAndConstantsMethodString());

    EXPECT_EQ(value, generatorProfile->interfaceComputeComputedConstantsMethodString());
    EXPECT_EQ(value, generatorProfile->implementationComputeComputedConstantsMethodString());

    EXPECT_EQ(value, generatorProfile->interfaceComputeRatesMethodString());
    EXPECT_EQ(value, generatorProfile->implementationComputeRatesMethodString());

    EXPECT_EQ(value, generatorProfile->interfaceComputeVariablesMethodString());
    EXPECT_EQ(value, generatorProfile->implementationComputeVariablesMethodString());

    EXPECT_EQ(value, generatorProfile->emptyMethodString());

    EXPECT_EQ(value, generatorProfile->indentString());

    EXPECT_EQ(value, generatorProfile->openArrayInitializerString());
    EXPECT_EQ(value, generatorProfile->closeArrayInitializerString());

    EXPECT_EQ(value, generatorProfile->openArrayString());
    EXPECT_EQ(value, generatorProfile->closeArrayString());

    EXPECT_EQ(value, generatorProfile->arrayElementSeparatorString());

    EXPECT_EQ(value, generatorProfile->stringDelimiterString());

    EXPECT_EQ(value, generatorProfile->commandSeparatorString());
}
