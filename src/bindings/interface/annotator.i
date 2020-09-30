%module(package="libcellml") annotator

#define LIBCELLML_EXPORT

%import "createconstructor.i"
%import "enums.i"
%import "logger.i"
%import "types.i"

%include <std_map.i>
%include <std_multimap.i>
//%include <std_pair.i>
//%include <std_string.i>
%include <std_vector.i>
//%include <stdint.i>

%feature("docstring") libcellml::Annotator
"Interacts with CellML objects using their id attribute.";

%feature("docstring") libcellml::Annotator::setModel
"Set the model for this Annototor to work with.";

%feature("docstring") libcellml::Annotator::item
"Return the item with the given id.";

%feature("docstring") libcellml::Annotator::component
"Return the component with the given id.";

%feature("docstring") libcellml::Annotator::encapsulation
"Return the model with the given encapsulation id.";

%feature("docstring") libcellml::Annotator::model
"Return the model assigned to this Annotator or the model with the given id.";

%feature("docstring") libcellml::Annotator::variable
"Return the variable with the given id.";

%feature("docstring") libcellml::Annotator::importSource
"Return the import source with the given id.";

%feature("docstring") libcellml::Annotator::units
"Return the units with the given id.";

%feature("docstring") libcellml::Annotator::reset
"Return the reset with the given id.";

%feature("docstring") libcellml::Annotator::testValue
"Return the ResetPtr whose test_value has the given id.";

%feature("docstring") libcellml::Annotator::resetValue
"Return the ResetPtr whose reset_value has the given id.";

%feature("docstring") libcellml::Annotator::componentRef
"Return the ComponentPtr with the given component_ref id.";

%feature("docstring") libcellml::Annotator::unit
"Return the UnitsPtr and index whose Unit is with given id.";

%feature("docstring") libcellml::Annotator::connection
"Return a pair of VariablePtrs that define a connection with given id.";

%feature("docstring") libcellml::Annotator::mapVariables
"Return a pair of VariablePtrs that define a map_variables with given id.";

%feature("docstring") libcellml::Annotator::assignAllIds
"Traverse the stored model and set any blank id fields to an automatically generated id.";

%feature("docstring") libcellml::Annotator::assignIds
"Set all items of the given type in the stored model to automatically generated, unique strings.";

%feature("docstring") libcellml::Annotator::assignId
"Set the id of the given item to an automatically generated, unique string.";

%feature("docstring") libcellml::Annotator::clearAllIds
"Clear all the id strings in the given or stored model.";

%feature("docstring") libcellml::Annotator::isUnique
"Return `true` if the given string is not duplicated in the stored model, or `false` otherwise.";

%feature("docstring") libcellml::Annotator::ids
"Return a list of all id strings within the stored model.";

%feature("docstring") libcellml::Annotator::duplicateIds
"Return a list of id strings which are duplicated within the stored model.";

%feature("docstring") libcellml::Annotator::assignComponentId
"Assign an automatically generated, unique id to the given component.";

%feature("docstring") libcellml::Annotator::assignComponentRefId
"Assign an automatically generated, unique id to the given component's encapsulation.";

%feature("docstring") libcellml::Annotator::assignConnectionId
"Assign an automatically generated, unique id to the connection between the given variables.";

%feature("docstring") libcellml::Annotator::assignEncapsulationId
"Assign an automatically generated, unique id to the encapsulation of the given model.";

%feature("docstring") libcellml::Annotator::assignImportSourceId
"Assign an automatically generated, unique id to the given import source.";

%feature("docstring") libcellml::Annotator::assignMapVariablesId
"Assign an automatically generated, unique id to the equivalence between the given variables.";

%feature("docstring") libcellml::Annotator::assignModelId
"Assign an automatically generated, unique id to the given model.";

%feature("docstring") libcellml::Annotator::assignResetId
"Assign an automatically generated, unique id to the given reset.";

%feature("docstring") libcellml::Annotator::assignResetValueId
"Assign an automatically generated, unique id to the reset value of the given reset.";

%feature("docstring") libcellml::Annotator::assignTestValueId
"Assign an automatically generated, unique id to the test value of the given reset.";

%feature("docstring") libcellml::Annotator::assignUnitId
"Assign an automatically generated, unique id to the unit given by the units and index supplied.";

%feature("docstring") libcellml::Annotator::assignUnitsId
"Assign an automatically generated, unique id to the given units.";

