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
#include "debug.h"

#include "libcellml/generator.h"

#include <algorithm>
#include <limits>
#include <list>
#include <regex>
#include <sstream>
#include <vector>

#include "libcellml/component.h"
#include "libcellml/generatorprofile.h"
#include "libcellml/model.h"
#include "libcellml/units.h"
#include "libcellml/validator.h"
#include "libcellml/variable.h"
#include "libcellml/version.h"

#include "internaltypes.h"
#include "utilities.h"
#include "xmldoc.h"

#ifdef __linux__
#    undef TRUE
#    undef FALSE
#endif

namespace libcellml {

static const size_t MAX_SIZE_T = std::numeric_limits<size_t>::max();

/**
 * @brief The GeneratorVariable::GeneratorVariableImpl struct.
 *
 * The private implementation for the GeneratorVariable class.
 */
struct GeneratorVariable::GeneratorVariableImpl
{
    VariablePtr mInitialValueVariable;
    VariablePtr mVariable;
    GeneratorVariable::Type mType = GeneratorVariable::Type::CONSTANT;

    void populate(const VariablePtr &initialValueVariable,
                  const VariablePtr &variable,
                  GeneratorVariable::Type type);
};

void GeneratorVariable::GeneratorVariableImpl::populate(const VariablePtr &initialValueVariable,
                                                        const VariablePtr &variable,
                                                        GeneratorVariable::Type type)
{
    mInitialValueVariable = initialValueVariable;
    mVariable = variable;
    mType = type;
}

GeneratorVariable::GeneratorVariable()
    : mPimpl(new GeneratorVariableImpl())
{
}

GeneratorVariable::~GeneratorVariable()
{
    delete mPimpl;
}

GeneratorVariablePtr GeneratorVariable::create() noexcept
{
    return std::shared_ptr<GeneratorVariable> {new GeneratorVariable {}};
}

VariablePtr GeneratorVariable::initialValueVariable() const
{
    return mPimpl->mInitialValueVariable;
}

VariablePtr GeneratorVariable::variable() const
{
    return mPimpl->mVariable;
}

GeneratorVariable::Type GeneratorVariable::type() const
{
    return mPimpl->mType;
}

struct GeneratorEquation;
using GeneratorEquationPtr = std::shared_ptr<GeneratorEquation>;
using GeneratorEquationWeakPtr = std::weak_ptr<GeneratorEquation>;

struct GeneratorInternalVariable
{
    enum struct Type
    {
        UNKNOWN,
        SHOULD_BE_STATE,
        VARIABLE_OF_INTEGRATION,
        STATE,
        CONSTANT,
        COMPUTED_TRUE_CONSTANT,
        COMPUTED_VARIABLE_BASED_CONSTANT,
        ALGEBRAIC,
        OVERCONSTRAINED
    };

    size_t mIndex = MAX_SIZE_T;
    Type mType = Type::UNKNOWN;

    VariablePtr mInitialValueVariable;
    VariablePtr mVariable;

    GeneratorEquationWeakPtr mEquation;

    explicit GeneratorInternalVariable(const VariablePtr &variable);

    void setVariable(const VariablePtr &variable);

    void makeVoi();
    void makeState();
};

using GeneratorInternalVariablePtr = std::shared_ptr<GeneratorInternalVariable>;

GeneratorInternalVariable::GeneratorInternalVariable(const VariablePtr &variable)
{
    setVariable(variable);
}

void GeneratorInternalVariable::setVariable(const VariablePtr &variable)
{
    mInitialValueVariable = mVariable = variable;

    if (!variable->initialValue().empty()) {
        // The variable has an initial value, so it can either be a constant or
        // a state. By default, we consider it to be a constant and, if we find
        // an ODE for that variable, we will know that it was actually a state.

        mType = Type::CONSTANT;
    }
}

void GeneratorInternalVariable::makeVoi()
{
    mType = Type::VARIABLE_OF_INTEGRATION;
}

void GeneratorInternalVariable::makeState()
{
    if (mType == Type::UNKNOWN) {
        mType = Type::SHOULD_BE_STATE;
    } else if (mType == Type::CONSTANT) {
        mType = Type::STATE;
    }
}

GeneratorEquationAst::GeneratorEquationAst() = default;

GeneratorEquationAst::GeneratorEquationAst(Type type,
                                           const GeneratorEquationAstPtr &parent)
    : mType(type)
    , mParent(parent)
{
}

GeneratorEquationAst::GeneratorEquationAst(Type type, const std::string &value,
                                           const GeneratorEquationAstPtr &parent)
    : mType(type)
    , mValue(value)
    , mParent(parent)
{
}

GeneratorEquationAst::GeneratorEquationAst(Type type, const VariablePtr &variable,
                                           const GeneratorEquationAstPtr &parent)
    : mType(type)
    , mVariable(variable)
    , mParent(parent)
{
}

GeneratorEquationAst::GeneratorEquationAst(const GeneratorEquationAstPtr &ast,
                                           const GeneratorEquationAstPtr &parent)
    : mType(ast->mType)
    , mVariable(ast->mVariable)
    , mParent(parent)
    , mLeft(ast->mLeft)
    , mRight(ast->mRight)
{
}

#ifdef SWIG
struct GeneratorEquation
#else
struct GeneratorEquation: public std::enable_shared_from_this<GeneratorEquation>
#endif
{
    enum struct Type
    {
        UNKNOWN,
        TRUE_CONSTANT,
        VARIABLE_BASED_CONSTANT,
        RATE,
        ALGEBRAIC
    };

    size_t mOrder = MAX_SIZE_T;
    Type mType = Type::UNKNOWN;

    std::list<GeneratorEquationPtr> mDependencies;

    GeneratorEquationAstPtr mAst;

    std::list<GeneratorInternalVariablePtr> mVariables;
    std::list<GeneratorInternalVariablePtr> mOdeVariables;

    GeneratorInternalVariablePtr mVariable = nullptr;
    ComponentPtr mComponent = nullptr;

    bool mComputedTrueConstant = true;
    bool mComputedVariableBasedConstant = true;

    bool mIsStateRateBased = false;

    explicit GeneratorEquation(const ComponentPtr &component);

    void addVariable(const GeneratorInternalVariablePtr &variable);
    void addOdeVariable(const GeneratorInternalVariablePtr &odeVariable);

    static bool containsNonUnknownVariables(const std::list<GeneratorInternalVariablePtr> &variables);
    static bool containsNonConstantVariables(const std::list<GeneratorInternalVariablePtr> &variables);

    static bool knownVariable(const GeneratorInternalVariablePtr &variable);
    static bool knownOdeVariable(const GeneratorInternalVariablePtr &odeVariable);

    bool check(size_t & equationOrder, size_t & stateIndex, size_t & variableIndex);
};

GeneratorEquation::GeneratorEquation(const ComponentPtr &component)
    : mAst(std::make_shared<GeneratorEquationAst>())
    , mComponent(component)
{
}

void GeneratorEquation::addVariable(const GeneratorInternalVariablePtr &variable)
{
    if (std::find(mVariables.begin(), mVariables.end(), variable) == mVariables.end()) {
        mVariables.push_back(variable);
    }
}

void GeneratorEquation::addOdeVariable(const GeneratorInternalVariablePtr &odeVariable)
{
    if (std::find(mOdeVariables.begin(), mOdeVariables.end(), odeVariable) == mOdeVariables.end()) {
        mOdeVariables.push_back(odeVariable);
    }
}

bool GeneratorEquation::containsNonUnknownVariables(const std::list<GeneratorInternalVariablePtr> &variables)
{
    return std::find_if(variables.begin(), variables.end(), [](const GeneratorInternalVariablePtr &variable) {
               return (variable->mType != GeneratorInternalVariable::Type::UNKNOWN);
           })
           != std::end(variables);
}

bool GeneratorEquation::containsNonConstantVariables(const std::list<GeneratorInternalVariablePtr> &variables)
{
    return std::find_if(variables.begin(), variables.end(), [](const GeneratorInternalVariablePtr &variable) {
               return (variable->mType != GeneratorInternalVariable::Type::UNKNOWN)
                      && (variable->mType != GeneratorInternalVariable::Type::CONSTANT)
                      && (variable->mType != GeneratorInternalVariable::Type::COMPUTED_TRUE_CONSTANT)
                      && (variable->mType != GeneratorInternalVariable::Type::COMPUTED_VARIABLE_BASED_CONSTANT);
           })
           != std::end(variables);
}

bool GeneratorEquation::knownVariable(const GeneratorInternalVariablePtr &variable)
{
    return (variable->mIndex != MAX_SIZE_T)
           || (variable->mType == GeneratorInternalVariable::Type::VARIABLE_OF_INTEGRATION)
           || (variable->mType == GeneratorInternalVariable::Type::STATE)
           || (variable->mType == GeneratorInternalVariable::Type::CONSTANT)
           || (variable->mType == GeneratorInternalVariable::Type::COMPUTED_TRUE_CONSTANT)
           || (variable->mType == GeneratorInternalVariable::Type::COMPUTED_VARIABLE_BASED_CONSTANT);
}

bool GeneratorEquation::knownOdeVariable(const GeneratorInternalVariablePtr &odeVariable)
{
    return (odeVariable->mIndex != MAX_SIZE_T)
           || (odeVariable->mType == GeneratorInternalVariable::Type::VARIABLE_OF_INTEGRATION);
}

bool sameOrEquivalentVariable(const VariablePtr &variable1,
                              const VariablePtr &variable2)
{
    // Return whether the given variables are the same or are equivalent (be it
    // directly or indirectly).

    return (variable1 == variable2) || variable1->hasEquivalentVariable(variable2, true);
}


bool GeneratorEquation::check(size_t &equationOrder, size_t &stateIndex,
                              size_t &variableIndex)
{
    // Nothing to check if the equation has already been given an order (i.e.
    // everything is fine) or if there is one known (ODE) variable left (i.e.
    // this equation is an overconstraint).

    if (mOrder != MAX_SIZE_T) {
        return false;
    }

    if (mVariables.size() + mOdeVariables.size() == 1) {
        GeneratorInternalVariablePtr variable = (mVariables.size() == 1) ? mVariables.front() : mOdeVariables.front();

        if ((variable->mIndex != MAX_SIZE_T)
            && (variable->mType != GeneratorInternalVariable::Type::UNKNOWN)
            && (variable->mType != GeneratorInternalVariable::Type::SHOULD_BE_STATE)) {
            variable->mType = GeneratorInternalVariable::Type::OVERCONSTRAINED;

            return false;
        }
    }

    // Determine, from the (new) known (ODE) variables, whether the equation is
    // truly constant or variable-based constant.

    mComputedTrueConstant = mComputedTrueConstant
                            && !containsNonUnknownVariables(mVariables)
                            && !containsNonUnknownVariables(mOdeVariables);
    mComputedVariableBasedConstant = mComputedVariableBasedConstant
                                     && !containsNonConstantVariables(mVariables)
                                     && !containsNonConstantVariables(mOdeVariables);

    // Determine whether the equation is state/rate based and add, as a
    // dependency, the equations used to compute the (new) known variables.
    // (Note that we don't account for the (new) known ODE variables since their
    // corresponding equation can obviously not be of algebraic type.)

    if (!mIsStateRateBased) {
        mIsStateRateBased = !mOdeVariables.empty();
    }

    for (const auto &variable : mVariables) {
        if (knownVariable(variable)) {
            GeneratorEquationPtr equation = variable->mEquation.lock();

            if (!mIsStateRateBased) {
                mIsStateRateBased = (equation == nullptr) ?
                                        (variable->mType == GeneratorInternalVariable::Type::STATE) :
                                        equation->mIsStateRateBased;
            }

            if (equation != nullptr) {
                mDependencies.push_back(equation);
            }
        }
    }

    // Stop tracking (new) known (ODE) variables.

    mVariables.remove_if(knownVariable);
    mOdeVariables.remove_if(knownOdeVariable);

    // If there is one (ODE) variable left then update its component (to be sure
    // that it's the same as the one in which the equation is), its type (if it
    // is currently unknown), determine its index and determine the type of our
    // equation and set its order, if the (ODE) variable is a state, computed
    // constant or algebraic variable.

    bool relevantCheck = false;

    if (mVariables.size() + mOdeVariables.size() == 1) {
        GeneratorInternalVariablePtr variable = (mVariables.size() == 1) ? mVariables.front() : mOdeVariables.front();
        VariablePtr realVariable = nullptr;

        for (size_t i = 0; i < mComponent->variableCount() && realVariable == nullptr; ++i) {
            VariablePtr testVariable = mComponent->variable(i);

            if (sameOrEquivalentVariable(variable->mVariable, testVariable)) {
                realVariable = testVariable;
            }
        }

        variable->mVariable = realVariable;

        if (variable->mType == GeneratorInternalVariable::Type::UNKNOWN) {
            variable->mType = mComputedTrueConstant ?
                                  GeneratorInternalVariable::Type::COMPUTED_TRUE_CONSTANT :
                                  mComputedVariableBasedConstant ?
                                  GeneratorInternalVariable::Type::COMPUTED_VARIABLE_BASED_CONSTANT :
                                  GeneratorInternalVariable::Type::ALGEBRAIC;
        }

        if ((variable->mType == GeneratorInternalVariable::Type::STATE)
            || (variable->mType == GeneratorInternalVariable::Type::COMPUTED_TRUE_CONSTANT)
            || (variable->mType == GeneratorInternalVariable::Type::COMPUTED_VARIABLE_BASED_CONSTANT)
            || (variable->mType == GeneratorInternalVariable::Type::ALGEBRAIC)) {
            variable->mIndex = (variable->mType == GeneratorInternalVariable::Type::STATE) ?
                                   ++stateIndex :
                                   ++variableIndex;

            variable->mEquation = shared_from_this();

            mOrder = ++equationOrder;
            mType = (variable->mType == GeneratorInternalVariable::Type::STATE) ?
                        Type::RATE :
                        (variable->mType == GeneratorInternalVariable::Type::COMPUTED_TRUE_CONSTANT) ?
                        Type::TRUE_CONSTANT :
                        (variable->mType == GeneratorInternalVariable::Type::COMPUTED_VARIABLE_BASED_CONSTANT) ?
                        Type::VARIABLE_BASED_CONSTANT :
                        Type::ALGEBRAIC;

            mVariable = variable;

            relevantCheck = true;
        }
    }

    return relevantCheck;
}

/**
 * @brief The Generator::GeneratorImpl struct.
 *
 * The private implementation for the Generator class.
 */
struct Generator::GeneratorImpl
{
    Generator *mGenerator = nullptr;

