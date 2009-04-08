/*
 * opencog/embodiment/Control/MessagingSystem/Pet.h
 *
 * Copyright (C) 2007-2008 Carlos Lopes
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
#ifndef PET_H
#define PET_H

#include <time.h>
#include <string>
#include <exception>

#include <opencog/atomspace/HandleSet.h>
#include <opencog/atomspace/AtomSpace.h>

#include "PAI.h"
#include "ActionPlan.h"
#include "MessageSender.h"
#include "PetInterface.h"

using namespace opencog;

namespace OperationalPetController
{

class RuleEngine;

// PetMode
enum PetMode {
    LEARNING,
    PLAYING,
    SCAVENGER_HUNT
};

/**
 *
 */
class Pet : public Control::PetInterface
{



private:

    // pet metadata
    std::string petId;
    std::string petName;
    std::string agentTraits;
    std::string ownerId;

    // NOTE: pet class will became a generic Agent class so that it could be
    // used by pets or humanoids. So the type used here.
    std::string agentType;

    double rayOfVicinity;

    PetMode mode;
    std::map<PetMode, Control::AgentModeHandler*> modeHandler;

    // opc components received as constructor parameter
    AtomSpace* atomSpace;
    MessageSender* sender;
    RuleEngine* ruleEngine;

    PerceptionActionInterface::PAI* pai;

    std::string exemplarAvatarId;       // the Id of the avatar that is
    // performing the exemplars
    std::string triedSchema;   // last schema tried
    std::vector<std::string> learningSchema;   // schema being learned

    unsigned long exemplarStartTimestamp; // start/end timestamp to get behaviors from
    unsigned long exemplarEndTimestamp;  // the atomSpace.

    unsigned long startLearningSessionTimestamp;
    unsigned long endLearningSessionTimestamp;

    bool candidateSchemaExecuted;


    std::string grabbedObjId; // the id of the object the pet has on its mouth,
    // empty if there is no object grabbed.

    opencog::RandGen* rng;

    /**
     * Populate atom space with behavior nodes
     */
    void executeBehaviorEncoder();

    /**
     * Update the maps that should be kept (persisted) in spaceServer during the last exemplar session.
     */
    void updatePersistentSpaceMaps() throw (opencog::RuntimeException, std::bad_exception);

    /**
     * Update is_exemplar_avatar predicate for the pet based on exemplarAvatarId object.
     *
     * @param active Inform if the predicate is active or not. An active predicate has its
     * TV set to 1.0 and 0.0 otherwise.
     */
    void adjustIsExemplarAvatarPredicate(bool active) throw (opencog::RuntimeException);

public:

    static const unsigned long UNDEFINED_TIMESTAMP;

    /**
     * A requested command is a trick sent to the pet to be executed
     */
    class RequestedCommand
    {
    public:
        inline RequestedCommand( void ) : name( "" ), readed( true ) { }
        virtual inline ~RequestedCommand( void ) { }
        //
        std::string name;
        std::vector<std::string> arguments;
        bool readed;
    };

    /**
     * Pet constructor.
     *
     * @param petID The SL id of the pet.
     * @param name The name the pet.
     * @param agentType The agent type (pet or humanoid)
     * @param agentTraits The agent traits type
     * @param ownerID The SL id of the owner of the pet.*
     * @param atomSpace A atomSpace with the Pet short memory AtomTable.
     */
    Pet(const std::string& petId, const std::string& petName, const std::string& agentType,
        const std::string& agentTraits, const std::string& ownerID, AtomSpace* atomSpace, MessageSender* sender);
    ~Pet();

    /**
     * Set the PAI of this Pet
     */
    void setPAI(PerceptionActionInterface::PAI* pai);

    inline PerceptionActionInterface::PAI& getPai( void ) {
        return *pai;
    }

    /**
     * Set/Get the RuleEngine of this Pet
     */
    void setRuleEngine(RuleEngine* ruleEngine);
    inline RuleEngine* getRuleEngine( void ) {
        return ruleEngine;
    }

    /**
     * Init AtomSpace with pet traits, feelings and other atoms
     */
    void initTraitsAndFeelings();

    /**
     * Getter and setter for the pet name.
     */
    const std::string& getName() const;
    void  setName(const std::string& petName);

    /**
     * Getter for agent type. An agent should never have its type changed
     */
    const std::string& getType() const;

    /**
     * Getter for pet id. A pet should never have its id changed
     */
    const std::string& getPetId() const;

    /**
     * Getter and setter for owner id.
     */
    const std::string& getOwnerId() const;
    const std::string& getExemplarAvatarId() const;
    void  setOwnerId(const std::string& ownerId);

