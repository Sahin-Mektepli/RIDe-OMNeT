/*
 * IoTNode.cpp
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm,sahinm
 */
#include "IoTNode.h"
#include "BlockchainMessage_m.h"
#include "omnetpp/csimulation.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <ostream>
#include <random>
#include <string>
#include <vector>

double expellLimit = 0.4;
std::default_random_engine gen;
std::uniform_real_distribution<double> uniform_real_dist{0, 1};
using namespace omnetpp;

Define_Module(IoTNode);
std::vector<Block> IoTNode::blockchain;
std::vector<IoTNode *> IoTNode::allNodes;

int IoTNode::numClusterHeads = 3;
int IoTNode::globalBlockId = 0;
int windowSize = 2;
// TODO this should be in the header.

std::set<int> IoTNode::maliciousNodeIds;
int IoTNode::totalBadServicesReceived = 0;
int IoTNode::totalBenevolentNodes = 0;

/**
 *Print the routing table of the node,
 *to see if the connections are correctly established*/
void printRoutingTable(const std::map<int, int> &routingTable) {
  EV << "Routing Table:\n";
  EV << "NodeID --> Gate Index\n";
  for (const auto &entry : routingTable) {
    EV << entry.first << "-->" << entry.second << "\n";
  }
}
void IoTNode::initialize() {
  serviceRequestEvent = new cMessage("serviceRequestTimer");
  scheduleAt(simTime() + uniform(1, 5), serviceRequestEvent);
  trustScore = uniform(0.1, 0.5);
  isClusterHead = false;
  allNodes.push_back(this);

  std::vector<std::string> serviceTypes = {"A", "B"};
  // TODO We won't have any types!!

  std::string assignedService =
      serviceTypes[intuniform(0, serviceTypes.size() - 1)];

  EV << "Node " << getId() << " provides service: " << assignedService << endl;
  providedService = assignedService;

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
  // Schedule service table update after all nodes are initialized
  scheduleAt(simTime() + 0.1, new cMessage("populateServiceTable"));

  int totalNodes = getParentModule()->par("numNodes");
  int numMalicious =
      int(par("maliciousNodePercentage").doubleValue() * totalNodes);

  if (getId() == 2) { // first node to initialize (OMNeT IDs  start at 2)bunu
                      // kontrol ettim gerçekten 2'den başlıyor
    std::vector<int> allIds;
    for (int i = 2; i < 2 + totalNodes; ++i)
      allIds.push_back(i);
    std::shuffle(allIds.begin(), allIds.end(), gen);
    maliciousNodeIds.insert(allIds.begin(), allIds.begin() + numMalicious);
  }

  if (maliciousNodeIds.count(getId()) > 0) {
    benevolent = false;

    // FIXME these are defined default values in the header already!
    attackerType = CAMOUFLAGE; // saldırının adı değişecek
    potency = -10;
    consistency = 1000;
  } else {
    benevolent = true;
    totalBenevolentNodes++;

    std::uniform_real_distribution<double> potency_dist(-10.0, 10.0);
    std::uniform_real_distribution<double> cons_dist(1, 2.0);

    potency = potency_dist(gen);
    consistency = cons_dist(gen);
  }
  if (!benevolent) {
    getDisplayString().setTagArg("i", 1, "red"); // highlight malicious nodes
  }

  // Start periodic logger(belirli aralıklarla kötü servis sayısını kaydetmek
  // için)
  badServiceLogger = new cMessage("badServiceLogger");
  scheduleAt(simTime() + 10.0, badServiceLogger); // 10 saniyede bir şu anda
}

/*
** Only use to debug!
*/
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

