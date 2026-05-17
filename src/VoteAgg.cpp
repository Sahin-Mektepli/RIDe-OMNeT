/*
 * VoteAgg.cpp
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm,sahinm
 */
#include "voteAgg.h"
#include "BlockchainMessage_m.h"
#include "omnetpp/clog.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <iostream>
#include <cmath>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
/*
 * VoteAgg modeli ile ilgili Notlar:
 1. direct trust is only local, so learning is slow
 2. unknown nodes remain neutral at 0.5 (bunu değiştirip deneyebiliriz)
 3. DT values are mostly 0/0.5/1 (bu neden böyle çözemedim hala)
 4. malicious voters are included equally in aggregation
 5. multiplicative is vulnerable to bad-mouthing
 6. global ranking can become poisoned and then mislead provider selection
 (weighted yapabiliriz herkes kendi gloabal trust'ına oranla oy verebilir)
 */
/*
** -----KRITIK BAZI NOTLAR------
** - Kalite hesabı nasıl olacak, artık nadirlik falan yok!
** - Çizgenin yapısı aslında değişmeli, ama bu ertelenebilir
** TODO Decay should be implemented, probably in updateTrustScore
** TODO Update TS only if you are connected to the provider node
** TODO Our graph is currently a fully connected one...
*/

static std::default_random_engine gen;
static std::uniform_real_distribution<double> uniform_real_dist{
    0, 1}; // bu da burda dursun madem
#define SERVICE_TYPE "A" // as a substitute for deprecated serviceTypes
using namespace omnetpp;

Define_Module(VoteAgg);
std::vector<Block> VoteAgg::blockchain;
std::vector<VoteAgg *> VoteAgg::allNodes;
int VoteAgg::numClusterHeads = 3;
int VoteAgg::globalBlockId = 0;
std::set<int> VoteAgg::maliciousNodeIds;
int VoteAgg::totalBadServicesReceived = 0;
int VoteAgg::totalBenevolentNodes = 0;
int VoteAgg::opportunisticNodeId = -1;
int VoteAgg::totalServicesReceived = 0;
std::map<int, double> VoteAgg::globalTrustScores;
std::vector<int> VoteAgg::globalTrustRanking;
double VoteAgg::totalReceivedQuality = 0.0;



void printRoutingTable(const std::map<int, int> &routingTable) {
  EV << "Routing Table:\n";
  EV << "NodeID --> Gate Index\n";
  for (const auto &entry : routingTable) {
    EV << entry.first << "-->" << entry.second << "\n";
  }
}
/**
 * Copy of the original code that does its name
 * I am not sure about every line of this code
 * but it has worked for generations -esm- */
