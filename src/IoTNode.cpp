/*
 * IoTNode.cpp
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm,sahinm
 */
#include "IoTNode.h"
#include "BlockchainMessage_m.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <random>
#include <string>
#include <vector>

std::default_random_engine gen; // WARN: bunu buraya koyabilir miyim?? koyabilirmişsin
// Some notes:
// TODO is a nice tag to have, with which we can follow what is left "to do" :p
// We should have a ClusterHeads vector ready. Creating one each time we need is
// costly
using namespace omnetpp;

Define_Module(IoTNode);
//!!!!!
std::vector<Block> IoTNode::blockchain;
std::vector<IoTNode *> IoTNode::allNodes;
// TODO: why do we have an allNodes vector in the IoTNode class?
// if I am not mistaken this should be in the blockchain class or sth
int IoTNode::numClusterHeads = 3;
int IoTNode::globalBlockId = 0;
int windowSize = 2; // TODO: buna karar vermek lazim. takibi kolay olsun diye
                    // gecici bir sey yaziyorum
// this is because I want to be able to print the table for debugging. not
// really necessary
void printRoutingTable(const std::map<int, int> &routingTable) {
  EV << "Routing Table:\n";
  EV << "NodeID --> Gate Index\n";
  for (const auto &entry : routingTable) {
    EV << entry.first << "-->" << entry.second << "\n";
  }
}
void IoTNode::initialize() {
  serviceRequestEvent =
      new cMessage("serviceRequestTimer"); // Give a distinct name
  scheduleAt(simTime() + uniform(1, 5), serviceRequestEvent);
  trustScore = uniform(
      50,
      100); // Initial random trust score
            // this can stay like this for the time being but
            // TODO: in the real sim, this should be decided by a "higher level"
  isClusterHead = false;
  allNodes.push_back(this);

  // Define possible service types
  std::vector<std::string> serviceTypes = {"A", "B", "C", "D"};

  // Her node'a rastgele bir servis tanımlıyoruz şu anda bunu belki daha farklı
  // da yapabiliriz servis tipi içinde başka şeyler de barındıran bir obje
  // olabilir ama gerek var mı emin değilim
  std::string assignedService = serviceTypes[intuniform(
      0,
      serviceTypes.size() - 1)]; // her çalıştırışımızda farklı servis verecek
                                 // ama bu kötü bir şey olmayabilir de
  EV << "Node " << getId() << " provides service: " << assignedService << endl;
  providedService = assignedService; // do we need this assignment?

  // Populate Routing Table
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

    printRoutingTable(routingTable);
  // Schedule service table update after all nodes are initialized
  scheduleAt(simTime() + 0.1, new cMessage("populateServiceTable"));
  // bunun dokumantasyonuna bakarken cancleEvent() komutunu ogrendim ki gayet
  // onemli olabilir: scheduleAt'in hazirladigi mesajin belli bir zamanad
  // iptalini sagliyor. "gec kalan servis"lerin tespitinde kullanilabilir!! if
  // (time > requesTime + tolerance) {cancleEvent()...} syntax'i tam anlamadim o
  // yuzden dokumantasyona yeniden bakmak gerekecek
}

void printBlockChain(std::vector<Block> blockchain) {
  EV << "je veux voir tous les block a la fois:\n";
  for (Block &block : blockchain) {
    EV << block.transactionData << "\nat time: " << block.timestamp << "\n";
  }
  EV << "on est finis lire le block!\n\n";
}

// ************************************************************
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

