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

#include "test_utils.h"

#include "gtest/gtest.h"

#include <fstream>
#include <iostream>
#include <libcellml>

static const std::string EMPTY_STRING;

TEST(Generator, emptyModel)
{
    libcellml::ModelPtr model = libcellml::Model::create();
    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::UNKNOWN, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, generatorIssues)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr invalidModel = parser->parseModel(fileContents("generator/initialized_variable_of_integration.cellml"));
    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(invalidModel);

    EXPECT_EQ(size_t(1), generator->issueCount());

    libcellml::ModelPtr emptyModel = libcellml::Model::create();

    generator->processModel(emptyModel);

    EXPECT_EQ(size_t(0), generator->issueCount());
}

TEST(Generator, initializedVariableOfIntegration)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/initialized_variable_of_integration.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "Variable 'time' in component 'my_component' of model 'initialized_variable_of_integration' cannot be both a variable of integration and initialised.",
    };
    const std::vector<libcellml::Issue::Cause> expectedCauses = {
        libcellml::Issue::Cause::GENERATOR,
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES_KINDS(expectedIsues, expectedCauses, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::INVALID, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, twoVariablesOfIntegration)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/two_variables_of_integration.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "Variable 'time' in component 'main' of model 'two_variables_of_integration' and variable 'other_time' in component 'sub_sub_sub' of model 'two_variables_of_integration' cannot both be a variable of integration.",
    };
    const std::vector<libcellml::Issue::Cause> expectedCauses = {
        libcellml::Issue::Cause::GENERATOR,
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES_KINDS(expectedIsues, expectedCauses, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::INVALID, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, nonFirstOrderOdes)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/non_first_order_odes.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "The differential equation for variable 'x' in component 'main' of model 'non_first_order_odes' must be of the first order.",
        "The differential equation for variable 'y' in component 'sub' of model 'non_first_order_odes' must be of the first order.",
        "The differential equation for variable 'z' in component 'sub_sub' of model 'non_first_order_odes' must be of the first order.",
    };
    const std::vector<libcellml::Issue::Cause> expectedCauses = {
        libcellml::Issue::Cause::GENERATOR,
        libcellml::Issue::Cause::GENERATOR,
        libcellml::Issue::Cause::GENERATOR,
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES_KINDS(expectedIsues, expectedCauses, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::INVALID, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, undefinedVariables)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/undefined_variables.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "Variable 'a' in component 'my_component' of model 'undefined_variables' is referenced in an equation, but it is not defined anywhere.",
        "Variable 'b' in component 'my_component' of model 'undefined_variables' is referenced in an equation, but it is not defined anywhere.",
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES(expectedIsues, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::INVALID, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, variableInitializedTwice)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/variable_initialized_twice.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "Variable 'x' in component 'sub' of model 'variable_initialized_twice' and variable 'x' in component 'main' of model 'variable_initialized_twice' are equivalent and cannot therefore both be initialised.",
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES(expectedIsues, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::INVALID, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, nonInitializedState)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/non_initialized_state.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "Variable 'x' in component 'my_component' of model 'non_initialized_state' is used in an ODE, but it is not initialised.",
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES(expectedIsues, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::UNDERCONSTRAINED, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, underconstrained)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/underconstrained.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "Variable 'x' in component 'my_component' of model 'my_model' is not computed.",
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES(expectedIsues, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::UNDERCONSTRAINED, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, overconstrained)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/overconstrained.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "Variable 'x' in component 'my_component' of model 'my_model' is computed more than once.",
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES(expectedIsues, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::OVERCONSTRAINED, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, unsuitablyConstrained)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/unsuitably_constrained.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    const std::vector<std::string> expectedIsues = {
        "Variable 'x' in component 'my_component' of model 'my_model' is not computed.",
        "Variable 'y' in component 'my_component' of model 'my_model' is computed more than once.",
    };

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ_ISSUES(expectedIsues, generator);

    EXPECT_EQ(libcellml::Generator::ModelType::UNSUITABLY_CONSTRAINED, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(EMPTY_STRING, generator->implementationCode());
}

TEST(Generator, algebraicEqnComputedVarOnRhs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/algebraic_eqn_computed_var_on_rhs/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ALGEBRAIC, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(2), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/algebraic_eqn_computed_var_on_rhs/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_computed_var_on_rhs/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_computed_var_on_rhs/model.py"), generator->implementationCode());
}

TEST(Generator, algebraicEqnConstVarOnRhs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/algebraic_eqn_const_var_on_rhs/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ALGEBRAIC, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(2), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/algebraic_eqn_const_var_on_rhs/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_const_var_on_rhs/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_const_var_on_rhs/model.py"), generator->implementationCode());
}

TEST(Generator, algebraicEqnConstantOnRhs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/algebraic_eqn_constant_on_rhs/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ALGEBRAIC, generator->modelType());

    EXPECT_EQ(size_t(0), generator->stateCount());
    EXPECT_EQ(size_t(1), generator->variableCount());

    EXPECT_EQ(nullptr, generator->voi());
    EXPECT_EQ(nullptr, generator->state(0));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/algebraic_eqn_constant_on_rhs/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_constant_on_rhs/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_constant_on_rhs/model.py"), generator->implementationCode());
}