void VoteAgg::populateRoutingTable() {
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

void VoteAgg::updateEpsilon() {
  epsilon = epsilon * epsilonDecay;
  if (epsilon < minEpsilon) {
    epsilon = 0.0;
  }
  EV << "Epsilon güncellendi: " << epsilon << endl;
}

/**
 * Examine each cluster in the graph and make sure that none of them are
 * "dominated" by malicious nodes.
 *  Dominated here means that at least 80 percent of the nodes are malicious. */
bool VoteAgg::noMalDominatedClusters() {
  int clusterSize =
      getParentModule()->par("clusterSize"); // we calculate this, not set
  int clusterCount = getParentModule()->par("clusterCount");
  int numNodes = getParentModule()->par("numNodes");

  for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {
    // check for each cluster in the system
    int countMal = 0;
    for (int nodeIndex = 0; nodeIndex < clusterSize; nodeIndex++) {
      // nodeIndex == index in that cluster
      // for 100 nodes and 10 clusters:
      // first cluster --> 2...11
      // second cluster--> 12...21
      int nodeId =
          (clusterIndex * clusterSize) + nodeIndex + 2; // ids start from 2
      if (maliciousNodeIds.count(nodeId))
        countMal++;
    }
    // counted malicious nodes in the cluster
    if (double(countMal) / clusterSize > 0.5)
      return false;
  }
  return true;
}

/**
 * Extracted from initialize method
 * Attacker type part should be refactored too. */
void VoteAgg::setMalicious(AttackerType type) {
  EV << "\n\n\n\nWe use attack number " << type
     << " in this simulation\n\n\n\n";

  int totalNodes = getParentModule()->par("numNodes");
  int numMalicious =
      int(par("maliciousNodePercentage").doubleValue() * totalNodes);

  // Only first node randomly selects malicious nodes
  if (getId() == 2) {
    do {
      maliciousNodeIds.clear();

      std::vector<int> allIds;
      for (int i = 2; i < 2 + totalNodes; ++i)
        allIds.push_back(i);

      std::shuffle(allIds.begin(), allIds.end(), gen);

      // For OPPORTUNISTIC, all selected malicious nodes will be opportunistic.
      // No single opportunisticNodeId anymore.
      maliciousNodeIds.insert(allIds.begin(), allIds.begin() + numMalicious);

    } while (!noMalDominatedClusters());
  }

  if (maliciousNodeIds.count(getId()) > 0) {

    if (type == OPPORTUNISTIC) {
      attackerType = OPPORTUNISTIC;
      isOpportunisticNode = true;
      benevolent = true;  // starts as good

      EV << "OPPORTUNISTIC BY NODE " << getId() << "\n";
      getDisplayString().setTagArg("i", 1, "yellow");
    }
    else {
      attackerType = type;
      benevolent = false;

      if (attackerType == CAMOUFLAGE) {
        if (hasPar("camouflageRate")) {
          camouflageRate = par("camouflageRate").doubleValue();
        }

        EV << "CAMOUFLAGE BY NODE " << getId() << "\n";
        getDisplayString().setTagArg("i", 1, "blue");
      }
      else if (attackerType == MALICIOUS_100) {
        getDisplayString().setTagArg("i", 1, "red");
      }
      else if (attackerType == BAD_MOUTHING) {
        getDisplayString().setTagArg("i", 1, "purple");
      }
      else if (attackerType == BAD_SERVICE_GOOD_RATING) {
        EV << "BAD_SERVICE_GOOD_RATING BY NODE " << getId() << "\n";
        getDisplayString().setTagArg("i", 1, "brown");
      }
      else if (attackerType == HYBRID) {
        getDisplayString().setTagArg("i", 1, "pink");
      }
    }
  }
  else {
    attackerType = BENEVOLENT;
    benevolent = true;
    totalBenevolentNodes++;
  }
}

/**
 * Sets the potency and consistency values of the node.
 * The values are uniformly distributed in "meaningful" ranges,
 * so that even the worst benevolent node is quite performant */
void VoteAgg::setPotencyAndConsistency() {
  int id = getId();

  double pot = uniform(6, 10);
  this->potency = pot;
  EV << "Potency of node " << id << " is " << pot << '\n';

  double cons = uniform(0.5, 2.0);
  this->consistency = cons;
  EV << "Consistency of node " << id << " is " << cons << '\n';
}

void VoteAgg::initialize() {

  setPotencyAndConsistency();
  // initialize()
  if (hasPar("camouflageRate"))
      this->camouflageRate = par("camouflageRate").doubleValue();
  else
      this->camouflageRate = 0.0;  // safe default
  recordScalar("camouflageRate", camouflageRate);

  if (hasPar("aggregationMethod")) {
      int methodValue = par("aggregationMethod").intValue();

      switch (methodValue) {
          case 0:
              aggregationMethod = AGG_ADDITIVE;
              break;
          case 1:
              aggregationMethod = AGG_MULTIPLICATIVE;
              break;
          case 2:
              aggregationMethod = AGG_BORDA;
              break;
          default:
              throw cRuntimeError("Invalid aggregationMethod value: %d", methodValue);
      }
  }
  if (hasPar("globalTrustUpdateInterval")) {
    globalTrustUpdateInterval = par("globalTrustUpdateInterval").doubleValue();
  }


  epsilon = 0.2;
  minEpsilon = 0.01;
  epsilonDecay = 0.90;

  serviceRequestEvent = new cMessage("serviceRequestTimer");
  scheduleAt(simTime() + uniform(1, 5), serviceRequestEvent);
  isClusterHead = false;
  allNodes.push_back(this);

  if (getId() == 2) {
    globalTrustUpdateEvent = new cMessage("globalTrustUpdate");
    scheduleAt(simTime() + globalTrustUpdateInterval, globalTrustUpdateEvent);
  }

  providedService = SERVICE_TYPE;
  populateRoutingTable();

  // Schedule service table update after all nodes are initialized
  scheduleAt(simTime() + 0.1, new cMessage("populateServiceTable"));

  // read the attacker type as an int from omnet.ini
  int attackerTypeValue = getParentModule()->par("attackerType");
  setMalicious(AttackerType(attackerTypeValue));

  // Start periodic logger(belirli aralıklarla kötü servis sayısını kaydetmek
  // için)
  badServiceLogger = new cMessage("badServiceLogger");
  scheduleAt(simTime() + 10.0, badServiceLogger); // 10 saniyede bir şu anda

  if (attackerType == OPPORTUNISTIC && isOpportunisticNode) {
    opportunisticTriggerMsg = new cMessage("triggerOpportunistic");
    scheduleAt(opportunisticAttackTime, opportunisticTriggerMsg);
    EV << "Node " << getId() << " is opportunistic and will switch at time "
       << opportunisticAttackTime << "\n";
  }
  if (attackerType == HYBRID) {
      cMessage* hybridTrigger = new cMessage("triggerHybrid");
      scheduleAt(hybridSwitchTime, hybridTrigger);
  }

}

void printBlockChain(std::vector<Block> blockchain) {
  EV << "je veux voir tous les block a la fois:\n";
  for (Block &block : blockchain) {
    EV << block.transactionData << "\nat time: " << block.timestamp << "\n";
  }
  EV << "on est finis lire le block!\n\n";
}

void VoteAgg::populateServiceTable() {
  EV << "Node " << getId()
     << " is now filling its service table with direct neighbors..." << endl;
  for (int i = 0; i < gateSize("inoutGate"); i++) {
    cGate *outGate = gate("inoutGate$o", i);
    if (outGate->isConnected()) {
      cModule *connectedModule = outGate->getNextGate()->getOwnerModule();
      VoteAgg *neighborNode = dynamic_cast<VoteAgg *>(connectedModule);
      if (neighborNode) {
        std::string neighborService = neighborNode->providedService;
        int neighborId = neighborNode->getId();
        serviceTable[neighborService].push_back(neighborId);
        EV << "Node " << getId() << " learned that Node " << neighborId
           << " provides service: " << neighborService << endl;
      }
    }
  }
  printServiceTable();
}

/**
 * Calculate how "similar" the newRating given by some other node
 * to the provider is from our current past experiences.
 *
 * Current formula is 1/(1+e^(6x-3)) where x is the difference
 * So *it assumes that diff is in (0,1)*
 *
 * However ratings are in (-10,10) so diff is in (0,20).
 * Normalize diff by dividing by 20.
 *
 * The result is the alpha coef we use, and must be in (0,1) */


void VoteAgg::printServiceTable() {
  EV << "Service Table for Node " << getId() << ":\n";

  if (serviceTable.empty()) {
    EV << "  (No services learned yet.)\n";
    return;
  }

  for (const auto &entry : serviceTable) {
    const std::string &serviceType = entry.first;
    const std::vector<int> &nodes = entry.second;

    EV << "  Service Type: " << serviceType << " provided by Nodes: ";
    for (int nodeId : nodes) {
      EV << nodeId << " ";
    }
    EV << "\n";
  }
}

void VoteAgg::handleServiceRequestMsg(cMessage *msg) {
  ServiceRequest *request = check_and_cast<ServiceRequest *>(msg);
  int requesterId = request->getRequesterId();
  std::string requestedService = request->getServiceType();

  if (providedService != requestedService) {
    EV << "Node " << getId()
       << " does NOT provide requested service: " << requestedService
       << ". Ignoring." << endl;
    delete request;
    return;
  }

  EV << "Node " << getId() << " will respond to service request from Node "
     << requesterId << endl;
  ServiceResponse *response = new ServiceResponse("serviceResponse");
  response->setRequesterId(requesterId);
  response->setProviderId(getId());
  response->setServiceType(requestedService.c_str());

  if (routingTable.find(requesterId) != routingTable.end()) {
    int gateIndex = routingTable[requesterId];
    send(response, "inoutGate$o", gateIndex);
  } else {
    EV << "No route to requester " << requesterId << endl;
    delete response;
  }

  delete request;
}
VoteAgg *VoteAgg::getNodeById(int targetId) {
  for (VoteAgg *node : allNodes) {
    if (node->getId() == targetId) {
      return node;
    }
  }
  std::cerr << "Warning: Node with ID " << targetId << " not found!" << std::endl;
  return nullptr;
}

/**
 * Generate a random integer in [0,n) */
int random_0_to_n(int n) {
  std::uniform_int_distribution<int> dist{0, n - 1};
  int rand = dist(gen);
  return rand;
}
/**
 * Returns a random pair of the given mapping.
 *
 * Throws an error if the mapping is empty. */
auto randomPairOfMapping(const std::map<int, double> &mapping) {
  if (mapping.empty())
    throw std::invalid_argument("Map cannot be empty in function " +
                                std::string(__FUNCTION__));
  int number = random_0_to_n(mapping.size());
  auto it = mapping.begin();
  std::advance(it, number);
  return it;
}

/**
 * Input is a mapping from NodeId --> Local Trust
 *
 * Generates a random double and if it is greater than epsilon, returns the pair
 * with the maximum Local Trust.
 * The lambda funtion compares the pairs by their second entries, which is
 * supposed to be the Local Trust value.
 *
 * Else, returns a random pair. */
std::map<int, double>::const_iterator VoteAgg::epsilonGreedyMaxPair(const std::map<int, double> &mapping) {
  std::uniform_real_distribution<double> dist{0, 1};
  double random = dist(gen);
  if (random > this->epsilon) {
    // return maximum
    return std::max_element(
        mapping.begin(), mapping.end(),
        [](const auto &a, const auto &b) { return a.second < b.second; });
  } else {
    // return random pair
    return randomPairOfMapping(mapping);
  }
}

void VoteAgg::handleServiceResponseMsg(cMessage *msg) {
  ServiceResponse *response = check_and_cast<ServiceResponse *>(msg);
  int responderId = response->getProviderId();
  std::string serviceType = response->getServiceType();

  if (requestedServiceType == serviceType) {
    int requestorId = this->getId();

    double localTrust = trustMap[responderId].value();
    respondedProviders[responderId] = localTrust;

    pendingResponses.erase(responderId);
    EV << "Received service response from Node " << responderId
       << " with Local Trust = " << localTrust << endl;

    if (pendingResponses.empty()) {
      int bestProviderId = -1;
      double maxTrust = -1;

      if (!respondedProviders.empty()) {
        std::map<int, double> mergedProviders;
        for (const auto &entry : respondedProviders) {
          int candidateId = entry.first;
          mergedProviders[candidateId] = mergeTrustScore(candidateId);//bu kısım yeni eklendi merge için
        }
        auto chosenPair = epsilonGreedyMaxPair(mergedProviders);
        bestProviderId = chosenPair->first;
        maxTrust = chosenPair->second;
      } else {
        // I don't think this should ever happen...
        EV << "---- NO ONE RESPONDED TO A SERVICE REQUEST ----\n";
      }

      // choose a random node with an epsilon probability!

      if (bestProviderId != -1 &&
          routingTable.find(bestProviderId) != routingTable.end()) {
        int gateIndex = routingTable[bestProviderId];
        FinalServiceRequest *finalRequest =
            new FinalServiceRequest("finalServiceRequest");
        finalRequest->setRequesterId(getId());
        finalRequest->setProviderId(bestProviderId);
        finalRequest->setServiceType(requestedServiceType.c_str());
        send(finalRequest, "inoutGate$o", gateIndex);

        EV << "Sent FINAL service request to Node " << bestProviderId
           << " with trust = " << maxTrust << endl;
      }
    }
  }

  delete response;
}
void VoteAgg::handleFinalServiceRequestMsg(cMessage *msg) {
  FinalServiceRequest *request = check_and_cast<FinalServiceRequest *>(msg);
  int requesterId = request->getRequesterId();
  std::string requestedService = request->getServiceType();

  if (providedService != requestedService) {
    EV << "Node " << getId() << " received final request for "
       << requestedService << ", but provides " << providedService
       << ". Ignoring." << endl;
    delete request;
    return;
  }

  EV << "Providing FINAL service to Node " << requesterId << endl;

  FinalServiceResponse *response =
      new FinalServiceResponse("finalServiceResponse");
  response->setRequesterId(requesterId);
  response->setProviderId(getId());
  response->setServiceType(requestedService.c_str());
  response->setServiceQuality(calcQuality(potency, consistency));
  // provider kendi potency ve consistency'sini ekliyor

  if (routingTable.find(requesterId) != routingTable.end()) {
    int gateIndex = routingTable[requesterId];
    send(response, "inoutGate$o", gateIndex);
  } else {
    EV << "No route to requester " << requesterId << " for final service!"
       << endl;
    delete response;
  }

  delete request;
}

double VoteAgg::updateMyRating(int providerId, double rating) {
  myRatings &alterandum = myRatingMap[providerId];
  alterandum.sumRatings += rating;
  alterandum.count += 1;
  return alterandum.value();
}

void VoteAgg::handleFinalServiceResponseMsg(cMessage *msg) {
  FinalServiceResponse *response = check_and_cast<FinalServiceResponse *>(msg);
  int providerId = response->getProviderId();
  double quality = response->getServiceQuality();
  totalServicesReceived++;   // <-- count every service received
  totalReceivedQuality += quality;

  std::string serviceType = response->getServiceType(); // lazim
  EV << "Node " << getId() << " received final service from " << providerId
     << " with quality: " << quality << endl;
  if (benevolent && quality < 0) {
    badServicesReceived++;
    totalBadServicesReceived++;
  }
  double rarity = 10;
  double timeliness = 10;      // TODO: bunu bilmiyom henuz...
  lastProviderId = providerId; // New global member needed

  double rating =
      calculateRating(quality, timeliness, rarity); // handles attacks too

  sendRating(providerId, rating);

  // Keep this only for raw rating history / debugging.
  // Do not use myRatingMap for aggregation or provider choice.
  updateMyRating(providerId, rating);//eskiden kalma silmedim şimdilik!!

  // Direct trust update.
  // This is the value used by VoteAgg.
  auto &alterandum = trustMap[providerId];

  if (rating > 0.0) {
      alterandum.sumOfPositiveRatings += rating;
  }

  alterandum.sumOfAllRatings += std::abs(rating);

  EV << "Direct trust update: requester=" << getId()
     << " provider=" << providerId
     << " rating=" << rating
     << " positiveSum=" << alterandum.sumOfPositiveRatings
     << " absSum=" << alterandum.sumOfAllRatings
     << " directTrust=" << alterandum.value()
     << "\n";
  delete response;
}

/**
 * Call for Cluster Heads only.
 * creates a replica of the provided transaction
 * and propagates it to the connected nodes,
 * so that they update their TS's accordingly.
 * NOTE could also be named "broadcast..."
 */
void VoteAgg::propagateRatingReplica(ServiceRating *transaction) {
  int providerId = transaction->getProviderId();
  int requesterId = transaction->getRequesterId();
  double rating = transaction->getRating();
  for (VoteAgg *node : allNodes) {
    if (node != this) {
      ServiceRating *replica = new ServiceRating("serviceRating");
      replica->setRequesterId(requesterId);
      replica->setProviderId(providerId);
      replica->setRating(rating);
      replica->setIsPropagated(true);
      int destId = node->getId();
      if (routingTable.find(destId) != routingTable.end()) {
        int gateIndex = routingTable[destId];
        send(replica, "inoutGate$o", gateIndex);
      } else {
        delete replica;
      }
    }
  }
}
/**
 * Helper function to create and add a block to the blockchain
 * return id of the added block for debugging */
int VoteAgg::addBlockToBC(double rating, int requesterId, int providerId) {
  int blockId = ++globalBlockId;
  Block newBlock = {.blockId = blockId,
                    .validatorId = getId(),
                    .transactionData = "Rating: " + std::to_string(rating) +
                                       " from " + std::to_string(requesterId) +
                                       " to " + std::to_string(providerId),
                    .timestamp = simTime().dbl()};
  blockchain.push_back(newBlock);
  return blockId;
}



void VoteAgg::handleServiceRatingMsg(cMessage *msg) {
  ServiceRating *transaction = check_and_cast<ServiceRating *>(msg);
  int providerId = transaction->getProviderId();
  int requesterId = transaction->getRequesterId();
  double rating = transaction->getRating();
  bool isPropagated = transaction->isPropagated();

  // In the vote aggregation model, nodes do not update direct trust
  // from other nodes' ratings.
  // Direct trust is updated only after this node personally receives service.
  if (!isPropagated) { // infinite loop olmasın diye
    if (isClusterHead) {
      EV << "Cluster Head Node " << getId() << " adding rating to blockchain."
         << endl;
      int blockId = addBlockToBC(rating, requesterId, providerId);
      EV << "Block " << blockId << " added to blockchain." << endl;

      // Broadcast to all other nodes
      propagateRatingReplica(transaction);
    } else {
      sendTransactionToClusterHead(transaction);
      return;
    }
  }

  delete transaction;
}

void VoteAgg::handleNetworkMessage(cMessage *msg) {
  const char *msgName = msg->getName();

  if (strcmp(msgName, "serviceRequest") == 0) {
    handleServiceRequestMsg(msg);
  } else if (strcmp(msgName, "serviceResponse") == 0) {
    handleServiceResponseMsg(msg);
  } else if (strcmp(msgName, "finalServiceRequest") == 0) {
    handleFinalServiceRequestMsg(msg);
  } else if (strcmp(msgName, "finalServiceResponse") == 0) {
    handleFinalServiceResponseMsg(msg);
  } else if (strcmp(msgName, "serviceRating") == 0) {
    handleServiceRatingMsg(msg);
  } else {
    EV << "Unhandled message type: " << msg->getName() << endl;
    delete msg;
  }
}

void VoteAgg::handleSelfMessage(cMessage *msg) {
  const char *msgName = msg->getName();
  if (strcmp(msg->getName(), "triggerOpportunistic") == 0) {
    EV << "Node " << getId()
       << " is now switching from opportunistic to malicious.\n";

    attackerType = CAMOUFLAGE; // davranışı değiştirdik
    camouflageRate = 0.0;      // artık hep kötü
    benevolent = false;
    getDisplayString().setTagArg("i", 1, "orange");

    delete msg;
    opportunisticTriggerMsg = nullptr;
    return;
  }
  else if (strcmp(msgName, "triggerHybrid") == 0) {
      EV << "HYBRID node " << getId()
         << " has switched: will now give BAD SERVICES.\n";

      hybridHasSwitched = true;
      getDisplayString().setTagArg("i", 1, "darkred");

      delete msg;
      return;
  }


  else if (strcmp(msgName, "globalTrustUpdate") == 0) {
    updateGlobalTrustList();

    recordScalar(("GlobalTrustListUpdatedAt_" +
                  std::to_string((int)simTime().dbl())).c_str(),
                 1);

    scheduleAt(simTime() + globalTrustUpdateInterval, msg);
    return;
  }

  else if (strcmp(msg->getName(), "badServiceLogger") == 0) {
    updateEpsilon();
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
        }
    if (totalServicesReceived > 0) {
        double avgQuality = totalReceivedQuality / totalServicesReceived;

        recordScalar(
            ("AverageReceivedQualityAt_" + std::to_string((int)simTime().dbl()))
                .c_str(),
            avgQuality
        );
    }
    // belirli sürede bir(şu anda 10 saniye) tekrar ettiği için
    // badServiceLogger'ın içine yazdım bu opportunistic saldırıyı başlatan
    // kısmı
    /*if (!opportunisticAttackTriggered &&
        simTime().dbl() >= opportunisticAttackTime) {
      opportunisticAttackTriggered = true;
      VoteAgg *mostTrusted = *std::max_element(
          allNodes.begin(), allNodes.end(),
          [](VoteAgg *a, VoteAgg *b) { return a->trustScore < b->trustScore; });

      mostTrusted->attackerType = CAMOUFLAGE;
      mostTrusted->camouflageRate =
          0.0; // hep kötücül davranıyor şu anda aslında kamuflaj dememeliydik o
               // yüzden ama böyle yapıp %100 kötücüle çevirmek daha kolay geldi
      mostTrusted->benevolent = false;
      mostTrusted->getDisplayString().setTagArg("i", 1,
                                                "orange"); // rengi değişiyor

      EV << "Opportunistic attack triggered! Node " << mostTrusted->getId()
         << " is now malicious.\n";
      opportunisticNode = mostTrusted;
    }
    if (opportunisticAttackTriggered) { // kötü davranmaya başlayan node'un
                                        // trust skorunu kaydetmek için
      recordScalar(("OpportunisticNodeTrustScoreAt_" +
                    std::to_string((int)simTime().dbl()))
                       .c_str(),
                   opportunisticNode->trustScore);
    }*/
    auto it = trustMap.find(opportunisticNodeId);
    if (it != trustMap.end()) {

      recordScalar(
          ("OpportunisticNodeTrustScoreAt_" +
           std::to_string((int)simTime().dbl()))
              .c_str(),
          it->second.value()); // her 10 saniyede bir opportunistic node'un
                               // trustını yazdıracak bütün nodelar
    }

    scheduleAt(simTime() + 10.0, msg); // repeat every 10s
    return;
  } else if (strcmp(msgName, "populateServiceTable") == 0) {
    populateServiceTable();
    delete msg;
  } else if (strcmp(msgName, "serviceRequestTimer") == 0) {
    EV << "VoteAgg " << getId() << " is initiating a service request." << endl;
    initiateServiceRequest();
    scheduleAt(simTime() + uniform(1, 5), msg); // Reschedule
  }
}