IoTNode * IoTNode::getNodeById(int targetId) {
    for (IoTNode* node : allNodes) {
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
    // double dt = calculateDirectTrust(*allNodes[responderId],
    // simTime().dbl());
    int requestorId = this->getId();
    EV << "I am about to calculate the DT of" << requestorId << " to "
       << responderId << "here is the entire BC";
    printBlockChain(blockchain);
    double dt = calculateDirectTrust(requestorId, responderId, simTime().dbl());
    // WARN: 'bu' dugumun 'responder'a DT'ini hesaplar.
    //General trust

    double gt = getNodeById(responderId)->trustScore;

    respondedProviders[responderId] = (a*dt+b*gt);
    pendingResponses.erase(responderId);
    EV << "Received service response from Node " << responderId
       << " with DT = " << dt << " and GT = "<< gt <<endl;

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
  response->setServiceQuality(calcQuality(
      potency,
      consistency)); // provider kendi potency ve consistency'sini ekliyor
  // TODO: BU METODU REFAKTOR ETMEK LAZIM KI KOTULER ICIN AYRI BIR ALT-METODU
  // CAGIRSIN, su an zor geliyo
  // response->setServiceQuality(
  //     uniform(3.0, 5.0)); // burası şu anda random ama random kalmayacak
  // saldırılara göre değiştireceğiz burayı

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
void IoTNode::handleFinalServiceResponseMsg(cMessage *msg) {
  FinalServiceResponse *response = check_and_cast<FinalServiceResponse *>(msg);
  int providerId = response->getProviderId();
  double quality = response->getServiceQuality();
  std::string serviceType = response->getServiceType(); // lazim
  EV << "Node " << getId() << " received final service from " << providerId
     << " with quality: " << quality << endl;
  // double providerPotency = getPotency(providerId);
  // double providerConsistency = getConsistency(providerId);

  // double rating =
  //     calculateRating(providerPotency, providerConsistency, timeliness,
  //     rarity);
  double rarity = calculateRarity(serviceType);
  double timeliness = 1; // TODO: bunu bilmiyom henuz...
  double rating = calculateRating(quality, timeliness, rarity);
  //burada bir yerde updateGeneralTrust'ı çağırmalıyız sanırım!!!!
  sendRating(providerId, rating);

  delete response;
}
void IoTNode::handleServiceRatingMsg(cMessage *msg) {
  ServiceRating *transaction = check_and_cast<ServiceRating *>(msg);

  if (isClusterHead) {
    EV << "Cluster Head Node " << getId() << " adding rating to blockchain."
       << endl;
    int blockId = ++globalBlockId;
    Block newBlock = {
        .blockId = blockId,
        .validatorId = getId(),
        .transactionData =
            "Rating: " + std::to_string(transaction->getRating()) + " from " +
            std::to_string(transaction->getRequesterId()) + " to " +
            std::to_string(transaction->getProviderId()),
        .timestamp = simTime().dbl()};

    blockchain.push_back(newBlock);
    EV << "Block " << blockId << " added to blockchain." << endl;
  } else {
    EV << "Forwarding rating to a Cluster Head" << endl;
    sendTransactionToClusterHead(transaction);
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

  if (strcmp(msgName, "populateServiceTable") == 0) {
    populateServiceTable();
    delete msg;
  } else if (strcmp(msgName, "serviceRequestTimer") == 0) {
    EV << "IoTNode " << getId() << " is initiating a service request." << endl;
    initiateServiceRequest();
    scheduleAt(simTime() + uniform(1, 5), msg); // Reschedule
  }
}

// bu fonksiyonu böldüm içindeki fonksiyonlar yukarıda yazıyor
// çoğu eski haliyle aynı sadece final service request ve response
// fonksiyonlarını ekledim
void IoTNode::handleMessage(cMessage *msg) {
  EV << "My id is: " << getId() << endl;

  if (msg->isSelfMessage()) {
    handleSelfMessage(msg);
  } else {
    handleNetworkMessage(msg);
  }
}

// TODO: bunun hesabini DT gibi bayesian bir seye cevir:
// yani ki sum(positiveRatings)/sum(all_ratings) olacak
//bunu her yeni rating'den sonra çağırmayı unutmayalım
void IoTNode::updateProviderGeneralTrust(IoTNode &provider,
                                         double requestorTrust, double rating) {
  // if rating is positive, add; else substract the update!
  double changeInTS = 0;
  if (rating >= 5) { //"positive" rating
    changeInTS = rating * requestorTrust;
  } else { // "negative" rating
    changeInTS = (-1) * rating * requestorTrust;
    // NOTE: burada -1 yerine kindarlikla ilgili bir katsayi??
  }
  // double updatedProvidedTrust =
  //     provider.trustScore + genTrustCoef * requestorTrust * rating;
  double updatedProviderTrust = genTrustCoef * provider.trustScore + changeInTS;
  provider.trustScore = updatedProviderTrust;

  double oldTS = provider.trustScore; // for debugging
  EV << "ProviderTS" << oldTS << " got a rating " << rating
     << "from a node with TS " << requestorTrust << " and was updated to "
     << updatedProviderTrust;
}

void IoTNode::electClusterHeads() {
  // sorts the allNodes array according to the nodes' trustScores
  // third param is a lambda func.
  std::sort(allNodes.begin(), allNodes.end(), [](IoTNode *a, IoTNode *b) {
    return a->trustScore > b->trustScore;
  });

  for (size_t i = 0; i < allNodes.size();
       i++) { // bunu da daha farklı yazabiliriz böyle biraz saçma oldu ama
              // doğru çalışıyor olmalı
    // en üstteki i node'u seçmek daha hızlı olur sanırım ama meh, ne fark
    // eder...
    allNodes[i]->isClusterHead = (i < numClusterHeads);
  }
  EV << "Updated Cluster Head selection." << endl;
}
int findRoute(int requesterId, int providerId) {
  // finds a route from requester to provider if they are not in the same
  // cluster
  EV << "Error: No known route to Node " << providerId << endl;
  return 0;
}
void IoTNode::initiateServiceRequest() {
  // Step 1: Choose a random service type
  std::vector<std::string> serviceTypes = {"A", "B", "C", "D"};
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

  // Optional: Set a timeout in case no one replies
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
// TODO: implement these...
double camouflageRating() { return 0; }
double badMouthingRating() { return 0; }
/* rating calculation for malicious nodes
 * this may need additional parameters...
 */
double IoTNode::calculateMalRating(enum AttackerType type) {
  switch (type) {
  case CAMOUFLAGE:
    camouflageRating();
  case BAD_MOUTHING:
    badMouthingRating();
  default:
    EV << "undefined attacker type issue in the rating calculationg method.\n";
    return 0;
  }
}

/* returns a double in (-10,10)
 * potency = mean & consistency = 1/stddev
 */
double IoTNode::calcQuality(const double potency, const double consistency) {
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
 * of ondes that can provide the given service
 */
double IoTNode::calculateRarity(std::string serviceType) {
  int howManyNodes = 0;
  for (const auto node : allNodes) {
    if (node->givesService(serviceType)) {
      ++howManyNodes;
    }
  }
  if (howManyNodes == 0) {
    return 1; // highest value it can get
  } else {
    return 1.0 / howManyNodes;
  }
}
/*
 * uses Quality, Timeliness and Rarity of the service to calculate a rating
 * all of which are in (-10,10)
 * simply takes the _weighted_ average of these three
 */
double IoTNode::calculateRating(double quality, double timeliness,
                                double rarity) {
  double rating;
  // double quality = calcQuality(potency, consistency);
  EV << "QUALITY IS CALCULATED WITH POTENCY: " << potency
     << " AND CONSISTENCY: " << consistency
     << " AND WAS FOUND TO BE: " << quality << '\n';
  // the weighted average of the thre components
  rating = (wQ * quality + wR * rarity + wT * timeliness) / (wQ + wR + wT);
  EV << "TIMELINESS IS: " << timeliness
     << " AND RARITY OF THE SERVICE IS: " << rarity << "\n";
  EV << "RATING IS CALCULATED AS: " << rating << '\n';
  return rating;
}
void IoTNode::sendRating(int providerId, double rating) {
  // rating in (-10,10)'e karar verildi
  double ratingToSend; // the actual rating the requestor sends
  if (this->benevolent) {
    ratingToSend = rating;
  } else { // kotuculse aldigi hizmetin hak ettiginden farkli oy verebilir
    ratingToSend = calculateMalRating(this->attackerType);
  }
  EV << "IoT Node " << getId() << " gives a score of " << ratingToSend
     << " to Node " << providerId << endl;

  ServiceRating *transaction = new ServiceRating("serviceRating");
  transaction->setRequesterId(getId());
  transaction->setProviderId(providerId);
  transaction->setRating(ratingToSend);

  sendTransactionToClusterHead(transaction);
}

void IoTNode::sendTransactionToClusterHead(ServiceRating *transaction) {
  // Ensure cluster heads are up-to-date
  // do we have to do this in "each" transaction? Bunu sanki chat yazmis
  // gibi :p
  electClusterHeads();

  std::vector<IoTNode *> clusterHeads;

  // Collect all available Cluster Heads
  for (IoTNode *node : allNodes) {
    if (node->isClusterHead) {
      clusterHeads.push_back(node);
    }
  }
  // I haven't read the entire code yet but this shoudl never happen I think
  // What happens at the beginning?
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
                        // this is not ideal.
                        // TODO: it should create a new route or sth if
                        // there is none!
    return;
  }

  int gateIndex = routingTable[clusterHeadId];

  EV << "IoT Node " << getId() << " forwarding transaction to Cluster Head "
     << clusterHeadId << " via gate index " << gateIndex << endl;

  // Send the transaction via the correct output gate
  send(transaction, "inoutGate$o", gateIndex);
}
// double IoTNode::calculateIndirectTrust(IoTNode &provider) {
//   // TOD:IMPLEMENT THIS!!!
//   // I feel like we need a array<IoTNode> neighbros for this.
//   return 0;
// }

double IoTNode::calculateIndirectTrust(int reqId, int provId, double time) {
  // TODO: This works for two layers only for now...

  // we know that i does not know j yet, get those who i knows:
  std::vector<IoTNode *> nodesKnownByRequestor;
  for (IoTNode *node : allNodes) {
    int nodeId = node->getId();
    if (nodeId == provId || nodeId == reqId) {
      // kendisine ve istemciye bakmasi gerekmiyor
      continue;
    }
    if (enoughInteractions(reqId, nodeId)) {
      nodesKnownByRequestor.push_back(node);
    }
  }
  // now check if a node known by i knows j
  for (IoTNode *node : nodesKnownByRequestor) {
    int nodeId = node->getId();
    // if it knows j, return the min of DTinode and DTnodej
    if (enoughInteractions(nodeId, provId)) {
      EV << "for node " << reqId << " node " << nodeId
         << " is known and it in turn knows " << provId << "\n";
      return std::min((calculateDirectTrust(provId, nodeId, time)),
                      (calculateDirectTrust(nodeId, provId, time)));
    }
  }
  // basaramadik abi...
  return 0.1;
  // TODO: bunu buyuk oranda debug icin 0.1 koydum
}
// bool IoTNode::enoughInteractions(IoTNode &provider) {
//   // TODO IMPLEMENT THIS TOO :p
//   // again, temporary for development purposes
//   if (blockchain.size() < windowSize) {
//     return false;
//   }
//   return true;
// }
bool IoTNode::enoughInteractions(int requestorId, int providerId) {
  if (blockchain.size() < windowSize) { // pencere bile dolmamis!
    EV << "window is not filled yet!\n";
    return false;
  }
  std::vector<Block> blocksInWindow(blockchain.end() - windowSize,
                                    blockchain.end());

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
  if (encounterCounter > enoughEncounterLimit) {
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
 */
double IoTNode::calculateDecay(double currentTime, double blockTime) {
  return std::exp(currentTime - blockTime);
}
/*calculates DT of requestor to provider
 * if 'enoughInteractions' which I will implement
 * else, resorts to indirectTrust
 */
double IoTNode::calculateDirectTrust(int requestorId, int providerId,
                                     double time) {
  if (!enoughInteractions(requestorId, providerId))
    return calculateIndirectTrust(requestorId, providerId, time);
  // so there are enough interactions, we calculate DT
  double dt = 0; // initialise DT
  double positiveRatings = 0;
  double all_ratings = 0;
  // bu ikisi decay'e tabii olacaklar

  // TODO: DT icin zincirin tamamina mi bakacagiz yoksa yalnizca pencereye
  // mi??
  //  bu tercihin tatbiki cok kolay ama uzun zincirler icin performans farki
  //  olabilir sanirim pencere kullanmak daha mantikli, su anlik boyle
  //  birakiyorum
  EV << "calculating DT of " << requestorId << " to " << providerId << '\n';
  for (auto &block : blockchain) {
    int tmpProvider, tmpRequestor;
    double rating;
    // extract ids and rating
    extract(block.transactionData, rating, tmpRequestor, tmpProvider);
    // if the block is not relevant, ignore!
    if ((tmpProvider != providerId) || !(tmpRequestor != requestorId))
      continue;
    double blockTime = block.timestamp;
    // double decayFactor = calculateDecay(time, blockTime);
    //   dt += rating * decayFactor;
    double addendum = rating * decayFactor;
    EV << "rating " << rating << '\n';
    if (rating >= 5.0) { // positive rating
      positiveRatings += addendum;
    } else { // negative rating;
      addendum *= rancorCoef;
      // olumsuzsa kin katsayisiyla carp ki fazla tesir etsin
    }
    all_ratings += addendum;
  }
  EV << "positive ratings :" << positiveRatings
     << "\nall ratings: " << all_ratings << '\n';
  dt = positiveRatings / all_ratings;
  return dt;
}
// calculates DT of 'this' to provider
// if 'enoughInteractions' which I will implement
// else, resorts to indirectTrust
// double IoTNode::calculateDirectTrust(IoTNode &provider, double time) {
//   int providerId = provider.getId();
//   int requertorId = getId();         // so that I can follow more easily
//   if (!enoughInteractions(provider)) // TODO: returns just 0 for now...
//     return calculateIndirectTrust(provider);
//   // so there are enough interactions, we calculate DT
//   double dt = 0; // initialise DT
//   for (auto &block : blockchain) {
//     int tmpProvider, tmpRequestor;
//     double rating;
//     // extract ids and rating
//     extract(block.transactionData, rating, tmpRequestor, tmpProvider);
//     // if the block is not relevant, ignore!
//     if ((tmpProvider != providerId) || !(tmpRequestor != requertorId))
//       continue;
//     double blockTime = block.timestamp;
//     double decayFactor = calculateDecay(time, blockTime);
//     dt += rating * decayFactor;
//   }
//   return dt;
// }

/*this is to extract rating and id values from a transaction message in a
 * block give the message as input and the extracted values will be written
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
// bunu kullanmıyorum şu anda
int IoTNode::selectPoTValidator() {
  double totalTrust = 0.0;
  std::vector<IoTNode *> clusterHeads;

  // Collect only Cluster Heads and sum their trust scores
  for (IoTNode *node : allNodes) {
    if (node->isClusterHead) {
      clusterHeads.push_back(node);
      totalTrust += node->trustScore;
    }
  }

  // Generate a weighted random selection
  double randomValue = uniform(0, totalTrust);
  double cumulativeTrust = 0.0;

  for (IoTNode *node : clusterHeads) {
    cumulativeTrust += node->trustScore;
    if (randomValue <= cumulativeTrust) {
      return node->getId(); // Return the selected validator
    }
  }

  return -1; // Should not happen, but included as a fallback
}
