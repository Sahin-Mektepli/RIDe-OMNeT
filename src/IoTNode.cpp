/*
 * IoTNode.cpp
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm,sahinm
 */
#include "IoTNode.h"
#include "BlockchainMessage_m.h"
#include "omnetpp/clog.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <random>
#include <string>
#include <vector>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
/*
** -----KRITIK BAZI NOTLAR------
** - Kalite hesabı nasıl olacak, artık nadirlik falan yok!
** - Çizgenin yapısı aslında değişmeli, ama bu ertelenebilir
** TODO Decay should be implemented, probably in updateTrustScore
** TODO Update TS only if you are connected to the provider node
** TODO Our graph is currently a fully connected one...
*/

std::default_random_engine gen;
std::uniform_real_distribution<double> uniform_real_dist{
    0, 1}; // bu da burda dursun madem
[[deprecated("There is only a single service!!!!")]]
std::vector<std::string> serviceTypes = {"A"}; // should be deleted all together
using namespace omnetpp;

Define_Module(IoTNode);
std::vector<Block> IoTNode::blockchain;
std::vector<IoTNode *> IoTNode::allNodes;
int IoTNode::numClusterHeads = 3;
int IoTNode::globalBlockId = 0;
std::set<int> IoTNode::maliciousNodeIds;
int IoTNode::totalBadServicesReceived = 0;
int IoTNode::totalBenevolentNodes = 0;
int IoTNode::opportunisticNodeId = -1;

void printRoutingTable(const std::map<int, int> &routingTable) {
  EV << "Routing Table:\n";
  EV << "NodeID --> Gate Index\n";
  for (const auto &entry : routingTable) {
    EV << entry.first << "-->" << entry.second << "\n";
  }
}
/**
 *called in initialize to assign providedService of the node */
std::string IoTNode::assignService() {
  std::string assignedService =
      serviceTypes[intuniform(0, serviceTypes.size() - 1)];
  providedService = assignedService;
  return assignedService;
}
/**
 * Copy of the original code that does its name
 * I am not sure about every line of this code
 * but it has worked for generations -esm- */