void VoteAgg::handleMessage(cMessage *msg) {
  EV << "My id is: " << getId() << endl;
  if (msg->isSelfMessage()) {
    handleSelfMessage(msg);
  } else {
    handleNetworkMessage(msg);
  }
}

void VoteAgg::electClusterHeads() {
  // sorts the allNodes array according to the nodes' trustScores
  // third param is a lambda func.
  std::sort(allNodes.begin(), allNodes.end(), [](VoteAgg *a, VoteAgg *b) {
    // TODO CLUSTER HEAD NEYE GORE BELIRLENECEK???
    return a->trustScore > b->trustScore;
  });

  // TODO: take the discussion below seriously, we sort and then traverse, no?
  for (size_t i = 0; i < allNodes.size();
       i++) { // bunu da daha farklı yazabiliriz böyle biraz saçma oldu ama
              // doğru çalışıyor olmalı
    // en üstteki i node'u seçmek daha hızlı olur sanırım ama meh, ne fark
    // eder...
    allNodes[i]->isClusterHead = (i < numClusterHeads);
  }
  EV << "Updated Cluster Head selection." << endl;
}
void VoteAgg::initiateServiceRequest() {
  std::vector<int> providerIds;
  // EVERY node can provide THE service
  for (auto node : routingTable) {
    providerIds.push_back(node.first);
  }
  // if providerIds vector is empty, this node has no connected nodes,
  // which is A FATAL PROBLEM; crash
  assert(!providerIds.empty());

  requestedServiceType = SERVICE_TYPE; // just the string "A"
  pendingResponses.clear();
  respondedProviders.clear();
  // Step 3: Send request to all eligible providers
  for (int providerId : providerIds) {
    if (providerId == getId())
      continue; // Don't request from self

    int gateIndex = routingTable[providerId];
    ServiceRequest *request = new ServiceRequest("serviceRequest");
    request->setRequesterId(getId());
    request->setProviderId(providerId);
    request->setServiceType(SERVICE_TYPE); // again, just the string "A"

    pendingResponses.insert(providerId);
    send(request, "inoutGate$o", gateIndex);
    EV << "Sent service request to Node " << providerId << " for type "
       << SERVICE_TYPE << endl;
  }
}