    Generator::ModelType mModelType = Generator::ModelType::UNKNOWN;

    std::list<GeneratorInternalVariablePtr> mInternalVariables;
    std::list<GeneratorEquationPtr> mEquations;

    GeneratorVariablePtr mVoi = nullptr;
    std::vector<GeneratorVariablePtr> mStates;
    std::vector<GeneratorVariablePtr> mVariables;

    GeneratorProfilePtr mProfile = libcellml::GeneratorProfile::create();

    bool mNeedEq = false;
    bool mNeedNeq = false;
    bool mNeedLt = false;
    bool mNeedLeq = false;
    bool mNeedGt = false;
    bool mNeedGeq = false;
    bool mNeedAnd = false;
    bool mNeedOr = false;
    bool mNeedXor = false;
    bool mNeedNot = false;

    bool mNeedMin = false;
    bool mNeedMax = false;

    bool mNeedSec = false;
    bool mNeedCsc = false;
    bool mNeedCot = false;
    bool mNeedSech = false;
    bool mNeedCsch = false;
    bool mNeedCoth = false;
    bool mNeedAsec = false;
    bool mNeedAcsc = false;
    bool mNeedAcot = false;
    bool mNeedAsech = false;
    bool mNeedAcsch = false;
    bool mNeedAcoth = false;

    static bool compareVariablesByName(const GeneratorInternalVariablePtr &variable1,
                                       const GeneratorInternalVariablePtr &variable2);
    static bool compareVariablesByTypeAndIndex(const GeneratorInternalVariablePtr &variable1,
                                               const GeneratorInternalVariablePtr &variable2);

    static bool compareEquationsByVariable(const GeneratorEquationPtr &equation1,
                                           const GeneratorEquationPtr &equation2);

    bool hasValidModel() const;

    size_t mathmlChildCount(const XmlNodePtr &node) const;
    XmlNodePtr mathmlChildNode(const XmlNodePtr &node, size_t index) const;

    GeneratorInternalVariablePtr generatorVariable(const VariablePtr &variable);

    GeneratorVariablePtr variableFirstOccurrence(const VariablePtr &variable,
                                                 const ComponentPtr &component);

    void processNode(const XmlNodePtr &node, GeneratorEquationAstPtr &ast,
                     const GeneratorEquationAstPtr &astParent,
                     const ComponentPtr &component,
                     const GeneratorEquationPtr &equation);
    GeneratorEquationPtr processNode(const XmlNodePtr &node,
                                     const ComponentPtr &component);
    void processComponent(const ComponentPtr &component);
    void processEquationAst(const GeneratorEquationAstPtr &ast);

    double scalingFactor(const VariablePtr &variable);

    void scaleEquationAst(const GeneratorEquationAstPtr &ast, bool debug,
                          int eqnNb);

    void printEquationsAst() const;

    void processModel(const ModelPtr &model, bool debug);

    bool isRelationalOperator(const GeneratorEquationAstPtr &ast) const;
    bool isAndOperator(const GeneratorEquationAstPtr &ast) const;
    bool isOrOperator(const GeneratorEquationAstPtr &ast) const;
    bool isXorOperator(const GeneratorEquationAstPtr &ast) const;
    bool isLogicalOperator(const GeneratorEquationAstPtr &ast) const;
    bool isPlusOperator(const GeneratorEquationAstPtr &ast) const;
    bool isMinusOperator(const GeneratorEquationAstPtr &ast) const;
    bool isTimesOperator(const GeneratorEquationAstPtr &ast) const;
    bool isDivideOperator(const GeneratorEquationAstPtr &ast) const;
    bool isPowerOperator(const GeneratorEquationAstPtr &ast) const;
    bool isRootOperator(const GeneratorEquationAstPtr &ast) const;
    bool isPiecewiseStatement(const GeneratorEquationAstPtr &ast) const;

    std::string replace(std::string string, const std::string &from,
                        const std::string &to);

    void updateVariableInfoSizes(size_t &componentSize, size_t &nameSize,
                                 size_t &unitsSize,
                                 const GeneratorVariablePtr &variable);

    bool modifiedProfile() const;

    void addOriginCommentCode(std::string &code);

    void addInterfaceHeaderCode(std::string &code);
    void addImplementationHeaderCode(std::string &code);

    void addVersionAndLibcellmlVersionCode(std::string &code,
                                           bool interface = false);

    void addStateAndVariableCountCode(std::string &code,
                                      bool interface = false);

    void addVariableTypeObjectCode(std::string &code);

    std::string generateVariableInfoObjectCode(const std::string &objectString);

    void addVariableInfoObjectCode(std::string &code);
    void addVariableInfoWithTypeObjectCode(std::string &code);

    std::string generateVariableInfoEntryCode(const std::string &name,
                                              const std::string &units,
                                              const std::string &component);

    void addInterfaceVoiStateAndVariableInfoCode(std::string &code);
    void addImplementationVoiInfoCode(std::string &code);
    void addImplementationStateInfoCode(std::string &code);
    void addImplementationVariableInfoCode(std::string &code);

    void addArithmeticFunctionsCode(std::string &code);
    void addTrigonometricFunctionsCode(std::string &code);

    void addInterfaceCreateDeleteArrayMethodsCode(std::string &code);
    void addImplementationCreateStatesArrayMethodCode(std::string &code);
    void addImplementationCreateVariablesArrayMethodCode(std::string &code);
    void addImplementationDeleteArrayMethodCode(std::string &code);

    std::string generateMethodBodyCode(const std::string &methodBody);

    std::string generateDoubleCode(const std::string &value);
    std::string generateVariableNameCode(const VariablePtr &variable,
                                         const GeneratorEquationAstPtr &ast = nullptr);

    std::string generateOperatorCode(const std::string &op,
                                     const GeneratorEquationAstPtr &ast);
    std::string generateMinusUnaryCode(const GeneratorEquationAstPtr &ast);
    std::string generateOneParameterFunctionCode(const std::string &function,
                                                 const GeneratorEquationAstPtr &ast);
    std::string generateTwoParameterFunctionCode(const std::string &function,
                                                 const GeneratorEquationAstPtr &ast);
    std::string generatePiecewiseIfCode(const std::string &condition,
                                        const std::string &value);
    std::string generatePiecewiseElseCode(const std::string &value);
    std::string generateCode(const GeneratorEquationAstPtr &ast);

    std::string generateInitializationCode(const GeneratorInternalVariablePtr &variable);
    std::string generateEquationCode(const GeneratorEquationPtr &equation,
                                     std::vector<GeneratorEquationPtr> &remainingEquations,
                                     bool onlyStateRateBasedEquations = false);

    void addInterfaceComputeModelMethodsCode(std::string &code);
    void addImplementationInitializeStatesAndConstantsMethodCode(std::string &code,
                                                                 std::vector<GeneratorEquationPtr> &remainingEquations);
    void addImplementationComputeComputedConstantsMethodCode(std::string &code,
                                                             std::vector<GeneratorEquationPtr> &remainingEquations);
    void addImplementationComputeRatesMethodCode(std::string &code,
                                                 std::vector<GeneratorEquationPtr> &remainingEquations);
    void addImplementationComputeVariablesMethodCode(std::string &code,
                                                     std::vector<GeneratorEquationPtr> &remainingEquations);
};

bool Generator::GeneratorImpl::compareVariablesByName(const GeneratorInternalVariablePtr &variable1,
                                                      const GeneratorInternalVariablePtr &variable2)
{
    VariablePtr realVariable1 = variable1->mInitialValueVariable;
    VariablePtr realVariable2 = variable2->mInitialValueVariable;
    ComponentPtr realComponent1 = std::dynamic_pointer_cast<Component>(realVariable1->parent());
    ComponentPtr realComponent2 = std::dynamic_pointer_cast<Component>(realVariable2->parent());

    if (realComponent1->name() == realComponent2->name()) {
        return realVariable1->name() < realVariable2->name();
    }

    return realComponent1->name() < realComponent2->name();
}

bool Generator::GeneratorImpl::compareVariablesByTypeAndIndex(const GeneratorInternalVariablePtr &variable1,
                                                              const GeneratorInternalVariablePtr &variable2)
{
    if (variable1->mType == variable2->mType) {
        return variable1->mIndex < variable2->mIndex;
    }

    return variable1->mType < variable2->mType;
}

bool Generator::GeneratorImpl::compareEquationsByVariable(const GeneratorEquationPtr &equation1,
                                                          const GeneratorEquationPtr &equation2)
{
    return compareVariablesByTypeAndIndex(equation1->mVariable, equation2->mVariable);
}

bool Generator::GeneratorImpl::hasValidModel() const
{
    return (mModelType == Generator::ModelType::ALGEBRAIC)
           || (mModelType == Generator::ModelType::ODE);
}

size_t Generator::GeneratorImpl::mathmlChildCount(const XmlNodePtr &node) const
{
    // Return the number of child elements, in the MathML namespace, for the
    // given node.

    XmlNodePtr childNode = node->firstChild();
    size_t res = childNode->isMathmlElement() ? 1 : 0;

    while (childNode != nullptr) {
        childNode = childNode->next();

        if (childNode && childNode->isMathmlElement()) {
            ++res;
        }
    }

    return res;
}

XmlNodePtr Generator::GeneratorImpl::mathmlChildNode(const XmlNodePtr &node, size_t index) const
{
    // Return the nth child element of the given node, skipping anything that is
    // not int he MathML namespace.

    XmlNodePtr res = node->firstChild();
    size_t childNodeIndex = res->isMathmlElement() ? 0 : MAX_SIZE_T;

    while ((res != nullptr) && (childNodeIndex != index)) {
        res = res->next();

        if (res && res->isMathmlElement()) {
            ++childNodeIndex;
        }
    }

    return res;
}

GeneratorInternalVariablePtr Generator::GeneratorImpl::generatorVariable(const VariablePtr &variable)
{
    // Find and return, if there is one, the generator variable associated with
    // the given variable.

    for (const auto &internalVariable : mInternalVariables) {
        if (sameOrEquivalentVariable(variable, internalVariable->mVariable)) {
            return internalVariable;
        }
    }

    // No generator variable exists for the given variable, so create one, track
    // it and return it.

    GeneratorInternalVariablePtr internalVariable = std::make_shared<GeneratorInternalVariable>(variable);

    mInternalVariables.push_back(internalVariable);

    return internalVariable;
}

GeneratorVariablePtr Generator::GeneratorImpl::variableFirstOccurrence(const VariablePtr &variable,
                                                                       const ComponentPtr &component)
{
    // Recursively look for the first occurrence of the given variable in the
    // given component.

    GeneratorVariablePtr voi = nullptr;

    for (size_t i = 0; i < component->variableCount() && voi == nullptr; ++i) {
        VariablePtr testVariable = component->variable(i);

        if (sameOrEquivalentVariable(variable, testVariable)) {
            voi = GeneratorVariable::create();

            voi->mPimpl->populate(testVariable, testVariable, GeneratorVariable::Type::VARIABLE_OF_INTEGRATION);
        }
    }

    for (size_t i = 0; i < component->componentCount() && voi == nullptr; ++i) {
        voi = variableFirstOccurrence(variable, component->component(i));
    }

    return voi;
}

void Generator::GeneratorImpl::processNode(const XmlNodePtr &node,
                                           GeneratorEquationAstPtr &ast,
                                           const GeneratorEquationAstPtr &astParent,
                                           const ComponentPtr &component,
                                           const GeneratorEquationPtr &equation)
{
    // Basic content elements.

    if (node->isMathmlElement("apply")) {
        // We may have 2, 3 or more child nodes, e.g.
        //
        //                 +--------+
        //                 |   +    |
        //        "+a" ==> |  / \   |
        //                 | a  nil |
        //                 +--------+
        //
        //                 +-------+
        //                 |   +   |
        //       "a+b" ==> |  / \  |
        //                 | a   b |
        //                 +-------+
        //
        //                 +-------------+
        //                 |   +         |
        //                 |  / \        |
        //                 | a   +       |
        //                 |    / \      |
        // "a+b+c+d+e" ==> |   b   +     |
        //                 |      / \    |
        //                 |     c   +   |
        //                 |        / \  |
        //                 |       d   e |
        //                 +-------------+

        size_t childCount = mathmlChildCount(node);

        processNode(mathmlChildNode(node, 0), ast, astParent, component, equation);
        processNode(mathmlChildNode(node, 1), ast->mLeft, ast, component, equation);

        if (childCount >= 3) {
            GeneratorEquationAstPtr astRight;
            GeneratorEquationAstPtr tempAst;

            processNode(mathmlChildNode(node, childCount - 1), astRight, nullptr, component, equation);

            for (size_t i = childCount - 2; i > 1; --i) {
                processNode(mathmlChildNode(node, 0), tempAst, nullptr, component, equation);
                processNode(mathmlChildNode(node, i), tempAst->mLeft, tempAst, component, equation);

                astRight->mParent = tempAst;

                tempAst->mRight = astRight;
                astRight = tempAst;
            }

            if (astRight != nullptr) {
                astRight->mParent = ast;
            }

            ast->mRight = astRight;
        }

        // Assignment, and relational and logical operators.

    } else if (node->isMathmlElement("eq")) {
        // This element is used both to describe "a = b" and "a == b". We can
        // distinguish between the two by checking its grand-parent. If it's a
        // "math" element then it means that it is used to describe "a = b"
        // otherwise it is used to describe "a == b". In the former case, there
        // is nothing more we need to do since `ast` is already of
        // GeneratorEquationAst::Type::ASSIGNMENT type.

        if (!node->parent()->parent()->isMathmlElement("math")) {
            ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::EQ, astParent);

            mNeedEq = true;
        }
    } else if (node->isMathmlElement("neq")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::NEQ, astParent);

