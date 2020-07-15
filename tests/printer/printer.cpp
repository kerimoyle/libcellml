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

#include <libcellml>

#include "test_utils.h"

TEST(Printer, printNullptrModel)
{
    const std::string e;

    libcellml::PrinterPtr p = libcellml::Printer::create();

    const std::string a = p->printModel(nullptr);

    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyModel)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\"/>\n";
    libcellml::ModelPtr m = libcellml::Model::create();

    libcellml::PrinterPtr p = libcellml::Printer::create();

    const std::string a = p->printModel(m);

    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyUnits)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\">\n"
        "  <units/>\n"
        "</model>\n";
    libcellml::ModelPtr m = libcellml::Model::create();
    libcellml::UnitsPtr u = libcellml::Units::create();

    m->addUnits(u);

    libcellml::PrinterPtr printer = libcellml::Printer::create();
    const std::string a = printer->printModel(m);
    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyVariable)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\">\n"
        "  <component>\n"
        "    <variable/>\n"
        "  </component>\n"
        "</model>\n";
    libcellml::ModelPtr m = createModelWithComponent();
    libcellml::ComponentPtr c = m->component(0);
    libcellml::VariablePtr v = libcellml::Variable::create();
    c->addVariable(v);

    libcellml::PrinterPtr printer = libcellml::Printer::create();
    const std::string a = printer->printModel(m);
    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyComponent)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\">\n"
        "  <component/>\n"
        "</model>\n";

    libcellml::ModelPtr m = createModelWithComponent();

    libcellml::PrinterPtr printer = libcellml::Printer::create();
    const std::string a = printer->printModel(m);
    EXPECT_EQ(e, a);
}

TEST(Printer, printEmptyReset)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\">\n"
        "  <component>\n"
        "    <reset/>\n"
        "  </component>\n"
        "</model>\n";

    libcellml::ModelPtr m = createModelWithComponent();
    libcellml::ComponentPtr c = m->component(0);
    libcellml::ResetPtr r = libcellml::Reset::create();

    c->addReset(r);

    libcellml::PrinterPtr printer = libcellml::Printer::create();
    const std::string a = printer->printModel(m);
    EXPECT_EQ(e, a);
}

TEST(Printer, printReset)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\" name=\"model\">\n"
        "  <component name=\"component\">\n"
        "    <variable name=\"variable1\"/>\n"
        "    <variable name=\"variable2\"/>\n"
        "    <reset variable=\"variable1\" test_variable=\"variable2\" order=\"1\">\n"
        "      <test_value>\n"
        "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\"/>\n"
        "      </test_value>\n"
        "      <reset_value>\n"
        "        <math xmlns=\"http://www.w3.org/1998/Math/MathML\"/>\n"
        "      </reset_value>\n"
        "    </reset>\n"
        "  </component>\n"
        "</model>\n";

    libcellml::ModelPtr m = createModelWithComponent("model", "component");
    libcellml::ComponentPtr c = m->component(0);
    libcellml::VariablePtr v1 = libcellml::Variable::create("variable1");
    libcellml::VariablePtr v2 = libcellml::Variable::create("variable2");
    libcellml::ResetPtr r = libcellml::Reset::create();

    r->setVariable(v1);
    r->setTestVariable(v2);
    r->setOrder(1);
    r->setResetValue(EMPTY_MATH);
    r->setTestValue(EMPTY_MATH);

    c->addVariable(v1);
    c->addVariable(v2);
    c->addReset(r);

    libcellml::PrinterPtr printer = libcellml::Printer::create();

    const std::string a = printer->printModel(m);
    EXPECT_EQ(e, a);
}

TEST(Printer, printEncapsulation)
{
    const std::string e_parent =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\">\n"
        "  <component/>\n"
        "  <component/>\n"
        "  <encapsulation>\n"
        "    <component_ref>\n"
        "      <component_ref/>\n"
        "    </component_ref>\n"
        "  </encapsulation>\n"
        "</model>\n";

    libcellml::ModelPtr model = libcellml::Model::create();
    libcellml::ComponentPtr parent = libcellml::Component::create();
    libcellml::ComponentPtr child = libcellml::Component::create();
    parent->addComponent(child);

    model->addComponent(parent);

    libcellml::PrinterPtr printer = libcellml::Printer::create();
    const std::string a_parent = printer->printModel(model);
    EXPECT_EQ(e_parent, a_parent);
}