/*
** Return *IoTNode with the given ID
*/
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
    // double dt = calculateDirectTrust(*allNodes[responderId],
    // simTime().dbl());
    int requestorId = this->getId();
    EV << "I am about to calculate the DT of" << requestorId << " to "
       << responderId << "here is the entire BC";
    printBlockChain(blockchain);
    double dt =
        calculateDirectTrust(requestorId, responderId, simTime().dbl(), 0);
    // WARN: 'bu' dugumun 'responder'a DT'ini hesaplar.
    // General Trust
    double gt = getNodeById(responderId)->trustScore;

    respondedProviders[responderId] = (a * dt + b * gt);
    pendingResponses.erase(responderId);
    EV << "Received service response from Node " << responderId
       << " with DT = " << dt << " and GT " << gt << endl;

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
  if (benevolent && quality <= 0) {
    badServicesReceived++;
    totalBadServicesReceived++;
  }
  double rarity = calculateRarity(serviceType);
  double timeliness = 10; // TODO: bunu bilmiyom henuz...
  double rating =
      calculateRating(quality, timeliness, rarity); // handles attacks too

  sendRating(providerId, rating);
  IoTNode *provider = getNodeById(providerId);
  updateProviderGeneralTrust(provider, trustScore, rating);

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

  if (strcmp(msg->getName(), "badServiceLogger") == 0) {
    if (totalBenevolentNodes > 0) {
      recordScalar(
          ("AverageBadServicesAt_" + std::to_string((int)simTime().dbl()))
              .c_str(),
          (double)totalBadServicesReceived / totalBenevolentNodes);
    }
    scheduleAt(simTime() + 10.0, msg); // repeat every 10s
    return;
  }
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

void writeToFile(const std::string &filename, const std::string &content) {
  std::ofstream outfile;
  outfile.open(filename, std::ios::app);
  if (!outfile.is_open()) {
    return;
  }
  outfile << content << std::endl;
  outfile.close();
}

void IoTNode::updateProviderGeneralTrust(IoTNode *provider,
                                         double requestorTrust, double rating) {
  double oldTS = provider->trustScore;
  double weightedRating = rating * requestorTrust;

  provider->sumOfPositveRatings *= decayFactor;
  provider->sumOfAllRatings *= decayFactor;
  if (rating >= 0) {
    provider->sumOfPositveRatings += weightedRating;
    provider->sumOfAllRatings += weightedRating;
  } else {
    provider->sumOfAllRatings += std::abs(weightedRating) * rancorCoef;
  }

  provider->trustScore = std::clamp(
      provider->sumOfPositveRatings /
          (provider->sumOfAllRatings +
           1e-6), // NaN olmasın diye ekledim 1e-6'yı
      0.1, 1.0);  // böyle yazınca trust skor 0.1'den küçük gelirse 0.1'e
                 // eşitleniyor, 1'den büyük olursa da 1'e (aslında bu mümkün
                 // değil ama ne olur ne olmaz ekledim)

  EV << "ProviderTS " << oldTS << " got a rating " << rating
     << " from a node with TS " << requestorTrust << " and was updated to "
     << provider->trustScore << "\n";
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
    allNodes[i]->isClusterHead = ((int)i < numClusterHeads);
  }
  EV << "Updated Cluster Head selection." << endl;
}

void IoTNode::initiateServiceRequest() {
  // Step 1: Choose a random service type

  std::vector<std::string> serviceTypes = {"A", "B"};
  // böyle yukarıda da var bundan nodeları oluştururken yazmışım teke düşürsek
  // daha kolay olabilir
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
  // FIXME how should this (and alike) change for the new system?
  double random = uniform_real_dist(gen); // random number in (0,1)
  return random < camouflageRate; // performing camouflage,normal service
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
  if (performsCamouflage(this->camouflageRate)) {       // normal service
    return calcQualityBenevolent(potency, consistency); // bu değer değişebilir
  } else {                                              // giving bad service
    return -10;
  }
}
// FIXME not implemented. I am not deleting for it can be used
double badMouthingRating() { return 0; }