        mNeedNeq = true;
    } else if (node->isMathmlElement("lt")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::LT, astParent);

        mNeedLt = true;
    } else if (node->isMathmlElement("leq")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::LEQ, astParent);

        mNeedLeq = true;
    } else if (node->isMathmlElement("gt")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::GT, astParent);

        mNeedGt = true;
    } else if (node->isMathmlElement("geq")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::GEQ, astParent);

        mNeedGeq = true;
    } else if (node->isMathmlElement("and")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::AND, astParent);

        mNeedAnd = true;
    } else if (node->isMathmlElement("or")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::OR, astParent);

        mNeedOr = true;
    } else if (node->isMathmlElement("xor")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::XOR, astParent);

        mNeedXor = true;
    } else if (node->isMathmlElement("not")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::NOT, astParent);

        mNeedNot = true;

        // Arithmetic operators.

    } else if (node->isMathmlElement("plus")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::PLUS, astParent);
    } else if (node->isMathmlElement("minus")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::MINUS, astParent);
    } else if (node->isMathmlElement("times")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::TIMES, astParent);
    } else if (node->isMathmlElement("divide")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::DIVIDE, astParent);
    } else if (node->isMathmlElement("power")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::POWER, astParent);
    } else if (node->isMathmlElement("root")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ROOT, astParent);
    } else if (node->isMathmlElement("abs")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ABS, astParent);
    } else if (node->isMathmlElement("exp")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::EXP, astParent);
    } else if (node->isMathmlElement("ln")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::LN, astParent);
    } else if (node->isMathmlElement("log")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::LOG, astParent);
    } else if (node->isMathmlElement("ceiling")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CEILING, astParent);
    } else if (node->isMathmlElement("floor")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::FLOOR, astParent);
    } else if (node->isMathmlElement("min")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::MIN, astParent);

        mNeedMin = true;
    } else if (node->isMathmlElement("max")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::MAX, astParent);

        mNeedMax = true;
    } else if (node->isMathmlElement("rem")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::REM, astParent);

        // Calculus elements.

    } else if (node->isMathmlElement("diff")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::DIFF, astParent);

        // Trigonometric operators.

    } else if (node->isMathmlElement("sin")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::SIN, astParent);
    } else if (node->isMathmlElement("cos")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::COS, astParent);
    } else if (node->isMathmlElement("tan")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::TAN, astParent);
    } else if (node->isMathmlElement("sec")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::SEC, astParent);

        mNeedSec = true;
    } else if (node->isMathmlElement("csc")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CSC, astParent);

        mNeedCsc = true;
    } else if (node->isMathmlElement("cot")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::COT, astParent);

        mNeedCot = true;
    } else if (node->isMathmlElement("sinh")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::SINH, astParent);
    } else if (node->isMathmlElement("cosh")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::COSH, astParent);
    } else if (node->isMathmlElement("tanh")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::TANH, astParent);
    } else if (node->isMathmlElement("sech")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::SECH, astParent);

        mNeedSech = true;
    } else if (node->isMathmlElement("csch")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CSCH, astParent);

        mNeedCsch = true;
    } else if (node->isMathmlElement("coth")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::COTH, astParent);

        mNeedCoth = true;
    } else if (node->isMathmlElement("arcsin")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ASIN, astParent);
    } else if (node->isMathmlElement("arccos")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ACOS, astParent);
    } else if (node->isMathmlElement("arctan")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ATAN, astParent);
    } else if (node->isMathmlElement("arcsec")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ASEC, astParent);

        mNeedAsec = true;
    } else if (node->isMathmlElement("arccsc")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ACSC, astParent);

        mNeedAcsc = true;
    } else if (node->isMathmlElement("arccot")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ACOT, astParent);

        mNeedAcot = true;
    } else if (node->isMathmlElement("arcsinh")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ASINH, astParent);
    } else if (node->isMathmlElement("arccosh")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ACOSH, astParent);
    } else if (node->isMathmlElement("arctanh")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ATANH, astParent);
    } else if (node->isMathmlElement("arcsech")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ASECH, astParent);

        mNeedAsech = true;
    } else if (node->isMathmlElement("arccsch")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ACSCH, astParent);

        mNeedAcsch = true;
    } else if (node->isMathmlElement("arccoth")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::ACOTH, astParent);

        mNeedAcoth = true;

        // Piecewise statement.

    } else if (node->isMathmlElement("piecewise")) {
        size_t childCount = mathmlChildCount(node);

        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::PIECEWISE, astParent);

        processNode(mathmlChildNode(node, 0), ast->mLeft, ast, component, equation);

        if (childCount >= 2) {
            GeneratorEquationAstPtr astRight;
            GeneratorEquationAstPtr tempAst;

            processNode(mathmlChildNode(node, childCount - 1), astRight, nullptr, component, equation);

            for (size_t i = childCount - 2; i > 0; --i) {
                tempAst = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::PIECEWISE, astParent);

                processNode(mathmlChildNode(node, i), tempAst->mLeft, tempAst, component, equation);

                astRight->mParent = tempAst;

                tempAst->mRight = astRight;
                astRight = tempAst;
            }

            astRight->mParent = ast;

            ast->mRight = astRight;
        }
    } else if (node->isMathmlElement("piece")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::PIECE, astParent);

        processNode(mathmlChildNode(node, 0), ast->mLeft, ast, component, equation);
        processNode(mathmlChildNode(node, 1), ast->mRight, ast, component, equation);
    } else if (node->isMathmlElement("otherwise")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::OTHERWISE, astParent);

        processNode(mathmlChildNode(node, 0), ast->mLeft, ast, component, equation);

        // Token elements.

    } else if (node->isMathmlElement("ci")) {
        std::string variableName = node->firstChild()->convertToString();
        VariablePtr variable = component->variable(variableName);

        if (variable != nullptr) {
            // Have our equation track the (ODE) variable (by ODE variable, we
            // mean a variable that is used in a "diff" element).

            if (node->parent()->firstChild()->isMathmlElement("diff")) {
                equation->addOdeVariable(generatorVariable(variable));
            } else if (!(node->parent()->isMathmlElement("bvar")
                         && node->parent()->parent()->firstChild()->isMathmlElement("diff"))) {
                equation->addVariable(generatorVariable(variable));
            }

            // Add the variable to our AST.

            ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CI, variable, astParent);
        } else {
            ErrorPtr err = Error::create();

            err->setDescription("Variable '" + variableName
                                + "' in component '" + component->name()
                                + "' is referenced in an equation, but it is not defined anywhere.");
            err->setKind(Error::Kind::GENERATOR);

            mGenerator->addError(err);
        }
    } else if (node->isMathmlElement("cn")) {
        if (mathmlChildCount(node) == 1) {
            // We are dealing with an e-notation based CN value.

            ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CN, node->firstChild()->convertToString() + "e" + node->firstChild()->next()->next()->convertToString(), astParent);
        } else {
            ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CN, node->firstChild()->convertToString(), astParent);
        }

        // Qualifier elements.

    } else if (node->isMathmlElement("degree")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::DEGREE, astParent);

        processNode(mathmlChildNode(node, 0), ast->mLeft, ast, component, equation);
    } else if (node->isMathmlElement("logbase")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::LOGBASE, astParent);

        processNode(mathmlChildNode(node, 0), ast->mLeft, ast, component, equation);
    } else if (node->isMathmlElement("bvar")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::BVAR, astParent);

        processNode(mathmlChildNode(node, 0), ast->mLeft, ast, component, equation);

        XmlNodePtr rightNode = mathmlChildNode(node, 1);

        if (rightNode != nullptr) {
            processNode(rightNode, ast->mRight, ast, component, equation);
        }

        // Constants.

    } else if (node->isMathmlElement("true")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::TRUE, astParent);
    } else if (node->isMathmlElement("false")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::FALSE, astParent);
    } else if (node->isMathmlElement("exponentiale")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::E, astParent);
    } else if (node->isMathmlElement("pi")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::PI, astParent);
    } else if (node->isMathmlElement("infinity")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::INF, astParent);
    } else if (node->isMathmlElement("notanumber")) {
        ast = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::NAN, astParent);
    }
}

GeneratorEquationPtr Generator::GeneratorImpl::processNode(const XmlNodePtr &node,
                                                           const ComponentPtr &component)
{
    // Create and keep track of the equation associated with the given node.

    GeneratorEquationPtr equation = std::make_shared<GeneratorEquation>(component);

    mEquations.push_back(equation);

    // Actually process the node and return its corresponding equation.

    processNode(node, equation->mAst, equation->mAst->mParent.lock(), component, equation);

    return equation;
}

void Generator::GeneratorImpl::processComponent(const ComponentPtr &component)
{
    // Retrieve the math string associated with the given component and process
    // it, one equation at a time.

    XmlDocPtr xmlDoc = std::make_shared<XmlDoc>();
    std::string math = component->math();

    if (!math.empty()) {
        xmlDoc->parseMathML(math, false);

        XmlNodePtr mathNode = xmlDoc->rootNode();

        for (XmlNodePtr node = mathNode->firstChild(); node != nullptr; node = node->next()) {
            if (node->isMathmlElement()) {
                processNode(node, component);
            }
        }
    }

    // Go through the given component's variables and make sure that everything
    // makes sense.

    for (size_t i = 0; i < component->variableCount(); ++i) {
        // Retrieve the variable's corresponding generator variable.

        VariablePtr variable = component->variable(i);
        GeneratorInternalVariablePtr generatorVariable = Generator::GeneratorImpl::generatorVariable(variable);

        // Replace the variable held by `generatorVariable`, in case the
        // existing one has no initial value while `variable` does. Otherwise,
        // generate an error if the variable held by `generatorVariable` and
        // `variable` are both initialised.

        if (!variable->initialValue().empty()
            && generatorVariable->mVariable->initialValue().empty()) {
            generatorVariable->setVariable(variable);
        } else if ((variable != generatorVariable->mVariable)
                   && !variable->initialValue().empty()
                   && !generatorVariable->mVariable->initialValue().empty()) {
            ComponentPtr trackedVariableComponent = std::dynamic_pointer_cast<Component>(generatorVariable->mVariable->parent());
            ErrorPtr err = Error::create();

            err->setDescription("Variable '" + variable->name()
                                + "' in component '" + component->name()
                                + "' and variable '" + generatorVariable->mVariable->name()
                                + "' in component '" + trackedVariableComponent->name()
                                + "' are equivalent and cannot therefore both be initialised.");
            err->setKind(Error::Kind::GENERATOR);

            mGenerator->addError(err);
        }
    }

    // Do the same for the components encapsulated by the given component.

    for (size_t i = 0; i < component->componentCount(); ++i) {
        processComponent(component->component(i));
    }
}

void Generator::GeneratorImpl::processEquationAst(const GeneratorEquationAstPtr &ast)
{
    // Look for the definition of a variable of integration and make sure that
    // we don't have more than one of it and that it's not initialised.

    GeneratorEquationAstPtr astParent = ast->mParent.lock();
    GeneratorEquationAstPtr astGrandParent = (astParent != nullptr) ? astParent->mParent.lock() : nullptr;
    GeneratorEquationAstPtr astGreatGrandParent = (astGrandParent != nullptr) ? astGrandParent->mParent.lock() : nullptr;

    if ((ast->mType == GeneratorEquationAst::Type::CI)
        && (astParent != nullptr) && (astParent->mType == GeneratorEquationAst::Type::BVAR)
        && (astGrandParent != nullptr) && (astGrandParent->mType == GeneratorEquationAst::Type::DIFF)) {
        VariablePtr variable = ast->mVariable;

        generatorVariable(variable)->makeVoi();
        // Note: we must make the variable a variable of integration in all
        //       cases (i.e. even if there is, for example, already another
        //       variable of integration) otherwise unnecessary error messages
        //       may be reported (since the type of the variable would be
        //       unknown).

        if (mVoi == nullptr) {
            // Before keeping track of the variable of integration, make sure
            // that it is not initialised.

            if (!variable->initialValue().empty()) {
                ComponentPtr component = std::dynamic_pointer_cast<Component>(variable->parent());
                ErrorPtr err = Error::create();

                err->setDescription("Variable '" + variable->name()
                                    + "' in component '" + component->name()
                                    + "' cannot be both a variable of integration and initialised.");
                err->setKind(Error::Kind::GENERATOR);

                mGenerator->addError(err);
            } else {
                // We have found our variable of integration, but this may not
                // be the one defined in our first component (i.e. the component
                // under which we are likely to expect to see the variable of
                // integration to be defined), so go through our components and
                // look for the first occurrence of our variable of integration.

                ModelPtr model = owningModel(variable->parent());

                for (size_t i = 0; i < model->componentCount(); ++i) {
                    GeneratorVariablePtr voi = variableFirstOccurrence(variable, model->component(i));

                    if (voi != nullptr) {
                        mVoi = voi;

                        break;
                    }
                }
            }
        } else if (!sameOrEquivalentVariable(variable, mVoi->variable())) {
            ComponentPtr voiComponent = std::dynamic_pointer_cast<Component>(mVoi->variable()->parent());
            ComponentPtr component = std::dynamic_pointer_cast<Component>(variable->parent());
            ErrorPtr err = Error::create();

            err->setDescription("Variable '" + mVoi->variable()->name()
                                + "' in component '" + voiComponent->name()
                                + "' and variable '" + variable->name()
                                + "' in component '" + component->name()
                                + "' cannot both be a variable of integration.");
            err->setKind(Error::Kind::GENERATOR);

            mGenerator->addError(err);
        }
    }

    // Make sure that we only use first-order ODEs.

    if ((ast->mType == GeneratorEquationAst::Type::CN)
        && (astParent != nullptr) && (astParent->mType == GeneratorEquationAst::Type::DEGREE)
        && (astGrandParent != nullptr) && (astGrandParent->mType == GeneratorEquationAst::Type::BVAR)
        && (astGreatGrandParent != nullptr) && (astGreatGrandParent->mType == GeneratorEquationAst::Type::DIFF)) {
        if (convertToDouble(ast->mValue) != 1.0) {
            VariablePtr variable = astGreatGrandParent->mRight->mVariable;
            ComponentPtr component = std::dynamic_pointer_cast<Component>(variable->parent());
            ErrorPtr err = Error::create();

            err->setDescription("The differential equation for variable '" + variable->name()
                                + "' in component '" + component->name()
                                + "' must be of the first order.");
            err->setKind(Error::Kind::GENERATOR);

            mGenerator->addError(err);
        }
    }

    // Make a variable a state if it is used in an ODE.

    if ((ast->mType == GeneratorEquationAst::Type::CI)
        && (astParent != nullptr) && (astParent->mType == GeneratorEquationAst::Type::DIFF)) {
        generatorVariable(ast->mVariable)->makeState();
    }

    // Recursively check the given AST's children.

    if (ast->mLeft != nullptr) {
        processEquationAst(ast->mLeft);
    }

    if (ast->mRight != nullptr) {
        processEquationAst(ast->mRight);
    }
}

double Generator::GeneratorImpl::scalingFactor(const VariablePtr &variable)
{
    // Return the scaling factor for the given variable.

    return Units::scalingFactor(variable->units(),
                                generatorVariable(variable)->mVariable->units());
}

