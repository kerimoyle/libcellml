/*
Copyright 2015 University of Auckland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.Some license of other
*/

#ifndef LIBCELLML_LIBCELLML_VARIABLE_H
#define LIBCELLML_LIBCELLML_VARIABLE_H

#include <string>

#include "libcellml/libcellml_export.h"
#include "libcellml/types.h"
#include "libcellml/namedentity.h"

namespace libcellml {

/**
 * @brief The Variable class.
 * Class for each variable in libCellML.
 */
class LIBCELLML_EXPORT Variable: public NamedEntity
{
public:
    Variable(); /**< Constructor */
    ~Variable(); /**< Destructor */
    Variable(const Variable &rhs); /**< Copy constructor */
    Variable(Variable &&rhs); /**< Move constructor */
    Variable& operator=(Variable n); /**< Assignment operator */

    /**
     * @brief The INTERFACE_TYPES enum.
     * Enum describing the interface types that are available for
     * a given variable.
     */
    enum INTERFACE_TYPES
    {
        INTERFACE_TYPE_NONE,
        INTERFACE_TYPE_PRIVATE,
        INTERFACE_TYPE_PUBLIC,
        INTERFACE_TYPE_PUBLIC_AND_PRIVATE
    };

    /**
     * @brief Set the units for this variable.
     * Set the units for this variable. Set to @c nullptr to unset the units.
     * @sa getUnits
     * @param u The units to set.
     */
    void setUnits(const UnitsPtr &u);

    /**
     * @brief Get the units for this variable.
     * Get the units for this variable.  If no units are set @c nullptr is returned.
     * @sa setUnits
     * @param u The units to get.
     */
    UnitsPtr getUnits() const;

    /**
     * @brief Set the initial value for this variable using a string.
     * Set the initial value for this variable using a string.
     *
     * @sa getInitialValue
     * @param initialValue The initial value to set.
     */
    void setInitialValue(const std::string &initialValue);

    /**
     * @brief Set the initial value for this variable using a real number.
     * Set the initial value for this variable using a real number.
     * The real number value will be converted to and stored as a string.
     *
     * @overload
     * @sa getInitialValue
     * @param initialValue The initial value to set.
     */
    void setInitialValue(double initialValue);

    /**
     * @brief Set the initial value for this variable using a variable reference.
     * Set the initial value for this variable using a variable reference.
     * The initial value will be set to the name of the referenced variable.
     *
     * @overload
     * @sa getInitialValue
     * @param variable The variable reference to use to set the initial value.
     */
    void setInitialValue(const VariablePtr &variable);

    /**
     * @brief Get the initial value for this variable.
     * Get the real number initial value for this variable.
     * @sa setInitialValue
     */
    std::string getInitialValue() const;

    /**
     * @brief Set the interface type for this variable.
     * Set the interface type for this variable from the available
     * options in the INTERFACE_TYPES enum.
     * @sa getInterfaceType
     * @param interfaceType The interface type to set for this variable.
     */
    void setInterfaceType(INTERFACE_TYPES interfaceType);

    /**
     * @brief Get the interface type for this variable.
     * Get the interface type for this variable from the available
     * options in the INTERFACE_TYPES enum.
     * @sa setInterfaceType
     * @param interfaceType The interface type to get for this variable.
     */
    INTERFACE_TYPES getInterfaceType() const;

private:
    void swap(Variable &rhs); /**< Swap method required for C++ 11 move semantics. */

    std::string doSerialisation(FORMATS format) const;

    struct VariableImpl; /**< Forward declaration for pImpl idiom. */
    VariableImpl* mPimpl; /**< Private member to implementation pointer */
};

}

#endif /* LIBCELLML_LIBCELLML_VARIABLE_H */
