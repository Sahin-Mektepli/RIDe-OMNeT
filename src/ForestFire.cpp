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
std::set<int> ForestFire::maliciousNodeIds;
int ForestFire::totalBadServicesReceived = 0;
int ForestFire::totalBenevolentNodes = 0;
int ForestFire::opportunisticNodeId = -1;
int ForestFire::totalServicesReceived = 0;

bool ForestFire::summaryWritten = false;//en son ekrana bütün sonuçları sadece bir kere yazdırmak için

std::default_random_engine gen;
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
    //scheduleAt(simTime() + 0.1, new cMessage("populateServiceTable"));

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

        scheduleAt(simTime() + 10.0, ffTick);
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
void ForestFire::setMalicious(AttackerType type){//burayı neden boş bıraktık eski kod ile aynı olmayacak mı? şimdilik eski kodu yapıştırdım buraya(burası bir hataya yol açıyor olabilir)

      EV << "\n\n\n\nWe use attack number " << type
         << " in this simulation\n\n\n\n";
      int totalNodes = getParentModule()->par("numNodes");
      int numMalicious =
          int(par("maliciousNodePercentage").doubleValue() * totalNodes);

      // Sadece ilk node random seçim yapar
      if (getId() == 2) {

          maliciousNodeIds.clear();
          std::vector<int> allIds;
          for (int i = 2; i < 2 + totalNodes; ++i)
            allIds.push_back(i);
          std::shuffle(allIds.begin(), allIds.end(), gen);

          if (type == OPPORTUNISTIC) {
            opportunisticNodeId = allIds.front();
            EV << "OPPORTUNISM BY NODE " << opportunisticNodeId << '\n';
          }
          maliciousNodeIds.insert(allIds.begin(), allIds.begin() + numMalicious);
          maliciousNodeIds.erase(opportunisticNodeId); // this is excluded for some reason

      }

      // read ffInterval and opportunisticStartTick from .ini if provided

      if (type == OPPORTUNISTIC && getId() == opportunisticNodeId) {
        attackerType = OPPORTUNISTIC;
        isOpportunisticNode = true;

        //benevolent = false; // Başta iyi
        // potency = 9; //These values should not be set after initalization
        // consistency = 2.0;
      } else if (maliciousNodeIds.count(getId()) > 0) {
        //!!opportunistic saldırıda iyi davranan bir node belirli bir süre sonra
        //! kötü davranmaya başlıyor ama onun dışında da kötü nodelar olabilir
        //! onların türünü kamuflaj yaptım kamuflajı 0 yaparsak yüzde yüz kötücül de
        //! olabilir
        attackerType =
            (type == OPPORTUNISTIC)
                ? CAMOUFLAGE
                : type; // opportunistic attack dışınadkiler direkt kendi türüne
                        // eşitleniyor saldırımız opportunistic ise seçilmiş node
                        // dışındakiler kamuflaj saldırısı yapıyor
        //benevolent = false;
        if (attackerType == CAMOUFLAGE) {
            // read camouflageRate from omnetpp.ini (defaults to 0.0 if not provided)
            if (hasPar("camouflageRate")) {
              camouflageRate = par("camouflageRate").doubleValue();
            }

          EV << "CAMOUFLAGE BY NODE " << getId() << "\n";
          getDisplayString().setTagArg("i", 1, "blue");
        } else if (attackerType == MALICIOUS_100) {
          getDisplayString().setTagArg("i", 1, "red");

        } else if (attackerType == BAD_MOUTHING) {
          // iyi servis kötü yorum ise
          getDisplayString().setTagArg("i", 1, "purple");
        }
      } else {
        attackerType = BENEVOLENT;
        //benevolent = true;
        totalBenevolentNodes++;
      }


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
/*auto ForestFire::handleCoreAndComp(){
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
    }while(badId);//burada loop'a giriyor diye düşündüm o yüzden değiştirdim
    coreNodeId = randomId;
    std::vector<int> availableNodeIds;
    for(const auto& node : allNodes){
        if(!node->banned){
            int id = node->getId();//burada da sadece getId() yazıyordu o zaman hep aynı node'un Id'sini eklemiş oluyorduk
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
}*/
auto ForestFire::handleCoreAndComp(){
    // 1) Banlı olmayanları topla
    std::vector<int> eligible;
    eligible.reserve(allNodes.size());
    for (auto* n : allNodes) {
        if (n && !n->banned) eligible.push_back(n->getId());
    }
    if (eligible.empty()) {
        setCoreNode(-1);
        setCompNodes({});
        return;
    }

    // 2) Core
    int coreIdx = intuniform(0, (int)eligible.size()-1);
    int coreNodeId = eligible[coreIdx];

    // 3) Complementary
    std::vector<int> pool;
    pool.reserve(eligible.size());
    for (int id : eligible) if (id != coreNodeId) pool.push_back(id);

    // 4) compNodes'ı tekrarsız rastgele doldur
    std::shuffle(pool.begin(), pool.end(), gen);
    const int take = std::min(10, (int)pool.size());
    std::vector<int> compNodeIds(pool.begin(), pool.begin()+take);


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

const ForestFire* ForestFire::getNodeById(int id) const {
    auto it = find_if(allNodes.begin(), allNodes.end(),
                      [id](const auto& node){ return node->getId() == id; });
    if (it != allNodes.end()) return *it;
    EV << "Warning: Node with ID " << id << " not found!" << endl;
    return nullptr;
}
void ForestFire::handleMessage(cMessage *msg) {
    if (msg == ffTick) {
        // 1) Bu tur için rastgele core + complementary seç
        handleCoreAndComp();
        // 2) Seçilen hedefler için T_final hesapla ve ban uygula
        updateTrustForSelected();
        // 3) ŞİMDİ servis denemesi yap (sonraki tur için Act_recent güncellensin)
        // Core
               if (coreNode >= 0 && coreNode != getId()) {
                   if (auto* s = getNodeById(coreNode); s && !s->banned) {
                       probeAndUpdate(coreNode);
                   }
               }
               // Complementary
               for (int id : compNodes) {
                   if (id == getId()) continue;
                   if (auto* s = getNodeById(id); s && !s->banned) {
                       probeAndUpdate(id);
                   }}
        // 4) sonraki tura planla
        scheduleAt(simTime() + 10.0, ffTick);
        return;
    }
    else if (strcmp(msg->getName(), "badServiceLogger") == 0) {

        if (totalBenevolentNodes > 0) {
          recordScalar(
              ("AverageBadServicesAt_" + std::to_string((int)simTime().dbl()))
                  .c_str(),
              (double)totalBadServicesReceived / totalBenevolentNodes);
        }
        if (totalServicesReceived > 0) {
                double ratio = (double)totalBadServicesReceived / totalServicesReceived;
                recordScalar(
                    ("BadServiceRatioAt_" + std::to_string((int)simTime().dbl()))
                        .c_str(),
                    ratio);
            }scheduleAt(simTime() + 10.0, msg); // repeat every 10s
            return;
          }

    // ...buraya eklme yapmamız gerekebilir düşünmedim daha ...
}

//todo i still do not understand what this should look like
//Aşağıdaki hesaplamaları kendileri dışındaki nodelar için yapacaklar o yüzden parametre olarak node ID almalıyız
auto ForestFire:: gettrecent(int targetId) const {
    auto it = recentTrust.find(targetId);
    if (it != recentTrust.end()) return it->second;
    return 0.5;
}
auto ForestFire::calcZeta(double Trecent){
    double zeta;
    zeta = 0.51 + Trecent;
    return zeta;
}
auto ForestFire::getTime()const{
    //TODO implement this
    return (simTime() - joinTime).dbl();
    //her node kendi içinde tutabilir banned olunca sıfırlanmalı
}

auto ForestFire::getActRecent(int targetId) const {
        auto it = activityRecent.find(targetId);
        if (it != activityRecent.end()) return it->second;
        return 0.5;
    }


double ForestFire::calcPsi(double actRec, double rho_, double tSecs) {
    return std::clamp((1-std::pow(rho_, tSecs) )* actRec, 0.0, 1.0);//1- yeni eklendi
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
        double t = getNodeById(targetId)->getTime();//TODO : observer'ın mı target'ın mı süresi olmalı burada ona karar ver
        const double Tr  = observer->gettrecent(targetId);
        const double Ar  = observer->getActRecent(targetId);
        const double z   = observer->calcZeta(Tr);
        const double psi = observer->calcPsi(Ar, observer->rho, t);

         double tn  = observer->calcT_now(z, psi, observer->lambdaScale);
        tn = std::clamp(tn, 0.0, 1.0);
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
}//servis al ver servisin içinde kalitesi yazsın gibi olan kısımları yazmak yerine böyle yazdım. Sıkıntı varsa değiştiririz
bool ForestFire::simulateServiceSuccessFrom(int serverId) {
    //dblrand(): 0-1 arası random değer döndürür
    ForestFire* s = getNodeById(serverId);
    if (!s || s->banned) return false;

    double pr = s->pGood;
    switch (s->attackerType) {
        case BENEVOLENT:
           // pr = s->pGood; break;
            pr = 1.0; break;
        case MALICIOUS_100:
            pr = 0.0; break;
        case CAMOUFLAGE:
            pr = (dblrand() < s->camouflageRate) ? s->pGood : s->pBad;
            break;

        case OPPORTUNISTIC:
                   // Opportunistic: benevolent until opportunisticSwitchTime, then fully malicious
                   if (s->isOpportunisticNode) {
                       if (simTime().dbl() >= s->opportunisticAttackTime) {
                           // still benevolent phase
                           pr = 1.0;
                       } else {
                           // after switch -> full malicious
                           pr = 0.0;
                       }
                   } else {
                       // defensive fallback: if the node type is opportunistic but not flagged as opportunistic
                       pr = (dblrand() < s->camouflageRate) ? s->pGood : s->pBad;
                   }
        break;


           }

           return dblrand() < pr;
       }


void ForestFire::probeAndUpdate(int serverId) {
    ForestFire* s = getNodeById(serverId);
    if (!s || s->banned) {
            // Banlı ya da bulunamadı -> deneme yok, kötü servis sayma
            return;
        }
    bool ok  = simulateServiceSuccessFrom(serverId);

    // --- AĞ GENELİ sayaçlar ---
        totalServicesReceived++;
        if (!ok) totalBadServicesReceived++;

    double old = getActRecent(serverId);      // yoksa 1.0
    double obs = ok ? 1.0 : 0.0;
    activityRecent[serverId] = (1.0 - actAlpha) * old + actAlpha * obs; // recent dediği için normal bir ortalama almak yerine yakın zamanda olanların etkisini daha çok aldım


}
const char* ForestFire::roleToStr(AttackerType a) {
    switch (a) {
        case BENEVOLENT:     return "benevolent";
        case MALICIOUS_100:  return "malicious_100";
        case CAMOUFLAGE:     return "camouflage";
        case BAD_MOUTHING:   return "bad_mouthing";
        case OPPORTUNISTIC:  return "opportunistic";
        default:             return "unknown";
    }
}
void ForestFire::finish() {
    if (!summaryWritten) {
        summaryWritten = true;

        EV_INFO << "\n===== FINAL NODE SNAPSHOT =====\n"
                << "nodeId   t_final     role            banned\n";

        int TP = 0, TN = 0, FP = 0, FN = 0;

        for (auto* n : allNodes) {
            if (!n) continue;
            const double tf = n->computeTfinalFor(n->getId());
            const bool banned = n->banned;
            const bool isMalicious = (n->attackerType != BENEVOLENT);

            // Confusion matrix components
            if (isMalicious && banned) TP++;           // correctly detected attacker
            else if (!isMalicious && !banned) TN++;    // correctly kept benevolent
            else if (!isMalicious && banned) FP++;     // wrongly banned benevolent
            else if (isMalicious && !banned) FN++;     // missed malicious

            EV << "nodeId=" << n->getId()
               << "  t_final=" << tf
               << "  role=" << roleToStr(n->attackerType)
               << "  banned=" << banned
               << "\n";
        }

        // ---- Compute Metrics ----
        double accuracy = 0.0, precision = 0.0, recall = 0.0, f1 = 0.0;
        int total = TP + TN + FP + FN;
        if (total > 0) accuracy = double(TP + TN) / total;
        if ((TP + FP) > 0) precision = double(TP) / (TP + FP);
        if ((TP + FN) > 0) recall = double(TP) / (TP + FN);
        if ((precision + recall) > 0) f1 = 2.0 * precision * recall / (precision + recall);

        // ---- Record Scalars ----
        recordScalar("Final_Accuracy", accuracy);
        recordScalar("Final_F1_Score", f1);
        recordScalar("Final_Precision", precision);
        recordScalar("Final_Recall", recall);
        recordScalar("TruePositives", TP);
        recordScalar("FalsePositives", FP);
        recordScalar("TrueNegatives", TN);
        recordScalar("FalseNegatives", FN);

        EV_INFO << "\n===== FINAL PERFORMANCE =====\n"
                << "Accuracy = " << accuracy
                << " | F1 = " << f1
                << " | Precision = " << precision
                << " | Recall = " << recall << "\n";
    }

    // Cleanup timers and lists (keep your existing part)
    if (ffTick) {
        cancelAndDelete(ffTick);
        ffTick = nullptr;
    }
    auto it = std::find(allNodes.begin(), allNodes.end(), this);
    if (it != allNodes.end()) allNodes.erase(it);
    auto it2 = std::find(allNodeIds.begin(), allNodeIds.end(), getId());
    if (it2 != allNodeIds.end()) allNodeIds.erase(it2);
}