void Generator::GeneratorImpl::scaleEquationAst(const GeneratorEquationAstPtr &ast,
                                                bool debug, int eqnNb)
{
    // Recursively scale the given AST's children.

    if (ast->mLeft != nullptr) {
        scaleEquationAst(ast->mLeft, debug, eqnNb);
    }

    if (ast->mRight != nullptr) {
        scaleEquationAst(ast->mRight, debug, eqnNb);
    }

    // If the given AST node is a variabe (i.e. a CI node) then we may need to
    // do some scaling.

    if (ast->mType == GeneratorEquationAst::Type::CI) {
        // The kind of scaling we may end up doing depends on whether we are
        // dealing with a rate or some other variable, i.e. whether or not it
        // has a DIFF node as a parent.

        GeneratorEquationAstPtr astParent = ast->mParent.lock();
if (debug && (eqnNb == 1)) {
std::cout << "Variable: " << ast->mVariable->name()
          << " | ASSIGNMENT: " << ((astParent->mType == GeneratorEquationAst::Type::ASSIGNMENT)?"YES":"NO")
          << " | Parent->left: " << ((astParent->mLeft == ast)?"YES":"NO")
          << " | DIFF: " << ((astParent->mType == GeneratorEquationAst::Type::DIFF)?"YES":"NO")
          << " | BVAR: " << ((astParent->mType == GeneratorEquationAst::Type::BVAR)?"YES":"NO")
          << " | Scaling factor: " << Generator::GeneratorImpl::scalingFactor(ast->mVariable)
          << " | Crt unit: " << ast->mVariable->units()->name()
          << " | Ref unit: " << generatorVariable(ast->mVariable)->mVariable->units()->name()
          << " | Crt comp: " << entityName(ast->mVariable->parent())
          << " | Ref comp: " << entityName(generatorVariable(ast->mVariable)->mVariable)
          << std::endl;
}

        if (astParent->mType == GeneratorEquationAst::Type::DIFF) {
            // We are dealing with a rate, so retrieve the scaling factor for
            // its corresponding variable of integration and apply it, if
            // needed.

            double scalingFactor = Generator::GeneratorImpl::scalingFactor(astParent->mLeft->mLeft->mVariable);

            if (!areEqual(scalingFactor, 1.0)) {
                // We need to scale, but how we do it depends on whether the
                // rate is to be computed or used.

                GeneratorEquationAstPtr astGrandParent = astParent->mParent.lock();

                if ((astGrandParent->mType == GeneratorEquationAst::Type::ASSIGNMENT)
                    && (astGrandParent->mLeft == astParent)) {
                    // The rate is to be computed, so apply the scaling factor
                    // to the RHS of the equation.

                    GeneratorEquationAstPtr rhsAst = astGrandParent->mRight;
                    GeneratorEquationAstPtr scaledAst = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::TIMES, astGrandParent);

                    scaledAst->mLeft = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CN, convertToString(scalingFactor), scaledAst);
                    scaledAst->mRight = rhsAst;

                    rhsAst->mParent = astGrandParent;

                    astGrandParent->mRight = scaledAst;
                } else {
                    // The rate is to be used, so scale it using the inverse of
                    // the scaling factor.

                    GeneratorEquationAstPtr scaledAst = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::TIMES, astGrandParent);

                    scaledAst->mLeft = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CN, convertToString(1.0 / scalingFactor), scaledAst);
                    scaledAst->mRight = astParent;

                    astParent->mParent = scaledAst;

                    if (astGrandParent->mLeft == astParent) {
                        astGrandParent->mLeft = scaledAst;
                    } else {
                        astGrandParent->mRight = scaledAst;
                    }
                }
            }
        } else if (astParent->mType != GeneratorEquationAst::Type::BVAR) {
            // We are dealing with a variable which is not our variable of
            // integration, so retrieve its scaling factor and apply it, if
            // needed.

            double scalingFactor = Generator::GeneratorImpl::scalingFactor(ast->mVariable);

            if (!areEqual(scalingFactor, 1.0)) {
                GeneratorEquationAstPtr scaledAst = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::TIMES, astParent);

                scaledAst->mLeft = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CN, convertToString(scalingFactor), scaledAst);
                scaledAst->mRight = ast;

                ast->mParent = scaledAst;

                if (astParent->mLeft == ast) {
                    astParent->mLeft = scaledAst;
                } else {
                    astParent->mRight = scaledAst;
                }
            }
        }
    }
}

void Generator::GeneratorImpl::printEquationsAst() const
{
    // Print our equations' AST.
    // Note: this method should be deleted once we are done with issue #409.

    size_t eqnNb = 0;

    for (const auto &equation : mEquations) {
        ++eqnNb;

        if (eqnNb == 1) {
            std::cout << "────────────────────────────────────┤Equation #" << eqnNb << "├───" << std::endl;

            printAst(equation->mAst);
        }
    }

    std::cout << "────────────────────────────────────┤THE END!├───" << std::endl;
}

void Generator::GeneratorImpl::processModel(const ModelPtr &model, bool debug)
{
    // Reset a few things in case we were to process the model more than once.
    // Note: one would normally process the model only once, so we shouldn't
    //       need to do this, but better be safe than sorry.

    mModelType = Generator::ModelType::UNKNOWN;

    mInternalVariables.clear();
    mEquations.clear();

    mVoi = nullptr;
    mStates.clear();
    mVariables.clear();

    mNeedMin = false;
    mNeedMax = false;

    mNeedSec = false;
    mNeedCsc = false;
    mNeedCot = false;
    mNeedSech = false;
    mNeedCsch = false;
    mNeedCoth = false;
    mNeedAsec = false;
    mNeedAcsc = false;
    mNeedAcot = false;
    mNeedAsech = false;
    mNeedAcsch = false;
    mNeedAcoth = false;

    mGenerator->removeAllErrors();

    // Recursively process the model's components, so that we end up with an AST
    // for each of the model's equations.

    for (size_t i = 0; i < model->componentCount(); ++i) {
        processComponent(model->component(i));
    }

    // Some more processing is needed, but it can only be done if we didn't come
    // across any errors during the processing of our components.

    if (mGenerator->errorCount() == 0) {
        // Process our different equations' AST to determine the type of our
        // variables.

        for (const auto &equation : mEquations) {
            processEquationAst(equation->mAst);
        }
    }

    // Some post-processing is now needed, but it can only be done if we didn't
    // come across any errors during the processing of our equations' AST.

    if (mGenerator->errorCount() == 0) {
        // Sort our variables, determine the index of our constant variables and
        // then loop over our equations, checking which variables, if any, can
        // be determined using a given equation.

        mInternalVariables.sort(compareVariablesByName);

        size_t variableIndex = MAX_SIZE_T;

        for (const auto &internalVariable : mInternalVariables) {
            if (internalVariable->mType == GeneratorInternalVariable::Type::CONSTANT) {
                internalVariable->mIndex = ++variableIndex;
            }
        }

        size_t equationOrder = MAX_SIZE_T;
        size_t stateIndex = MAX_SIZE_T;

        for (;;) {
            bool relevantCheck = false;

            for (const auto &equation : mEquations) {
                relevantCheck = equation->check(equationOrder, stateIndex, variableIndex)
                                || relevantCheck;
            }

            if (!relevantCheck) {
                break;
            }
        }

        // Make sure that our variables are valid.

        for (const auto &internalVariable : mInternalVariables) {
            std::string errorType;

            switch (internalVariable->mType) {
            case GeneratorInternalVariable::Type::UNKNOWN:
                errorType = "is not computed";

                break;
            case GeneratorInternalVariable::Type::SHOULD_BE_STATE:
                errorType = "is used in an ODE, but it is not initialised";

                break;
            case GeneratorInternalVariable::Type::VARIABLE_OF_INTEGRATION:
            case GeneratorInternalVariable::Type::STATE:
            case GeneratorInternalVariable::Type::CONSTANT:
            case GeneratorInternalVariable::Type::COMPUTED_TRUE_CONSTANT:
            case GeneratorInternalVariable::Type::COMPUTED_VARIABLE_BASED_CONSTANT:
            case GeneratorInternalVariable::Type::ALGEBRAIC:
                break;
            case GeneratorInternalVariable::Type::OVERCONSTRAINED:
                errorType = "is computed more than once";

                break;
            }

            if (!errorType.empty()) {
                ErrorPtr err = Error::create();
                VariablePtr realVariable = internalVariable->mVariable;
                ComponentPtr realComponent = std::dynamic_pointer_cast<Component>(realVariable->parent());

                err->setDescription("Variable '" + realVariable->name()
                                    + "' in component '" + realComponent->name()
                                    + "' " + errorType + ".");
                err->setKind(Error::Kind::GENERATOR);

                mGenerator->addError(err);
            }
        }

        // Determine the type of our model.

        bool hasUnderconstrainedVariables = std::find_if(mInternalVariables.begin(), mInternalVariables.end(), [](const GeneratorInternalVariablePtr &variable) {
                                                return (variable->mType == GeneratorInternalVariable::Type::UNKNOWN)
                                                       || (variable->mType == GeneratorInternalVariable::Type::SHOULD_BE_STATE);
                                            })
                                            != std::end(mInternalVariables);
        bool hasOverconstrainedVariables = std::find_if(mInternalVariables.begin(), mInternalVariables.end(), [](const GeneratorInternalVariablePtr &variable) {
                                               return variable->mType == GeneratorInternalVariable::Type::OVERCONSTRAINED;
                                           })
                                           != std::end(mInternalVariables);

        if (hasUnderconstrainedVariables) {
            if (hasOverconstrainedVariables) {
                mModelType = Generator::ModelType::UNSUITABLY_CONSTRAINED;
            } else {
                mModelType = Generator::ModelType::UNDERCONSTRAINED;
            }
        } else if (hasOverconstrainedVariables) {
            mModelType = Generator::ModelType::OVERCONSTRAINED;
        } else if (mVoi != nullptr) {
            mModelType = Generator::ModelType::ODE;
        } else if (!mInternalVariables.empty()) {
            mModelType = Generator::ModelType::ALGEBRAIC;
        }
    } else {
        mModelType = Generator::ModelType::INVALID;
    }

    // Some final post-processing is now needed, if we have a valid model.

    if ((mModelType == Generator::ModelType::ODE)
        || (mModelType == Generator::ModelType::ALGEBRAIC)) {
        // Print our equations' AST.

        if (debug) {
            printEquationsAst();
        }

        // Scale our equations' AST, i.e. take into account the fact that we may
        // have mapped variables that use compatible units rather than
        // equivalent ones.

        int eqNb = 0;

        for (const auto &equation : mEquations) {
            if (debug) {
                ++eqNb;
            }

            scaleEquationAst(equation->mAst, debug, eqNb);
        }

        // Print our updated equations' AST.

        if (debug) {
            printEquationsAst();
        }

        // Sort our variables and equations and make our internal variables
        // available through our API.

        mInternalVariables.sort(compareVariablesByTypeAndIndex);
        mEquations.sort(compareEquationsByVariable);

        for (const auto &internalVariable : mInternalVariables) {
            GeneratorVariable::Type type;

            if (internalVariable->mType == GeneratorInternalVariable::Type::STATE) {
                type = GeneratorVariable::Type::STATE;
            } else if (internalVariable->mType == GeneratorInternalVariable::Type::CONSTANT) {
                type = GeneratorVariable::Type::CONSTANT;
            } else if ((internalVariable->mType == GeneratorInternalVariable::Type::COMPUTED_TRUE_CONSTANT)
                       || (internalVariable->mType == GeneratorInternalVariable::Type::COMPUTED_VARIABLE_BASED_CONSTANT)) {
                type = GeneratorVariable::Type::COMPUTED_CONSTANT;
            } else if (internalVariable->mType == GeneratorInternalVariable::Type::ALGEBRAIC) {
                type = GeneratorVariable::Type::ALGEBRAIC;
            } else {
                // This is the variable of integration, so skip it.

                continue;
            }

            GeneratorVariablePtr stateOrVariable = GeneratorVariable::create();

            stateOrVariable->mPimpl->populate(internalVariable->mInitialValueVariable,
                                              internalVariable->mVariable,
                                              type);

            if (type == GeneratorVariable::Type::STATE) {
                mStates.push_back(stateOrVariable);
            } else {
                mVariables.push_back(stateOrVariable);
            }
        }
    }
}

bool Generator::GeneratorImpl::isRelationalOperator(const GeneratorEquationAstPtr &ast) const
{
    return ((ast->mType == GeneratorEquationAst::Type::EQ)
            && mProfile->hasEqOperator())
           || ((ast->mType == GeneratorEquationAst::Type::NEQ)
               && mProfile->hasNeqOperator())
           || ((ast->mType == GeneratorEquationAst::Type::LT)
               && mProfile->hasLtOperator())
           || ((ast->mType == GeneratorEquationAst::Type::LEQ)
               && mProfile->hasLeqOperator())
           || ((ast->mType == GeneratorEquationAst::Type::GT)
               && mProfile->hasGtOperator())
           || ((ast->mType == GeneratorEquationAst::Type::GEQ)
               && mProfile->hasGeqOperator());
}

bool Generator::GeneratorImpl::isAndOperator(const GeneratorEquationAstPtr &ast) const
{
    return (ast->mType == GeneratorEquationAst::Type::AND)
           && mProfile->hasAndOperator();
}

bool Generator::GeneratorImpl::isOrOperator(const GeneratorEquationAstPtr &ast) const
{
    return (ast->mType == GeneratorEquationAst::Type::OR)
           && mProfile->hasOrOperator();
}

bool Generator::GeneratorImpl::isXorOperator(const GeneratorEquationAstPtr &ast) const
{
    return (ast->mType == GeneratorEquationAst::Type::XOR)
           && mProfile->hasXorOperator();
}

bool Generator::GeneratorImpl::isLogicalOperator(const GeneratorEquationAstPtr &ast) const
{
    // Note: GeneratorEquationAst::Type::NOT is a unary logical operator, hence
    //       we don't include it here since this method is only used to
    //       determine whether parentheses should be added around some code.

    return isAndOperator(ast) || isOrOperator(ast) || isXorOperator(ast);
}

bool Generator::GeneratorImpl::isPlusOperator(const GeneratorEquationAstPtr &ast) const
{
    return ast->mType == GeneratorEquationAst::Type::PLUS;
}

bool Generator::GeneratorImpl::isMinusOperator(const GeneratorEquationAstPtr &ast) const
{
    return ast->mType == GeneratorEquationAst::Type::MINUS;
}