TEST(Printer, printEncapsulationWithNames)
{
    const std::string e_parent =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\">\n"
        "  <component name=\"parent_component\"/>\n"
        "  <component name=\"child_component\"/>\n"
        "  <encapsulation>\n"
        "    <component_ref component=\"parent_component\">\n"
        "      <component_ref component=\"child_component\"/>\n"
        "    </component_ref>\n"
        "  </encapsulation>\n"
        "</model>\n";

    libcellml::ModelPtr model = libcellml::Model::create();
    libcellml::ComponentPtr parent = libcellml::Component::create();
    parent->setName("parent_component");
    libcellml::ComponentPtr child = libcellml::Component::create();
    child->setName("child_component");
    parent->addComponent(child);

    model->addComponent(parent);

    libcellml::PrinterPtr printer = libcellml::Printer::create();
    const std::string a_parent = printer->printModel(model);
    EXPECT_EQ(e_parent, a_parent);
}

TEST(Printer, printModelWithImports)
{
    const std::string e_model =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\" name=\"sin_approximations_import\" id=\"sin_approximations_import\">\n"
        "  <import xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"sin.xml\">\n"
        "    <component component_ref=\"sin\" name=\"actual_sin\"/>\n"
        "  </import>\n"
        "  <import xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"deriv_approx_sin.xml\">\n"
        "    <component component_ref=\"sin\" name=\"deriv_approx_sin\"/>\n"
        "  </import>\n"
        "  <import xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"parabolic_approx_sin.xml\">\n"
        "    <component component_ref=\"sin\" name=\"parabolic_approx_sin\"/>\n"
        "  </import>\n"
        "  <component name=\"main\" id=\"main\">\n"
        "    <variable name=\"x\" units=\"dimensionless\" interface=\"public_and_private\" id=\"x\"/>\n"
        "    <variable name=\"sin1\" units=\"dimensionless\" interface=\"public_and_private\" id=\"sin\"/>\n"
        "    <variable name=\"sin2\" units=\"dimensionless\" interface=\"public_and_private\" id=\"deriv_approx\"/>\n"
        "    <variable name=\"deriv_approx_initial_value\" units=\"dimensionless\" initial_value=\"0\" interface=\"public_and_private\" id=\"deriv_approx_initial_value\"/>\n"
        "    <variable name=\"sin3\" units=\"dimensionless\" interface=\"public_and_private\" id=\"parabolic_approx\"/>\n"
        "    <variable name=\"C\" units=\"dimensionless\" initial_value=\"0.75\" interface=\"public_and_private\"/>\n"
        "  </component>\n"
        "  <connection component_1=\"main\" component_2=\"actual_sin\">\n"
        "    <map_variables variable_1=\"x\" variable_2=\"x\"/>\n"
        "    <map_variables variable_1=\"sin1\" variable_2=\"sin\"/>\n"
        "  </connection>\n"
        "  <connection component_1=\"main\" component_2=\"deriv_approx_sin\">\n"
        "    <map_variables variable_1=\"x\" variable_2=\"x\"/>\n"
        "    <map_variables variable_1=\"sin2\" variable_2=\"sin\"/>\n"
        "    <map_variables variable_1=\"deriv_approx_initial_value\" variable_2=\"sin_initial_value\"/>\n"
        "  </connection>\n"
        "  <connection component_1=\"main\" component_2=\"parabolic_approx_sin\">\n"
        "    <map_variables variable_1=\"x\" variable_2=\"x\"/>\n"
        "    <map_variables variable_1=\"sin3\" variable_2=\"sin\"/>\n"
        "    <map_variables variable_1=\"C\" variable_2=\"C\"/>\n"
        "  </connection>\n"
        "  <encapsulation>\n"
        "    <component_ref component=\"main\">\n"
        "      <component_ref component=\"actual_sin\"/>\n"
        "      <component_ref component=\"deriv_approx_sin\"/>\n"
        "      <component_ref component=\"parabolic_approx_sin\"/>\n"
        "    </component_ref>\n"
        "  </encapsulation>\n"
        "</model>\n";

    libcellml::ParserPtr p = libcellml::Parser::create();
    libcellml::ModelPtr model = p->parseModel(fileContents("sine_approximations_import.xml"));

    EXPECT_EQ(size_t(0), p->issueCount());
    EXPECT_TRUE(model->hasUnresolvedImports());

    libcellml::PrinterPtr printer = libcellml::Printer::create();
    const std::string a_model = printer->printModel(model);
    EXPECT_EQ(e_model, a_model);
}