TEST(Generator, algebraicEqnDerivativeOnRhs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/algebraic_eqn_derivative_on_rhs/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(2), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/algebraic_eqn_derivative_on_rhs/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_derivative_on_rhs/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_derivative_on_rhs/model.py"), generator->implementationCode());
}

TEST(Generator, algebraicEqnDerivativeOnRhsOneComponent)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/algebraic_eqn_derivative_on_rhs_one_component/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(2), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/algebraic_eqn_derivative_on_rhs_one_component/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_derivative_on_rhs_one_component/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_derivative_on_rhs_one_component/model.py"), generator->implementationCode());
}

TEST(Generator, algebraicEqnStateVarOnRhs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/algebraic_eqn_state_var_on_rhs/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(2), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/algebraic_eqn_state_var_on_rhs/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_state_var_on_rhs/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_state_var_on_rhs/model.py"), generator->implementationCode());
}

TEST(Generator, algebraicEqnStateVarOnRhsOneComponent)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/algebraic_eqn_state_var_on_rhs_one_component/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(2), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/algebraic_eqn_state_var_on_rhs_one_component/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_state_var_on_rhs_one_component/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/algebraic_eqn_state_var_on_rhs_one_component/model.py"), generator->implementationCode());
}

TEST(Generator, odeComputedVarOnRhs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_computed_var_on_rhs/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(1), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/ode_computed_var_on_rhs/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_computed_var_on_rhs/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_computed_var_on_rhs/model.py"), generator->implementationCode());
}

TEST(Generator, odeComputedVarOnRhsOneComponent)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_computed_var_on_rhs_one_component/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(1), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/ode_computed_var_on_rhs_one_component/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_computed_var_on_rhs_one_component/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_computed_var_on_rhs_one_component/model.py"), generator->implementationCode());
}

TEST(Generator, odeConstVarOnRhs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_const_var_on_rhs/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(1), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/ode_const_var_on_rhs/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_const_var_on_rhs/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_const_var_on_rhs/model.py"), generator->implementationCode());
}

TEST(Generator, odeConstVarOnRhsOneComponent)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_const_var_on_rhs_one_component/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(1), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/ode_const_var_on_rhs_one_component/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_const_var_on_rhs_one_component/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_const_var_on_rhs_one_component/model.py"), generator->implementationCode());
}

TEST(Generator, odeConstantOnRhs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_constant_on_rhs/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(fileContents("generator/ode_constant_on_rhs/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_constant_on_rhs/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_constant_on_rhs/model.py"), generator->implementationCode());
}

TEST(Generator, odeConstantOnRhsOneComponent)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_constant_on_rhs_one_component/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(0), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_EQ(nullptr, generator->variable(0));

    EXPECT_EQ(fileContents("generator/ode_constant_on_rhs_one_component/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_constant_on_rhs_one_component/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_constant_on_rhs_one_component/model.py"), generator->implementationCode());
}

TEST(Generator, odeMultipleDependentOdes)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_multiple_dependent_odes/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(2), generator->stateCount());
    EXPECT_EQ(size_t(1), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/ode_multiple_dependent_odes/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_multiple_dependent_odes/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_multiple_dependent_odes/model.py"), generator->implementationCode());
}