%feature("docstring") libcellml::Annotator::assignVariableId
"Assign an automatically generated, unique id to the given variable.";

%feature("docstring") libcellml::Annotator::duplicateCount
"Return the number of items in the stored model with the given id.";

%feature("docstring") libcellml::Annotator::hasModel
"Report whether the annotator has a model assigned or not.";

%feature("docstring") libcellml::Annotator::dictionary
"Return the dictionary of ids to CellMLElement for this model.";

// PRIVATE: Functions only written to support bindings. They are not
// intended to be called from anywhere other than this file.

%feature("docstring") libcellml::Annotator::_itemTypeForSWIG
"Private: Utility function to retrieve item type integer based on id.";

%feature("docstring") libcellml::Annotator::_unitIndexForSWIG
"Private: Utility function to retrieve index of Unit item within parent Units item based on given id.";

%feature("docstring") libcellml::Annotator::_unitParentForSWIG
"Private: Utility function to retrieve UnitsPtr parent for Unit item with given id.";

%feature("docstring") libcellml::Annotator::_connectionForSWIG
"Private: Utility function to retrieve one of VariablePtrs connection with given id.";

%feature("docstring") libcellml::Annotator::_mapVariablesForSWIG
"Private: Utility function to retrieve one of VariablePtrs map_variables with given id.";

%feature("docstring") libcellml::Annotator::_assignConnectionIdForSWIG
"Private: Utility function to set a unique id for the connection between a given variable pair..";

%feature("docstring") libcellml::Annotator::_assignMapVariablesIdForSWIG
"Private: Utility function to set a unique id for the equivalence between a given variable pair.";

%feature("docstring") libcellml::Annotator::_assignUnitIdForSWIG
"Private: Utility function to set a unique id for the unit located by the given index and units.";

%{
#include <iostream>
#include "libcellml/annotator.h"
%}

//%template(IdCellMLElementMap) std::multimap< std::string, libcellml::CellMLElement>;

%create_constructor(Annotator)

//typedef unsigned int size_t;

//%template() std::pair<libcellml::VariablePtr, libcellml::VariablePtr>;
//%template(SmartStringPair) std::pair< std::shared_ptr< std::string >, int >;

%ignore libcellml::Annotator::item;
%ignore libcellml::Annotator::items;
//%ignore libcellml::Annotator::unit;
//%ignore libcellml::Annotator::connection;
//%ignore libcellml::Annotator::mapVariables;
%ignore libcellml::Annotator::assignId(const AnyItem &item);
%ignore libcellml::Annotator::dictionary;
//%ignore libcellml::Annotator::assignUnitId;

//typedef std::pair<libcellml::UnitsPtr, size_t> libcellml::UnitItem;
%pythoncode %{
# libCellML generated wrapper code starts here.

from libcellml import CellMLElement
%}

//%typemap(out) IdCellMLElementMap {
//  $result = PyInt_FromLong($1);
//}

%template() std::vector<std::string>;
%template() std::vector<libcellml::CellMLElement>;

%include "libcellml/annotator.h"

%template(UnitItem) std::pair< libcellml::UnitsPtr, size_t >;
%template(VariablePair) std::pair< libcellml::VariablePtr, libcellml::VariablePtr >;

//using UnitItem = std::pair<libcellml::UnitsPtr, size_t>;

//%rename(bar_id) bar(int, double);
//%rename(_item) item;

//

//%template(IdCellMLElementMap) std::multimap< std::string, libcellml::CellMLElement>;

//%template(AnyItem) std::pair< libcellml::CellMLElement, libcellml::EntityPtr >;
//typedef std::shared_ptr< libcellml::Units > UnitsPtr;
//typedef std::shared_ptr< libcellml::VariablePtr > VariablePtr;