    // always use this method, even inside the class because it controls
    // the is_teacher predicate for the pet
    void  setExemplarAvatarId(const std::string& exemplarAvatarId);

    /**
     * Getter and setter for  pet mode (PLAYING or LEARNING).
     */
    const PetMode getMode() const;
    void  setMode(PetMode);

    /**
     * Get the tried and learning schemata names
     */
    const std::string & getTriedSchema();
    const std::vector<std::string> & getLearningSchema();

    void setTriedSchema(const std::string & triedSchema);

    unsigned long getExemplarStartTimestamp();
    unsigned long getExemplarEndTimestamp();

    /**
     * Restart a learning process when the LS goes down. This method should
     * ONLY be called when the LS goes up again and the Pet is still in
     * LEARNING mode.
     */
    void restartLearning() throw (opencog::RuntimeException, std::bad_exception);


    /**
     * Create a new Pet loading pet metadata from file.
     *
     * @param filename The name of the file with the pet metadata.
     * @param petId The pet identification number.
     * @param atomSpace An AtomSpace containing the pet short memory.
     *
     * @return The pet newly created.
     */
    static Pet* importFromFile(const std::string& filename, const std::string& petId, AtomSpace* atomSpace, MessageSender* sender);

    /**
     * Save pet metadata into file.
     *
     * @param filename The name of the file where data will be written.
     * @param pet The pet whose metadata will be written.
     */
    static void exportToFile(const std::string& filename, Pet & pet) throw (opencog::IOException, std::bad_exception);

    // IMPLEMENTATION OF METHODS OF PetInterface (getPetId() is already defined above):

    AtomSpace& getAtomSpace();

    void stopExecuting(const std::vector<std::string> &commandStatement, unsigned long timestamp);

    bool isInLearningMode() const;
    void startLearning(const std::vector<std::string> &commandStatement, unsigned long timestamp);
    void stopLearning(const std::vector<std::string> &commandStatement, unsigned long timestamp);

    bool isExemplarInProgress() const;
    void startExemplar(const std::vector<std::string> &commandStatement, unsigned long timestamp);
    void endExemplar(const std::vector<std::string> &commandStatement, unsigned long timestamp);

    void trySchema(const std::vector<std::string> &commandStatement, unsigned long timestamp);
    void reward(unsigned long timestamp);
    void punish(unsigned long timestamp);

    Control::AgentModeHandler& getCurrentModeHandler( void );

    float computeWalkingSpeed() const;

    const std::string& getTraits( void ) const;

    unsigned long getLatestRewardTimestamp( void );
    unsigned long getLatestPunishmentTimestamp( void );

    void getAllActionsDoneInATrickAtTime(const Temporal& time, HandleSeq& actionsDone);
    void getAllObservedActionsDoneAtTime(const Temporal& time, HandleSeq& actionsDone);
    bool isNear(const Handle& objectHandle);
    bool getVicinityAtTime(unsigned long timestamp, HandleSeq& petVicinity);
    void getHighLTIObjects(HandleSeq& highLTIObjects);

    /**
     * @see PetInterface::setLatestGotoTarget
     */
    void setLatestGotoTarget( const std::pair<std::string, Spatial::Point>& target ) {
        this->targetObject = target;
    };
    /**
     * @see PetInterface::getLatestGotoTarget
     */
    const std::pair<std::string, Spatial::Point>& getLatestGotoTarget( void ) {
        return this->targetObject;
    };

    void setRequestedCommand(string command, vector<string> parameters);

    inline RequestedCommand& getLatestRequestedCommand( void ) {
        lastRequestedCommand.readed = true;
        return lastRequestedCommand;
    };

    inline bool isRequestedCommandNotReaded( void ) {
        return !lastRequestedCommand.readed;
    }

    /**
     * Callback method called by RuleEngine to indicate the candidate schema was selected to be executed
     */
    void schemaSelectedToExecute(const std::string& schemaName);

    Handle getMyHandle() const;

    // functions used to set, get and verify if Pet has something in its
    // mouth, i.e., if it has grabbed something

    /**
     * Set the id of the grabbed object, i.e., the object that is in pet's
     * mouth
     *
     * @param The object id
     */
    void setGrabbedObj(const string& id);

    /**
     * Return the id of the grabbed object, i.e., the object that is in pet's
     * mouth
     *
     * @return The object id
     */
    const std::string& getGrabbedObj();

    /**
     * Return true if grabbed object name is different from an empty string,
     * false otherwise.
     */
    bool hasGrabbedObj();

    RequestedCommand lastRequestedCommand;

    unsigned long latestRewardTimestamp;
    unsigned long latestPunishmentTimestamp;

    // target object used by goto and gonear combo functions
    std::pair<std::string, Spatial::Point> targetObject;

}; // class
}  // namespace

#endif