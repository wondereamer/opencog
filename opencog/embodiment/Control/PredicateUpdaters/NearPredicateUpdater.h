/*
 * opencog/embodiment/Control/PredicateUpdaters/NearPredicateUpdater.h
 *
 * Copyright (C) 2007-2008 Ari Heljakka, Welter Luigi
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
#ifndef NEARPREDICATEUPDATER_H_
#define NEARPREDICATEUPDATER_H_

/**
 * This class is used to update the near predicates whenever an object
 * changes its position in the latest SpaceMap.
 */
#include "BasicPredicateUpdater.h"
#include <opencog/atomspace/AtomSpace.h>

namespace OperationalPetController
{

class NearPredicateUpdater : public OperationalPetController::BasicPredicateUpdater
{

public:

    NearPredicateUpdater(AtomSpace& _atomSpace);
    ~NearPredicateUpdater();

    void update(Handle object, Handle pet, unsigned long timestamp );

}; // class
}  // namespace

#endif