void VoteAgg::handleServiceRequest(int requesterId) {
  EV << "IoT Node " << getId() << " is providing service to Node "
     << requesterId << endl;

  ServiceResponse *serviceResponse = new ServiceResponse("serviceResponse");
  serviceResponse->setRequesterId(requesterId);
  serviceResponse->setProviderId(getId());

  if (routingTable.find(requesterId) != routingTable.end()) {
    int gateIndex = routingTable[requesterId];
    send(serviceResponse, "inoutGate$o", gateIndex);
  } else {
    EV << "Error: No route to send service response to Node " << requesterId
       << endl;
    delete serviceResponse;
  }
}
// return true if the node performs camouflage; i.e. behaving "normally"
bool performsCamouflage(double camouflageRate) {
  double random = uniform_real_dist(gen); // random number in (0,1)
  return random < camouflageRate; // performing camouflage,normal service
}

/* With a prob of camouflageRate, perform camouflage and return "normal" rating
 * else, return -10
 */
double VoteAgg::calculateRatingCamouflage(double quality, double timeliness,
                                          double rarity) {
  if (performsCamouflage(this->camouflageRate)) { // normal rating
    return calculateRatingBenevolent(quality, timeliness, rarity);
  } else {
    return -10;
  }
}

//"normal" quality if camouflage, -10 else
double VoteAgg::calcQualityCamouflage(double potency, double consistency) {
  if (performsCamouflage(this->camouflageRate)) { // normal service
    return calcQualityBenevolent(potency, consistency);
  } else { // giving bad service
    return -10;
  }
}
// TODO implement rating calculation for bad mouthing
double badMouthingRating() { return 0; }
double VoteAgg::calculateRatingBadMouthing(double quality,
                                           double timeliness,
                                           double rarity)
{
    VoteAgg *provider = getNodeById(lastProviderId);
    if (!provider) {
        EV_WARN << "BadMouthing: provider not found\n";
        return 0;
    }

    if (provider->benevolent) {
        return -10;
    }


    return 10; //bu kısım değişebilir şimdilik kötüler kötülere en yüksek ratingi veriyor
}

