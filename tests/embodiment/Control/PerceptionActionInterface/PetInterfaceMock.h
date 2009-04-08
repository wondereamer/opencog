#ifndef PET_INTERFACE_MOCK_H_
#define PET_INTERFACE_MOCK_H_

#include "PetInterfaceMock.h"
#include "behavior/BE.h"
#include "behavior/BDTracker.h"
#include "PredefinedProcedureNames.h"
#include "behavior/PAIWorldProvider.h"
#include "PetInterface.h"
#include "PAIUtils.h"
#include "AgentModeHandler.h"

#include <cassert>

class DummyAgentModeHandler : public Control::AgentModeHandler
{
public:
    DummyAgentModeHandler( void ) : modeName( "DUMMY" ) {}
    virtual ~DummyAgentModeHandler( void ) { }
    void handleCommand( const std::string& name, const std::vector<std::string>& arguments ) {
    }
    inline const std::string& getModeName( void ) {
        return this->modeName;
    }
    inline void update( void ) { }
private:
    std::string modeName;
};

using namespace behavior;
using namespace PerceptionActionInterface;
using namespace opencog;
using namespace std;


class PetInterfaceMock: public Control::PetInterface
{

    std::pair<std::string, Spatial::Point> latestGotoTarget;
    string pet;
    string petName;
    string ownerId;
    string exemplarAvatarId;
    string grabbedObjId;
    PerceptionActionInterface::PAI* pai;
    bool isLearning;
    set<string> learningWhat;
    bool exemplarInProgress;
    string whatExemplar;
    unsigned long exemplarStartTimestamp;
    unsigned long exemplarEndTimestamp;
    string traits;
    string type;
    DummyAgentModeHandler modeHandler;

    // Method copied from Pet.cc on September 14th, 2007
    void executeBehaviorEncoder(const char* learningSchema) {
        string pet_id = "pet_id";

        AtomSpace& atomSpace = getAtomSpace();

        // Define the behavior interval
        Temporal exemplarTimeInterval(exemplarStartTimestamp, exemplarEndTimestamp);
        Handle trickConceptNode = atomSpace.addNode(CONCEPT_NODE, learningSchema);
        Handle trickExemplarAtTimeLink = atomSpace.addTimeInfo(trickConceptNode, exemplarTimeInterval);
        BehaviorEncoder encoder(new PAIWorldProvider(pai), pet_id, trickExemplarAtTimeLink, 1);

        // Adds the inheritance link as Ari asked
        Handle exemplarConceptNode = atomSpace.addNode(CONCEPT_NODE, "exemplar");
        HandleSeq inhLinkHS;
        inhLinkHS.push_back(trickConceptNode);
        inhLinkHS.push_back(exemplarConceptNode);
        atomSpace.addLink(INHERITANCE_LINK, inhLinkHS);

        // position tracker
        atom_tree *positionTemplate =
            makeVirtualAtom(EVALUATION_LINK,
                            makeVirtualAtom(atomSpace.addNode(PREDICATE_NODE, AGISIM_POSITION_PREDICATE_NAME), NULL),
                            makeVirtualAtom(NODE, NULL), NULL
                           );

        encoder.addBETracker(*positionTemplate, new ActionBDTracker(&atomSpace));

        // action tracker
        atom_tree *actionTemplate =
            makeVirtualAtom(EVALUATION_LINK,
                            makeVirtualAtom(atomSpace.addNode(PREDICATE_NODE, ACTION_DONE_PREDICATE_NAME), NULL),
                            makeVirtualAtom(NODE, NULL),
                            NULL
                           );

        encoder.addBETracker(*actionTemplate, new MovementBDTracker(&atomSpace));

        // TODO: Use the exemplarEndTimestamp as well -- the current BehaviorEncoder considers the "NOW" as end of the examplar interval.
        Temporal startTime(exemplarStartTimestamp);
        encoder.update(startTime);
    }

public:
    PetInterfaceMock() {
        pet = PAIUtils::getInternalId("1");
        petName = "no_name";
        //ownerId = "no_owner_id";
        ownerId = PAIUtils::getInternalId("2");
        exemplarInProgress = false;
        grabbedObjId.assign("");
        type = "pet";
        traits = "Maxie";
        isLearning = false;
        pai = NULL;
    }

    PetInterfaceMock(string _pet, string _petName, string _ownerId) {

        pet = _pet;
        petName = _petName;
        ownerId = _ownerId;

        exemplarInProgress = false;
        isLearning = false;
        pai = NULL;
    }

    void setPAI(PerceptionActionInterface::PAI* _pai) {
        pai = _pai;
    }

