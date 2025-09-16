//all the necessary #include and #define directives
#include "ForestFire.h"
#include <vector>
#include <algorithm>

using namespace omnetpp; //we need this for omnet reasons
Define_Module(ForestFire); //some OMNeT magic
std::vector<ForestFire *> ForestFire::allNodes;
std::vector<int> ForestFire::allNodeIds;
//NOTE there is a chance that having this in the header is better
double rho = 0.9; //discount factor
double lambda = 1; //TODO CHANGE THIS, I DON'T KNOW WHAT IS SHOULD BE

//returns a random int in range [low,high]
int uniform_int_in_range(int low, int high){
    std::uniform_int_distribution<int> dist{low, high};
    return dist(genF);
}
void ForestFire::initialize(){
    setPotencyAndConsistency();
    allNodes.push_back(this);

    cMessage *serviceRequestEvent = new cMessage("serviceRequestTimer");
    scheduleAt(simTime() + uniform_real_in_range(1, 5), serviceRequestEvent);

    // For nodes to find each other
    populateRoutingTable();
    // Schedule service table update after all nodes are initialized
    scheduleAt(simTime() + 0.1, new cMessage("populateServiceTable"));

    int attackerTypeValue = getParentModule()->par("attackerType");
    setMalicious(AttackerType(attackerTypeValue));

    badServiceLogger = new cMessage("badServiceLogger");
    scheduleAt(simTime() + 10.0, badServiceLogger); // log every 10 seconds

    this->groupNo = uniform_int_in_range(0,9);
    allNodeIds.push_back(getId());
    //default değerlerini veriyoruz
    for (auto* n : allNodes) {
            recentTrust.try_emplace(n->getId(), 0.51);
            activityRecent.try_emplace(n->getId(), 1.0);
        }
    //
    ffTick = new cMessage("ff_tick");

        scheduleAt(simTime() + 5.0, ffTick);
    joinTime = simTime();
}
/**
 * Sets the potency and consistency values of the node.
 * The values are uniformly distributed in "meaningful" ranges,
 * so that even the worst benevolent node is quite performant */
void ForestFire::setPotencyAndConsistency() {
  double pot = uniform_real_in_range(6, 10);
  this->potency = pot;
  double cons = uniform_real_in_range(0.5, 2.0);
  this->consistency = cons;
}
void ForestFire::setMalicious(AttackerType type){//burayı neden boş bıraktık eski kod ile aynı olmayacak mı?
}

/**
 * Copy of the original code that does its name
 * I am not sure about every line of this code
 * but it has worked for generations -esm- */
void ForestFire::populateRoutingTable() {
  for (int i = 0; i < gateSize("inoutGate"); i++) {
    cGate *outGate = gate("inoutGate$o", i);
    if (outGate->isConnected()) {
      cModule *connectedModule = outGate->getNextGate()->getOwnerModule();
      if (connectedModule) {
        int destNodeId = connectedModule->getId();
        routingTable[destNodeId] = i; // Store the gate index
        EV << "Node " << getId() << " connected to Node " << destNodeId
           << " via gate index " << i << endl;
      }
    }
  }
}
/* Deals with the core and comp node selection issue.
 * Just takes them by random
 * TODO should there be a core node for each group? evet ama bizim uyarlamamızda olması ya da olmaması bir şey değiştirmiyor
 */
auto ForestFire::handleCoreAndComp(){
    int coreNodeId;
    std::vector<int> compNodeIds;

    int numNodes = getParentModule()->par("numNodes"); //how many nodes there are
    int randomId = uniform_int_in_range(2,numNodes+2);
    bool badId = true; //assume the id is bad first;
    do{
        auto it = std::find(allNodeIds.begin(), allNodeIds.end(),randomId);
        if(it != allNodeIds.end()){
            //so randomId is a valid id
            ForestFire* node = getNodeById(randomId);
           if(!node->banned){
               //the node is not banned either, it's OK!
               badId = false;
               break;
           }
        }
    }while(badId);
    coreNodeId = randomId;
    std::vector<int> availableNodeIds;
    for(const auto& node : allNodes){
        if(!node->banned){
            int id = getId();
            if(id != coreNodeId)
                availableNodeIds.push_back(id);
        }
    }
    //clear compNodeIds and set anew:
    compNodeIds.clear();
    std::sample(availableNodeIds.begin(),availableNodeIds.end(),
                std::back_inserter(compNodeIds), 10, genF);
    //wasn't that a mouthful!

    setCoreNode(coreNodeId);
    setCompNodes(compNodeIds);
}
void ForestFire::setCoreNode(int id){coreNode = id;}
void ForestFire::setCompNodes(std::vector<int> ids){
    //this function "sets" the compNodes vector
    //so clear it first:
    compNodes.clear();
    for(auto id : ids){
        compNodes.push_back(id);
    }
}
ForestFire* ForestFire::getNodeById(int id){
    auto it = find_if(allNodes.begin(),allNodes.end(),
                      [id](const auto& node){return node->getId()==id;});
    if(it != allNodes.end()){
        //found it!
        return *it;
    }
    else{
        EV << "Warning: Node with ID " << id << " not found!" << endl;
        return nullptr;
    }
}
void ForestFire::handleMessage(cMessage *msg) {
    if (msg == ffTick) {
        // 1) Bu tur için rastgele core + complementary seç
        handleCoreAndComp();
        // 2) Seçilen hedefler için T_final hesapla ve ban uygula
        updateTrustForSelected();
        // 3) Bir sonraki tura planla

        scheduleAt(simTime() + 5.0, ffTick);
        return;
    }

    // ...buraya eklme yapmamız gerekebilir düşünmedim daha ...
}

