/**
 *      TUTORIAL 6: POTASSIUM CHANNEL MODEL
 *
 *  This tutorial explores the ability of CellML to represent more than one
 *  modelled process at a time using components with connections between them.
 *  By the time you have worked through Tutorial 6 you will be able to:
 *      - import a Component or Units item from an existing CellML file **TODO not working **
 *      - assemble a multi-component model using the API
 *      - inter-connect the components using the equivalent variables
 *        functionality
 *      - validate and debug the constructed model
 *
 *  Tutorial 6 assumes that you are already comfortable with:
 *      - file manipulation and summarising using the utility functions
 *      - model creation through the API
 *      - debugging the model using the Validator functionality
 */

#include <iostream>
#include <libcellml>

#include "../utilities/tutorial_utilities.h"

int main()
{
    //  0.a Setup for useful things
    auto parser = libcellml::Parser::create();
    auto validator = libcellml::Validator::create();
    auto generator = libcellml::Generator::create();
    std::string mathHeader = "<math xmlns=\"http://www.w3.org/1998/Math/MathML\" xmlns:cellml=\"http://www.cellml.org/cellml/2.0#\">\n";
    std::string mathFooter = "</math>";

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "  STEP 1: Read the ion channel component" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

#if 0

    // TODO have removed this section until the parent/child stuff with variables and components
    // has been sorted.

    //  1.a Read the model created in Tutorial 4.  Note that if you didn't
    //      do that tutorial you can simply copy the CellML file
    //      from Resources/Tutorial4_IonChannelModel.cellml
    std::string inFileName = "../resources/tutorial5_IonChannelModel.cellml";
    std::ifstream inFile(inFileName);
    std::stringstream inFileContents;
    inFileContents << inFile.rdbuf();
    std::cout << "Opening the CellML file: '" << inFileName << "'" << std::endl;

    auto model = parser->parseModel(inFileContents.str());
    model->setName("Tutorial6_PotassiumChannelModel");

    //  1.b Print to the screen and notice that there are three variables in
    //      this component which we'll need to move into the parent
    //      potassiumChannel component when it's created in step ??
    printModelToTerminal(model);

    //  1.c Check that the model is ok so far
    validator->validateModel(model);
    printErrorsToTerminal(validator);

    //  1.d Retrieve and rename the component.
    auto nGate = model->component(0);
    nGate->setName("nGate");

    //  1.e Remove its maths
    nGate->removeMath();
    {
        std::string equation1 =
            "  <apply><eq/>\n"
            "    <apply><diff/>\n"
            "      <bvar><ci>t</ci></bvar>\n"
            "      <ci>n</ci>\n"
            "    </apply>\n" // end diff
            "    <apply><minus/>\n"
            "      <apply><times/>\n"
            "        <ci>alpha_n</ci>\n"
            "        <apply><minus/>\n"
            "          <cn cellml:units=\"dimensionless\">1</cn>\n"
            "          <ci>n</ci>\n"
            "        </apply>\n" // end minus
            "      </apply>\n" // end times
            "      <apply><times/>\n"
            "        <ci>beta_n</ci>\n"
            "        <ci>n</ci>\n"
            "      </apply>\n" // end times
            "    </apply>\n" // end minus
            "  </apply>\n"; // end eq

        std::string equation2 =
            "  <apply><eq/>\n"
            "    <ci>alpha_n</ci>\n"
            "    <apply><divide/>\n"
            "      <apply><times/>\n"
            "        <cn cellml:units=\"per_millivolt_millisecond\">0.01</cn>\n"
            "        <apply><plus/>\n"
            "          <ci>V</ci>\n"
            "          <cn cellml:units=\"millivolt\">10</cn>\n"
            "        </apply>\n" // end plus
            "      </apply>\n" // end times, end denominator
            "      <apply><minus/>\n"
            "        <apply><exp/>\n"
            "          <apply><divide/>\n"
            "            <apply><plus/>\n"
            "              <ci>V</ci>\n"
            "              <cn cellml:units=\"millivolt\">10</cn>\n"
            "            </apply>\n" // end plus
            "            <cn cellml:units=\"millivolt\">10</cn>\n"
            "          </apply>\n" // end divide
            "         </apply>\n" // end exp
            "         <cn cellml:units=\"dimensionless\">1</cn>\n"
            "      </apply>\n" // end minus
            "    </apply>\n" // end divide
            "  </apply>\n"; // end eq

        std::string equation3 =
            "  <apply><eq/>\n"
            "    <ci>beta_n</ci>\n"
            "    <apply><times/>\n"
            "      <cn cellml:units=\"per_millisecond\">0.125</cn>\n"
            "      <apply><exp/>\n"
            "        <apply><divide/>\n"
            "          <ci>V</ci>\n"
            "          <cn cellml:units=\"millivolt\">80</cn>\n"
            "        </apply>\n" // end divide
            "      </apply>\n" // end exp
            "    </apply>\n" // end times
            "  </apply>\n"; // end eq

        nGate->setMath(mathHeader);
        nGate->appendMath(equation1);
        nGate->appendMath(equation2);
        nGate->appendMath(equation3);
        nGate->appendMath(mathFooter);
    }
    validator->validateModel(model);
    printErrorsToTerminal(validator);
#endif

    auto model = libcellml::Model::create("Tutorial6_PotassiumChannelModel");
    auto nGate = libcellml::Component::create("nGate");
    {
        std::string equation1 =
            "  <apply><eq/>\n"
            "    <apply><diff/>\n"
            "      <bvar><ci>t</ci></bvar>\n"
            "      <ci>n</ci>\n"
            "    </apply>\n" // end diff
            "    <apply><minus/>\n"
            "      <apply><times/>\n"
            "        <ci>alpha_n</ci>\n"
            "        <apply><minus/>\n"
            "          <cn cellml:units=\"dimensionless\">1</cn>\n"
            "          <ci>n</ci>\n"
            "        </apply>\n" // end minus
            "      </apply>\n" // end times
            "      <apply><times/>\n"
            "        <ci>beta_n</ci>\n"
            "        <ci>n</ci>\n"
            "      </apply>\n" // end times
            "    </apply>\n" // end minus
            "  </apply>\n"; // end eq

        std::string equation2 =
            "  <apply><eq/>\n"
            "    <ci>alpha_n</ci>\n"
            "    <apply><divide/>\n"
            "      <apply><times/>\n"
            "        <cn cellml:units=\"per_millivolt_millisecond\">0.01</cn>\n"
            "        <apply><plus/>\n"
            "          <ci>V</ci>\n"
            "          <cn cellml:units=\"millivolt\">10</cn>\n"
            "        </apply>\n" // end plus
            "      </apply>\n" // end times, end denominator
            "      <apply><minus/>\n"
            "        <apply><exp/>\n"
            "          <apply><divide/>\n"
            "            <apply><plus/>\n"
            "              <ci>V</ci>\n"
            "              <cn cellml:units=\"millivolt\">10</cn>\n"
            "            </apply>\n" // end plus
            "            <cn cellml:units=\"millivolt\">10</cn>\n"
            "          </apply>\n" // end divide
            "        </apply>\n" // end exp
            "        <cn cellml:units=\"dimensionless\">1</cn>\n"
            "      </apply>\n" // end minus
            "    </apply>\n" // end divide
            "  </apply>\n"; // end eq

        std::string equation3 =
            "  <apply><eq/>\n"
            "    <ci>beta_n</ci>\n"
            "    <apply><times/>\n"
            "      <cn cellml:units=\"per_millisecond\">0.125</cn>\n"
            "      <apply><exp/>\n"
            "        <apply><divide/>\n"
            "          <ci>V</ci>\n"
            "          <cn cellml:units=\"millivolt\">80</cn>\n"
            "        </apply>\n" // end divide
            "      </apply>\n" // end exp
            "    </apply>\n" // end times
            "  </apply>\n"; // end eq

        nGate->setMath(mathHeader);
        nGate->appendMath(equation1);
        nGate->appendMath(equation2);
        nGate->appendMath(equation3);
        nGate->appendMath(mathFooter);
    }

    {
        auto t = libcellml::Variable::create("t");
        t->setUnits("millisecond");
        nGate->addVariable(t);

        auto V = libcellml::Variable::create("V");
        V->setUnits("millivolt");
        nGate->addVariable(V);

        auto alpha_n = libcellml::Variable::create("alpha_n");
        alpha_n->setUnits("per_millisecond");
        nGate->addVariable(alpha_n);

        auto beta_n = libcellml::Variable::create("beta_n");
        beta_n->setUnits("per_millisecond");
        nGate->addVariable(beta_n);

        auto n = libcellml::Variable::create("n");
        n->setUnits("dimensionless");
        n->setInitialValue(0.325);
        nGate->addVariable(n);
    }

    auto ms = libcellml::Units::create("millisecond");
    ms->addUnit("second", "milli");

    auto mV = libcellml::Units::create("millivolt");
    mV->addUnit("volt", "milli");

    auto per_ms = libcellml::Units::create("per_millisecond");
    per_ms->addUnit("millisecond", -1.0);

    auto per_mV_ms = libcellml::Units::create("per_millivolt_millisecond");
    per_mV_ms->addUnit("per_millisecond");
    per_mV_ms->addUnit("millivolt", -1);

    //  4.b Add these units into the model
    model->addUnits(ms);
    model->addUnits(mV);
    model->addUnits(per_ms);
    model->addUnits(per_mV_ms);

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << " STEP 2: Define the potassiumChannel component " << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    //  2.a Create the potassiumChannel component and add to the model.
    auto potassiumChannel = libcellml::Component::create("potassiumChannel");
    model->addComponent(potassiumChannel);
    potassiumChannel->addComponent(nGate);

    // //  2.b Move the variables out of the nGate and into to the potassiumChannel component.
    // potassiumChannel->addVariable(nGate->variable("E_K"));
    // potassiumChannel->addVariable(nGate->variable("i_K"));
    // potassiumChannel->addVariable(nGate->variable("g_K"));

    // // TODO removing the variables from their old component, shouldn't be needed??
    // nGate->removeVariable("E_K");
    // nGate->removeVariable("i_K");
    // nGate->removeVariable("g_K");
    // nGate->removeVariable("gamma"); // this one needs removal anyway?

    //  2.c Create the remaining variables for the potassium channel
    {
        auto E_K = libcellml::Variable::create("E_K");
        E_K->setUnits("millivolt");
        potassiumChannel->addVariable(E_K);

        auto i_K = libcellml::Variable::create("i_K");
        i_K->setUnits("microA_per_cm2");
        potassiumChannel->addVariable(i_K);

        auto g_K = libcellml::Variable::create("g_K");
        g_K->setUnits("milliS_per_cm2");
        potassiumChannel->addVariable(g_K);

        auto V = libcellml::Variable::create("V");
        V->setUnits("millivolt");
        potassiumChannel->addVariable(V);

        auto t = libcellml::Variable::create("t");
        t->setUnits("millisecond");
        potassiumChannel->addVariable(t);

        auto n = libcellml::Variable::create("n");
        n->setUnits("dimensionless");
        potassiumChannel->addVariable(n);

        auto Ko = libcellml::Variable::create("Ko");
        Ko->setUnits("millimol");
        potassiumChannel->addVariable(Ko);

        auto Ki = libcellml::Variable::create("Ki");
        Ki->setUnits("millimol");
        potassiumChannel->addVariable(Ki);

        auto RTF = libcellml::Variable::create("RTF");
        RTF->setUnits("millivolt");
        potassiumChannel->addVariable(RTF);

        auto K_conductance = libcellml::Variable::create("K_conductance");
        K_conductance->setUnits("milliS_per_cm2");
        potassiumChannel->addVariable(K_conductance);

        //  2.d Defining the maths inside the potassiumChannel component
        std::string equation1 =
            "  <apply><eq/>\n"
            "    <ci>E_K</ci>\n"
            "    <apply><times/>\n"
            "      <ci>RTF</ci>\n"
            "      <apply><ln/>\n"
            "        <apply><divide/>\n"
            "          <ci>Ko</ci>\n"
            "          <ci>Ki</ci>\n"
            "        </apply>\n" // end divide
            "      </apply>\n" // end ln
            "    </apply>\n" // end times
            "  </apply>\n"; // end eq
        std::string equation2 =
            "  <apply><eq/>\n"
            "    <ci>K_conductance</ci>\n"
            "    <apply><times/>\n"
            "      <ci>g_K</ci>\n"
            "      <apply><power/>\n"
            "        <ci>n</ci>\n"
            "        <cn cellml:units=\"dimensionless\">4</cn>\n"
            "      </apply>\n" // end pow
            "    </apply>\n" // end times
            "  </apply>\n"; //end eq
        std::string equation3 =
            "  <apply><eq/>\n"
            "    <ci>i_K</ci>\n"
            "    <apply><times/>\n"
            "      <ci>K_conductance</ci>\n"
            "      <apply><minus/>\n"
            "        <ci>V</ci>\n"
            "        <ci>E_K</ci>\n"
            "      </apply>\n" //end minus
            "    </apply>\n" // end times
            "  </apply>\n"; // end eq

        potassiumChannel->setMath(mathHeader);
        potassiumChannel->appendMath(equation1);
        potassiumChannel->appendMath(equation2);
        potassiumChannel->appendMath(equation3);
        potassiumChannel->appendMath(mathFooter);
    } // end scope of variables and maths in this component

    //  2.e Check for errors.  Expect warnings about the undefined units in the maths and in the
    //      new variables added.
    validator->validateModel(model);
    printErrorsToTerminal(validator);

    //  2.f Add the missing units and recheck the validation
    auto mM = libcellml::Units::create("millimol");
    mM->addUnit("mole", "milli");

    auto microA_per_cm2 = libcellml::Units::create("microA_per_cm2");
    microA_per_cm2->addUnit("ampere", "micro");
    microA_per_cm2->addUnit("metre", "centi", -2.0);

    auto mS_per_cm2 = libcellml::Units::create("milliS_per_cm2");
    mS_per_cm2->addUnit("siemens", "milli");
    mS_per_cm2->addUnit("metre", "centi", -2.0);

    model->addUnits(mM);
    model->addUnits(microA_per_cm2);
    model->addUnits(mS_per_cm2);

    validator->validateModel(model);
    printErrorsToTerminal(validator);

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "  STEP 3: Create the environment component" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    //  3.a Creating the new environment component
    auto environment = libcellml::Component::create("environment");

    //  3.b Add variables to the component.  Use brackets to define the scope of
    //      these variables. This means that we can use variables with the same
    //      symbol elsewhere.
    {
        auto V = libcellml::Variable::create("V");
        V->setUnits("millivolt");
        environment->addVariable(V);

        auto t = libcellml::Variable::create("t");
        t->setUnits("millisecond");
        environment->addVariable(t);
    } // end of the environment scope for variables

    //  3.c Add the new component to the model
    model->addComponent(environment);

    //  3.d Calling the validator to check that our model is valid so far.
    validator->validateModel(model);
    printErrorsToTerminal(validator);

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "   STEP 4: Define the component hierarchy " << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    //  4.a Change the nGate component to be a child of the potassiumChannel component
    // nGate->removeParent();
    // model->removeComponent(nGate);
    // potassiumChannel->addComponent(nGate);

    //  4.b Verify the component hierarchy by printing the model to the screen
    printModelToTerminal(model);

    //  4.c Define the equivalent variables between components.  Note that because
    //      the variables have been defined within a limited scope (using the {} above)
    //      you will need to retrieve them from each component first.
    libcellml::Variable::addEquivalence(environment->variable("t"), potassiumChannel->variable("t"));
    libcellml::Variable::addEquivalence(environment->variable("V"), potassiumChannel->variable("V"));

    // TODO not sure what's happening here - this line below should fail as not neighbouring components,
    // creates a validation error but should be allowed?
    // libcellml::Variable::addEquivalence(environment->variable("t"), nGate->variable("t"));

    //  4.d Validating the model: this should show an error reporting that an
    //      invalid connection has been made between the environment and nGate
    //      components

    // TODO This should produce a validation error but currently does not?
    // validator->validateModel(model);
    // printErrorsToTerminal(validator);

    //  4.e  Fix the connection error above, and add the voltage and gating variable equivalences
    // libcellml::Variable::removeEquivalence(environment->variable("t"), nGate->variable("t"));

    libcellml::Variable::addEquivalence(potassiumChannel->variable("t"), nGate->variable("t"));
    libcellml::Variable::addEquivalence(potassiumChannel->variable("V"), nGate->variable("V"));
    libcellml::Variable::addEquivalence(potassiumChannel->variable("n"), nGate->variable("n"));

    // TODO This should produce a validation error but currently does not?
    validator->validateModel(model);
    printErrorsToTerminal(validator);

    //  4.f Add the interface specification.  The environment component is a sibling of the potassiumChannel
    //      component, so they will both use the public interface.  The nGate component is a child of the
    //      potassiumChannel, so will use the public interface.  The potassiumChannel is the parent of the
    //      nGate component, so will need an additional private interface.  The nGate will have a public
    //      interface to its parent, the potassiumChannel.  Thus for the V and t variables:
    //          - environment -> public
    //          - potassiumChannel -> public_and_private
    //          - nGate -> public
    //      Because the n variable is shared only between the potassiumChannel and the nGate, we don't need to
    //      use the public_and_private designation there; just private on the parent and public on the child.

    environment->variable("t")->setInterfaceType("public");
    environment->variable("V")->setInterfaceType("public");
    potassiumChannel->variable("t")->setInterfaceType("public_and_private");
    potassiumChannel->variable("V")->setInterfaceType("public_and_private");
    nGate->variable("t")->setInterfaceType("public");
    nGate->variable("V")->setInterfaceType("public");
    potassiumChannel->variable("n")->setInterfaceType("private");
    nGate->variable("n")->setInterfaceType("public");

    validator->validateModel(model);
    printErrorsToTerminal(validator);

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "     STEP 5: Define the driving function " << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    //  5.a Define a MathML string representing the voltage clamp
    std::string voltageClampMaths =
        "  <apply><eq/>\n"
        "    <ci>V</ci>\n"
        "    <piecewise>\n"
        "      <piece>\n"
        "        <cn cellml:units=\"millivolt\">0</cn>\n"
        "        <apply><lt/><ci>t</ci><cn cellml:units=\"millisecond\">5</cn></apply>\n"
        "      </piece>\n"
        "      <piece>\n"
        "        <cn cellml:units=\"millivolt\">0</cn>\n"
        "        <apply><gt/><ci>t</ci><cn cellml:units=\"millisecond\">15</cn></apply>\n"
        "      </piece>\n"
        "      <otherwise>\n"
        "        <cn cellml:units=\"millivolt\">-85</cn>\n"
        "      </otherwise>\n"
        "    </piecewise>\n"
        "  </apply>\n";

    //  5.b Add this to the maths for the environment component.
    environment->setMath(mathHeader);
    environment->appendMath(voltageClampMaths);
    environment->appendMath(mathFooter);

    //  5.c Validate the model including this new maths block
    validator->validateModel(model);
    printErrorsToTerminal(validator);

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "  STEP 6: Generate and initalise" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    //  6.a Call the generator to process the model and output errors to the terminal
    //      Expect errors related to initialisation/non-computation of variables in the potassium channel.
    generator->processModel(model);
    printErrorsToTerminal(generator);

    //  6.b Initialise the variables: Ko = 3, Ki = 90, RTF = 25, g_K = 36.0
    potassiumChannel->variable("Ko")->setInitialValue(3);
    potassiumChannel->variable("Ki")->setInitialValue(90);
    potassiumChannel->variable("RTF")->setInitialValue(25);
    potassiumChannel->variable("g_K")->setInitialValue(36.0);

    //  6.c Call the generator again and verify that there are no more errors.
    generator->processModel(model);
    printErrorsToTerminal(generator);

    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "  STEP 7: Output the model" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    //  7.a Write the interface code to a .h file
    std::ofstream outFile("tutorial6_PotassiumChannelModel_generated.h");
    outFile << generator->interfaceCode();
    outFile.close();

    //  7.b Write the implementation code to a .c file
    outFile.open("tutorial6_PotassiumChannelModel_generated.c");
    outFile << generator->implementationCode();
    outFile.close();

    //  7.c Change the profile to Python and reprocess the model
    auto profile = libcellml::GeneratorProfile::create(libcellml::GeneratorProfile::Profile::PYTHON);
    generator->setProfile(profile);
    generator->processModel(model);

    //  7.d Write the Python implementation code to a .py file
    outFile.open("tutorial6_PotassiumChannelModel_generated.py");
    outFile << generator->implementationCode();
    outFile.close();

    //  7.e Serialise the model using a Printer and write to a CellML file
    auto printer = libcellml::Printer::create();
    outFile.open("tutorial6_PotassiumChannelModel_generated.cellml");
    outFile << printer->printModel(model);
    outFile.close();

    std::cout << "The created '" << model->name()
              << "' model has been output to tutorial6_PotassiumChannelModel_generated.[cellml,py,c,h]" << std::endl;

    //  7.f Please see the instructions in the tutorial for how to run a
    //      simulation of this model using the simple solver provided.
    //      Then go and have a cuppa, you're done!

}