TEST(Generator, odeMultipleDependentOdesOneComponent)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_multiple_dependent_odes_one_component/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(2), generator->stateCount());
    EXPECT_EQ(size_t(1), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/ode_multiple_dependent_odes_one_component/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_multiple_dependent_odes_one_component/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_multiple_dependent_odes_one_component/model.py"), generator->implementationCode());
}

TEST(Generator, odeMultipleOdesWithSameName)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/ode_multiple_odes_with_same_name/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(2), generator->stateCount());
    EXPECT_EQ(size_t(1), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/ode_multiple_odes_with_same_name/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_multiple_odes_with_same_name/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/ode_multiple_odes_with_same_name/model.py"), generator->implementationCode());
}

TEST(Generator, cellmlMappingsAndEncapsulations)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/cellml_mappings_and_encapsulations/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(2), generator->stateCount());
    EXPECT_EQ(size_t(2), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/cellml_mappings_and_encapsulations/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/cellml_mappings_and_encapsulations/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/cellml_mappings_and_encapsulations/model.py"), generator->implementationCode());
}

TEST(Generator, dependentEqns)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/dependent_eqns/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(2), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/dependent_eqns/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/dependent_eqns/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/dependent_eqns/model.py"), generator->implementationCode());
}

TEST(Generator, fabbriFantiniWildersSeveriHumanSanModel2017)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/fabbri_fantini_wilders_severi_human_san_model_2017/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(33), generator->stateCount());
    EXPECT_EQ(size_t(217), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/fabbri_fantini_wilders_severi_human_san_model_2017/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/fabbri_fantini_wilders_severi_human_san_model_2017/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/fabbri_fantini_wilders_severi_human_san_model_2017/model.py"), generator->implementationCode());
}

TEST(Generator, garnyKohlHunterBoyettNobleRabbitSanModel2003)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/garny_kohl_hunter_boyett_noble_rabbit_san_model_2003/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(15), generator->stateCount());
    EXPECT_EQ(size_t(185), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/garny_kohl_hunter_boyett_noble_rabbit_san_model_2003/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/garny_kohl_hunter_boyett_noble_rabbit_san_model_2003/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/garny_kohl_hunter_boyett_noble_rabbit_san_model_2003/model.py"), generator->implementationCode());
}

TEST(Generator, hodgkinHuxleySquidAxonModel1952)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/hodgkin_huxley_squid_axon_model_1952/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(4), generator->stateCount());
    EXPECT_EQ(size_t(18), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/hodgkin_huxley_squid_axon_model_1952/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/hodgkin_huxley_squid_axon_model_1952/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/hodgkin_huxley_squid_axon_model_1952/model.py"), generator->implementationCode());
}

TEST(Generator, nobleModel1962)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/noble_model_1962/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(4), generator->stateCount());
    EXPECT_EQ(size_t(17), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/noble_model_1962/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/noble_model_1962/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/noble_model_1962/model.py"), generator->implementationCode());
}

TEST(Generator, sineImports)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("sine_approximations_import.xml"));

    EXPECT_EQ(size_t(0), parser->issueCount());
    EXPECT_TRUE(model->hasUnresolvedImports());

    model->resolveImports(resourcePath());

    EXPECT_FALSE(model->hasUnresolvedImports());

    model->flatten();

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(10), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/sine_model_imports/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/sine_model_imports/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);

    generator->setProfile(profile);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/sine_model_imports/model.py"), generator->implementationCode());
}