bool Generator::GeneratorImpl::isTimesOperator(const GeneratorEquationAstPtr &ast) const
{
    return ast->mType == GeneratorEquationAst::Type::TIMES;
}

bool Generator::GeneratorImpl::isDivideOperator(const GeneratorEquationAstPtr &ast) const
{
    return ast->mType == GeneratorEquationAst::Type::DIVIDE;
}

bool Generator::GeneratorImpl::isPowerOperator(const GeneratorEquationAstPtr &ast) const
{
    return (ast->mType == GeneratorEquationAst::Type::POWER)
           && mProfile->hasPowerOperator();
}

bool Generator::GeneratorImpl::isRootOperator(const GeneratorEquationAstPtr &ast) const
{
    return (ast->mType == GeneratorEquationAst::Type::ROOT)
           && mProfile->hasPowerOperator();
}

bool Generator::GeneratorImpl::isPiecewiseStatement(const GeneratorEquationAstPtr &ast) const
{
    return (ast->mType == GeneratorEquationAst::Type::PIECEWISE)
           && mProfile->hasConditionalOperator();
}

std::string Generator::GeneratorImpl::replace(std::string string,
                                              const std::string &from,
                                              const std::string &to)
{
    auto index = string.find(from);

    return (index == std::string::npos) ?
               string :
               string.replace(index, from.length(), to);
}

void Generator::GeneratorImpl::updateVariableInfoSizes(size_t &componentSize,
                                                       size_t &nameSize,
                                                       size_t &unitsSize,
                                                       const GeneratorVariablePtr &variable)
{
    auto variableComponentSize = entityName(variable->variable()->parent()).length() + 1;
    auto variableNameSize = variable->variable()->name().length() + 1;
    auto variableUnitsSize = variable->variable()->units()->name().length() + 1;
    // Note: +1 to account for the end of string termination.

    componentSize = (componentSize > variableComponentSize) ? componentSize : variableComponentSize;
    nameSize = (nameSize > variableNameSize) ? nameSize : variableNameSize;
    unitsSize = (unitsSize > variableUnitsSize) ? unitsSize : variableUnitsSize;
}

bool Generator::GeneratorImpl::modifiedProfile() const
{
    // Whether the profile requires an interface to be generated.

    const std::string trueValue = "true";
    const std::string falseValue = "false";

    std::string profileContents = mProfile->hasInterface() ?
                                      trueValue :
                                      falseValue;

    // Assignment.

    profileContents += mProfile->assignmentString();

    // Relational and logical operators.

    profileContents += mProfile->eqString()
                       + mProfile->neqString()
                       + mProfile->ltString()
                       + mProfile->leqString()
                       + mProfile->gtString()
                       + mProfile->geqString()
                       + mProfile->andString()
                       + mProfile->orString()
                       + mProfile->xorString()
                       + mProfile->notString();

    profileContents += (mProfile->hasEqOperator() ?
                            trueValue :
                            falseValue)
                       + (mProfile->hasNeqOperator() ?
                              trueValue :
                              falseValue)
                       + (mProfile->hasLtOperator() ?
                              trueValue :
                              falseValue)
                       + (mProfile->hasLeqOperator() ?
                              trueValue :
                              falseValue)
                       + (mProfile->hasGtOperator() ?
                              trueValue :
                              falseValue)
                       + (mProfile->hasGeqOperator() ?
                              trueValue :
                              falseValue)
                       + (mProfile->hasAndOperator() ?
                              trueValue :
                              falseValue)
                       + (mProfile->hasOrOperator() ?
                              trueValue :
                              falseValue)
                       + (mProfile->hasXorOperator() ?
                              trueValue :
                              falseValue)
                       + (mProfile->hasNotOperator() ?
                              trueValue :
                              falseValue);

    // Arithmetic operators.

    profileContents += mProfile->plusString()
                       + mProfile->minusString()
                       + mProfile->timesString()
                       + mProfile->divideString()
                       + mProfile->powerString()
                       + mProfile->squareRootString()
                       + mProfile->squareString()
                       + mProfile->absoluteValueString()
                       + mProfile->exponentialString()
                       + mProfile->napierianLogarithmString()
                       + mProfile->commonLogarithmString()
                       + mProfile->ceilingString()
                       + mProfile->floorString()
                       + mProfile->minString()
                       + mProfile->maxString()
                       + mProfile->remString();

    profileContents += mProfile->hasPowerOperator() ?
                           trueValue :
                           falseValue;

    // Trigonometric operators.

    profileContents += mProfile->sinString()
                       + mProfile->cosString()
                       + mProfile->tanString()
                       + mProfile->secString()
                       + mProfile->cscString()
                       + mProfile->cotString()
                       + mProfile->sinhString()
                       + mProfile->coshString()
                       + mProfile->tanhString()
                       + mProfile->sechString()
                       + mProfile->cschString()
                       + mProfile->cothString()
                       + mProfile->asinString()
                       + mProfile->acosString()
                       + mProfile->atanString()
                       + mProfile->asecString()
                       + mProfile->acscString()
                       + mProfile->acotString()
                       + mProfile->asinhString()
                       + mProfile->acoshString()
                       + mProfile->atanhString()
                       + mProfile->asechString()
                       + mProfile->acschString()
                       + mProfile->acothString();

    // Piecewise statement.

    profileContents += mProfile->conditionalOperatorIfString()
                       + mProfile->conditionalOperatorElseString()
                       + mProfile->piecewiseIfString()
                       + mProfile->piecewiseElseString();

    profileContents += mProfile->hasConditionalOperator() ?
                           trueValue :
                           falseValue;

    // Constants.

    profileContents += mProfile->trueString()
                       + mProfile->falseString()
                       + mProfile->eString()
                       + mProfile->piString()
                       + mProfile->infString()
                       + mProfile->nanString();

    // Arithmetic functions.

    profileContents += mProfile->eqFunctionString()
                       + mProfile->neqFunctionString()
                       + mProfile->ltFunctionString()
                       + mProfile->leqFunctionString()
                       + mProfile->gtFunctionString()
                       + mProfile->geqFunctionString()
                       + mProfile->andFunctionString()
                       + mProfile->orFunctionString()
                       + mProfile->xorFunctionString()
                       + mProfile->notFunctionString()
                       + mProfile->minFunctionString()
                       + mProfile->maxFunctionString();

    // Trigonometric functions.

    profileContents += mProfile->secFunctionString()
                       + mProfile->cscFunctionString()
                       + mProfile->cotFunctionString()
                       + mProfile->sechFunctionString()
                       + mProfile->cschFunctionString()
                       + mProfile->cothFunctionString()
                       + mProfile->asecFunctionString()
                       + mProfile->acscFunctionString()
                       + mProfile->acotFunctionString()
                       + mProfile->asechFunctionString()
                       + mProfile->acschFunctionString()
                       + mProfile->acothFunctionString();

    // Miscellaneous.

    profileContents += mProfile->commentString()
                       + mProfile->originCommentString();

    profileContents += mProfile->interfaceFileNameString();

    profileContents += mProfile->interfaceHeaderString()
                       + mProfile->implementationHeaderString();

    profileContents += mProfile->interfaceVersionString()
                       + mProfile->implementationVersionString();

    profileContents += mProfile->interfaceLibcellmlVersionString()
                       + mProfile->implementationLibcellmlVersionString();

    profileContents += mProfile->interfaceStateCountString()
                       + mProfile->implementationStateCountString();

    profileContents += mProfile->interfaceVariableCountString()
                       + mProfile->implementationVariableCountString();

    profileContents += mProfile->variableTypeObjectString();

    profileContents += mProfile->constantVariableTypeString()
                       + mProfile->computedConstantVariableTypeString()
                       + mProfile->algebraicVariableTypeString();

    profileContents += mProfile->variableInfoObjectString()
                       + mProfile->variableInfoWithTypeObjectString();

    profileContents += mProfile->interfaceVoiInfoString()
                       + mProfile->implementationVoiInfoString();

    profileContents += mProfile->interfaceStateInfoString()
                       + mProfile->implementationStateInfoString();

    profileContents += mProfile->interfaceVariableInfoString()
                       + mProfile->implementationVariableInfoString();

    profileContents += mProfile->variableInfoEntryString()
                       + mProfile->variableInfoWithTypeEntryString();

    profileContents += mProfile->voiString();

    profileContents += mProfile->statesArrayString()
                       + mProfile->ratesArrayString()
                       + mProfile->variablesArrayString();

    profileContents += mProfile->interfaceCreateStatesArrayMethodString()
                       + mProfile->implementationCreateStatesArrayMethodString();

    profileContents += mProfile->interfaceCreateVariablesArrayMethodString()
                       + mProfile->implementationCreateVariablesArrayMethodString();

    profileContents += mProfile->interfaceDeleteArrayMethodString()
                       + mProfile->implementationDeleteArrayMethodString();

    profileContents += mProfile->interfaceInitializeStatesAndConstantsMethodString()
                       + mProfile->implementationInitializeStatesAndConstantsMethodString();

    profileContents += mProfile->interfaceComputeComputedConstantsMethodString()
                       + mProfile->implementationComputeComputedConstantsMethodString();

    profileContents += mProfile->interfaceComputeRatesMethodString()
                       + mProfile->implementationComputeRatesMethodString();

    profileContents += mProfile->interfaceComputeVariablesMethodString()
                       + mProfile->implementationComputeVariablesMethodString();

    profileContents += mProfile->emptyMethodString();

    profileContents += mProfile->indentString();

    profileContents += mProfile->openArrayInitializerString()
                       + mProfile->closeArrayInitializerString();

    profileContents += mProfile->openArrayString()
                       + mProfile->closeArrayString();

    profileContents += mProfile->arrayElementSeparatorString();

    profileContents += mProfile->stringDelimiterString();

    profileContents += mProfile->commandSeparatorString();

    // Compute and check the hash of our profile contents.

    bool res = false;
    std::string profileContentsSha1 = sha1(profileContents);

    switch (mProfile->profile()) {
    case GeneratorProfile::Profile::C:
        res = profileContentsSha1 != "e2aa9af2767ab84b217cf996c491c485ae876563";

        break;
    case GeneratorProfile::Profile::PYTHON:
        res = profileContentsSha1 != "1abb41ecb908526b51c2ac8c44bc9542942a9652";

        break;
    }

    return res;
}

void Generator::GeneratorImpl::addOriginCommentCode(std::string &code)
{
    if (!mProfile->commentString().empty()
        && !mProfile->originCommentString().empty()) {
        std::string profileInformation = modifiedProfile() ?
                                             "a modified " :
                                             "the ";

        switch (mProfile->profile()) {
        case GeneratorProfile::Profile::C:
            profileInformation += "C";

            break;
        case GeneratorProfile::Profile::PYTHON:
            profileInformation += "Python";

            break;
        }

        profileInformation += " profile of";

        std::string commentCode = replace(replace(mProfile->originCommentString(),
                                                  "<PROFILE_INFORMATION>", profileInformation),
                                          "<LIBCELLML_VERSION>", versionString());

        code += replace(mProfile->commentString(),
                        "<CODE>", commentCode);
    }
}

void Generator::GeneratorImpl::addInterfaceHeaderCode(std::string &code)
{
    if (!mProfile->interfaceHeaderString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->interfaceHeaderString();
    }
}

void Generator::GeneratorImpl::addImplementationHeaderCode(std::string &code)
{
    if (!mProfile->implementationHeaderString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += replace(mProfile->implementationHeaderString(),
                        "<INTERFACE_FILE_NAME>", mProfile->interfaceFileNameString());
    }
}

void Generator::GeneratorImpl::addVersionAndLibcellmlVersionCode(std::string &code,
                                                                 bool interface)
{
    std::string versionAndLibcellmlCode;

    if ((interface && !mProfile->interfaceVersionString().empty())
        || (!interface && !mProfile->implementationVersionString().empty())) {
        if (interface) {
            versionAndLibcellmlCode += mProfile->interfaceVersionString();
        } else {
            if (modifiedProfile()) {
                std::regex regEx("([0-9]+\\.[0-9]+\\.[0-9]+)");

                versionAndLibcellmlCode += std::regex_replace(mProfile->implementationVersionString(), regEx, "$1.post0");
            } else {
                versionAndLibcellmlCode += mProfile->implementationVersionString();
            }
        }
    }

    if ((interface && !mProfile->interfaceLibcellmlVersionString().empty())
        || (!interface && !mProfile->implementationLibcellmlVersionString().empty())) {
        versionAndLibcellmlCode += interface ?
                                       mProfile->interfaceLibcellmlVersionString() :
                                       replace(mProfile->implementationLibcellmlVersionString(),
                                               "<LIBCELLML_VERSION>", versionString());
    }

    if (!versionAndLibcellmlCode.empty()) {
        code += "\n";
    }

    code += versionAndLibcellmlCode;
}

void Generator::GeneratorImpl::addStateAndVariableCountCode(std::string &code,
                                                            bool interface)
{
    std::string stateAndVariableCountCode;

    if ((interface && !mProfile->interfaceStateCountString().empty())
        || (!interface && !mProfile->implementationStateCountString().empty())) {
        stateAndVariableCountCode += interface ?
                                         mProfile->interfaceStateCountString() :
                                         replace(mProfile->implementationStateCountString(),
                                                 "<STATE_COUNT>", std::to_string(mStates.size()));
    }

    if ((interface && !mProfile->interfaceVariableCountString().empty())
        || (!interface && !mProfile->implementationVariableCountString().empty())) {
        stateAndVariableCountCode += interface ?
                                         mProfile->interfaceVariableCountString() :
                                         replace(mProfile->implementationVariableCountString(),
                                                 "<VARIABLE_COUNT>", std::to_string(mVariables.size()));
    }

    if (!stateAndVariableCountCode.empty()) {
        code += "\n";
    }

    code += stateAndVariableCountCode;
}

void Generator::GeneratorImpl::addVariableTypeObjectCode(std::string &code)
{
    if (!mProfile->variableTypeObjectString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->variableTypeObjectString();
    }
}

std::string Generator::GeneratorImpl::generateVariableInfoObjectCode(const std::string &objectString)
{
    size_t componentSize = 0;
    size_t nameSize = 0;
    size_t unitsSize = 0;

    if (mVoi != nullptr) {
        updateVariableInfoSizes(componentSize, nameSize, unitsSize, mVoi);
    }

    for (const auto &state : mStates) {
        updateVariableInfoSizes(componentSize, nameSize, unitsSize, state);
    }

    for (const auto &variable : mVariables) {
        updateVariableInfoSizes(componentSize, nameSize, unitsSize, variable);
    }

    return replace(replace(replace(objectString,
                                   "<COMPONENT_SIZE>", std::to_string(componentSize)),
                           "<NAME_SIZE>", std::to_string(nameSize)),
                   "<UNITS_SIZE>", std::to_string(unitsSize));
}