    const string& getPetId() const {
        return pet;
    }
    AtomSpace& getAtomSpace() {
        return pai->getAtomSpace();
    }

    void stopExecuting(const vector<string> &commandStatement, unsigned long timestamp) {
        cout << "PetInterfaceMock: stop executing '" << commandStatement.front() << "' at " << timestamp << endl;
    }

    bool isInLearningMode() const {
        return isLearning;
    }
    void startLearning(const vector<string> &commandStatement, unsigned long timestamp) {
        cout << "PetInterfaceMock: start learning '" << commandStatement.front() << "' at " << timestamp << endl;
        isLearning = true;
        learningWhat.insert(commandStatement.front());
    }
    void stopLearning(const vector<string> &commandStatement, unsigned long timestamp) {
        cout << "PetInterfaceMock: stop learning '" << commandStatement.front() << "' at " << timestamp << endl;
        isLearning = false;
        learningWhat.erase(commandStatement.front());
    }

    bool isExemplarInProgress() const {
        return exemplarInProgress;
    }
    void startExemplar(const vector<string> &commandStatement, unsigned long timestamp) {
        if (exemplarInProgress) {
            endExemplar(vector<string>(1, whatExemplar), timestamp);
        }
        exemplarInProgress = true;
        whatExemplar = commandStatement.front();
        cout << "PetInterfaceMock: Exemplar of '" << whatExemplar << "' started at " << timestamp << " done by " << exemplarAvatarId << endl;
        exemplarStartTimestamp = timestamp;
    }
    void endExemplar(const vector<string> &commandStatement, unsigned long timestamp) {
        // an exemplar should be in progress
        assert(exemplarInProgress);

        cout << "PetInterfaceMock: Exemplar of '" << whatExemplar << "' ended at " << timestamp << " done by " << exemplarAvatarId << endl;
        exemplarEndTimestamp = timestamp;
        exemplarInProgress = false;

        executeBehaviorEncoder(commandStatement.front().c_str());
    }

    void trySchema(const vector<string> &commandStatement, unsigned long timestamp) {
        cout << "PetInterfaceMock: TrySchema!" << endl;
    }

    void reward(unsigned long timestamp) {
        cout << "PetInterfaceMock: Rewarding pet!" << endl;
    }
    void punish(unsigned long timestamp) {
        cout << "PetInterfaceMock: Punishing pet!" << endl;
    }

    Control::AgentModeHandler& getCurrentModeHandler( void ) {
        return modeHandler;
    }


    void boostSGILink(Handle sgiLink) {
        cout << "PetInterfaceMock: boostSGILink(" << sgiLink << ") called!" << endl;
    }

    void setOwnerId(const string& _ownerId) {
        ownerId = _ownerId;
    }
    void setExemplarAvatarId(const string& _exemplarAvatarId) {
        exemplarAvatarId = _exemplarAvatarId;
    }
    const string& getOwnerId() const {
        return ownerId;
    }
    const string& getExemplarAvatarId() const {
        return exemplarAvatarId;
    }
    void setName(const string& _petName) {
        petName = _petName;
    }
    const string& getName() const {
        return petName;
    }

    void getAllActionsDoneInATrickAtTime(const Temporal& recentPeriod, HandleSeq& actionsDone) {}
    void getAllObservedActionsDoneAtTime(const Temporal& recentPeriod, HandleSeq& behaviourDescriptions) {}
    bool isNear(const Handle& objectHandle) {
        return false;
    }
    bool getVicinityAtTime(unsigned long timestamp, HandleSeq& petVicinity) {
        return true;
    }
    void getHighLTIObjects(HandleSeq& highLTIObjects) {};
    void setLatestGotoTarget( const std::pair<std::string, Spatial::Point>& target ) {
        cout << "PetInterfaceMock: setLatestGotoTarget!" << endl;
    }
    const std::pair<std::string, Spatial::Point>& getLatestGotoTarget( void ) {
        cout << "PetInterfaceMock: getLatestGotoTarget!" << endl;
        return this->latestGotoTarget;
    }
    void setRequestedCommand(string command, vector<string> parameters) {}

    // functions used to set, get and verify if Pet has something in its
    // mouth, i.e., if it has grabbed something
    void setGrabbedObj(const string& id) {
        grabbedObjId = id;
    }

    const std::string& getGrabbedObj() {
        return grabbedObjId;
    }

    bool hasGrabbedObj() {
        return !grabbedObjId.empty();
    }

    const std::string& getTraits( void ) const {
        return traits;
    }
    const std::string& getType( void ) const {
        return type;
    }

};

#endif /*PET_INTERFACE_MOCK_H_*/