//todo i still do not understand what this should look like
//Aşağıdaki hesaplamaları kendileri dışındaki nodelar için yapacaklar o yüzden parametre olarak node ID almalıyız
auto ForestFire:: gettrecent(int targetId) const {
    auto it = recentTrust.find(targetId);
    if (it != recentTrust.end()) return it->second;
    return 0.51;
}
auto ForestFire::calcZeta(double Trecent){
    double zeta;
    zeta = 0.51 + Trecent;
    return zeta;
}
auto ForestFire::getTime(){
    //TODO implement this
    return (simTime() - joinTime).dbl();
    //her node kendi içinde tutabilir banned olunca sıfırlanmalı
}

auto ForestFire::getActRecent(int targetId) const {
        auto it = activityRecent.find(targetId);
        if (it != activityRecent.end()) return it->second;
        return 1.0;
    }


double ForestFire::calcPsi(double actRec, double rho_, double tSecs) {
    return std::pow(rho_, tSecs) * actRec;
}
auto ForestFire::calcT_now(double zeta, double psi, double lambda_){

    double numerator = 1 - pow(zeta,1+psi);
    double denominator = 1 - zeta;

    return lambda_ * (numerator/denominator);
}
//T final'ı hesaplamamız lazım T_now'ların ortalaması
//TFinal 0.5'in altındaysa sistemden atılacak banned= true
double ForestFire::computeTfinalFor(int targetId) const {
    std::vector<double> votes;
    votes.reserve(allNodes.size() > 1 ? allNodes.size()-1 : 0);

    for (auto* observer : allNodes) {
        if (!observer) continue;
        if (observer->banned) continue;           // banlı gözlemci oy vermez dedim
        if (observer->getId() == targetId) continue; // self-vote yok

        // Her gözlemci kendi hafızasından bakarak T_now hesaplar:
        const double t   = observer->getTime();//TODO : observer'ın mı target'ın mı süresi olmalı burada ona karar ver
        const double Tr  = observer->gettrecent(targetId);
        const double Ar  = observer->getActRecent(targetId);
        const double z   = observer->calcZeta(Tr);
        const double psi = observer->calcPsi(Ar, observer->rho, t);

        const double tn  = observer->calcT_now(z, psi, observer->lambdaScale);
        votes.push_back(tn);
    }

    if (votes.empty()) {
        // Gözlemci yoksa
        return std::clamp(gettrecent(targetId), 0.0, 1.0);
    }
//normal bir şekilde ortalamasını alıyoruz ağırlıklar eşit, mesafeye bakmıyoruz
    double sum = std::accumulate(votes.begin(), votes.end(), 0.0);
    double avg = sum / (double)votes.size();
    return std::clamp(avg, 0.0, 1.0);
}
void ForestFire::updateTrustForSelected() {
    // Bu tur değerlendirilecek hedefler: {core} ∪ comp
    std::vector<int> targets;
    if (coreNode >= 0) targets.push_back(coreNode);
    for (int id : compNodes) targets.push_back(id);

    // Yinelenenleri kaldır
    std::sort(targets.begin(), targets.end());
    targets.erase(std::unique(targets.begin(), targets.end()), targets.end());

    for (int xId : targets) {
        ForestFire* tgt = getNodeById(xId);
        if (!tgt || tgt->banned) continue;

        double tf = computeTfinalFor(xId);  // diğer tüm düğümlerin T_now ortalaması
        recentTrust[xId] = tf;

        ban(xId, tf);
    }
}
void ForestFire::ban(int nodeId, double tfinal) {
    if (tfinal < trustThreshold) {
        ForestFire* n = getNodeById(nodeId);
        n ->joinTime = simTime();//süreyi sıfırlamak için yapıyorum ama bunu yeri burası değil kod oturunca taşıyalım. Ban'ı kalkınca yapmak lazım bunu
        if (n && !n->banned) {
            n->banned = true;
            EV << "Node " << nodeId << " banned (T_final=" << tfinal
               << " < threshold=" << trustThreshold << ")\n";
        }
    }
}