TEST(Printer, printModelWithTabs)
{
    libcellml::ParserPtr parser = libcellml::Parser::create();
    libcellml::ModelPtr model = parser->parseModel(fileContents("printer/tabulated_model.cellml"));
    libcellml::PrinterPtr printer = libcellml::Printer::create();

    EXPECT_EQ(fileContents("printer/spaced_model.cellml"), printer->printModel(model));
}

TEST(Printer, printModelWithStandardUnitsAdded)
{
    const std::string e =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<model xmlns=\"http://www.cellml.org/cellml/2.0#\" name=\"my_model\"/>\n";

    libcellml::ModelPtr model = libcellml::Model::create("my_model");
    libcellml::UnitsPtr u = libcellml::Units::create("second");

    model->addUnits(u);

    EXPECT_EQ(size_t(1), model->unitsCount());

    libcellml::PrinterPtr printer = libcellml::Printer::create();
    EXPECT_EQ(e, printer->printModel(model));
}

TEST(Printer, printMultipleChildrenOfImports)
{
    std::string in = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                     "<model xmlns=\"http://www.cellml.org/cellml/2.0#\" name=\"everything\" id=\"model_1\">\n"
                     "  <import xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"some-other-model.xml\" id=\"import_1\">\n"
                     "    <component component_ref=\"a_component_in_that_model\" name=\"component1\" id=\"component_1\"/>\n"
                     "    <component component_ref=\"another_component_in_that_model\" name=\"component2\" id=\"component_2\"/>\n"
                     "    <units units_ref=\"a_units_in_that_model\" name=\"units1\" id=\"units_1\"/>\n"
                     "    <units units_ref=\"another_units_in_that_model\" name=\"units2\" id=\"units_2\"/>\n"
                     "    <units units_ref=\"yet_another_units_in_that_model\" name=\"units3\" id=\"units_3\"/>\n"
                     "  </import>\n"
                     "</model>\n";

    auto parser = libcellml::Parser::create();
    auto model = parser->parseModel(in);
    auto printer = libcellml::Printer::create();
    EXPECT_EQ(in, printer->printModel(model));
}

TEST(Printer, printMultipleChildrenOfImportsMixedOrder)
{
    std::string in = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                     "<model xmlns=\"http://www.cellml.org/cellml/2.0#\" name=\"everything\" id=\"model_1\">\n"
                     "  <import xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"some-other-model.xml\" id=\"import_1\">\n"
                     "    <component component_ref=\"a_component_in_that_model\" name=\"component1\" id=\"component_1\"/>\n"
                     "    <units units_ref=\"a_units_in_that_model\" name=\"units1\" id=\"units_1\"/>\n"
                     "    <units units_ref=\"another_units_in_that_model\" name=\"units2\" id=\"units_2\"/>\n"
                     "    <component component_ref=\"another_component_in_that_model\" name=\"component2\" id=\"component_2\"/>\n"
                     "    <units units_ref=\"yet_another_units_in_that_model\" name=\"units3\" id=\"units_3\"/>\n"
                     "  </import>\n"
                     "</model>\n";

    std::string out = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                      "<model xmlns=\"http://www.cellml.org/cellml/2.0#\" name=\"everything\" id=\"model_1\">\n"
                      "  <import xmlns:xlink=\"http://www.w3.org/1999/xlink\" xlink:href=\"some-other-model.xml\" id=\"import_1\">\n"
                      "    <component component_ref=\"a_component_in_that_model\" name=\"component1\" id=\"component_1\"/>\n"
                      "    <component component_ref=\"another_component_in_that_model\" name=\"component2\" id=\"component_2\"/>\n"
                      "    <units units_ref=\"a_units_in_that_model\" name=\"units1\" id=\"units_1\"/>\n"
                      "    <units units_ref=\"another_units_in_that_model\" name=\"units2\" id=\"units_2\"/>\n"
                      "    <units units_ref=\"yet_another_units_in_that_model\" name=\"units3\" id=\"units_3\"/>\n"
                      "  </import>\n"
                      "</model>\n";

    auto parser = libcellml::Parser::create();
    auto model = parser->parseModel(in);
    auto printer = libcellml::Printer::create();
    EXPECT_EQ(out, printer->printModel(model));
}