double VoteAgg::calculateRating(double quality, double timeliness,
                                double rarity) {
  enum AttackerType type = this->attackerType;

  switch (type) {
  case BENEVOLENT:
    return calculateRatingBenevolent(quality, timeliness, rarity);
  case CAMOUFLAGE:
    return calculateRatingCamouflage(quality, timeliness, rarity);
  case BAD_MOUTHING:
      return calculateRatingBadMouthing(quality, timeliness, rarity);
  case MALICIOUS_100:
      return -10;
  case BAD_SERVICE_GOOD_RATING:
      return calculateRatingBenevolent(quality, timeliness, rarity);
  case HYBRID:{
      VoteAgg *provider = getNodeById(lastProviderId);
          if (!provider)
            return 0;
      return provider->benevolent
                    ? calculateRatingBenevolent(-10, timeliness, rarity)
                    : calculateRatingBenevolent(
                          8, timeliness, rarity); }// punish honest, reward malicious
  case OPPORTUNISTIC:
    return calculateRatingBenevolent(quality, timeliness, rarity);
  case COLLABORATIVE: {
    VoteAgg *provider = getNodeById(lastProviderId);
    if (!provider)
      return 0;
    return provider->benevolent
               ? calculateRatingBenevolent(-10, timeliness, rarity)
               : calculateRatingBenevolent(
                     8, timeliness, rarity); // punish honest, reward malicious
  }
  default:
    EV << "SOMETHING WENT WRONG WITH calculateRating!!\n";
    return 0; // should not defualt to here!
  }
}
double VoteAgg::calcQuality(const double potency, const double consistency) {
  enum AttackerType type = this->attackerType;
  switch (type) {
  case BENEVOLENT:
    return calcQualityBenevolent(potency, consistency);
  case CAMOUFLAGE:
    return calcQualityCamouflage(potency, consistency);
  case OPPORTUNISTIC:
    return calcQualityBenevolent(potency, consistency);
  case BAD_MOUTHING:
      return calcQualityBenevolent(potency, consistency);
  case BAD_SERVICE_GOOD_RATING:
      return -10;
  case HYBRID:
      return hybridHasSwitched ? -10 : calcQualityBenevolent(potency, consistency);  // before switch: good service

  default:
    EV << "SOMETHING WENT WRONG WITH calcQuality!!\n";
    return -10; // should not defualt to here!
  }
}