void IoTNode::populateRoutingTable() {
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
/**
 * Extracted from initialize method
 * Attacker type part should be refactored too. */
void IoTNode::setMalicious(AttackerType type) {
  int totalNodes = getParentModule()->par("numNodes");
  int numMalicious =
      int(par("maliciousNodePercentage").doubleValue() * totalNodes);

  // Sadece ilk node random seçim yapar
  if (getId() == 2) {
    std::vector<int> allIds;
    for (int i = 2; i < 2 + totalNodes; ++i)
      allIds.push_back(i);
    std::shuffle(allIds.begin(), allIds.end(), gen);

    if (type == OPPORTUNISTIC) {
      opportunisticNodeId = allIds.front();
      for (int i = 0; i < numMalicious; ++i) {
        if (allIds[i] != opportunisticNodeId)
          maliciousNodeIds.insert(allIds[i]); // Diğerleri malicious
      }
    } else {
      // OPPORTUNISTIC değilse, seçilen sayıda saldırgan belirle
      maliciousNodeIds.insert(allIds.begin(), allIds.begin() + numMalicious);
    }
  }

  if (type == OPPORTUNISTIC && getId() == opportunisticNodeId) {
    attackerType = OPPORTUNISTIC;
    isOpportunisticNode = true;
    benevolent = true; // Başta iyi
    potency = 9;
    consistency = 2.0;
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
    benevolent = false;
    // TODO :eğer kötülerin potency ve consistency değerleri saldırıya göre
    // değişmeyecekse aşağıdaki satırları silebiliriz. Sadece potency = -10;
    // consistency = 1000; kalmalı
    if (attackerType == CAMOUFLAGE) {
      // camouflageRate = 0.8;
      potency = -10;
      consistency = 1000;
    } else if (attackerType == MALICIOUS_100) {
      potency = -10;
      consistency = 1000;
    } else if (attackerType == BAD_MOUTHING) {
      // iyi servis kötü yorum ise
      potency = 9;
      consistency = 2.0;
    } else {
      potency = -10;
      consistency = 1000;
    }
  } else {
    attackerType = BENEVOLENT;
    benevolent = true;
    totalBenevolentNodes++;
    potency = 6;
    consistency = 0.5;
  }

  if (!benevolent) {
    getDisplayString().setTagArg("i", 1, "red");
  }
  if (attackerType == CAMOUFLAGE)
    getDisplayString().setTagArg(
        "i", 1, "blue"); // TODO Let each other have differnet colours?
}

void IoTNode::initialize() {
  serviceRequestEvent = new cMessage("serviceRequestTimer");
  scheduleAt(simTime() + uniform(1, 5), serviceRequestEvent);
  isClusterHead = false;
  allNodes.push_back(this);

  std::string assignedService = assignService();
  populateRoutingTable();

  // Schedule service table update after all nodes are initialized
  scheduleAt(simTime() + 0.1, new cMessage("populateServiceTable"));

  setMalicious(
      OPPORTUNISTIC); // BENEVOLENT, CAMOUFLAGE,BAD_MOUTHING,MALICIOUS_100,
                      // COLLABORATIVE, OPPORTUNISTIC

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
}

void printBlockChain(std::vector<Block> blockchain) {
  EV << "je veux voir tous les block a la fois:\n";
  for (Block &block : blockchain) {
    EV << block.transactionData << "\nat time: " << block.timestamp << "\n";
  }
  EV << "on est finis lire le block!\n\n";
}

void IoTNode::populateServiceTable() {
  EV << "Node " << getId()
     << " is now filling its service table with direct neighbors..." << endl;
  for (int i = 0; i < gateSize("inoutGate"); i++) {
    cGate *outGate = gate("inoutGate$o", i);
    if (outGate->isConnected()) {
      cModule *connectedModule = outGate->getNextGate()->getOwnerModule();
      IoTNode *neighborNode = dynamic_cast<IoTNode *>(connectedModule);
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
double IoTNode::calculateRatingSimilarityCoefficient(int providerId,
                                                     double newRating) {

  double currentRating = myRatingMap[providerId].value();
  EV << "Current Rating:" << currentRating << "\n";
  if (currentRating == NAN) {
    EV_WARN << "Could not calculate alpha, current rating is not a number";
    return NAN;
  }
  double diff = std::abs(newRating - currentRating);
  //!! NOTE Ratings are in (-10,10) so diff is in (0,20), divide by 20 to
  //! normalize
  diff = diff / 20.0; // so that diff is now in (0,1)
  double exponent = 6 * diff - 3;
  double denom = 1 + exp(exponent); // cannot possibly be 0 or negative
  EV << "my current rating is " << currentRating << "\nnewRating is "
     << newRating << '\n';
  EV << "diff between my rating and current rating is " << diff
     << " therefore alpha is " << 1 / denom;
  double alpha = 1 / denom;
  if (alpha == 1) { // extra error catching
    EV << "!!!!!ALPHA IS EXACTLY 1. THIS SHOULD NOT BE!!!!!!!!!\n";
  }
  return alpha; // certainly in (0,1)
}

void IoTNode::printServiceTable() {
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

void IoTNode::handleServiceRequestMsg(cMessage *msg) {
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
IoTNode *IoTNode::getNodeById(int targetId) {
  for (IoTNode *node : allNodes) {
    if (node->getId() == targetId) {
      return node;
    }
  }
  EV << "Warning: Node with ID " << targetId << " not found!" << endl;
  return nullptr;
}
void IoTNode::handleServiceResponseMsg(cMessage *msg) {
  ServiceResponse *response = check_and_cast<ServiceResponse *>(msg);
  int responderId = response->getProviderId();
  std::string serviceType = response->getServiceType();

  if (requestedServiceType == serviceType) {
    int requestorId = this->getId();

    // NEW: Use only local trust scores
    // double localTrust =
    //     localTrustScores.count(responderId) > 0
    //         ? localTrustScores[responderId]
    //         : 0.5; // TODO: 0.5 olarak kalmayabilir burası diğer güvenilir
    //                // nodeların verdiği ratinglerin ortalaması alınabilir

    // TODO DOES THIS FIX THE TS CALCULATION??
    // if so we can remove localTrust variables in general
    double localTrust = trustMap[responderId].value();
    respondedProviders[responderId] = localTrust;

    pendingResponses.erase(responderId);
    EV << "Received service response from Node " << responderId
       << " with Local Trust = " << localTrust << endl;

    if (pendingResponses.empty()) {
      int bestProviderId = -1;
      double maxTrust = -1;

      for (const auto &entry : respondedProviders) {
        if (entry.second > maxTrust) {
          bestProviderId = entry.first;
          maxTrust = entry.second;
        }
      }

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
void IoTNode::handleFinalServiceRequestMsg(cMessage *msg) {
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

double IoTNode::updateMyRating(int providerId, double rating) {
  myRatings &alterandum = myRatingMap[providerId];
  alterandum.sumRatings += rating;
  alterandum.count += 1;
  return alterandum.value();
}

void IoTNode::handleFinalServiceResponseMsg(cMessage *msg) {
  FinalServiceResponse *response = check_and_cast<FinalServiceResponse *>(msg);
  int providerId = response->getProviderId();
  double quality = response->getServiceQuality();
  std::string serviceType = response->getServiceType(); // lazim
  EV << "Node " << getId() << " received final service from " << providerId
     << " with quality: " << quality << endl;
  if (benevolent && quality <= 0) {
    badServicesReceived++;
    totalBadServicesReceived++;
  }
  double rarity = 10;
  double timeliness = 10;      // TODO: bunu bilmiyom henuz...
  lastProviderId = providerId; // New global member needed

  double rating =
      calculateRating(quality, timeliness, rarity); // handles attacks too

  sendRating(providerId, rating);
  updateMyRating(providerId, rating);

  delete response;
}

/**
 * Call for Cluster Heads only.
 * creates a replica of the provided transaction
 * and propagates it to the connected nodes,
 * so that they update their TS's accordingly.
 * NOTE could also be named "broadcast..."
 */
void IoTNode::propagateRatingReplica(ServiceRating *transaction) {
  int providerId = transaction->getProviderId();
  int requesterId = transaction->getRequesterId();
  double rating = transaction->getRating();
  for (IoTNode *node : allNodes) {
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
int IoTNode::addBlockToBC(double rating, int requesterId, int providerId) {
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

/**
 * Update the trustScore to the node providerId with given rating, which is in
 * [-10,10] and similarity coefficient alpha, which is strictly in (0,1) */
double IoTNode::updateTrustScore(int providerId, double rating, double alpha) {
  if (alpha < 0 || alpha > 1) {
    EV_WARN << "Could not update trust of " << getId() << " to " << providerId
            << ".\nSomehow alpha was not in [0,1]!" << std::endl;
  }
  double ratingEffect = rating * alpha;
  // The following adds to the map if providerId does not already exist
  // We ought to add it anyways...
  struct trustScore &alterandum = trustMap[providerId];
  if (rating >= 0)
    alterandum.sumOfPositiveRatings += ratingEffect;
  else // rating is negative, make it positive
    ratingEffect = (-ratingEffect);
  alterandum.sumOfAllRatings += ratingEffect;
  // test için
  EV << "updateTrustScore for " << providerId << ": rating=" << rating
     << ", alpha=" << alpha << ", effect=" << ratingEffect
     << ", sumOfPositiveRatings=" << alterandum.sumOfPositiveRatings
     << ", sumOfAllRatings=" << alterandum.sumOfAllRatings << "\n";

  return alterandum.value();
}

void IoTNode::handleServiceRatingMsg(cMessage *msg) {
  ServiceRating *transaction = check_and_cast<ServiceRating *>(msg);
  int providerId = transaction->getProviderId();
  int requesterId = transaction->getRequesterId();
  double rating = transaction->getRating();
  bool isPropagated = transaction->isPropagated();

  // All nodes (including CH) process this rating to update local trust
  if (providerId != getId()) {
    // kendi içinde kendi trust'ını hesaplamasına gerek yok
    double alpha = calculateRatingSimilarityCoefficient(providerId, rating);
    // TODO This updates TS for EVERY node, not just if connected!!
    double updatedTrust = updateTrustScore(providerId, rating, alpha);
    // TODO E- I may have broken this...!!buraya bak
    /*recordScalar(("TrustOf_" + std::to_string(providerId) + "_InNode_" +
                  std::to_string(getId()))
                     .c_str(),
                 updatedTrust);*/
  }
  // cluster head rating'i blockchain'e ekleyip diğer nodelara bildiriyor
  // cluster'daki
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

void IoTNode::handleNetworkMessage(cMessage *msg) {
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

void IoTNode::handleSelfMessage(cMessage *msg) {
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

  else if (strcmp(msg->getName(), "badServiceLogger") == 0) {
    if (totalBenevolentNodes > 0) {
      recordScalar(
          ("AverageBadServicesAt_" + std::to_string((int)simTime().dbl()))
              .c_str(),
          (double)totalBadServicesReceived / totalBenevolentNodes);
    }
    // belirli sürede bir(şu anda 10 saniye) tekrar ettiği için
    // badServiceLogger'ın içine yazdım bu opportunistic saldırıyı başlatan
    // kısmı
    /*if (!opportunisticAttackTriggered &&
        simTime().dbl() >= opportunisticAttackTime) {
      opportunisticAttackTriggered = true;
      IoTNode *mostTrusted = *std::max_element(
          allNodes.begin(), allNodes.end(),
          [](IoTNode *a, IoTNode *b) { return a->trustScore < b->trustScore; });

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
    EV << "IoTNode " << getId() << " is initiating a service request." << endl;
    initiateServiceRequest();
    scheduleAt(simTime() + uniform(1, 5), msg); // Reschedule
  }
}

void IoTNode::handleMessage(cMessage *msg) {
  EV << "My id is: " << getId() << endl;
  if (msg->isSelfMessage()) {
    handleSelfMessage(msg);
  } else {
    handleNetworkMessage(msg);
  }
}

void IoTNode::electClusterHeads() {
  // sorts the allNodes array according to the nodes' trustScores
  // third param is a lambda func.
  std::sort(allNodes.begin(), allNodes.end(), [](IoTNode *a, IoTNode *b) {
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
void IoTNode::initiateServiceRequest() {
  // TODO WE HAVE ONLY A SINGLE SERVICE TYPE!!!
  std::string chosenService =
      serviceTypes[intuniform(0, serviceTypes.size() - 1)];
  EV << "IoT Node " << getId()
     << " is requesting service type: " << chosenService << endl;
  // Step 2: Look up nodes that provide this service
  if (serviceTable.find(chosenService) == serviceTable.end()) {
    EV << "No known providers for service type " << chosenService << endl;
    return;
  }

  std::vector<int> providerIds = serviceTable[chosenService];
  if (providerIds.empty()) {
    EV << "Service list is empty for service type " << chosenService << endl;
    return;
  }

  requestedServiceType = chosenService;
  pendingResponses.clear();
  respondedProviders.clear();
  // Step 3: Send request to all eligible providers
  for (int providerId : providerIds) {
    if (providerId == getId())
      continue; // Don't request from self
    if (routingTable.find(providerId) == routingTable.end())
      continue; // No route

    int gateIndex = routingTable[providerId];
    ServiceRequest *request = new ServiceRequest("serviceRequest");
    request->setRequesterId(getId());
    request->setProviderId(providerId);
    request->setServiceType(chosenService.c_str());

    pendingResponses.insert(providerId);
    send(request, "inoutGate$o", gateIndex);
    EV << "Sent service request to Node " << providerId << " for type "
       << chosenService << endl;
  }
}

void IoTNode::handleServiceRequest(int requesterId) {
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
  if (random < camouflageRate) { // performing camouflage,normal service
    return true;
  } else { // giving bad service
    return false;
  }
}
/* With a prob of camouflageRate, perform camouflage and return "normal" rating
 * else, return -10
 */
double IoTNode::calculateRatingCamouflage(double quality, double timeliness,
                                          double rarity) {
  if (performsCamouflage(this->camouflageRate)) { // normal rating
    return calculateRatingBenevolent(quality, timeliness, rarity);
  } else {
    return -10;
  }
}

//"normal" quality if camouflage, -10 else
double IoTNode::calcQualityCamouflage(double potency, double consistency) {
  if (performsCamouflage(this->camouflageRate)) { // normal service
    // return calcQualityBenevolent(potency, consistency);//burada hata var
    // bence çünkü kötü node'un potency'si farklı
    return calcQualityBenevolent(9, 2.0); // bu değer değişebilir
  } else {                                // giving bad service
    return -10;
  }
}
// TODO implement rating calculation for bad mouthing
double badMouthingRating() { return 0; }

double IoTNode::calculateRating(double quality, double timeliness,
                                double rarity) {
  enum AttackerType type = this->attackerType;

  switch (type) {
  case BENEVOLENT:
    return calculateRatingBenevolent(quality, timeliness, rarity);
  case CAMOUFLAGE:
    return calculateRatingCamouflage(quality, timeliness, rarity);
  case OPPORTUNISTIC:
    return calculateRatingBenevolent(quality, timeliness, rarity);
  case COLLABORATIVE: {
    IoTNode *provider = getNodeById(lastProviderId);
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
double IoTNode::calcQuality(const double potency, const double consistency) {
  enum AttackerType type = this->attackerType;
  switch (type) {
  case BENEVOLENT:
    return calcQualityBenevolent(potency, consistency);
  case CAMOUFLAGE:
    return calcQualityCamouflage(potency, consistency);
  case OPPORTUNISTIC:
    return calcQualityBenevolent(potency, consistency);
  default:
    EV << "SOMETHING WENT WRONG WITH calcQuality!!\n";
    return -10; // should not defualt to here!
  }
}

/* The "normal" way of calculating the quality of a service
 * returns a double in (-10,10)
 * potency = mean & consistency = 1/stddev
 */
double IoTNode::calcQualityBenevolent(const double potency,
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
bool IoTNode::givesService(std::string serviceType) {
  if (providedService == serviceType) {
    return true;
  } else {
    return false;
  }
}
/* traverses ALL NODES IN THE SYSTEM, and returns the reciprocal of the number
 * of ondes that can provide the given service in (0,10)
 */
double IoTNode::calculateRarity(std::string serviceType) {
  int howManyNodes = 0;
  for (const auto node : allNodes) {
    if (node->givesService(serviceType)) {
      ++howManyNodes;
    }
  }
  if (howManyNodes == 0) {
    return 10; // highest value it can get
  } else {
    return 10.0 / howManyNodes;
  }
}
/** uses Quality, Timeliness and Rarity of the service to calculate a rating
 * all of which are in (-10,10)
 * simply takes the _weighted_ average of these three
 */
double IoTNode::calculateRatingBenevolent(double quality,
                                          double timeliness = 10,
                                          double rarity = 10) {
  double rating;
  // the weighted average of the thre components
  rating = (wQ * quality + wR * rarity + wT * timeliness) / (wQ + wR + wT);
  EV << "RATING IS CALCULATED AS: " << rating << '\n';
  return rating;
}
/**
 * The node that calls this method gives a rating to the provider of a service.
 * This rating is sent as a transaction and its sent to the CH */
void IoTNode::sendRating(int providerId, double rating) {
  // rating in (-10,10)'e karar verildi
  EV << "IoT Node " << getId() << " gives a score of " << rating << " to Node "
     << providerId << endl;

  ServiceRating *transaction = new ServiceRating("serviceRating");
  transaction->setRequesterId(getId());
  transaction->setProviderId(providerId);
  transaction->setRating(rating);

  sendTransactionToClusterHead(transaction);
}

void IoTNode::sendTransactionToClusterHead(ServiceRating *transaction) {
  // Ensure cluster heads are up-to-date
  // TODO We should have a clusterHeads vector and update that periodically
  electClusterHeads();

  std::vector<IoTNode *> clusterHeads;
  // Collect all available Cluster Heads
  for (IoTNode *node : allNodes) {
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
  IoTNode *bestClusterHead = clusterHeads[randomIndex];

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

bool IoTNode::enoughInteractions(int requestorId, int providerId) {
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
double IoTNode::calculateDecay(double currentTime, double blockTime) {
  return std::exp(currentTime - blockTime);
}


/**This is to extract rating and id values from a transaction message in a
 * block. Give the message as input and the extracted values will be written
 * in the other parameters the input must be in the following format:
 * 'rating: <rating> from <reqId> to <provId>' a disposition of the colon
 * breaks the function
 */
bool IoTNode::extract(const std::string &input, double &rating,
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

void IoTNode::finish() {
  // her node'un kendi tuttuğu trustları yazdırmak için
  for (auto &entry : trustMap) {
    int targetId = entry.first;
    double trustValue = entry.second.value();
    // Örneğin: TrustOf_5_in_3
    std::string scalarName = "TrustOf_" + std::to_string(targetId) + "_in_" +
                             std::to_string(getId());
    recordScalar(scalarName.c_str(), trustValue);
  }
  // Accuracy için
  /*if (getId() == 2) { // tek bir node içinde hesplamak için yazdım bu kısmı
  node
                      // id'leri 2'den başlıyor omnet'te
    std::vector<std::pair<double, bool>> trustAndLabel;

    for (IoTNode *node : allNodes) {
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

  if (benevolent) {
    recordScalar("FinalBadServicesReceived", badServicesReceived);
  }

  if (totalBenevolentNodes > 0) {
    recordScalar("FinalAverageBadServices",
                 (double)totalBadServicesReceived / totalBenevolentNodes);
  }
}
