/*
 * opencog/embodiment/PetComboVocabulary/pet_perception.cc
 *
 * Copyright (C) 2007-2008 Nil Geisweiller
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "pet_perception.h"
#include "comboreduct/combo/type_tree.h"

using namespace combo;
using namespace pet_perception_properties;

pet_perception::pet_perception()
{
    _ultrametric = false;
    _transitive = false;
    _irreflexive = false;
    _reflexive = false;
    _symmetric = false;
    _identity_of_indiscernibles = false;
}

const perception_basic_description* pet_perception::get_basic_description_array() const
{
    return pet_perception_properties::pbd;
}

unsigned int pet_perception::get_basic_description_array_count() const
{
    return sizeof(pet_perception_properties::pbd) / sizeof(basic_description);
}

const pet_perception* pet_perception::init_perceptions()
{
    static pet_perception* perceptions =
        new pet_perception[id::pet_perception_count];
    for (unsigned int i = 0; i < id::pet_perception_count; i++)
        perceptions[i].set_perception((pet_perception_enum)i);
    return perceptions;
}

void pet_perception::set_perception(pet_perception_enum ppe)
{
    opencog::cassert(TRACE_INFO, ppe < id::pet_perception_count);
    _enum = ppe;
    //fill the various properties using the arrays edited by the developer
    set_basic_description(ppe);
    //standard properties specific to action
    unsigned int ppd_count = sizeof(ppd) / sizeof(perception_property_description);
    opencog::cassert(TRACE_INFO,
                     ppd_count == (unsigned int)id::pet_perception_count,
                     "there must be entries for all perceptions.");
    bool found = false;
    for (unsigned int i = 0; i < ppd_count && !found; ++i) {
        if (ppd[i].perception == ppe) {
            found = true;
            _ultrametric = ppd[i].ultrametric;
            _transitive = ppd[i].transitive;
            _irreflexive = ppd[i].irreflexive;
            _reflexive = ppd[i].reflexive;
            _symmetric = ppd[i].symmetric;
            _identity_of_indiscernibles = ppd[i].identity_of_indiscernibles;
        }
    }
    opencog::cassert(TRACE_INFO, found,
                     "pet_perception with enum %d has not been found in ppd", ppe);
}

perception pet_perception::instance(const std::string& name)
{
    //look up for pet_builtin_action_enum corresponding to that name
    bool found = false;
    perception p = NULL;
    for (unsigned int i = 0; i < id::pet_perception_count && !found; i++) {
        p = pet_perception::instance((pet_perception_enum)i);
        found = p->get_name() == name;
    }
    return (found ? p : NULL);
}

perception pet_perception::instance(pet_perception_enum ppe)
{
    static const pet_perception* perceptions = init_perceptions();
    opencog::cassert(TRACE_INFO, ppe < id::pet_perception_count);
    return static_cast<perception>(&perceptions[ppe]);
}

const std::string& pet_perception::get_name() const
{
    return _name;
}

const type_tree& pet_perception::get_type_tree() const
{
    return _type_tree;
}

arity_t pet_perception::arity() const
{
    return _arity;
}

type_tree pet_perception::get_output_type_tree() const
{
    return _output_type;
}

const type_tree& pet_perception::get_input_type_tree(arity_t i) const
{
    return argument_type_list_input_type(_arg_type_tree, _arity, i);
}

bool pet_perception::is_ultrametric() const
{
    return _ultrametric;
}
bool pet_perception::is_transitive() const
{
    return _transitive;
}
bool pet_perception::is_irreflexive() const
{
    return _irreflexive;
}
bool pet_perception::is_reflexive() const
{
    return _reflexive;
}
bool pet_perception::is_symmetric() const
{
    return _symmetric;
}
bool pet_perception::is_identity_of_indiscernibles() const
{
    return _identity_of_indiscernibles;
}