/* The "normal" way of calculating the quality of a service
 * returns a double in (-10,10)
 * potency = mean & consistency = 1/stddev
 */
double VoteAgg::calcQualityBenevolent(const double potency,
                                      const double consistency) {
  double quality;

  double stddev = 1.0 / consistency;
  std::normal_distribution<double> dist{potency, stddev};
  quality = dist(gen);
  if (quality > 10) {
    quality = 10;
  } else if (quality < -10) {
    quality = -10;
  }
  return quality;
}

/** uses Quality, Timeliness and Rarity of the service to calculate a rating
 * all of which are in (-10,10)
 * simply takes the _weighted_ average of these three
 */
double VoteAgg::calculateRatingBenevolent(double quality,
                                          double timeliness,
                                          double rarity) {
  double rating;
  //deneme için koydum bu if olan kısmı
  if (quality < 0) {
     EV << "Bad quality detected, rating forced negative: " << quality << "\n";
     return quality;
   }
  // the weighted average of the thre components
  rating = (wQ * quality + wR * rarity + wT * timeliness) / (wQ + wR + wT);
  EV << "RATING IS CALCULATED AS: " << rating << '\n';
  return rating;
}
/**
 * The node that calls this method gives a rating to the provider of a service.
 * This rating is sent as a transaction and its sent to the CH */
void VoteAgg::sendRating(int providerId, double rating) {
  // rating in (-10,10)'e karar verildi
  EV << "IoT Node " << getId() << " gives a score of " << rating << " to Node "
     << providerId << endl;

  ServiceRating *transaction = new ServiceRating("serviceRating");
  transaction->setRequesterId(getId());
  transaction->setProviderId(providerId);
  transaction->setRating(rating);

  sendTransactionToClusterHead(transaction);
}

void VoteAgg::sendTransactionToClusterHead(ServiceRating *transaction) {
  // Ensure cluster heads are up-to-date
  // TODO We should have a clusterHeads vector and update that periodically
  electClusterHeads();

  std::vector<VoteAgg *> clusterHeads;
  // Collect all available Cluster Heads
  for (VoteAgg *node : allNodes) {
    if (node->isClusterHead) {
      clusterHeads.push_back(node);
    }
  }
  if (clusterHeads.empty()) {
    throw cRuntimeError("No valid Cluster Head found");
  }
  // Select a random Cluster Head
  // TODO: This should not be random. Each node must have one and only one
  // CH to which it sends.
  int randomIndex = intuniform(0, clusterHeads.size() - 1);
  VoteAgg *bestClusterHead = clusterHeads[randomIndex];

  int clusterHeadId = bestClusterHead->getId();

  // Check if there's a known route to the selected Cluster Head
  if (routingTable.find(clusterHeadId) == routingTable.end()) {
    EV << "Error: No known route to Cluster Head " << clusterHeadId << endl;
    delete transaction; // Prevent memory leak
    return;
  }

  int gateIndex = routingTable[clusterHeadId];

  EV << "IoT Node " << getId() << " forwarding transaction to Cluster Head "
     << clusterHeadId << " via gate index " << gateIndex << endl;

  // Send the transaction via the correct output gate
  send(transaction, "inoutGate$o", gateIndex);
}