%extend libcellml::Annotator {

    CellMLElement _itemCellMLElement(const std::string &id, size_t index)
    {
        auto itemInfo = $self->item(id, index);
        return itemInfo.first;
    }

//    int _unitIndexForSWIG(const std::string &id, size_t index)
//    {
//        auto u = $self->unit(id, index);
//        if(u.first == nullptr){
//            return -1;
//        }
//        return u.second;
//    }

//    UnitsPtr _unitParentForSWIG(const std::string &id, size_t index)
//    {
//        auto u = $self->unit(id, index);
//        return u.first;
//    }

 //   VariablePtr _connectionForSWIG(const std::string &id, bool useFirst, const size_t &index)
 //   {
//        auto vPair = $self->connection(id, index);
//        if (useFirst) {
//            return vPair.first;
//        }
//        return vPair.second;
//    }

 //   VariablePtr _mapVariablesForSWIG(const std::string &id, bool useFirst, const size_t &index)
 //   {
 //       auto vPair = $self->mapVariables(id, index);
 //       if (useFirst) {
 //           return vPair.first;
 //       }
 //       return vPair.second;
 //   }

//    std::string _assignConnectionIdForSWIG(VariablePtr &item, VariablePtr &item2) {
//        libcellml::VariablePair variablePair = std::make_pair(item, item2);
//        return $self->assignConnectionId(variablePair);
//    }

//    std::string _assignMapVariablesIdForSWIG(VariablePtr &item, VariablePtr &item2) {
//        libcellml::VariablePair variablePair = std::make_pair(item, item2);
//        return $self->assignMapVariablesId(variablePair);
//    }

 //   std::string assignUnitId(const UnitItem &item) {
 //       libcellml::UnitItem unitItem = std::make_pair(item, index);
 //       return $self->assignUnitId(unitItem);
 //   }

//    std::vector<std::string> _dictionaryForSWIG(bool first) {
//        std::vector<std::string> rtn;
//        if(first){
//            for( auto &i : $self->dictionary()) {
//                rtn.push_back(i.first);
//            }
//        }
//        else {
//            for( auto &i : $self->dictionary()) {
//                rtn.push_back(cellMLElementAsString(i.second));
//            }
//        }
//        return rtn;
//    }
    std::vector<std::string> _dictionaryFirst() {
        std::vector<std::string> rtn;
        for( auto &i : $self->dictionary()) {
            rtn.push_back(i.first);
        }
        return rtn;
    }
    std::vector<enum libcellml::CellMLElement> _dictionarySecond() {
        std::vector<enum libcellml::CellMLElement> rtn;
        for( auto &i : $self->dictionary()) {
        std::cout << "element: " << cellMLElementAsString(i.second) << std::endl;
            rtn.push_back(i.second);
        }
        return rtn;
    }

%pythoncode %{

    #def assignId(self, a, b=None, c=None):
    #    r"""Sets the given item's id to an automatically generated, unique string."""
    #    if b is None:
    #        type = a[0]
    #        if type == CellMLElement.CONNECTION or type == CellMLElement.MAP_VARIABLES or type == CellMLElement.UNIT:
    #            item2 = a[1][1]
    #            item = a[1][0]
    #        else:
    #            item = a[1]
    #    elif c is None:
    #        item = a
    #        type = b
    #    else:
    #        item = a
    #        item2 = b
    #        type = c

#        if type == CellMLElement.COMPONENT:
#            return _annotator.Annotator_assignComponentId(self, item)
#        elif type == CellMLElement.COMPONENT_REF:
#            return _annotator.Annotator_assignComponentRefId(self, item)
#        elif type == CellMLElement.ENCAPSULATION:
#            return _annotator.Annotator_assignEncapsulationId(self, item)
#        elif type == CellMLElement.IMPORT:
#            return _annotator.Annotator_assignImportSourceId(self, item)
#        elif type == CellMLElement.MODEL:
#            return _annotator.Annotator_assignModelId(self, item)
#        elif type == CellMLElement.RESET:
#            return _annotator.Annotator_assignResetId(self, item)
#        elif type == CellMLElement.RESET_VALUE:
#            return _annotator.Annotator_assignResetValueId(self, item)
#        elif type == CellMLElement.TEST_VALUE:
#            return _annotator.Annotator_assignTestValueId(self, item)
#        elif type == CellMLElement.UNITS:
#            return _annotator.Annotator_assignUnitsId(self, item)
#        elif type == CellMLElement.VARIABLE:
#            return _annotator.Annotator_assignVariableId(self, item)
#        if type == CellMLElement.CONNECTION:
#            return _annotator.Annotator_assignConnectionId(self, item)
#        elif type == CellMLElement.MAP_VARIABLES:
#            return _annotator.Annotator_assignMapVariablesId(self, item)
#        elif type == CellMLElement.UNIT:
#            print("here")
#            print(dir(item))
#            print(item)
#            return _annotator.Annotator_assignUnitId(self, item)
#        return ""

#    def unit(self, id, index=0):
#        r"""Return the UnitItem with the given id.  The first item is the parent UnitsPtr item, the second is the index of this unit."""
#        return (_annotator.Annotator__unitParentForSWIG(self, id, index), _annotator.Annotator__unitIndexForSWIG(self, id, index))

#    def connection(self, id, index=0):
#        r"""Retrieve a tuple of VariablePtr items from a connection with the given id."""
#        return (_annotator.Annotator__connectionForSWIG(self, id, True, index), _annotator.Annotator__connectionForSWIG(self, id, False, index))

#    def mapVariables(self, id, index=0):
#        r"""Retrieve a tuple of VariablePtr items from a map_variables with the given id."""
#        return (_annotator.Annotator__mapVariablesForSWIG(self, id, True, index), _annotator.Annotator__mapVariablesForSWIG(self, id, False, index))

    def item(self, id, index=-1):
        r"""Retrieve a unique item with the given id."""
        if index == -1 and not _annotator.Annotator_isUnique(self, id):
            return (CellMLElement.UNDEFINED, None)

        if index == -1:
            index = 0

        type = _annotator.Annotator__itemCellMLElement(self, id, index)
        if type == CellMLElement.COMPONENT:
            return (type, _annotator.Annotator_component(self, id, index))
        elif type == CellMLElement.COMPONENT_REF:
            return (type, _annotator.Annotator_componentRef(self, id, index))
        elif type == CellMLElement.CONNECTION:
            return (type, _annotator.Annotator_connection(self, id, index))
        elif type == CellMLElement.ENCAPSULATION:
            return (type, _annotator.Annotator_encapsulation(self, id, index))
        elif type == CellMLElement.IMPORT:
            return (type, _annotator.Annotator_importSource(self, id, index))
        elif type == CellMLElement.MAP_VARIABLES:
            return (type, _annotator.Annotator_mapVariables(self, id, index))
        elif type == CellMLElement.MODEL:
            return (type, _annotator.Annotator_model(self, id, index))
        elif type == CellMLElement.RESET:
            return (type, _annotator.Annotator_reset(self, id, index))
        elif type == CellMLElement.RESET_VALUE:
            return (type, _annotator.Annotator_resetValue(self, id, index))
        elif type == CellMLElement.TEST_VALUE:
            return (type, _annotator.Annotator_testValue(self, id, index))
        elif type == CellMLElement.UNIT:
            return (type, _annotator.Annotator_unit(self, id, index))
        elif type == CellMLElement.UNITS:
            return (type, _annotator.Annotator_units(self, id, index))
        elif type == CellMLElement.VARIABLE:
            return (type, _annotator.Annotator_variable(self, id, index))
        return (CellMLElement.UNDEFINED, None)

    def items(self, id):
        r"""Returns everything with the given id as as list of (type, item) tuples."""
        count = _annotator.Annotator_duplicateCount(self, id)
        itemsList = []
        for c in range(0, count):
            items_with_id = self.item(id, c)
            itemsList.append(items_with_id)
        return itemsList

    def dictionary(self):
        r"""Return a dictionary of all id strings and their type within the stored model."""
        rtn = {}
        #print()
        #print("dictionary")
        #d = _annotator.Annotator_dictionary(self)
        #print(d)
        #for entry in d:
        #    print("entry: ", entry)

        #t = d.next()
        #while t is not None:
        #    print(t)

        f = _annotator.Annotator__dictionaryFirst(self)
        s = _annotator.Annotator__dictionarySecond(self)
        print()
        print("========")
        print(f)
        print(s)
        for k, v in zip(_annotator.Annotator__dictionaryFirst(self), _annotator.Annotator__dictionarySecond(self)):
            if k in rtn:
                rtn[k].append(v)
            else:
                rtn[k] = [v]

        return rtn
%}


}

//typedef std::shared_ptr< libcellml::Units > UnitsPtr;
//%template(SharedUnits) std::shared_ptr< libcellml::Units >;

//%include <std_shared_ptr.i>
//%shared_ptr( std::string )

//%template(Thingy) std::shared_ptr< std::string >;
//%template(SmartPair) std::pair< std::shared_ptr< std::string >, int >;
//%template(StringInt) std::pair< std::string, int >;

//%include "libcellml/test.h"