void Generator::GeneratorImpl::addVariableInfoObjectCode(std::string &code)
{
    if (!mProfile->variableInfoObjectString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += generateVariableInfoObjectCode(mProfile->variableInfoObjectString());
    }
}

void Generator::GeneratorImpl::addVariableInfoWithTypeObjectCode(std::string &code)
{
    if (!mProfile->variableInfoWithTypeObjectString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += generateVariableInfoObjectCode(mProfile->variableInfoWithTypeObjectString());
    }
}

std::string Generator::GeneratorImpl::generateVariableInfoEntryCode(const std::string &name,
                                                                    const std::string &units,
                                                                    const std::string &component)
{
    return replace(replace(replace(mProfile->variableInfoEntryString(),
                                   "<NAME>", name),
                           "<UNITS>", units),
                   "<COMPONENT>", component);
}

void Generator::GeneratorImpl::addInterfaceVoiStateAndVariableInfoCode(std::string &code)
{
    std::string interfaceVoiStateAndVariableInfoCode;

    if (!mProfile->interfaceVoiInfoString().empty()) {
        interfaceVoiStateAndVariableInfoCode += mProfile->interfaceVoiInfoString();
    }

    if (!mProfile->interfaceStateInfoString().empty()) {
        interfaceVoiStateAndVariableInfoCode += mProfile->interfaceStateInfoString();
    }

    if (!mProfile->interfaceVariableInfoString().empty()) {
        interfaceVoiStateAndVariableInfoCode += mProfile->interfaceVariableInfoString();
    }

    if (!interfaceVoiStateAndVariableInfoCode.empty()) {
        code += "\n";
    }

    code += interfaceVoiStateAndVariableInfoCode;
}

void Generator::GeneratorImpl::addImplementationVoiInfoCode(std::string &code)
{
    if (!mProfile->implementationVoiInfoString().empty()
        && !mProfile->variableInfoEntryString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        std::string name = (mVoi != nullptr) ? mVoi->variable()->name() : "";
        std::string units = (mVoi != nullptr) ? mVoi->variable()->units()->name() : "";
        std::string component = (mVoi != nullptr) ? entityName(mVoi->variable()->parent()) : "";

        code += replace(mProfile->implementationVoiInfoString(),
                        "<CODE>", generateVariableInfoEntryCode(name, units, component));
    }
}

void Generator::GeneratorImpl::addImplementationStateInfoCode(std::string &code)
{
    if (!mProfile->implementationStateInfoString().empty()
        && !mProfile->variableInfoEntryString().empty()
        && !mProfile->arrayElementSeparatorString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        std::string infoElementsCode;

        for (const auto &state : mStates) {
            if (!infoElementsCode.empty()) {
                infoElementsCode += mProfile->arrayElementSeparatorString() + "\n";
            }

            infoElementsCode += mProfile->indentString()
                                + generateVariableInfoEntryCode(state->variable()->name(),
                                                                state->variable()->units()->name(),
                                                                entityName(state->variable()->parent()));
        }

        if (!infoElementsCode.empty()) {
            infoElementsCode += "\n";
        }

        code += replace(mProfile->implementationStateInfoString(),
                        "<CODE>", infoElementsCode);
    }
}

void Generator::GeneratorImpl::addImplementationVariableInfoCode(std::string &code)
{
    if (!mProfile->implementationVariableInfoString().empty()
        && !mProfile->variableInfoWithTypeEntryString().empty()
        && !mProfile->arrayElementSeparatorString().empty()
        && !mProfile->constantVariableTypeString().empty()
        && !mProfile->computedConstantVariableTypeString().empty()
        && !mProfile->algebraicVariableTypeString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        std::string infoElementsCode;

        for (const auto &variable : mVariables) {
            if (!infoElementsCode.empty()) {
                infoElementsCode += mProfile->arrayElementSeparatorString() + "\n";
            }

            std::string variableType;

            if (variable->type() == GeneratorVariable::Type::CONSTANT) {
                variableType = mProfile->constantVariableTypeString();
            } else if (variable->type() == GeneratorVariable::Type::COMPUTED_CONSTANT) {
                variableType = mProfile->computedConstantVariableTypeString();
            } else if (variable->type() == GeneratorVariable::Type::ALGEBRAIC) {
                variableType = mProfile->algebraicVariableTypeString();
            }

            infoElementsCode += mProfile->indentString()
                                + replace(replace(replace(replace(mProfile->variableInfoWithTypeEntryString(),
                                                                  "<NAME>", variable->variable()->name()),
                                                          "<UNITS>", variable->variable()->units()->name()),
                                                  "<COMPONENT>", entityName(variable->variable()->parent())),
                                          "<TYPE>", variableType);
        }

        if (!infoElementsCode.empty()) {
            infoElementsCode += "\n";
        }

        code += replace(mProfile->implementationVariableInfoString(),
                        "<CODE>", infoElementsCode);
    }
}

void Generator::GeneratorImpl::addArithmeticFunctionsCode(std::string &code)
{
    if (mNeedEq && !mProfile->hasEqOperator()
        && !mProfile->eqFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->eqFunctionString();
    }

    if (mNeedNeq && !mProfile->hasNeqOperator()
        && !mProfile->neqFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->neqFunctionString();
    }

    if (mNeedLt && !mProfile->hasLtOperator()
        && !mProfile->ltFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->ltFunctionString();
    }

    if (mNeedLeq && !mProfile->hasLeqOperator()
        && !mProfile->leqFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->leqFunctionString();
    }

    if (mNeedGt && !mProfile->hasGtOperator()
        && !mProfile->gtFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->gtFunctionString();
    }

    if (mNeedGeq && !mProfile->hasGeqOperator()
        && !mProfile->geqFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->geqFunctionString();
    }

    if (mNeedAnd && !mProfile->hasAndOperator()
        && !mProfile->andFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->andFunctionString();
    }

    if (mNeedOr && !mProfile->hasOrOperator()
        && !mProfile->orFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->orFunctionString();
    }

    if (mNeedXor && !mProfile->hasXorOperator()
        && !mProfile->xorFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->xorFunctionString();
    }

    if (mNeedNot && !mProfile->hasNotOperator()
        && !mProfile->notFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->notFunctionString();
    }

    if (mNeedMin
        && !mProfile->minFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->minFunctionString();
    }

    if (mNeedMax
        && !mProfile->maxFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->maxFunctionString();
    }
}

void Generator::GeneratorImpl::addTrigonometricFunctionsCode(std::string &code)
{
    if (mNeedSec
        && !mProfile->secFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->secFunctionString();
    }

    if (mNeedCsc
        && !mProfile->cscFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->cscFunctionString();
    }

    if (mNeedCot
        && !mProfile->cotFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->cotFunctionString();
    }

    if (mNeedSech
        && !mProfile->sechFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->sechFunctionString();
    }

    if (mNeedCsch
        && !mProfile->cschFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->cschFunctionString();
    }

    if (mNeedCoth
        && !mProfile->cothFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->cothFunctionString();
    }

    if (mNeedAsec
        && !mProfile->asecFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->asecFunctionString();
    }

    if (mNeedAcsc
        && !mProfile->acscFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->acscFunctionString();
    }

    if (mNeedAcot
        && !mProfile->acotFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->acotFunctionString();
    }

    if (mNeedAsech
        && !mProfile->asechFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->asechFunctionString();
    }

    if (mNeedAcsch
        && !mProfile->acschFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->acschFunctionString();
    }

    if (mNeedAcoth
        && !mProfile->acothFunctionString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->acothFunctionString();
    }
}

void Generator::GeneratorImpl::addInterfaceCreateDeleteArrayMethodsCode(std::string &code)
{
    std::string interfaceCreateDeleteArraysCode;

    if (!mProfile->interfaceCreateStatesArrayMethodString().empty()) {
        interfaceCreateDeleteArraysCode += mProfile->interfaceCreateStatesArrayMethodString();
    }

    if (!mProfile->interfaceCreateVariablesArrayMethodString().empty()) {
        interfaceCreateDeleteArraysCode += mProfile->interfaceCreateVariablesArrayMethodString();
    }

    if (!mProfile->interfaceDeleteArrayMethodString().empty()) {
        interfaceCreateDeleteArraysCode += mProfile->interfaceDeleteArrayMethodString();
    }

    if (!interfaceCreateDeleteArraysCode.empty()) {
        code += "\n";
    }

    code += interfaceCreateDeleteArraysCode;
}

void Generator::GeneratorImpl::addImplementationCreateStatesArrayMethodCode(std::string &code)
{
    if (!mProfile->implementationCreateStatesArrayMethodString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->implementationCreateStatesArrayMethodString();
    }
}

void Generator::GeneratorImpl::addImplementationCreateVariablesArrayMethodCode(std::string &code)
{
    if (!mProfile->implementationCreateVariablesArrayMethodString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->implementationCreateVariablesArrayMethodString();
    }
}

void Generator::GeneratorImpl::addImplementationDeleteArrayMethodCode(std::string &code)
{
    if (!mProfile->implementationDeleteArrayMethodString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        code += mProfile->implementationDeleteArrayMethodString();
    }
}

std::string Generator::GeneratorImpl::generateMethodBodyCode(const std::string &methodBody)
{
    return methodBody.empty() ?
               mProfile->emptyMethodString().empty() ?
               "" :
               mProfile->indentString() + mProfile->emptyMethodString() :
               methodBody;
}

std::string Generator::GeneratorImpl::generateDoubleCode(const std::string &value)
{
    if (value.find('.') != std::string::npos) {
        return value;
    }

    size_t ePos = value.find('e');

    if (ePos == std::string::npos) {
        return value + ".0";
    }

    return value.substr(0, ePos) + ".0" + value.substr(ePos);
}

std::string Generator::GeneratorImpl::generateVariableNameCode(const VariablePtr &variable, const GeneratorEquationAstPtr &ast)
{
    GeneratorInternalVariablePtr generatorVariable = Generator::GeneratorImpl::generatorVariable(variable);

    if (generatorVariable->mType == GeneratorInternalVariable::Type::VARIABLE_OF_INTEGRATION) {
        return mProfile->voiString();
    }

    std::string arrayName;

    if (generatorVariable->mType == GeneratorInternalVariable::Type::STATE) {
        arrayName = ((ast != nullptr) && (ast->mParent.lock()->mType == GeneratorEquationAst::Type::DIFF)) ?
                        mProfile->ratesArrayString() :
                        mProfile->statesArrayString();
    } else {
        arrayName = mProfile->variablesArrayString();
    }

    std::ostringstream index;

    index << generatorVariable->mIndex;

    return arrayName + mProfile->openArrayString() + index.str() + mProfile->closeArrayString();
}

