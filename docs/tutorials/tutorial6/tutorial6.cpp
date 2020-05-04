/**
 *      TUTORIAL 6: POTASSIUM CHANNEL MODEL
 *
 *  This tutorial explores the ability of CellML to represent more than one
 *  modelled process at a time using components with connections between them.
 *  By the time you have worked through Tutorial 6 you will be able to:
 *      - Assemble a multi-component model using the API; and
 *      - Inter-connect the components using the equivalent variables functionality.
 *
 *  Tutorial 6 assumes that you are already comfortable with:
 *      - File manipulation and summarising using the utility functions;
 *      - Model creation through the API; and
 *      - Debugging the model using the Validator functionality.
 */

#include <iostream>
#include <libcellml>

#include "../utilities/tutorial_utilities.h"

int main()
{
    //  0.a Setup useful things
    auto validator = libcellml::Validator::create();
    std::string mathHeader = "<math xmlns=\"http://www.w3.org/1998/Math/MathML\" xmlns:cellml=\"http://www.cellml.org/cellml/2.0#\">\n";
    std::string mathFooter = "</math>";

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "   STEP 1: Define the potassiumChannel component " << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    //  STEP 1: Define the potassiumChannel component.
    //      As in the previous Tutorial 5, we define an ion channel, but this time make it
    //      specific to potassium.  We also introduce voltage dependence into the alpha and
    //      beta gate rates, and move them into a separate component to make it cleaner.
    //      This separate component is the nGate component, and will be defined in Step 2.

    //  1.a Create a model and add to it a new potassiumChannel component.

    //  1.b Define the maths inside the potassiumChannel component.  You might like to use
    //      braces {} to limit the scope of your definition.

    //  1.c Call the validator and expect it to report errors related to missing variables and units.

    //  1.d Create the variables needed and add them to the potassium channel component

    //  1.e Check for errors.  Expect warnings about the undefined units in the maths and in the
    //      new variables added.

    //  1.f Add the missing units.  Verify that the model is now free of validation errors.
    //
    //  NOTE: If, even after adding the Units the validator continues to return an error, you
    //      need to call the Model::linkUnits() function to link the units.  Unlinked units
    //      can occur when variables call their setUnits() function with
    //      a string argument of the Units item's name, rather than the Units item itself.

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "   STEP 2: Create the nGate component  " << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    //  STEP 2: Create the nGate component
    //      This model differs from the one in Tutorial 5 in that voltage dependence is
    //      introduced into the opening and closing rates.  This dependence is handled in
    //      a separate component, the nGate component, which you'll define here.

    //  2.a Create an nGate component, and add it to the potassiumChannel component (NB: not the model!).
    //      Adding it to the component creates an encapsulation hierarchy, which affects which components
    //      have access to each other.  It also means that if the potassiumChannel component is
    //      moved or imported somewhere else, then the nGate child component will be included too.

    //  2.b Add the mathematics to the nGate component and validate the model.
    //      Expect errors relating to missing variables.

    //  2.c Add the missing variables to the nGate component, link the units, and validate again.
    //      Expect errors relating to missing units.

    //  2.d Create the missing units and add them to the model.  Link the model's
    //      units and validate again.  Expect the model to be free of errors.

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "   STEP 3: Connect the components together " << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    //  STEP 3: Connect the components together.
    //      In order for the voltage dependence of the alpha and beta rates within
    //      the nGate component to affect the current in the potassiumChannel component
    //      some of the variables need to share their values between the components.
    //      This is done using variable equivalence and interfaces.

    //  3.a Set the equivalent variable pairs between the nGate and potassiumChannel components.
    //      These are:
    //          - voltage, V
    //          - time, t
    //          - gate status, n
    //      Use the Variable::addEquivalence(VariablePtr, VariablePtr) function.

    //  3.b Validate the model.  Expect errors related to unspecified interface types.

    //  3.c Set the recommended interface types for all of the variables with connections.

    //  3.d Revalidate the model, and check that it is now free of errors.

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "   STEP 4: Read the controller " << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    //  STEP 4: The controller is a component at the top level of this model.
    //      This allows for it to be more easily interchangable between runs and stimulus
    //      conditions.  In later tutorials we will move this component into an external file so that the
    //      model is separate from the running parameters and initial conditions.

    //  4.a Create a Parser, and use it to read a temporary model from the controller file supplied.

    //  4.b From the parsed model, retrieve the component named "controller", remove its parent,
    //      and add it to the current model at the top level.

    //  4.c Repeat the process to retrieve the "potassiumChannel_initialiser" component and add to
    //      the potassiumChannel component.

    //  4.d Repeat the process to retrieve the "nGate_initialiser" component and add to
    //      the nGate component.

    //  4.e Connect the variables in the nGate and potassiumChannel components to their initialised versions
    //      in the nGateInit and potassiumChannelInit components.
    //      Connect the variables in the controller throughout the model.

    //  4.f Validate the model, expecting errors related to unspecified interfaces.
    //      If you encounter errors related to missing units, you may need to use the ModelPtr::linkUnits()
    //      function before calling the validator.

    //  4.g Set the recommended interface types on the connected variables.

    //  4.h Revalidate the model, and check that it is now free of errors.

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "   STEP 5: Generate the model and output" << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    //  5.a Create a Generator instance and pass it the model for processing.  Check for errors and
    //      print them to the terminal.  We expect to see errors reporting that some variables are not
    //      computed or not initialised.

    //  5.b Create a Printer instance and use it to serialise the model.  Print this to a *.cellml file.

    //  5.c Write the interface code to a .h file.

    //  5.d Write the implementation code to a .c file.

    //  5.e Change the profile to Python and reprocess the model

    //  5.f Write the Python implementation code to a .py file

    //  5.g Please see the instructions in the tutorial for how to run a
    //      simulation of this model using the simple solver provided.
    //      Then go and have a cuppa, you're done!

}
