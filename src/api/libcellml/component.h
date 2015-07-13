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

#ifndef LIBCELLML_LIBCELLML_COMPONENT_H
#define LIBCELLML_LIBCELLML_COMPONENT_H

#include <string>
#include <memory>

#include "libcellml/libcellml_export.h"
#include "libcellml/componententity.h"

namespace libcellml {

/**
 * @brief The Component class.
 * The Component class is for representing a CellML Component.
 */
class LIBCELLML_EXPORT Component: public ComponentEntity
{
public:
    /**
     * @brief Component Constructor.
     * Default Component constructor.
     */
    Component();
    /**
     * @brief Component Destructor.
     * Default Component destructor.
     */
    ~Component();

    Component(Component&& rhs); /**< Move constructor */
    Component(const Component& rhs); /**< Copy constructor */
    Component& operator=(Component rhs); /**< Assignment operator */

    /**
     * @brief Add a child component to this component.
     * Add a copy of the given component as a child component of this component.
     * @param c The component to add.
     */
    void addComponent(const Component &c);

    /**
     * @brief Remove the component at the given index.
     * Remove the component from the model with the index @p index.  If the index is
     * not in the range [0, #components) a std::out_of_range exception
     * will be thrown.
     * @param index The index of the component to remove (zero-based).
     */
    void removeComponent(size_t index);

    /**
     * @brief Remove the component with the given name.
     * Remove the first component found that matches the name @p name.  If the
     * name is not found no component is removed.
     *
     * This is an overloaded method.
     * @param name The name of the component to remove.
     */
    void removeComponent(const std::string &name);

    /**
     * @brief Tests to see if the component is contained within the Component.
     * Tests to see if the component with the given name 'name' is contained
     * within the Component.  Returns true if the component is in the Component and
     * false otherwise.
     * @param name The name of the Component to test for existence in the Component.
     * @return @c true if the named Component is in the model and @c false otherwise.
     */
    bool containsComponent(const std::string &name);

    /**
     * @brief Get a component at index.
     * Returns a const reference to a component at the index @p index.  If the
     * index is not valid a std::out_of_range exception is thrown.
     *
     * This is an overloaded method.
     * @param index The index of the Component to return (zero-based).
     * @return A const reference to the Component at the given index.
     */
    const Component& getComponent(size_t index) const;

    /**
     * @brief Get a component at index.
     * Returns a reference to a component at the index @p index.  If the index
     * is not valid a std::out_of_range exception is thrown.
     *
     * This is an overloaded method.
     * @param index The index of the Component to return (zero-based).
     * @return A reference to the Component at the given index.
     */
    Component& getComponent(size_t index);

    /**
     * @brief Take the component at the given index and return it.
     * Removes the component at the given index position and returns it.
     * If an invalid index is passed to the method a std::out_of_range
     * exception is thrown.
     * @param index The index of the Component to take (zero-based).
     * @return The Component at the given index.
     */
    Component takeComponent(size_t index);

    /**
     * @brief Take the component with the given name @p name and return it.
     * Takes the component at the given index position and returns it.
     * If an invalid name is passed to the method a std::out_of_range
     * exception is thrown.
     *
     * This is an overloaded method.
     * @param name The name of the Component to take.
     * @return The Component identified with the given name.
     */
    Component takeComponent(const std::string &name);

    /**
     * @brief Replace a component at index.
     * Replaces the component at index @p index with @p c.  Index must be a valid index
     * position.
     * @param index Index of the Component to replace (zero-based).
     * @param c The Component to use for replacement.
     */
    void replaceComponent(size_t index, const Component& c);

    /**
     * @brief Replace a component with the given name.
     * Replaces the component with the given name @p name with @p c.  Name must be a valid name
     * of a component in the Component, if it isn't a std::out_of_range exception is thrown.
     *
     * This is an overloaded method.
     * @param name The name of the Component to replace.
     * @param c The Component to use for replacement.
     */
    void replaceComponent(const std::string &name, const Component& c);

    /**
     * @brief get the number of components in the component.
     * Returns the number of components the component directly contains.
     * @return the number of components.
     */
    size_t componentCount() const;

private:
    std::string doSerialisation(CELLML_FORMATS format) const;
    void swap(Component &rhs);

    struct ComponentImpl; /**< Forward declaration for pImpl idiom. */
    ComponentImpl* mPimpl; /**< Private member to implementation pointer */

};

}

#endif // LIBCELLML_LIBCELLML_COMPONENT_H

