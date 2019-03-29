/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEChange_DemandElement.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// A network change in which a busStop is created or deleted
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNEVehicleTypeFrame.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEViewNet.h>

#include "GNEChange_DemandElement.h"

// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_DemandElement, GNEChange, nullptr, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_DemandElement::GNEChange_DemandElement(GNEDemandElement* demandElement, bool forward) :
    GNEChange(demandElement->getViewNet()->getNet(), forward),
    myDemandElement(demandElement),
    myEdgeParents(demandElement->getEdgeParents()),
    myDemandElementParents(demandElement->getDemandElementParents()) {
    myDemandElement->incRef("GNEChange_DemandElement");
}


GNEChange_DemandElement::~GNEChange_DemandElement() {
    assert(myDemandElement);
    myDemandElement->decRef("GNEChange_DemandElement");
    if (myDemandElement->unreferenced()) {
        // show extra information for tests
        WRITE_DEBUG("Deleting unreferenced " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "'");
        // make sure that demand element isn't in net before removing
        if (myNet->demandElementExist(myDemandElement)) {
            myNet->deleteDemandElement(myDemandElement, false);
        }
        delete myDemandElement;
    }
}


void
GNEChange_DemandElement::undo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // 2 - If demand element own a edge parent, remove it from edge
        for (auto i : myEdgeParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // 3 - If demand element has parents, remove it from their demand element childs
        for (auto i : myDemandElementParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // delete demand element of net
        myNet->deleteDemandElement(myDemandElement, false);
        // update vehicle type frame if it's shown
        if (myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
            myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // insert demand element of test
        myNet->insertDemandElement(myDemandElement);
        // update vehicle type frame if it's shown
        if (myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
            myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
        }
        // 2 - If demand element own a edge parent, add it to edge
        for (auto i : myEdgeParents) {
            i->addDemandElementChild(myDemandElement);
        }
        // 3 - If demand element has parents, add it into demand element childs
        for (auto i : myDemandElementParents) {
            i->addDemandElementChild(myDemandElement);
        }
    }
    // Requiere always save demandElements
    myNet->requiereSaveDemandElements(true);
}


void
GNEChange_DemandElement::redo() {
    if (myForward) {
        // show extra information for tests
        WRITE_DEBUG("Adding " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // insert demand element into net
        myNet->insertDemandElement(myDemandElement);
        // update vehicle type frame if it's shown
        if (myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
            myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
        }
        // 2 - If demand element own a edge parent, add it to edge
        for (auto i : myEdgeParents) {
            i->addDemandElementChild(myDemandElement);
        }
        // 3 - If demand element has parents, add it into demand element childs
        for (auto i : myDemandElementParents) {
            i->addDemandElementChild(myDemandElement);
        }
    } else {
        // show extra information for tests
        WRITE_DEBUG("Removing " + myDemandElement->getTagStr() + " '" + myDemandElement->getID() + "' in GNEChange_DemandElement");
        // 2 - If demand element own a edge parent, remove it from edge
        for (auto i : myEdgeParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // 3 - If demand element has parents, remove it from their demand element childs
        for (auto i : myDemandElementParents) {
            i->removeDemandElementChild(myDemandElement);
        }
        // delete demand element of net
        myNet->deleteDemandElement(myDemandElement, false);
        // update vehicle type frame if it's shown
        if (myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->shown()) {
            myNet->getViewNet()->getViewParent()->getVehicleTypeFrame()->getVehicleTypeSelector()->refreshVehicleTypeSelector();
        }
    }
    // Requiere always save demandElements
    myNet->requiereSaveDemandElements(true);
}


FXString
GNEChange_DemandElement::undoName() const {
    if (myForward) {
        return ("Undo create " + myDemandElement->getTagStr()).c_str();
    } else {
        return ("Undo delete " + myDemandElement->getTagStr()).c_str();
    }
}


FXString
GNEChange_DemandElement::redoName() const {
    if (myForward) {
        return ("Redo create " + myDemandElement->getTagStr()).c_str();
    } else {
        return ("Redo delete " + myDemandElement->getTagStr()).c_str();
    }
}