std::string Generator::GeneratorImpl::generateOperatorCode(const std::string &op,
                                                           const GeneratorEquationAstPtr &ast)
{
    // Generate the code for the left and right branches of the given AST.

    std::string left = generateCode(ast->mLeft);
    std::string right = generateCode(ast->mRight);

    // Determine whether parentheses should be added around the left and/or
    // right piece of code, and this based on the precedence of the operators
    // used in CellML, which are listed below from higher to lower precedence:
    //  1. Parentheses                                           [Left to right]
    //  2. POWER (as an operator, not as a function, i.e.        [Left to right]
    //            as in Matlab and not in C, for example)
    //  3. Unary PLUS, Unary MINUS, NOT                          [Right to left]
    //  4. TIMES, DIVIDE                                         [Left to right]
    //  5. PLUS, MINUS                                           [Left to right]
    //  6. LT, LEQ, GT, GEQ                                      [Left to right]
    //  7. EQ, NEQ                                               [Left to right]
    //  8. XOR (bitwise)                                         [Left to right]
    //  9. AND (logical)                                         [Left to right]
    // 10. OR (logical)                                          [Left to right]
    // 11. PIECEWISE (as an operator)                            [Right to left]

    if (isPlusOperator(ast)) {
        if (isRelationalOperator(ast->mLeft)
            || isLogicalOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        }

        if (isRelationalOperator(ast->mRight)
            || isLogicalOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        }
    } else if (isMinusOperator(ast)) {
        if (isRelationalOperator(ast->mLeft)
            || isLogicalOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        }

        if (isRelationalOperator(ast->mRight)
            || isLogicalOperator(ast->mRight)
            || isMinusOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isPlusOperator(ast->mRight)) {
            if (ast->mRight->mRight != nullptr) {
                right = "(" + right + ")";
            }
        }
    } else if (isTimesOperator(ast)) {
        if (isRelationalOperator(ast->mLeft)
            || isLogicalOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isPlusOperator(ast->mLeft)
                   || isMinusOperator(ast->mLeft)) {
            if (ast->mLeft->mRight != nullptr) {
                left = "(" + left + ")";
            }
        }

        if (isRelationalOperator(ast->mRight)
            || isLogicalOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isPlusOperator(ast->mRight)
                   || isMinusOperator(ast->mRight)) {
            if (ast->mRight->mRight != nullptr) {
                right = "(" + right + ")";
            }
        }
    } else if (isDivideOperator(ast)) {
        if (isRelationalOperator(ast->mLeft)
            || isLogicalOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isPlusOperator(ast->mLeft)
                   || isMinusOperator(ast->mLeft)) {
            if (ast->mLeft->mRight != nullptr) {
                left = "(" + left + ")";
            }
        }

        if (isRelationalOperator(ast->mRight)
            || isLogicalOperator(ast->mRight)
            || isTimesOperator(ast->mRight)
            || isDivideOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isPlusOperator(ast->mRight)
                   || isMinusOperator(ast->mRight)) {
            if (ast->mRight->mRight != nullptr) {
                right = "(" + right + ")";
            }
        }
    } else if (isAndOperator(ast)) {
        // Note: according to the precedence rules above, we only need to add
        //       parentheses around OR and PIECEWISE. However, it looks
        //       better/clearer to have some around some other operators
        //       (agreed, this is somewhat subjective).

        if (isRelationalOperator(ast->mLeft)
            || isOrOperator(ast->mLeft)
            || isXorOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isPlusOperator(ast->mLeft)
                   || isMinusOperator(ast->mLeft)) {
            if (ast->mLeft->mRight != nullptr) {
                left = "(" + left + ")";
            }
        } else if (isPowerOperator(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isRootOperator(ast->mLeft)) {
            left = "(" + left + ")";
        }

        if (isRelationalOperator(ast->mRight)
            || isOrOperator(ast->mRight)
            || isXorOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isPlusOperator(ast->mRight)
                   || isMinusOperator(ast->mRight)) {
            if (ast->mRight->mRight != nullptr) {
                right = "(" + right + ")";
            }
        } else if (isPowerOperator(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isRootOperator(ast->mRight)) {
            right = "(" + right + ")";
        }
    } else if (isOrOperator(ast)) {
        // Note: according to the precedence rules above, we only need to add
        //       parentheses around PIECEWISE. However, it looks better/clearer
        //       to have some around some other operators (agreed, this is
        //       somewhat subjective).

        if (isRelationalOperator(ast->mLeft)
            || isAndOperator(ast->mLeft)
            || isXorOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isPlusOperator(ast->mLeft)
                   || isMinusOperator(ast->mLeft)) {
            if (ast->mLeft->mRight != nullptr) {
                left = "(" + left + ")";
            }
        } else if (isPowerOperator(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isRootOperator(ast->mLeft)) {
            left = "(" + left + ")";
        }

        if (isRelationalOperator(ast->mRight)
            || isAndOperator(ast->mRight)
            || isXorOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isPlusOperator(ast->mRight)
                   || isMinusOperator(ast->mRight)) {
            if (ast->mRight->mRight != nullptr) {
                right = "(" + right + ")";
            }
        } else if (isPowerOperator(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isRootOperator(ast->mRight)) {
            right = "(" + right + ")";
        }
    } else if (isXorOperator(ast)) {
        // Note: according to the precedence rules above, we only need to add
        //       parentheses around AND, OR and PIECEWISE. However, it looks
        //       better/clearer to have some around some other operators
        //       (agreed, this is somewhat subjective).

        if (isRelationalOperator(ast->mLeft)
            || isAndOperator(ast->mLeft)
            || isOrOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isPlusOperator(ast->mLeft)
                   || isMinusOperator(ast->mLeft)) {
            if (ast->mLeft->mRight != nullptr) {
                left = "(" + left + ")";
            }
        } else if (isPowerOperator(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isRootOperator(ast->mLeft)) {
            left = "(" + left + ")";
        }

        if (isRelationalOperator(ast->mRight)
            || isAndOperator(ast->mRight)
            || isOrOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isPlusOperator(ast->mRight)
                   || isMinusOperator(ast->mRight)) {
            if (ast->mRight->mRight != nullptr) {
                right = "(" + right + ")";
            }
        } else if (isPowerOperator(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isRootOperator(ast->mRight)) {
            right = "(" + right + ")";
        }
    } else if (isPowerOperator(ast)) {
        if (isRelationalOperator(ast->mLeft)
            || isLogicalOperator(ast->mLeft)
            || isMinusOperator(ast->mLeft)
            || isTimesOperator(ast->mLeft)
            || isDivideOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isPlusOperator(ast->mLeft)) {
            if (ast->mLeft->mRight != nullptr) {
                left = "(" + left + ")";
            }
        }

        if (isRelationalOperator(ast->mRight)
            || isLogicalOperator(ast->mRight)
            || isMinusOperator(ast->mLeft)
            || isTimesOperator(ast->mRight)
            || isDivideOperator(ast->mRight)
            || isPowerOperator(ast->mRight)
            || isRootOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isPlusOperator(ast->mRight)) {
            if (ast->mRight->mRight != nullptr) {
                right = "(" + right + ")";
            }
        }
    } else if (isRootOperator(ast)) {
        if (isRelationalOperator(ast->mRight)
            || isLogicalOperator(ast->mRight)
            || isMinusOperator(ast->mRight)
            || isTimesOperator(ast->mRight)
            || isDivideOperator(ast->mRight)
            || isPiecewiseStatement(ast->mRight)) {
            right = "(" + right + ")";
        } else if (isPlusOperator(ast->mRight)) {
            if (ast->mRight->mRight != nullptr) {
                right = "(" + right + ")";
            }
        }

        if (isRelationalOperator(ast->mLeft)
            || isLogicalOperator(ast->mLeft)
            || isMinusOperator(ast->mLeft)
            || isTimesOperator(ast->mLeft)
            || isDivideOperator(ast->mLeft)
            || isPowerOperator(ast->mLeft)
            || isRootOperator(ast->mLeft)
            || isPiecewiseStatement(ast->mLeft)) {
            left = "(" + left + ")";
        } else if (isPlusOperator(ast->mLeft)) {
            if (ast->mLeft->mRight != nullptr) {
                left = "(" + left + ")";
            }
        }

        return right + op + "(1.0/" + left + ")";
    }

    return left + op + right;
}

std::string Generator::GeneratorImpl::generateMinusUnaryCode(const GeneratorEquationAstPtr &ast)
{
    // Generate the code for the left branch of the given AST.

    std::string left = generateCode(ast->mLeft);

    // Determine whether parentheses should be added around the left code.

    if (isRelationalOperator(ast->mLeft)
        || isLogicalOperator(ast->mLeft)
        || isPlusOperator(ast->mLeft)
        || isMinusOperator(ast->mLeft)
        || isPiecewiseStatement(ast->mLeft)) {
        left = "(" + left + ")";
    }

    return mProfile->minusString() + left;
}

std::string Generator::GeneratorImpl::generateOneParameterFunctionCode(const std::string &function,
                                                                       const GeneratorEquationAstPtr &ast)
{
    return function + "(" + generateCode(ast->mLeft) + ")";
}

std::string Generator::GeneratorImpl::generateTwoParameterFunctionCode(const std::string &function,
                                                                       const GeneratorEquationAstPtr &ast)
{
    return function + "(" + generateCode(ast->mLeft) + ", " + generateCode(ast->mRight) + ")";
}

std::string Generator::GeneratorImpl::generatePiecewiseIfCode(const std::string &condition,
                                                              const std::string &value)
{
    return replace(replace(mProfile->hasConditionalOperator() ?
                               mProfile->conditionalOperatorIfString() :
                               mProfile->piecewiseIfString(),
                           "<CONDITION>", condition),
                   "<IF_STATEMENT>", value);
}

std::string Generator::GeneratorImpl::generatePiecewiseElseCode(const std::string &value)
{
    return replace(mProfile->hasConditionalOperator() ?
                       mProfile->conditionalOperatorElseString() :
                       mProfile->piecewiseElseString(),
                   "<ELSE_STATEMENT>", value);
}

std::string Generator::GeneratorImpl::generateCode(const GeneratorEquationAstPtr &ast)
{
    // Generate the code for the given AST.

    switch (ast->mType) {
        // Assignment.

    case GeneratorEquationAst::Type::ASSIGNMENT:
        return generateOperatorCode(mProfile->assignmentString(), ast);

        // Relational and logical operators.

    case GeneratorEquationAst::Type::EQ:
        if (mProfile->hasEqOperator()) {
            return generateOperatorCode(mProfile->eqString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->eqString(), ast);
    case GeneratorEquationAst::Type::NEQ:
        if (mProfile->hasNeqOperator()) {
            return generateOperatorCode(mProfile->neqString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->neqString(), ast);
    case GeneratorEquationAst::Type::LT:
        if (mProfile->hasLtOperator()) {
            return generateOperatorCode(mProfile->ltString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->ltString(), ast);
    case GeneratorEquationAst::Type::LEQ:
        if (mProfile->hasLeqOperator()) {
            return generateOperatorCode(mProfile->leqString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->leqString(), ast);
    case GeneratorEquationAst::Type::GT:
        if (mProfile->hasGtOperator()) {
            return generateOperatorCode(mProfile->gtString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->gtString(), ast);
    case GeneratorEquationAst::Type::GEQ:
        if (mProfile->hasGeqOperator()) {
            return generateOperatorCode(mProfile->geqString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->geqString(), ast);
    case GeneratorEquationAst::Type::AND:
        if (mProfile->hasAndOperator()) {
            return generateOperatorCode(mProfile->andString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->andString(), ast);
    case GeneratorEquationAst::Type::OR:
        if (mProfile->hasOrOperator()) {
            return generateOperatorCode(mProfile->orString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->orString(), ast);
    case GeneratorEquationAst::Type::XOR:
        if (mProfile->hasXorOperator()) {
            return generateOperatorCode(mProfile->xorString(), ast);
        }

        return generateTwoParameterFunctionCode(mProfile->xorString(), ast);
    case GeneratorEquationAst::Type::NOT:
        if (mProfile->hasNotOperator()) {
            return mProfile->notString() + generateCode(ast->mLeft);
        }

        return generateOneParameterFunctionCode(mProfile->notString(), ast);

        // Arithmetic operators.

    case GeneratorEquationAst::Type::PLUS:
        if (ast->mRight != nullptr) {
            return generateOperatorCode(mProfile->plusString(), ast);
        }

        return generateCode(ast->mLeft);
    case GeneratorEquationAst::Type::MINUS:
        if (ast->mRight != nullptr) {
            return generateOperatorCode(mProfile->minusString(), ast);
        }

        return generateMinusUnaryCode(ast);
    case GeneratorEquationAst::Type::TIMES:
        return generateOperatorCode(mProfile->timesString(), ast);
    case GeneratorEquationAst::Type::DIVIDE:
        return generateOperatorCode(mProfile->divideString(), ast);
    case GeneratorEquationAst::Type::POWER: {
        std::string stringValue = generateCode(ast->mRight);
        double doubleValue = convertToDouble(stringValue);

        if (areEqual(doubleValue, 0.5)) {
            return generateOneParameterFunctionCode(mProfile->squareRootString(), ast);
        }

        if (areEqual(doubleValue, 2.0) && !mProfile->squareString().empty()) {
            return generateOneParameterFunctionCode(mProfile->squareString(), ast);
        }

        return mProfile->hasPowerOperator() ?
                   generateOperatorCode(mProfile->powerString(), ast) :
                   mProfile->powerString() + "(" + generateCode(ast->mLeft) + ", " + stringValue + ")";
    }
    case GeneratorEquationAst::Type::ROOT:
        if (ast->mRight != nullptr) {
            double doubleValue = convertToDouble(generateCode(ast->mLeft));

            if (areEqual(doubleValue, 2.0)) {
                return mProfile->squareRootString() + "(" + generateCode(ast->mRight) + ")";
            }

            GeneratorEquationAstPtr rootValueAst = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::DIVIDE, ast);

            rootValueAst->mLeft = std::make_shared<GeneratorEquationAst>(GeneratorEquationAst::Type::CN, "1.0", rootValueAst);
            rootValueAst->mRight = std::make_shared<GeneratorEquationAst>(ast->mLeft, rootValueAst);

            return mProfile->hasPowerOperator() ?
                       generateOperatorCode(mProfile->powerString(), ast) :
                       mProfile->powerString() + "(" + generateCode(ast->mRight) + ", " + generateOperatorCode(mProfile->divideString(), rootValueAst) + ")";
        }

        return generateOneParameterFunctionCode(mProfile->squareRootString(), ast);
    case GeneratorEquationAst::Type::ABS:
        return generateOneParameterFunctionCode(mProfile->absoluteValueString(), ast);
    case GeneratorEquationAst::Type::EXP:
        return generateOneParameterFunctionCode(mProfile->exponentialString(), ast);
    case GeneratorEquationAst::Type::LN:
        return generateOneParameterFunctionCode(mProfile->napierianLogarithmString(), ast);
    case GeneratorEquationAst::Type::LOG:
        if (ast->mRight != nullptr) {
            std::string stringValue = generateCode(ast->mLeft);
            double doubleValue = convertToDouble(stringValue);

            if (areEqual(doubleValue, 10.0)) {
                return mProfile->commonLogarithmString() + "(" + generateCode(ast->mRight) + ")";
            }

            return mProfile->napierianLogarithmString() + "(" + generateCode(ast->mRight) + ")/" + mProfile->napierianLogarithmString() + "(" + stringValue + ")";
        }

        return generateOneParameterFunctionCode(mProfile->commonLogarithmString(), ast);
    case GeneratorEquationAst::Type::CEILING:
        return generateOneParameterFunctionCode(mProfile->ceilingString(), ast);
    case GeneratorEquationAst::Type::FLOOR:
        return generateOneParameterFunctionCode(mProfile->floorString(), ast);
    case GeneratorEquationAst::Type::MIN:
        return generateTwoParameterFunctionCode(mProfile->minString(), ast);
    case GeneratorEquationAst::Type::MAX:
        return generateTwoParameterFunctionCode(mProfile->maxString(), ast);
    case GeneratorEquationAst::Type::REM:
        return generateTwoParameterFunctionCode(mProfile->remString(), ast);

        // Calculus elements.

    case GeneratorEquationAst::Type::DIFF:
        return generateCode(ast->mRight);

        // Trigonometric operators.

    case GeneratorEquationAst::Type::SIN:
        return generateOneParameterFunctionCode(mProfile->sinString(), ast);
    case GeneratorEquationAst::Type::COS:
        return generateOneParameterFunctionCode(mProfile->cosString(), ast);
    case GeneratorEquationAst::Type::TAN:
        return generateOneParameterFunctionCode(mProfile->tanString(), ast);
    case GeneratorEquationAst::Type::SEC:
        return generateOneParameterFunctionCode(mProfile->secString(), ast);
    case GeneratorEquationAst::Type::CSC:
        return generateOneParameterFunctionCode(mProfile->cscString(), ast);
    case GeneratorEquationAst::Type::COT:
        return generateOneParameterFunctionCode(mProfile->cotString(), ast);
    case GeneratorEquationAst::Type::SINH:
        return generateOneParameterFunctionCode(mProfile->sinhString(), ast);
    case GeneratorEquationAst::Type::COSH:
        return generateOneParameterFunctionCode(mProfile->coshString(), ast);
    case GeneratorEquationAst::Type::TANH:
        return generateOneParameterFunctionCode(mProfile->tanhString(), ast);
    case GeneratorEquationAst::Type::SECH:
        return generateOneParameterFunctionCode(mProfile->sechString(), ast);
    case GeneratorEquationAst::Type::CSCH:
        return generateOneParameterFunctionCode(mProfile->cschString(), ast);
    case GeneratorEquationAst::Type::COTH:
        return generateOneParameterFunctionCode(mProfile->cothString(), ast);
    case GeneratorEquationAst::Type::ASIN:
        return generateOneParameterFunctionCode(mProfile->asinString(), ast);
    case GeneratorEquationAst::Type::ACOS:
        return generateOneParameterFunctionCode(mProfile->acosString(), ast);
    case GeneratorEquationAst::Type::ATAN:
        return generateOneParameterFunctionCode(mProfile->atanString(), ast);
    case GeneratorEquationAst::Type::ASEC:
        return generateOneParameterFunctionCode(mProfile->asecString(), ast);
    case GeneratorEquationAst::Type::ACSC:
        return generateOneParameterFunctionCode(mProfile->acscString(), ast);
    case GeneratorEquationAst::Type::ACOT:
        return generateOneParameterFunctionCode(mProfile->acotString(), ast);
    case GeneratorEquationAst::Type::ASINH:
        return generateOneParameterFunctionCode(mProfile->asinhString(), ast);
    case GeneratorEquationAst::Type::ACOSH:
        return generateOneParameterFunctionCode(mProfile->acoshString(), ast);
    case GeneratorEquationAst::Type::ATANH:
        return generateOneParameterFunctionCode(mProfile->atanhString(), ast);
    case GeneratorEquationAst::Type::ASECH:
        return generateOneParameterFunctionCode(mProfile->asechString(), ast);
    case GeneratorEquationAst::Type::ACSCH:
        return generateOneParameterFunctionCode(mProfile->acschString(), ast);
    case GeneratorEquationAst::Type::ACOTH:
        return generateOneParameterFunctionCode(mProfile->acothString(), ast);

        // Piecewise statement.

    case GeneratorEquationAst::Type::PIECEWISE:
        if (ast->mRight != nullptr) {
            if (ast->mRight->mType == GeneratorEquationAst::Type::PIECE) {
                return generateCode(ast->mLeft) + generatePiecewiseElseCode(generateCode(ast->mRight) + generatePiecewiseElseCode(mProfile->nanString()));
            }

            return generateCode(ast->mLeft) + generatePiecewiseElseCode(generateCode(ast->mRight));
        }

        return generateCode(ast->mLeft) + generatePiecewiseElseCode(mProfile->nanString());
    case GeneratorEquationAst::Type::PIECE:
        return generatePiecewiseIfCode(generateCode(ast->mRight), generateCode(ast->mLeft));
    case GeneratorEquationAst::Type::OTHERWISE:
        return generateCode(ast->mLeft);

        // Token elements.

    case GeneratorEquationAst::Type::CI:
        return generateVariableNameCode(ast->mVariable, ast);
    case GeneratorEquationAst::Type::CN:
        return generateDoubleCode(ast->mValue);

        // Qualifier elements.

    case GeneratorEquationAst::Type::DEGREE:
    case GeneratorEquationAst::Type::LOGBASE:
    case GeneratorEquationAst::Type::BVAR:
        return generateCode(ast->mLeft);

        // Constants.

    case GeneratorEquationAst::Type::TRUE:
        return mProfile->trueString();
    case GeneratorEquationAst::Type::FALSE:
        return mProfile->falseString();
    case GeneratorEquationAst::Type::E:
        return mProfile->eString();
    case GeneratorEquationAst::Type::PI:
        return mProfile->piString();
    case GeneratorEquationAst::Type::INF:
        return mProfile->infString();
    case GeneratorEquationAst::Type::NAN:
        return mProfile->nanString();
    }
}

std::string Generator::GeneratorImpl::generateInitializationCode(const GeneratorInternalVariablePtr &variable)
{
    return mProfile->indentString() + generateVariableNameCode(variable->mVariable) + " = " + generateDoubleCode(variable->mInitialValueVariable->initialValue()) + mProfile->commandSeparatorString() + "\n";
}

std::string Generator::GeneratorImpl::generateEquationCode(const GeneratorEquationPtr &equation,
                                                           std::vector<GeneratorEquationPtr> &remainingEquations,
                                                           bool onlyStateRateBasedEquations)
{
    std::string res;

    for (const auto &dependency : equation->mDependencies) {
        if (!onlyStateRateBasedEquations
            || ((dependency->mType == GeneratorEquation::Type::ALGEBRAIC)
                && dependency->mIsStateRateBased)) {
            res += generateEquationCode(dependency, remainingEquations, onlyStateRateBasedEquations);
        }
    }

    auto equationIter = std::find(remainingEquations.begin(), remainingEquations.end(), equation);

    if (equationIter != remainingEquations.end()) {
        res += mProfile->indentString() + generateCode(equation->mAst) + mProfile->commandSeparatorString() + "\n";

        remainingEquations.erase(equationIter);
    }

    return res;
}

void Generator::GeneratorImpl::addInterfaceComputeModelMethodsCode(std::string &code)
{
    std::string interfaceComputeModelMethodsCode;

    if (!mProfile->interfaceInitializeStatesAndConstantsMethodString().empty()) {
        interfaceComputeModelMethodsCode += mProfile->interfaceInitializeStatesAndConstantsMethodString();
    }

    if (!mProfile->interfaceComputeComputedConstantsMethodString().empty()) {
        interfaceComputeModelMethodsCode += mProfile->interfaceComputeComputedConstantsMethodString();
    }

    if (!mProfile->interfaceComputeRatesMethodString().empty()) {
        interfaceComputeModelMethodsCode += mProfile->interfaceComputeRatesMethodString();
    }

    if (!mProfile->interfaceComputeVariablesMethodString().empty()) {
        interfaceComputeModelMethodsCode += mProfile->interfaceComputeVariablesMethodString();
    }

    if (!interfaceComputeModelMethodsCode.empty()) {
        code += "\n";
    }

    code += interfaceComputeModelMethodsCode;
}

void Generator::GeneratorImpl::addImplementationInitializeStatesAndConstantsMethodCode(std::string &code,
                                                                                       std::vector<GeneratorEquationPtr> &remainingEquations)
{
    if (!mProfile->implementationInitializeStatesAndConstantsMethodString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        std::string methodBody;

        for (const auto &internalVariable : mInternalVariables) {
            if ((internalVariable->mType == GeneratorInternalVariable::Type::STATE)
                || (internalVariable->mType == GeneratorInternalVariable::Type::CONSTANT)) {
                methodBody += generateInitializationCode(internalVariable);
            }
        }

        for (const auto &equation : mEquations) {
            if (equation->mType == GeneratorEquation::Type::TRUE_CONSTANT) {
                methodBody += generateEquationCode(equation, remainingEquations);
            }
        }

        code += replace(mProfile->implementationInitializeStatesAndConstantsMethodString(),
                        "<CODE>", generateMethodBodyCode(methodBody));
    }
}

void Generator::GeneratorImpl::addImplementationComputeComputedConstantsMethodCode(std::string &code,
                                                                                   std::vector<GeneratorEquationPtr> &remainingEquations)
{
    if (!mProfile->implementationComputeComputedConstantsMethodString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        std::string methodBody;

        for (const auto &equation : mEquations) {
            if (equation->mType == GeneratorEquation::Type::VARIABLE_BASED_CONSTANT) {
                methodBody += generateEquationCode(equation, remainingEquations);
            }
        }

        code += replace(mProfile->implementationComputeComputedConstantsMethodString(),
                        "<CODE>", generateMethodBodyCode(methodBody));
    }
}

void Generator::GeneratorImpl::addImplementationComputeRatesMethodCode(std::string &code,
                                                                       std::vector<GeneratorEquationPtr> &remainingEquations)
{
    if (!mProfile->implementationComputeRatesMethodString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        std::string methodBody;

        for (const auto &equation : mEquations) {
            if (equation->mType == GeneratorEquation::Type::RATE) {
                methodBody += generateEquationCode(equation, remainingEquations);
            }
        }

        code += replace(mProfile->implementationComputeRatesMethodString(),
                        "<CODE>", generateMethodBodyCode(methodBody));
    }
}

void Generator::GeneratorImpl::addImplementationComputeVariablesMethodCode(std::string &code,
                                                                           std::vector<GeneratorEquationPtr> &remainingEquations)
{
    if (!mProfile->implementationComputeVariablesMethodString().empty()) {
        if (!code.empty()) {
            code += "\n";
        }

        std::vector<GeneratorEquationPtr> newRemainingEquations {std::begin(mEquations), std::end(mEquations)};

        std::string methodBody;

        for (const auto &equation : mEquations) {
            if ((std::find(remainingEquations.begin(), remainingEquations.end(), equation) != remainingEquations.end())
                || ((equation->mType == GeneratorEquation::Type::ALGEBRAIC)
                    && equation->mIsStateRateBased)) {
                methodBody += generateEquationCode(equation, newRemainingEquations, true);
            }
        }

        code += replace(mProfile->implementationComputeVariablesMethodString(),
                        "<CODE>", generateMethodBodyCode(methodBody));
    }
}

Generator::Generator()
    : mPimpl(new GeneratorImpl())
{
    mPimpl->mGenerator = this;
}

Generator::~Generator()
{
    delete mPimpl;
}

GeneratorPtr Generator::create() noexcept
{
    return std::shared_ptr<Generator> {new Generator {}};
}

GeneratorProfilePtr Generator::profile()
{
    return mPimpl->mProfile;
}

void Generator::setProfile(const GeneratorProfilePtr &profile)
{
    mPimpl->mProfile = profile;
}

void Generator::processModel(const ModelPtr &model, bool debug)
{
    // Make sure that the model is valid before processing it.

    /*TODO: enable the below code once validation is known to work fine.
    ValidatorPtr validator = Validator::create();

    validator->validateModel(model);

    if (validator->errorCount() > 0) {
        // The model is not valid, so retrieve the validation errors and make
        // them our own.

        for (size_t i = 0; i < validator->errorCount(); ++i) {
            addError(validator->error(i));
        }

        return;
    }
*/

    // Process the model.

    mPimpl->processModel(model, debug);
}

Generator::ModelType Generator::modelType() const
{
    return mPimpl->mModelType;
}

size_t Generator::stateCount() const
{
    if (!mPimpl->hasValidModel()) {
        return 0;
    }

    return mPimpl->mStates.size();
}

size_t Generator::variableCount() const
{
    if (!mPimpl->hasValidModel()) {
        return 0;
    }

    return mPimpl->mVariables.size();
}

GeneratorVariablePtr Generator::voi() const
{
    if (!mPimpl->hasValidModel()) {
        return {};
    }

    return mPimpl->mVoi;
}

GeneratorVariablePtr Generator::state(size_t index) const
{
    if (!mPimpl->hasValidModel() || (index >= mPimpl->mStates.size())) {
        return {};
    }

    return mPimpl->mStates[index];
}

GeneratorVariablePtr Generator::variable(size_t index) const
{
    if (!mPimpl->hasValidModel() || (index >= mPimpl->mVariables.size())) {
        return {};
    }

    return mPimpl->mVariables[index];
}

std::string Generator::interfaceCode() const
{
    if (!mPimpl->hasValidModel() || !mPimpl->mProfile->hasInterface()) {
        return {};
    }

    // Add code for the origin comment.

    std::string res;

    mPimpl->addOriginCommentCode(res);

    // Add code for the header.

    mPimpl->addInterfaceHeaderCode(res);

    // Add code for the interface of the version of the profile and libCellML.

    mPimpl->addVersionAndLibcellmlVersionCode(res, true);

    // Add code for the interface of the number of states and variables.

    mPimpl->addStateAndVariableCountCode(res, true);

    // Add code for the variable information related objects.

    if (mPimpl->mProfile->hasInterface()) {
        mPimpl->addVariableTypeObjectCode(res);
        mPimpl->addVariableInfoObjectCode(res);
        mPimpl->addVariableInfoWithTypeObjectCode(res);
    }

    // Add code for the interface of the information about the variable of
    // integration, states and (other) variables.

    mPimpl->addInterfaceVoiStateAndVariableInfoCode(res);

    // Add code for the interface to create and delete arrays.

    mPimpl->addInterfaceCreateDeleteArrayMethodsCode(res);

    // Add code for the interface to compute the model.

    mPimpl->addInterfaceComputeModelMethodsCode(res);

    return res;
}

std::string Generator::implementationCode() const
{
    if (!mPimpl->hasValidModel()) {
        return {};
    }

    std::string res;

    // Add code for the origin comment.

    mPimpl->addOriginCommentCode(res);

    // Add code for the header.

    mPimpl->addImplementationHeaderCode(res);

    // Add code for the implementation of the version of the profile and
    // libCellML.

    mPimpl->addVersionAndLibcellmlVersionCode(res);

    // Add code for the implementation of the number of states and variables.

    mPimpl->addStateAndVariableCountCode(res);

    // Add code for the variable information related objects.

    if (!mPimpl->mProfile->hasInterface()) {
        mPimpl->addVariableTypeObjectCode(res);
        mPimpl->addVariableInfoObjectCode(res);
        mPimpl->addVariableInfoWithTypeObjectCode(res);
    }

    // Add code for the implementation of the information about the variable of
    // integration, states and (other) variables.

    mPimpl->addImplementationVoiInfoCode(res);
    mPimpl->addImplementationStateInfoCode(res);
    mPimpl->addImplementationVariableInfoCode(res);

    // Add code for the arithmetic and trigonometric functions.

    mPimpl->addArithmeticFunctionsCode(res);
    mPimpl->addTrigonometricFunctionsCode(res);

    // Add code for the implementation to create and delete arrays.

    mPimpl->addImplementationCreateStatesArrayMethodCode(res);
    mPimpl->addImplementationCreateVariablesArrayMethodCode(res);
    mPimpl->addImplementationDeleteArrayMethodCode(res);

    // Add code for the implementation to initialise our states and constants.

    std::vector<GeneratorEquationPtr> remainingEquations {std::begin(mPimpl->mEquations), std::end(mPimpl->mEquations)};

    mPimpl->addImplementationInitializeStatesAndConstantsMethodCode(res, remainingEquations);

    // Add code for the implementation to compute our computed constants.

    mPimpl->addImplementationComputeComputedConstantsMethodCode(res, remainingEquations);

    // Add code for the implementation to compute our rates (and any variables
    // on which they depend).

    mPimpl->addImplementationComputeRatesMethodCode(res, remainingEquations);

    // Add code for the implementation to compute our variables.
    // Note: this method computes the remaining variables, i.e. the ones not
    //       needed to compute our rates, but also the variables that depend on
    //       the value of some states/rates. Indeed, this method is typically
    //       called after having integrated a model, thus ensuring that
    //       variables that rely on the value of some states/rates are up to
    //       date.

    mPimpl->addImplementationComputeVariablesMethodCode(res, remainingEquations);

    return res;
}

} // namespace libcellml