TEST(Generator, coverage)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("generator/coverage/model.cellml"));

    EXPECT_EQ(size_t(0), parser->issueCount());

    libcellml::GeneratorPtr generator = libcellml::Generator::create();

    generator->processModel(model);

    EXPECT_EQ(size_t(0), generator->issueCount());

    EXPECT_EQ(libcellml::Generator::ModelType::ODE, generator->modelType());

    EXPECT_EQ(size_t(1), generator->stateCount());
    EXPECT_EQ(size_t(186), generator->variableCount());

    EXPECT_NE(nullptr, generator->voi());
    EXPECT_NE(nullptr, generator->state(0));
    EXPECT_EQ(nullptr, generator->state(generator->stateCount()));
    EXPECT_NE(nullptr, generator->variable(0));
    EXPECT_EQ(nullptr, generator->variable(generator->variableCount()));

    EXPECT_EQ(fileContents("generator/coverage/model.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/coverage/model.c"), generator->implementationCode());

    libcellml::GeneratorProfilePtr profile = generator->profile();

    profile->setInterfaceCreateStatesArrayMethodString("double * createStatesVector();\n");
    profile->setImplementationCreateStatesArrayMethodString("double * createStatesVector()\n"
                                                            "{\n"
                                                            "    return (double *) malloc(STATE_COUNT*sizeof(double));\n"
                                                            "}\n");

    EXPECT_EQ(fileContents("generator/coverage/model.modified.profile.h"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/coverage/model.modified.profile.c"), generator->implementationCode());

    profile = libcellml::GeneratorProfile::create();

    generator->setProfile(profile);

    profile->setHasInterface(false);

    profile->setXorString("^");
    profile->setHasXorOperator(true);

    profile->setPowerString("^^");
    profile->setSquareString("sqr");
    profile->setHasPowerOperator(true);

    profile->setPiecewiseIfString("piecewise(<CONDITION>, <IF_STATEMENT>");
    profile->setPiecewiseElseString(", <ELSE_STATEMENT>)");
    profile->setHasConditionalOperator(false);

    profile->setInterfaceFileNameString("customheaderfile.h");

    profile->setImplementationHeaderString("#include \"<INTERFACE_FILE_NAME>\"\n");

    profile->setImplementationVersionString("");

    profile->setImplementationLibcellmlVersionString("");

    profile->setImplementationStateCountString("");

    profile->setImplementationVariableCountString("");

    profile->setVariableTypeObjectString("");

    profile->setConstantVariableTypeString("");
    profile->setComputedConstantVariableTypeString("");
    profile->setAlgebraicVariableTypeString("");

    profile->setVariableInfoObjectString("");
    profile->setVariableInfoWithTypeObjectString("");

    profile->setImplementationVoiInfoString("");

    profile->setImplementationStateInfoString("");

    profile->setImplementationVariableInfoString("");

    profile->setVariableInfoEntryString("");
    profile->setVariableInfoWithTypeEntryString("");

    profile->setImplementationCreateStatesArrayMethodString("");

    profile->setImplementationCreateVariablesArrayMethodString("");

    profile->setImplementationDeleteArrayMethodString("");

    profile->setImplementationComputeVariablesMethodString("// We should have computeVariables() here, but we replaced it with this comment\n"
                                                           "// and no code template that can be replaced so that our replace() method can\n"
                                                           "// is forced to return an empty string, ensuring 100% coverage using llvm-cov...\n");

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/coverage/model.out"), generator->implementationCode());

    profile = libcellml::GeneratorProfile::create();

    generator->setProfile(profile);

    profile->setHasInterface(true);

    profile->setEqString("eq");
    profile->setNeqString("neq");
    profile->setLtString("lt");
    profile->setLeqString("leq");
    profile->setGtString("gt");
    profile->setGeqString("geq");
    profile->setAndString("and");
    profile->setOrString("or");
    profile->setNotString("not");

    profile->setHasEqOperator(false);
    profile->setHasNeqOperator(false);
    profile->setHasLtOperator(false);
    profile->setHasLeqOperator(false);
    profile->setHasGtOperator(false);
    profile->setHasGeqOperator(false);
    profile->setHasAndOperator(false);
    profile->setHasOrOperator(false);
    profile->setHasNotOperator(false);

    profile->setImplementationHeaderString("");

    profile->setInterfaceVersionString("");
    profile->setImplementationVersionString("");

    profile->setInterfaceLibcellmlVersionString("");
    profile->setImplementationLibcellmlVersionString("");

    profile->setInterfaceStateCountString("");
    profile->setImplementationStateCountString("");

    profile->setInterfaceVariableCountString("");
    profile->setImplementationVariableCountString("");

    profile->setVariableTypeObjectString("");

    profile->setConstantVariableTypeString("");
    profile->setComputedConstantVariableTypeString("");
    profile->setAlgebraicVariableTypeString("");

    profile->setVariableInfoObjectString("");
    profile->setVariableInfoWithTypeObjectString("");

    profile->setInterfaceVoiInfoString("");
    profile->setImplementationVoiInfoString("");

    profile->setInterfaceStateInfoString("");
    profile->setImplementationStateInfoString("");

    profile->setInterfaceVariableInfoString("");
    profile->setImplementationVariableInfoString("");

    profile->setVariableInfoEntryString("");
    profile->setVariableInfoWithTypeEntryString("");

    EXPECT_EQ(fileContents("generator/coverage/model.interface.out"), generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/coverage/model.implementation.out"), generator->implementationCode());

    profile->setProfile(libcellml::GeneratorProfile::Profile::PYTHON);

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/coverage/model.py"), generator->implementationCode());

    profile->setImplementationCreateStatesArrayMethodString("\n"
                                                            "def create_states_vector():\n"
                                                            "    return [nan]*STATE_COUNT\n");

    EXPECT_EQ(EMPTY_STRING, generator->interfaceCode());
    EXPECT_EQ(fileContents("generator/coverage/model.modified.profile.py"), generator->implementationCode());
}

TEST(Generator, KRM_test)
{
    //  0.a Create a new model instance representing the combined model and name it.
    auto model = libcellml::Model::create("Tutorial8_HHModel");
    auto validator = libcellml::Validator::create();

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 1: Import the membrane component          " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  In order to use the import functionality we need to know three things:
    //      - the import destination (the component/unit to which the imported item will be assigned)
    //      - the file we're importing it from (the url to the model containing the item to be imported)
    //      - which item within the file should be imported (the name of the component/units inside the import model file)
    //  We'll address these now.

    //  1.a As previously, create a component to represent the membrane in the HH model,
    //      and add it to the model.  This deals with the first point above: the import destination
    auto membrane = libcellml::Component::create("membrane");
    model->addComponent(membrane);

    //  1.b Next we need to create an ImportSource item and use its setUrl() function to specify the
    //      name (without the path) of the file contianing the model to be imported.
    auto membraneImporter = libcellml::ImportSource::create();
    membraneImporter->setUrl("tutorial8_MembraneModel.cellml");

    //  1.c Thirdly we need to link our import source to the import destination using the
    //      Component::setImportSource() function on the membrane component, and to use the
    //      Component::setImportReference() function to specify the name of the component inside
    //      the import model to retrieve.f
    membrane->setImportSource(membraneImporter);
    membrane->setImportReference("membrane_for_importing");

    //  1.d At this stage, our model has only one component in it, and even though we've specified the
    //      imports completely, we still need to resolve the imports and flatten the model before the
    //      imported items will be instantiated here.
    //      Use the Model::hasUnresolvedImports() function to show that the imports have not been resolved yet.
    if (model->hasUnresolvedImports()) {
        std::cout << "Imports are UNRESOLVED" << std::endl;
    } else {
        std::cout << "Imports are found" << std::endl;
    }

    //  1.e Print the model to the terminal to show that it contains only one empty component at this stage.
    // printModelToTerminal(model, false);

    //  1.f Use the Model::resolveImports() function of the model to (erm) resolve the imports.  This takes an
    //      argument of a string representing the full absolute path to the directory in which the import
    //      file specified in 1.b is stored, and must end with a slash.
    model->resolveImports(resourcePath("generator/krm/"));
    std::cout << resourcePath("generator/krm/") << std::endl;

    //  1.g Call the Model::hasUnreolvedImports() function again and verify that they are now resolved.
    EXPECT_FALSE(model->hasUnresolvedImports());

    //  1.h Call the Model::flatten() function.  This will recursively search through all of the imported items
    //      in the model, and create local instances of them here.
    //      Note that:
    //          - if you call the flatten() function without first resolving the imports, nothing will change.
    //          - flattening a model fundamentally changes it ... and cannot be undone.
    model->flatten();
    membrane = model->component("membrane");

    //  1.i Print the flattened model to the terminal and verify that it now contains:
    //      - 4 units
    //      - 1 component with
    //          - 8 variables
    //          - a mathml block
    //  from the imported file.
    //  printModelToTerminal(model, false);

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 2: Import the sodium channel component    " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  2.a Create a component representing the sodium channel.  This will be encapsulated inside the membrane
    //      component, so add it there instead of adding it to the model.
    auto sodiumChannel = libcellml::Component::create("sodium_channel");
    membrane->addComponent(sodiumChannel);

    //  2.b Create an importer for the sodium channel, and point it to the file you created in Tutorial 7.
    //      Note that you will need to make sure it exists in the same path as the earlier files.
    //      If you did not complete Tutorial 7 you can use the tutorial7_SodiumChannelModel.cellml
    //      file in the resources folder.
    auto sodiumImporter = libcellml::ImportSource::create();
    sodiumImporter->setUrl("tutorial7_SodiumChannelModel.cellml");

    //  2.c Link the sodium channel component to the importer and set the reference to import.
    //      In the file provided this is named "sodiumChannel".  You will need to change this to
    //      whatever you named the component in Tutorial 7.
    sodiumChannel->setImportSource(sodiumImporter);
    sodiumChannel->setImportReference("sodiumChannel");

    //  2.d Create dummy variables for voltage, time, current:
    {
        auto V = libcellml::Variable::create("V");
        sodiumChannel->addVariable(V);
        auto t = libcellml::Variable::create("t");
        sodiumChannel->addVariable(t);
        auto i_Na = libcellml::Variable::create("i_Na");
        sodiumChannel->addVariable(i_Na);
    }
    // Create dummy variables for m and h gate status so that they can be initialised:
    {
        auto h = libcellml::Variable::create("h");
        sodiumChannel->addVariable(h);
        auto m = libcellml::Variable::create("m");
        sodiumChannel->addVariable(m);

        auto g_Na = libcellml::Variable::create("g_Na");
        sodiumChannel->addVariable(g_Na);
        auto E_Na = libcellml::Variable::create("E_Na");
        sodiumChannel->addVariable(E_Na);
    }

    // TODO Remove when https://github.com/cellml/libcellml/issues/594 is resolved.
    {
        sodiumChannel->variable("V")->setUnits("mV");
        sodiumChannel->variable("t")->setUnits("ms");
        sodiumChannel->variable("i_Na")->setUnits("mA");
        sodiumChannel->variable("m")->setUnits("dimensionless");
        sodiumChannel->variable("h")->setUnits("dimensionless");

        sodiumChannel->variable("V")->setInterfaceType("public_and_private");
        sodiumChannel->variable("t")->setInterfaceType("public_and_private");
        sodiumChannel->variable("i_Na")->setInterfaceType("public_and_private");
        sodiumChannel->variable("m")->setInterfaceType("public_and_private");
        sodiumChannel->variable("h")->setInterfaceType("public_and_private");
    }

    model->resolveImports(resourcePath("generator/krm/"));
    EXPECT_FALSE(model->hasUnresolvedImports());

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 3: Import the potassium channel component " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  3.a Repeat all the tasks in Step 2, this time for the potassium channel model you created in
    //      Tutorial 6.  If you did not complete Tutorial 6 you can use the tutorial6_PotassiumChannelModel.cellml
    //      from the resources folder, importing the component called "potassiumChannel"
    auto potassiumChannel = libcellml::Component::create("potassium_channel");
    membrane->addComponent(potassiumChannel);

    auto potassiumImporter = libcellml::ImportSource::create();
    potassiumImporter->setUrl("tutorial6_PotassiumChannelModel.cellml");

    potassiumChannel->setImportSource(potassiumImporter);
    potassiumChannel->setImportReference("potassiumChannel");

    model->resolveImports(resourcePath("generator/krm/"));
    EXPECT_FALSE(model->hasUnresolvedImports());

    {
        auto V = libcellml::Variable::create("V");
        potassiumChannel->addVariable(V);
        auto t = libcellml::Variable::create("t");
        potassiumChannel->addVariable(t);
        auto i_K = libcellml::Variable::create("i_K");
        potassiumChannel->addVariable(i_K);

        auto n = libcellml::Variable::create("n");
        potassiumChannel->addVariable(n);
        auto g_K = libcellml::Variable::create("g_K");
        potassiumChannel->addVariable(g_K);
        auto E_K = libcellml::Variable::create("E_K");
        potassiumChannel->addVariable(E_K);
    }

    // TODO Remove when https://github.com/cellml/libcellml/issues/594 is resolved.
    {
        potassiumChannel->variable("V")->setUnits("mV");
        potassiumChannel->variable("t")->setUnits("ms");
        potassiumChannel->variable("i_K")->setUnits("mA");
        potassiumChannel->variable("n")->setUnits("dimensionless");

        potassiumChannel->variable("V")->setInterfaceType("public_and_private");
        potassiumChannel->variable("t")->setInterfaceType("public_and_private");
        potassiumChannel->variable("i_K")->setInterfaceType("public_and_private");

        potassiumChannel->variable("n")->setInterfaceType("public_and_private");
        potassiumChannel->variable("g_K")->setInterfaceType("public_and_private");
        potassiumChannel->variable("E_K")->setInterfaceType("public_and_private");
    }

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 4: Import the leakage component           " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  4.a Repeat all the tasks in Step 2, this time for the leakageCurrent component in
    //      the model supplied inside resources/tutorial8_LeakageModel.cellml.
    auto leakage = libcellml::Component::create("leakage");
    membrane->addComponent(leakage);

    auto leakageImporter = libcellml::ImportSource::create();
    leakageImporter->setUrl("tutorial8_LeakageCurrentModel.cellml");

    leakage->setImportSource(leakageImporter);
    leakage->setImportReference("leakageCurrent");

    {
        auto V = libcellml::Variable::create("V");
        leakage->addVariable(V);
        auto t = libcellml::Variable::create("t");
        leakage->addVariable(t);
        auto i_L = libcellml::Variable::create("i_L");
        leakage->addVariable(i_L);
    }

    // TODO Remove when https://github.com/cellml/libcellml/issues/594 is resolved.
    {
        leakage->variable("V")->setUnits("mV");
        leakage->variable("t")->setUnits("ms");
        leakage->variable("i_L")->setUnits("mA");

        leakage->variable("V")->setInterfaceType("public_and_private");
        leakage->variable("t")->setInterfaceType("public_and_private");
        leakage->variable("i_L")->setInterfaceType("public_and_private");
    }

    model->resolveImports(resourcePath("generator/krm/"));
    EXPECT_FALSE(model->hasUnresolvedImports());

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 5: Import parameters                      " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  5.a Repeat all the tasks in Step 2, this time for the
    //      parameters components in the model supplied inside
    //      resources/tutorial8_controller.cellml.
    auto parameters = libcellml::Component::create("parameters");
    model->addComponent(parameters);

    auto parametersImporter = libcellml::ImportSource::create();
    parametersImporter->setUrl("tutorial8_controller.cellml");

    parameters->setImportSource(parametersImporter);
    parameters->setImportReference("parameters");

    //  5.b Set up a dummy variable for voltage so that the parameters component
    //      can initialise the ODE in the membrane component.
    {
        auto V = libcellml::Variable::create("V");
        parameters->addVariable(V);
        auto Cm = libcellml::Variable::create("Cm");
        parameters->addVariable(Cm);
        auto h = libcellml::Variable::create("h");
        parameters->addVariable(h);
        auto m = libcellml::Variable::create("m");
        parameters->addVariable(m);
        auto n = libcellml::Variable::create("n");
        parameters->addVariable(n);
        auto E_K = libcellml::Variable::create("E_K");
        parameters->addVariable(E_K);
        auto g_K = libcellml::Variable::create("g_K");
        parameters->addVariable(g_K);
        auto E_Na = libcellml::Variable::create("E_Na");
        parameters->addVariable(E_Na);
        auto g_Na = libcellml::Variable::create("g_Na");
        parameters->addVariable(g_Na);
    }

    // TODO Remove when https://github.com/cellml/libcellml/issues/594 is resolved.
    {
        parameters->variable("V")->setUnits("mV");
        parameters->variable("V")->setInterfaceType("public");
        parameters->variable("Cm")->setUnits("microF_per_cm2");
        parameters->variable("Cm")->setInterfaceType("public_and_private");

        parameters->variable("h")->setInterfaceType("public_and_private");
        parameters->variable("m")->setInterfaceType("public_and_private");
        parameters->variable("n")->setInterfaceType("public_and_private");
        parameters->variable("g_Na")->setInterfaceType("public_and_private");
        parameters->variable("g_K")->setInterfaceType("public_and_private");
        parameters->variable("E_Na")->setInterfaceType("public_and_private");
        parameters->variable("E_K")->setInterfaceType("public_and_private");
    }

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 6: Connect variables between components   " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  6.a Now that we've got all the imports done, we need to connect the imported
    //      components and their dummy variables together.  The variables to connect are:
    //          - voltage:  parameters -> membrane -> sodium channel, potassium channel, leakage
    //          - time: membrane -> sodium channel, potassium channel
    //          - current variables (i_Na, i_K, i_L): membrane -> channels
    //          - Cm: parameters -> membrane

    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("V"), membrane->variable("V")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("Cm"), membrane->variable("Cm")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("h"), membrane->variable("h")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("m"), membrane->variable("m")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("n"), membrane->variable("n")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("g_K"), membrane->variable("g_K")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("g_Na"), membrane->variable("g_Na")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("E_K"), membrane->variable("E_K")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(parameters->variable("E_Na"), membrane->variable("E_Na")));

    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("V"), sodiumChannel->variable("V")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("t"), sodiumChannel->variable("t")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("i_Na"), sodiumChannel->variable("i_Na")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("E_Na"), sodiumChannel->variable("E_Na")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("g_Na"), sodiumChannel->variable("g_Na")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("h"), sodiumChannel->variable("h")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("m"), sodiumChannel->variable("m")));

    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("V"), potassiumChannel->variable("V")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("t"), potassiumChannel->variable("t")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("i_K"), potassiumChannel->variable("i_K")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("E_K"), potassiumChannel->variable("E_K")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("g_K"), potassiumChannel->variable("g_K")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("n"), potassiumChannel->variable("n")));

    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("V"), leakage->variable("V")));
    EXPECT_TRUE(libcellml::Variable::addEquivalence(membrane->variable("i_L"), leakage->variable("i_L")));

    //  6.b Serialise and write the model to a CellML file.  In the steps below the model will
    //      be flattened for code generation, but it's good to keep an unflattened copy too.
    auto printer = libcellml::Printer::create();
    std::ofstream outFile(resourcePath("generator/krm/tutorial8_HodgkinHuxleyModel.cellml"));
    outFile << printer->printModel(model);
    outFile.close();

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 7: Resolve imports and flatten the model  " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  7.a Resolve the model's imports to the folder where all of the files are located, and
    //      check that there are no unresolved imports outstanding.
    model->resolveImports(resourcePath("generator/krm/"));
    EXPECT_TRUE(model->hasUnresolvedImports() == false);

    //  7.b Flatten the model, and print the flattened model structure to the terminal for checking.
    model->flatten();
    // printEncapsulationStructureToTerminal(model);
    // printModelToTerminal(model, false);

    //  7.c Validate the flattened model, expecting that there are no errors.
    validator->validateModel(model);
    // printErrorsToTerminal(validator);
    EXPECT_TRUE(validator->errorCount() == 0);

    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "  STEP 8: Generate the model and output  " << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    //  8.a Create a Generator instance and submit the model for processing.
    //      Expect that there are no errors logged in the generator afterwards.
    auto generator = libcellml::Generator::create();
    generator->processModel(model);
    // printErrorsToTerminal(generator);

    EXPECT_TRUE(generator->errorCount() == 0);

    //  8.b Retrieve and write the interface code (*.h) and implementation code (*.c) to files.
    outFile.open(resourcePath("generator/krm/tutorial8_HodgkinHuxleyModel.h"));
    outFile << generator->interfaceCode();
    outFile.close();

    outFile.open(resourcePath("generator/krm/tutorial8_HodgkinHuxleyModel.c"));
    outFile << generator->implementationCode();
    outFile.close();

    //  8.c Change the generator profile to Python and reprocess the model.
    auto profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);
    generator->setProfile(profile);
    generator->processModel(model);

    //  8.d Retrieve and write the implementation code (*.py) to a file.
    outFile.open(resourcePath("generator/krm/tutorial8_HodgkinHuxleyModel.py"));
    outFile << generator->implementationCode();
    outFile.close();

    std::cout << "The model has been output into tutorial8_HodgkinHuxleyModel.[c,h,py,cellml]"
              << std::endl;
}