double IoTNode::calculateRating(double quality, double timeliness,
                                double rarity) {
  enum AttackerType type = this->attackerType;

  switch (type) {
  case BENEVOLENT:
    return calculateRatingBenevolent(quality, timeliness, rarity);
  case CAMOUFLAGE:
    return calculateRatingCamouflage(quality, timeliness, rarity);
  case MALFUNCTION:
    // Malfunciton does not affect the ratings it gives
    return calculateRatingBenevolent(quality, timeliness, rarity);
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
  case MALFUNCTION:
    // with a certain prob, give bad service == camouflage!
    return calcQualityCamouflage(potency, consistency);
  default:
    EV << "SOMETHING WENT WRONG WITH calcQuality!!\n";
    return 0; // should not defualt to here!
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
  return providedService == serviceType;
}
/* traverses ALL NODES IN THE SYSTEM, and returns the reciprocal of the number
 * of nodes that can provide the given service in (0,10)
 */
double IoTNode::calculateRarity(std::string serviceType) {
  int howManyNodes = 0;
  for (const auto node : allNodes) {
    if (node->givesService(serviceType)) {
      if (node->banished) {
        continue;
      }
      ++howManyNodes;
    }
  }
  if (howManyNodes == 0) {
    return 10; // highest value it can get
  } else {
    return 10.0 / howManyNodes;
  }
}
/*
 * uses Quality, Timeliness and Rarity of the service to calculate a rating
 * all of which are in (-10,10)
 * simply takes the _weighted_ average of these three
 */
double IoTNode::calculateRatingBenevolent(double quality, double timeliness,
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
  // do we have to do this in "each" transaction?
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

double IoTNode::calculateIndirectTrust(int reqId, int provId, double time,
                                       int depth) {

  std::vector<IoTNode *> nodesKnownByRequestor;
  for (IoTNode *node : allNodes) {
    int nodeId = node->getId();
    if (nodeId == provId || nodeId == reqId)
      continue;
    if (enoughInteractions(reqId, nodeId)) {
      nodesKnownByRequestor.push_back(node);
    }
  }

  for (IoTNode *node : nodesKnownByRequestor) {
    int nodeId = node->getId();
    if (enoughInteractions(nodeId, provId)) {
      EV << "Node " << reqId << " knows " << nodeId << ", who knows " << provId
         << "\n";
      return std::min(calculateDirectTrust(provId, nodeId, time, depth + 1),
                      calculateDirectTrust(nodeId, provId, time, depth + 1));
    }
  }

  return 0.5;
}

bool IoTNode::enoughInteractions(int requestorId, int providerId) {
  std::vector<Block> blocksInWindow;
  if ((int)blockchain.size() < windowSize) {
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
 * FIXME bu su an kullanilmiyor!!
 */
double IoTNode::calculateDecay(double currentTime, double blockTime) {
  return std::exp(currentTime - blockTime);
}
/*calculates DT of requestor to provider
 * if 'enoughInteractions' which I will implement
 * else, resorts to indirectTrust
 */
double IoTNode::calculateDirectTrust(int requestorId, int providerId,
                                     double time, int depth) {

  if (!enoughInteractions(requestorId, providerId))
    return calculateIndirectTrust(requestorId, providerId, time, depth + 1);

  double positiveRatings = 0.0;
  double all_ratings = 0.0;

  for (auto &block : blockchain) {
    int tmpProvider, tmpRequestor;
    double rating;
    if (!extract(block.transactionData, rating, tmpRequestor, tmpProvider))
      continue;
    if ((tmpProvider != providerId) || (tmpRequestor != requestorId))
      continue;

    double addendum = rating * decayFactor;
    if (rating >= 0) {
      positiveRatings += addendum;
    } else {
      addendum *= (-1) * rancorCoef;
    }
    all_ratings += addendum;
  }

  double dt;
  if (all_ratings == 0) {
    dt = 0.5;
  }
  dt = positiveRatings / all_ratings;
  return std::clamp(dt, 0.0, 1.0);
}

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
void IoTNode::finish() {
  double potencyXconsistency = this->potency * this->consistency;
  std::string ts = std::to_string(potencyXconsistency) + " " +
                   std::to_string(this->trustScore);
  writeToFile("finalTSs.txt", ts);
  if (badServiceLogger != nullptr) {
    cancelAndDelete(badServiceLogger);
    badServiceLogger = nullptr;
  }

  if (benevolent) {
    recordScalar("FinalBadServicesReceived", badServicesReceived);
  }

  if (totalBenevolentNodes > 0) {
    recordScalar("FinalAverageBadServices",
                 (double)totalBadServicesReceived / totalBenevolentNodes);
  }
}
