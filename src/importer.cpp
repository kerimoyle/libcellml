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

#include "libcellml/importer.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <libxml/uri.h>
#include <sstream>
#include <stdexcept>

#include "libcellml/component.h"
#include "libcellml/importsource.h"
#include "libcellml/model.h"
#include "libcellml/parser.h"
#include "libcellml/reset.h"
#include "libcellml/units.h"
#include "libcellml/variable.h"

#include "namespaces.h"
#include "utilities.h"
#include "xmldoc.h"
#include "xmlutils.h"

namespace libcellml {

/**
 * @brief The Importer::ImporterImpl struct.
 *
 * This struct is the private implementation struct for the Importer class.  Separating
 * the implementation from the definition allows for greater flexibility when
 * distributing the code.
 */
struct Importer::ImporterImpl
{
};

Importer::Importer()
    : mPimpl(new ImporterImpl())
{
}

ImporterPtr Importer::create() noexcept
{
    return std::shared_ptr<Importer> {new Importer {}};
}

Importer::~Importer()
{
    delete mPimpl;
}

// ******************************* RESOLVING FUNCTIONS *******************************

/**
 * @brief Resolve the path of the given filename using the given base.
 *
 * Resolves the full path to the given @p filename using the @p base.
 *
 * This function is only intended to work with local files.  It may not
 * work with bases that use the 'file://' prefix.
 *
 * @param filename The @c std::string relative path from the base path.
 * @param base The @c std::string location on local disk for determining the full path from.
 *
 * @return The full path from the @p base location to the @p filename
 */
std::string resolvePath(const std::string &filename, const std::string &base)
{
    // We can be naive here as we know what we are dealing with
    std::string path = base.substr(0, base.find_last_of('/') + 1) + filename;
    return path;
}

bool resolveImport(const ImportedEntityPtr &importedEntity,
                   const std::string &destination,
                   const std::string &baseFile,
                   std::vector<std::tuple<std::string, std::string, std::string>> &history,
                   std::vector<IssuePtr> &issues);

bool resolveComponentImports(const ComponentEntityPtr &parentComponentEntity,
                             const std::string &baseFile,
                             std::vector<std::tuple<std::string, std::string, std::string>> &history,
                             std::vector<IssuePtr> &issues);

bool doResolveImports(ModelPtr &model, const std::string &baseFile, std::vector<std::tuple<std::string, std::string, std::string>> &history, std::vector<libcellml::IssuePtr> &issues)
{
    for (size_t n = 0; n < model->unitsCount(); ++n) {
        auto units = model->units(n);

        if ((!resolveImport(units, units->name(), baseFile, history, issues)) && (!history.empty())) {
            std::string msg = "Cyclic dependencies were found when attempting to resolve units in model '" + model->name() + "'. The dependency loop is:\n";
            std::string spacer = "    ";
            for (auto &h : history) {
                msg += spacer + "units '" + std::get<0>(h) + "' imports '" + std::get<1>(h) + "' from '" + std::get<2>(h);
                spacer = "',\n    ";
            }
            msg += "'.";
            auto issue = Issue::create();
            issue->setDescription(msg);
            issue->setLevel(libcellml::Issue::Level::WARNING);
            issue->setModel(model);
            issues.push_back(issue);
            std::vector<std::tuple<std::string, std::string, std::string>>().swap(history);
            return false;
        }
    }
    return resolveComponentImports(model, baseFile, history, issues);
}

bool resolveImport(const ImportedEntityPtr &importedEntity,
                   const std::string &destination,
                   const std::string &baseFile,
                   std::vector<std::tuple<std::string, std::string, std::string>> &history,
                   std::vector<IssuePtr> &issues)
{
    if (importedEntity->isImport()) {
        ImportSourcePtr importSource = importedEntity->importSource();
        auto h = std::make_tuple(destination, importedEntity->importReference(), importSource->url());
        if (std::find(history.begin(), history.end(), h) != history.end()) {
            history.emplace_back(h);
            return false;
        }
        history.emplace_back(h);

        if (!importSource->hasModel()) {
            std::string url = resolvePath(importSource->url(), baseFile);
            std::ifstream file(url);

            if (file.good()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                ParserPtr parser = Parser::create();
                ModelPtr model = parser->parseModel(buffer.str());
                importSource->setModel(model);
                return doResolveImports(model, url, history, issues);
            }
        }
    }
    return true;
}

bool resolveComponentImports(const ComponentEntityPtr &parentComponentEntity,
                             const std::string &baseFile,
                             std::vector<std::tuple<std::string, std::string, std::string>> &history,
                             std::vector<IssuePtr> &issues)
{
    bool noErrors = true;
    for (size_t n = 0; n < parentComponentEntity->componentCount(); ++n) {
        libcellml::ComponentPtr component = parentComponentEntity->component(n);
        if (component->isImport()) {
            if (!resolveImport(component, component->name(), baseFile, history, issues)) {
                if (!history.empty()) {
                    std::string msg = "Cyclic dependencies were found when attempting to resolve components";
                    auto parentModel = owningModel(component);
                    if (parentModel != nullptr) {
                        msg += " in model '" + parentModel->name() + "'";
                    }
                    msg += ". The dependency loop is:\n";
                    std::string spacer = "    ";
                    for (auto &h : history) {
                        msg += spacer + "component '" + std::get<0>(h) + "' imports '" + std::get<1>(h) + "' from '" + std::get<2>(h);
                        spacer = "',\n    ";
                    }
                    msg += "'.";
                    auto issue = Issue::create();
                    issue->setDescription(msg);
                    issue->setLevel(libcellml::Issue::Level::WARNING);
                    issues.push_back(issue);
                    std::vector<std::tuple<std::string, std::string, std::string>>().swap(history);
                }
                noErrors = false;
            }
        }
        if (!resolveComponentImports(component, baseFile, history, issues)) {
            noErrors = false;
        }
    }
    return noErrors;
}

void Importer::resolveImports(ModelPtr &model, const std::string &baseFile)
{
    std::vector<libcellml::IssuePtr> issues = {};
    std::vector<std::tuple<std::string, std::string, std::string>> history = {};
    doResolveImports(model, baseFile, history, issues);
    if (!issues.empty()) {
        for (auto &issue : issues) {
            addIssue(issue);
        }
    }
}

/*

void resolveImport(const ImportedEntityPtr &importedEntity,
                   const std::string &baseFile)
{
    if (importedEntity->isImport()) {
        ImportSourcePtr importSource = importedEntity->importSource();
        if (!importSource->hasModel()) {
            std::string url = resolvePath(importSource->url(), baseFile);
            std::ifstream file(url);
            if (file.good()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                auto parser = Parser::create();
                auto model = parser->parseModel(buffer.str());
                auto importer = Importer::create();
                importSource->setModel(model);
                importer->resolveImports(model, url);
            }
        }
    }
}

void resolveComponentImports(const ComponentEntityPtr &parentComponentEntity,
                             const std::string &baseFile)
{
    for (size_t n = 0; n < parentComponentEntity->componentCount(); ++n) {
        libcellml::ComponentPtr component = parentComponentEntity->component(n);
        if (component->isImport()) {
            resolveImport(component, baseFile);
        }
        resolveComponentImports(component, baseFile);
    }
}

void Importer::resolveImports(ModelPtr &model, const std::string &baseFile)
{
    for (size_t n = 0; n < model->unitsCount(); ++n) {
        libcellml::UnitsPtr units = model->units(n);
        resolveImport(units, baseFile);
    }
    resolveComponentImports(model, baseFile);
}


*/

// ******************************* FLATTENING FUNCTIONS *******************************

void flattenComponent(const ComponentEntityPtr &parent, const ComponentPtr &component, size_t index)
{
    if (component->isImport()) {
        auto model = owningModel(component);
        auto importSource = component->importSource();
        auto importModel = importSource->model();
        auto importedComponent = importModel->component(component->importReference());

        // Determine names of components already in use.
        NameList compNames = componentNames(model);

        // Determine the stack for the destination component.
        IndexStack destinationComponentBaseIndexStack = reverseEngineerIndexStack(component);

        // Determine the stack for the source component.
        IndexStack importedComponentBaseIndexStack = reverseEngineerIndexStack(importedComponent);

        // Generate equivalence map for the source component.
        EquivalenceMap map;
        recordVariableEquivalences(importedComponent, map, importedComponentBaseIndexStack);
        generateEquivalenceMap(importedComponent, map, importedComponentBaseIndexStack);

        // Rebase the generated equivalence map from the source component to the destination component.
        auto rebasedMap = rebaseEquivalenceMap(map, importedComponentBaseIndexStack, destinationComponentBaseIndexStack);

        // Take a copy of the imported component which will be used to replace the import defined in this model.
        auto importedComponentCopy = importedComponent->clone();
        importedComponentCopy->setName(component->name());
        for (size_t i = 0; i < component->componentCount(); ++i) {
            importedComponentCopy->addComponent(component->component(i));
        }

        // Get list of required units from component's variables.
        std::vector<UnitsPtr> requiredUnits = unitsUsed(importModel, importedComponentCopy);

        // Add all required units to a model so referenced units can be resolved.
        auto requiredUnitsModel = Model::create();
        for (const auto &units : requiredUnits) {
            requiredUnitsModel->addUnits(units);
        }

        // Make a map of component name to component pointer.
        ComponentNameMap newComponentNames = createComponentNamesMap(importedComponentCopy);
        for (const auto &entry : newComponentNames) {
            std::string newName = entry.first;
            size_t count = 1;
            while (std::find(compNames.begin(), compNames.end(), newName) != compNames.end()) {
                newName += "_" + convertToString(count++);
            }
            if (newName != entry.first) {
                entry.second->setName(newName);
            }
        }

        // If the component 'component' has variables then they are equivalent variables and they
        // need to be exchanged with the real variables from the component 'importedComponent'.
        for (size_t i = 0; i < component->variableCount(); ++i) {
            auto placeholderVariable = component->variable(i);
            for (size_t j = 0; j < placeholderVariable->equivalentVariableCount(); ++j) {
                auto localModelVariable = placeholderVariable->equivalentVariable(j);
                auto importedComponentVariable = importedComponentCopy->variable(placeholderVariable->name());
                Variable::removeEquivalence(placeholderVariable, localModelVariable);
                Variable::addEquivalence(importedComponentVariable, localModelVariable);
            }
        }
        parent->replaceComponent(index, importedComponentCopy);

        // Apply the rebased equivalence map onto the modified model.
        applyEquivalenceMapToModel(rebasedMap, model);

        // Copy over units used in imported component to this model.
        std::map<std::string, std::string> unitsNamesToReplace;
        for (const auto &u : requiredUnits) {
            if (!model->hasUnits(u)) {
                auto orignalName = u->name();
                size_t count = 0;
                while (!model->hasUnits(u) && model->hasUnits(u->name())) {
                    auto name = u->name();
                    name += "_" + convertToString(++count);
                    u->setName(name);
                }
                model->addUnits(u);
                if (orignalName != u->name()) {
                    unitsNamesToReplace[orignalName] = u->name();
                }
            }
        }
        findAndReplaceComponentsCnUnitsNames(importedComponentCopy, unitsNamesToReplace);
    }
}

void flattenComponentTree(const ComponentEntityPtr &parent, const ComponentPtr &component, size_t componentIndex)
{
    flattenComponent(parent, component, componentIndex);
    auto flattenedComponent = parent->component(componentIndex);
    for (size_t index = 0; index < flattenedComponent->componentCount(); ++index) {
        auto c = flattenedComponent->component(index);
        flattenComponentTree(flattenedComponent, c, index);
    }
}

// Move the flatten() function here:
ModelPtr Importer::flatten(const ModelPtr &inModel)
{
    if (inModel->hasUnresolvedImports()) {
        // Mimicking behaviour of previous flatten() function... not sure this is the way to go?
        return inModel;
    }
    auto model = inModel->clone();

    while (model->hasImports()) {
        // Go through Units and instantiate any imported Units.
        for (size_t index = 0; index < model->unitsCount(); ++index) {
            auto u = model->units(index);
            if (u->isImport()) {
                auto importedUnits = u->importSource()->model()->units(u->importReference());
                auto importedUnitsCopy = importedUnits->clone();
                importedUnitsCopy->setName(u->name());
                model->replaceUnits(index, importedUnitsCopy);
            }
        }

        // Go through Components and instatiate any imported Components
        for (size_t index = 0; index < model->componentCount(); ++index) {
            auto c = model->component(index);
            flattenComponentTree(model, c, index);
        }
    }

    model->linkUnits();
    return model;
}

} // namespace libcellml