bool VoteAgg::enoughInteractions(int requestorId, int providerId) {
  std::vector<Block> blocksInWindow;
  if (blockchain.size() < windowSize) {
    EV << "Blockchain not full — using available " << blockchain.size()
       << " blocks.\n";
    blocksInWindow.assign(blockchain.begin(), blockchain.end());
  } else {
    blocksInWindow.assign(blockchain.end() - windowSize, blockchain.end());
  }

  int encounterCounter = 0; // I like naming stuff :p
  for (Block block : blocksInWindow) {

    double dummyRating = 0;
    int reqId, provId;
    extract(block.transactionData, dummyRating, reqId, provId);
    // mahut blok bizim iki eleman arasinda ise sayaci arttir
    if (reqId == requestorId && provId == providerId) {
      ++encounterCounter;
    }
  }
  if (encounterCounter > enoughEncounterLimit) { // bu kısıma bağlı aslında DT
                                                 // mi yoksa IT mi çalışacağı
    EV << "Enough interactions between " << requestorId << " and " << providerId
       << "\n";
    return true;
  } else {
    EV << "not enough past interactions between nodes" << requestorId << " and "
       << providerId << "\n";
    EV << "because counter is at: " << encounterCounter;
    return false;
  }
}
/*calculates the decay factor in the Trust Score calculations
 * takes the current and block times as argument
 * this simple implementation just takes the exp of the difference.
 * FIXME: bu su an kullanilmiyor!!
 */
double VoteAgg::calculateDecay(double currentTime, double blockTime) {
  return std::exp(currentTime - blockTime);
}

/**This is to extract rating and id values from a transaction message in a
 * block. Give the message as input and the extracted values will be written
 * in the other parameters the input must be in the following format:
 * 'rating: <rating> from <reqId> to <provId>' a disposition of the colon
 * breaks the function
 */
bool VoteAgg::extract(const std::string &input, double &rating,
                      int &requesterId, int &providerId) {
  // this is to parse the input; rather cool!
  std::istringstream iss(input);
  // to store filler words
  std::string tmp;
  // this if statement is ugly, but it works, and it is terse!
  if ((iss >> tmp >> rating) && (iss >> tmp >> requesterId) &&
      (iss >> tmp >> providerId)) {
    return true;
  } else {
    return false;
  }
}


/*VoteAgg::AggregationMethod VoteAgg::parseAggregationMethod(const char *methodName) {
  std::string method = methodName ? std::string(methodName) : "additive";
  std::transform(method.begin(), method.end(), method.begin(), ::tolower);

  if (method == "additive" ) {
    return AGG_ADDITIVE;
  }
  if (method == "multiplicative" ) {
    return AGG_MULTIPLICATIVE;
  }
  if (method == "bordascore" ) {
    return AGG_BORDA;
  }

  throw cRuntimeError("Unknown aggregationMethod: %s", methodName);
}
*/
std::vector<int> VoteAgg::sortNodesByScore(const std::map<int, double> &scores) {
  std::vector<std::pair<int, double>> entries(scores.begin(), scores.end());

  std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b) {
    if (a.second == b.second)
      return a.first < b.first;
    return a.second > b.second;
  });

  std::vector<int> ranking;
  for (const auto &entry : entries) {
    ranking.push_back(entry.first);
  }
  return ranking;
}

VoteAgg::DirectTrustMatrix VoteAgg::buildDirectTrustMatrix() {
  DirectTrustMatrix matrix;

  for (VoteAgg *evaluator : allNodes) {
    int evaluatorId = evaluator->getId();

    for (VoteAgg *target : allNodes) {
      int targetId = target->getId();
      if (evaluatorId == targetId)
        continue;

      auto it = evaluator->trustMap.find(targetId);
      matrix[evaluatorId][targetId] =
          (it != evaluator->trustMap.end()) ? it->second.value() : 0.0;
    }
  }

  return matrix;
}
//TODO: Approval yöntemleri eklenecek
void VoteAgg::updateGlobalTrustList() {
  if (allNodes.empty())
    return;

  switch (allNodes.front()->aggregationMethod) {
  case AGG_ADDITIVE:
    updateGlobalTrustAdditive();
    break;
  case AGG_MULTIPLICATIVE:
    updateGlobalTrustMultiplicative();
    break;
  case AGG_BORDA:
    updateGlobalTrustBorda();
    break;
  default:
    throw cRuntimeError("Invalid aggregation method");
  }

  globalTrustRanking = sortNodesByScore(globalTrustScores);

  // Static function: avoid EV/simTime() here because EV requires a module instance.
}

void VoteAgg::updateGlobalTrustAdditive() {
  globalTrustScores.clear();
  DirectTrustMatrix matrix = buildDirectTrustMatrix();

  for (VoteAgg *target : allNodes) {
    int targetId = target->getId();
    double total = 0.0;
    int count = 0;

    for (VoteAgg *evaluator : allNodes) {
      int evaluatorId = evaluator->getId();
      if (evaluatorId == targetId)
        continue;

      total += matrix[evaluatorId][targetId];
      count++;
    }

    globalTrustScores[targetId] = (count > 0) ? total / count : 0.0;
  }
}

void VoteAgg::updateGlobalTrustMultiplicative() {
  globalTrustScores.clear();
  DirectTrustMatrix matrix = buildDirectTrustMatrix();

  for (VoteAgg *target : allNodes) {
    int targetId = target->getId();
    double product = 1.0;
    int count = 0;

    for (VoteAgg *evaluator : allNodes) {
      int evaluatorId = evaluator->getId();
      if (evaluatorId == targetId)
        continue;

      double trust = matrix[evaluatorId][targetId];
      if (trust < 0.0)
        trust = 0.0;
      if (trust > 1.0)
        trust = 1.0;
      product *= trust;
      count++;
    }

    globalTrustScores[targetId] = (count > 0) ? product : 0.0;
  }
}

void VoteAgg::updateGlobalTrustBorda() {
  globalTrustScores.clear();

  for (VoteAgg *node : allNodes) {
    globalTrustScores[node->getId()] = 0.0;
  }

  DirectTrustMatrix matrix = buildDirectTrustMatrix();

  for (VoteAgg *evaluator : allNodes) {
    int evaluatorId = evaluator->getId();
    std::map<int, double> localScores;

    for (VoteAgg *candidate : allNodes) {
      int candidateId = candidate->getId();
      if (candidateId == evaluatorId)
        continue;
      localScores[candidateId] = matrix[evaluatorId][candidateId];
    }

    std::vector<int> localRanking = sortNodesByScore(localScores);
    int n = localRanking.size();

    for (int i = 0; i < n; i++) {
      int candidateId = localRanking[i];
      int points = n - i;
      globalTrustScores[candidateId] += points;
    }
  }
}

int VoteAgg::getRankPointFromGlobalOrdering(int nodeId, const std::vector<int> &candidates) {
  int n = candidates.size();
  std::vector<int> filteredRanking;

  for (int rankedId : globalTrustRanking) {
    if (std::find(candidates.begin(), candidates.end(), rankedId) !=
        candidates.end()) {
      filteredRanking.push_back(rankedId);
    }
  }

  for (int i = 0; i < (int)filteredRanking.size(); i++) {
    if (filteredRanking[i] == nodeId)
      return n - i;
  }

  return 0;
}

double VoteAgg::mergeTrustScore(int candidateId) {
  std::vector<int> candidates;
  double directTrustSum = 0.0;

  for (const auto &entry : respondedProviders) {
    int id = entry.first;
    candidates.push_back(id);

    auto it = trustMap.find(id);
    double directTrust = (it != trustMap.end()) ? it->second.value() : 0.0;
    directTrustSum += directTrust;
  }

  int n = candidates.size();
  if (n == 0)
    return 0.0;

  double generalPoints = getRankPointFromGlobalOrdering(candidateId, candidates);

  auto it = trustMap.find(candidateId);
  double directTrust = (it != trustMap.end()) ? it->second.value() : 0.0;

  double totalGeneralPoints = n * (n + 1) / 2.0;
  double personalPoints = 0.0;

  if (directTrustSum > 0.0) {
    personalPoints = (directTrust / directTrustSum) * totalGeneralPoints;
  }

  EV << "Merge score for requester " << getId() << " candidate " << candidateId
     << ": general=" << generalPoints << " personal=" << personalPoints
     << " total=" << generalPoints + personalPoints << "\n";

  //return generalPoints + personalPoints;
//return generalPoints ;
  return directTrust ;

}

/**
 *  */
void VoteAgg::recordLocalTrust() {
  double sumOfLocalTrust = 0;
  int countOfNodesTrusting = 0;
  int thisId = getId();
  for (auto &node : allNodes) {
    if (node->trustMap.find(thisId) != node->trustMap.end()) {
      countOfNodesTrusting++;
      sumOfLocalTrust += node->trustMap[thisId].value();
    }
  }
  // if no nodes are counted, there has been a FATAL FLAW
  assert(countOfNodesTrusting != 0);
  double average = sumOfLocalTrust / countOfNodesTrusting;
  std::string scalarName = "Local Trust to Node " + std::to_string(thisId);
  recordScalar(scalarName.c_str(), average);
}

void VoteAgg::recordAbility(){
  double ability = this->potency * this->consistency;
  recordScalar("Ability of the node", ability);
}
void VoteAgg::finish() {
    recordScalar("IsMalicious", benevolent ? 0 : 1);
  // her node'un kendi tuttuğu trustları yazdırmak için
  for (auto &entry : trustMap) {
    int targetId = entry.first;
    double trustValue = entry.second.value();
    // Örneğin: TrustOf_5_in_3
    std::string scalarName = "TrustOf_" + std::to_string(getId()) + "_in_" +
                             std::to_string(targetId);
    recordScalar(scalarName.c_str(), trustValue);
  }
  // Let us have each node record its "local trust" by its neighbours as well:
  recordLocalTrust();
  recordAbility();

  // Accuracy için
  /*if (getId() == 2) { // tek bir node içinde hesplamak için yazdım bu kısmı
  node
                      // id'leri 2'den başlıyor omnet'te
    std::vector<std::pair<double, bool>> trustAndLabel;

    for (VoteAgg *node : allNodes) {
      trustAndLabel.emplace_back(node->trustScore, node->benevolent);
    }

    double bestF1 = 0.0;
    double bestThreshold = 0.5; // default threshold
    double bestPrecision = 0.0, bestRecall = 0.0, bestAccuracy = 0.0;
    double threshold = 0.5; //!!

    // for (double threshold = 0.0; threshold <= 1.0; threshold += 0.01) {
    int TP = 0, TN = 0, FP = 0, FN = 0;

    for (const auto &[score, isBenevolent] : trustAndLabel) {
      bool predictedBenevolent = (score >= threshold);
      // bu önemli!!!!
      // positive= malicious bu testlerde çünkü amacımız kötüyü bulmak
      if (!isBenevolent && !predictedBenevolent)
        TP++; // kötü olana kötü demiş
      else if (isBenevolent && predictedBenevolent)
        TN++; // iyi olana iyi demiş
      else if (!isBenevolent && predictedBenevolent)
        FN++; // kötüye iyi demiş
      else if (isBenevolent && !predictedBenevolent)
        FP++; // iyiye kötü demiş
    }

    double precision = (double)TP / (TP + FP + 1e-6);
    double recall = (double)TP / (TP + FN + 1e-6);
    double f1 = 2 * precision * recall / (precision + recall + 1e-6);
    double accuracy = (double)(TP + TN) / (TP + TN + FP + FN + 1e-6);

    if (f1 > bestF1) {
      bestF1 = f1;
      bestThreshold = threshold;
      bestPrecision = precision;
      bestRecall = recall;
      bestAccuracy = accuracy;
    }
    //}

    // Record best metrics
    recordScalar("BestThreshold", bestThreshold);
    recordScalar("BestF1Score", bestF1);
    recordScalar("BestPrecision", bestPrecision);
    recordScalar("BestRecall", bestRecall);
    recordScalar("BestAccuracy", bestAccuracy);
  }*/
  if (badServiceLogger != nullptr) {
    cancelAndDelete(badServiceLogger);
    badServiceLogger = nullptr;
  }
  if (opportunisticTriggerMsg != nullptr) {
    cancelAndDelete(opportunisticTriggerMsg);
    opportunisticTriggerMsg = nullptr;
  }
  if (globalTrustUpdateEvent != nullptr) {
    cancelAndDelete(globalTrustUpdateEvent);
    globalTrustUpdateEvent = nullptr;
  }

  if (benevolent) {
    recordScalar("FinalBadServicesReceived", badServicesReceived);
  }

  if (totalBenevolentNodes > 0) {
    recordScalar("FinalAverageBadServices",
                 (double)totalBadServicesReceived / totalBenevolentNodes);
  }
}
