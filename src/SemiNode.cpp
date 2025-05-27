/*
 * SemiNode.cpp
 *
 *  Created on: 3 May 2025
 *      Author: ipekm,sahinm
 *  IoTNode.cpp'nin bugunku hali uzerinden
 */
#include "SemiNode.h"
#include "BlockchainMessage_m.h"
#include "omnetpp/csimulation.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <map>
#include <random>
#include <string>
#include <vector>

std::default_random_engine genS;
std::uniform_real_distribution<double> uniform_real_distS{0, 1};
using namespace omnetpp;

Define_Module(SemiNode);

// TODO: bunlari henuz guncellemiyorum
// a mapping from nodes to a mapping from nodes to counters
static std::map<int, std::map<int, int>> nodeResponseCounts;
// how many times this node requested from these nodes nodeId-->counter
static std::map<int, std::map<int, int>> nodeRequestCounts;
// how many times did k recommend j to i
// [req][prov][rec]
static std::map<int, std::map<int, std::map<int, int>>>
    nodeRecommendationCounts;
// I hate type names in Cpp...

std::vector<Block> SemiNode::blockchain;
std::vector<SemiNode *> SemiNode::allNodes;
int SemiNode::numClusterHeads = 3;
int SemiNode::globalBlockId = 0;

std::set<int> SemiNode::maliciousNodeIds;
int SemiNode::totalBadServicesReceived = 0;
int SemiNode::totalBenevolentNodes = 0;

/*
 * How many times did I request a service from J
 * Works the same as the response counter function
 */
int totalRequestFromItoJ(int reqId, int provId) {
  return nodeRequestCounts[reqId][provId];
}

// i'nin j'den "resmi" bir istemde bulundugu halde cagir
// bu istemlerin sayisini bir arttirir
void incRequestOfIFromJ(int reqId, int provId) {
  ++nodeRequestCounts[reqId][provId];
}

/*
 * Terribly named but still conveys the message.
 * Returns the *total* number of returns from the provider to a request from the
 * requestor
 */
int positiveResponseToIfromJ(int reqId, int provId) {
  return nodeResponseCounts[reqId][provId];
}

// how many times did k recommend j to i
void incRecToIOfJByK(int reqId, int provId, int recId) {
  ++nodeRecommendationCounts[reqId][provId][recId];
}
// How many times did recId recommended prov to req
int recommendationNumber(int reqId, int provId, int recommenderId) {
  return nodeRecommendationCounts[reqId][provId][recommenderId];
  ;
}

// mukabili olan fonksiyonla ayni, donutleri arttiriyor
void incResponseToIFromJ(int reqId, int provId) {
  ++nodeResponseCounts[reqId][provId];
}

// RT_ij =  how many times j responded /how many times i asked
double SemiNode::responseTrustTo(int thisId, int nodeId) {
  // find the counts from both maps
  int totalRequests = totalRequestFromItoJ(thisId, nodeId);
  if (totalRequests == 0) {
    return 0;
  }
  int positiveResponses = positiveResponseToIfromJ(thisId, nodeId);

  return (double)positiveResponses / totalRequests;
}

// DONE: bu daha bitmedi, bittieeee
double SemiNode::recommendationDecay(int reqId, int provId, int recommenderId,
                                     double timeDif) {
  // exp (-coef / recomNumber * timeDif)
  int recomNumber = recommendationNumber(reqId, provId, recommenderId);

  return exp((-1) * recomDecayConstantCr / recomNumber * timeDif);
}

double SemiNode::semiDecay(int thisId, int nodeId, double timeDif) {
  // I need positive response number again...
  int positiveResponses = positiveResponseToIfromJ(thisId, nodeId);

  double exponandum = 0;
  double firstTerm = -(semiDecayConst / positiveResponses);
  exponandum = firstTerm * timeDif;
  return exp(exponandum);
}

std::vector<Block> SemiNode::takeBlocksInWindow() {
  std::vector<Block> blocksInWindow;
  if ((int)blockchain.size() < windowSize) {
    blocksInWindow.assign(blockchain.begin(), blockchain.end());
  } else {
    blocksInWindow.assign(blockchain.end() - windowSize, blockchain.end());
  }
  return blocksInWindow;
}

// DONE: YANLIS BU, 8 FALAN GELIYOR, tamam, sorun rating'in (0,1)'de olmamasi
// sum (ratings to j * decay)
double SemiNode::ratingTrustTo(int thisId, int nodeId) {
  double ratings = 0.0;
  double decays = 0.0;
  // examine the blocks in the window
  std::vector<Block> blocksInWindow = takeBlocksInWindow();

  for (auto &block : blocksInWindow) {
    int tmpProvider, tmpRequestor;
    double rating;
    if (!extract(block.transactionData, rating, tmpRequestor, tmpProvider))
      continue;
    if ((tmpProvider != nodeId) || (tmpRequestor != thisId))
      continue;
    double blockTime = block.timestamp;

    double timeDif = simTime().dbl() - blockTime;
    // sum up the rating of this to node with decay
    double decay = semiDecay(thisId, nodeId, timeDif);
    double addendum = rating * decay;
    ratings += addendum;
    decays += decay;
  }
  if (ratings == 0 || decays == 0) {
    // defaut deger vermemisler napim...
    return 0.5;
  }
  // over all the decay coefs (for some reason)
  return ratings / decays * 0.1; // bizde rating'ler (-10,10)'da...
}

// just responseTrust * ratingTrust
double SemiNode::semiDT(int thisId, int nodeId) {
  double rest = responseTrustTo(thisId, nodeId);
  double rat = ratingTrustTo(thisId, nodeId);
  EV << "response trust and rating trust are" << rest << "  " << rat << endl;
  return rest * rat;
}

// tavsiye dugumlerini sanirim ayni bir kume olarak almak lazim ama su an bu
// sekilde tatbik etmek kabil
std::vector<int> SemiNode::findRecommenders(int provId) {
  // i dunno how to spell
  std::vector<int> recommendors;
  for (auto node : allNodes) {
    if (enoughInteractions(node->getId(), provId)) {
      recommendors.push_back(node->getId());
    }
  }
  return recommendors;
}
// How many nodes can recommend THE PROVIDER. N_phi
int SemiNode::numberOfRecommendors(int provId) {
  int counter{};
  for (const auto &node : allNodes) {
    if (enoughInteractions(node->getId(), provId)) {
      ++counter;
    }
  }
  return counter;
}
// recTrust of this node i, to a recommender k, about node j
double SemiNode::recommendationTrust(int reqId, int provId, int recId) {
  // find out the difference between the previous recommendation trust and the
  // rating of the service resulted because of that recom

  // sum over a time window:
  // RR_ik(t) * DR_ik(t) --> numerator
  // DR_ik(t) --> denominator

  double numerator = 0;
  double denominator = 0;

  // inspect the part of the BC that is relevant
  std::vector<Block> blocksInWindow = takeBlocksInWindow();
  int blockReqId, blockProvId;
  double rating;
  for (const auto &block : blocksInWindow) {
    extract(block.transactionData, rating, blockReqId, blockProvId);
    // trying to see if the block is relevant
    if (blockReqId != reqId || blockProvId != provId) {
      continue;
    }
    // if the block is relevant, go on
    double blockTime = block.timestamp;
    double timeDif = simTime().dbl() - blockTime;

    double decayParameter = recommendationDecay(reqId, provId, recId, timeDif);

    // bu, mevzubahis oylamanin verildigi anda i'nin j'ye verdigi oy ile
    // k'nin j'ye ne kadar guvendiginin farki.
    double diff;
    double directTrust_kj = semiDT(recId, provId); // direct trust of k to j
    double rating_ij = rating;                     // the rating of i to j
    diff = std::abs(directTrust_kj - rating_ij);

    // recommendation rating of i to k is max(1 - diff_ik*2 , 0)
    double recomRating_ik = std::max(1 - diff * 2, 0.0);

    numerator += recomRating_ik * decayParameter;
    denominator += decayParameter;
  }
  if (numerator == 0) {
    return 0.5;
  }
  return numerator / denominator;
}
double SemiNode::semiIT(int thisId, int nodeId) {
  // a set of recommenders should be created
  std::vector<int> recommenderIds = findRecommenders(nodeId);

  // sum of dt_kj * recommendationTrust_ik
  // over sum of recTrust_ik s
  double recAndDirectTrusts = 0;
  double sumOfRecTrusts = 0;
  for (int recId : recommenderIds) {
    double DT_kj = semiDT(recId, nodeId);
    double recTrust_ik = recommendationTrust(thisId, recId, nodeId);
    recAndDirectTrusts += DT_kj * recTrust_ik;
    sumOfRecTrusts += recTrust_ik;
  }
  if (recAndDirectTrusts == 0 || sumOfRecTrusts == 0) {
    // WARN: MAKALEDE DEFAULT VAR MI, YOKSA BUNU BI DUSUNUN.
    //  defualt deger yok.......
    return 0.5;
  }
  // vallahi boyle yaziyor makalede...
  return recAndDirectTrusts / sumOfRecTrusts;
}
double calcBeta(double a, double b) { return b / (a + b); }
double calcAlpha(double a, double b) { return a / (a + b); }
// log(m+1)_(1+min(NS_ji,m))
double SemiNode::aWeight(int reqId, int provId) {
  double NS_ji = positiveResponseToIfromJ(reqId, provId);
  double logarithmand = 1 + std::min(NS_ji, m);
  if (logarithmand == 1) { // results in dividing by zero
    ++logarithmand;        // they did not have a default, again..
  }
  double base = m + 1;
  double logarithm =
      std::log(logarithmand) /
      std::log(base); // I don't think there is a func in the library to take
                      // the logartithm with a certain base.
  EV << "logarithman: " << logarithmand << " and base: " << base << "gives "
     << logarithm << '\n';
  return logarithm;
}
// log(l+1)_(1+min(N_phi,l))
double SemiNode::bWeight(int provId) {
  double N_phi = numberOfRecommendors(provId);
  double logarithmand = 1 + std::min(N_phi, l);
  if (logarithmand == 1) { // results in dividing by zero
    ++logarithmand;        // they did not have a default, again..
  }
  double base = l + 1;
  return std::log(logarithmand) / std::log(base);
}
// a*DT $ b*IT
double SemiNode::totalTrust(int thisId, int nodeId) {
  double DT_ij = semiDT(thisId, nodeId);
  double IT_ij = semiIT(thisId, nodeId);

  EV << "DT of " << thisId << " to " << nodeId << " is " << DT_ij << endl;
  EV << "IT of " << thisId << " to " << nodeId << " is " << IT_ij << endl;
  double a = aWeight(thisId, nodeId);
  double b = bWeight(nodeId);
  double alpha = calcAlpha(a, b);
  double beta = calcBeta(a, b);

  return alpha * DT_ij + beta * IT_ij;
}

void SemiNode::printRoutingTable(const std::map<int, int> &routingTable) {
  EV << "Routing Table:\n";
  EV << "NodeID --> Gate Index\n";
  for (const auto &entry : routingTable) {
    EV << entry.first << "-->" << entry.second << "\n";
  }
}
void SemiNode::initialize() {
  serviceRequestEvent =
      new cMessage("serviceRequestTimer"); // Give a distinct name
  scheduleAt(simTime() + uniform(1, 5), serviceRequestEvent);
  trustScore =
      uniform(0.5, 1.0); // start with moderate to high trust between 0.5 to 1.0
                         // Initial random trust score
                         // this can stay like this for the time being but
  isClusterHead = false;
  allNodes.push_back(this);

  // Define possible service types
  // std::vector<std::string> serviceTypes = {"A", "B", "C", "D", "E"};
  // std::vector<std::string> serviceTypes = {"A"};
  // bunu azalttım şimdilik!!! initiateServiceRequest'e bak orada da
  // servis tipleri var

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
  //  printRoutingTable(routingTable);
  // Schedule service table update after all nodes are initialized
  scheduleAt(simTime() + 0.1, new cMessage("populateServiceTable"));
  // bunun dokumantasyonuna bakarken cancleEvent() komutunu ogrendim ki gayet
  // onemli olabilir: scheduleAt'in hazirladigi mesajin belli bir zamanad
  // iptalini sagliyor. "gec kalan servis"lerin tespitinde kullanilabilir!! if
  // (time > requesTime + tolerance) {cancleEvent()...} syntax'i tam anlamadim o
  // yuzden dokumantasyona yeniden bakmak gerekecek
  // malicious olanları belirlemek için başka bir yol bulamadım
  int totalNodes = getParentModule()->par("numNodes");
  int numMalicious =
      int(par("maliciousNodePercentage").doubleValue() * totalNodes);
  EV << "******************************" << numMalicious;
  if (getId() == 2) { // first node to initialize (OMNeT IDs  start at 2)bunu
                      // kontrol ettim gerçekten 2'den başlıyor
    std::vector<int> allIds;
    for (int i = 2; i < 2 + totalNodes; ++i)
      allIds.push_back(i);
    std::shuffle(allIds.begin(), allIds.end(), genS);
    maliciousNodeIds.insert(allIds.begin(), allIds.begin() + numMalicious);
  }

  if (maliciousNodeIds.count(getId()) > 0) {
    benevolent = false;
    attackerType = CAMOUFLAGE; // saldırının adı değişecek
    potency = -10; // bunlar kaç olmalı bilmiyorum burada chatgpt'nin yazdığını
                   // bıraktım buraya bakalım
    consistency = 1000;
  } else {
    benevolent = true;
    totalBenevolentNodes++;
    potency = 9; // quality 8-9 arası olacak diye düşündüm
    consistency = 2.0;
  }
  if (!benevolent) {
    getDisplayString().setTagArg("i", 1, "red"); // highlight malicious nodes
  }

  // Start periodic logger(belirli aralıklarla kötü servis sayısını kaydetmek
  // için)
  badServiceLogger = new cMessage("badServiceLogger");
  scheduleAt(simTime() + 10.0, badServiceLogger); // 10 saniyede bir şu anda
}

void SemiNode::printBlockChain(std::vector<Block> blockchain) {
  EV << "je veux voir tous les block a la fois:\n";
  for (Block &block : blockchain) {
    EV << block.transactionData << "\nat time: " << block.timestamp << "\n";
  }
  EV << "on est finis lire le block!\n\n";
}
// ************************************************************
void SemiNode::populateServiceTable() {
  EV << "Node " << getId()
     << " is now filling its service table with direct neighbors..." << endl;
  for (int i = 0; i < gateSize("inoutGate"); i++) {
    cGate *outGate = gate("inoutGate$o", i);
    if (outGate->isConnected()) {
      cModule *connectedModule = outGate->getNextGate()->getOwnerModule();
      SemiNode *neighborNode = dynamic_cast<SemiNode *>(connectedModule);
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

void SemiNode::printServiceTable() {
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

void SemiNode::handleServiceRequestMsg(cMessage *msg) {
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
SemiNode *SemiNode::getNodeById(int targetId) {
  for (SemiNode *node : allNodes) {
    if (node->getId() == targetId) {
      return node;
    }
  }
  EV << "Warning: Node with ID " << targetId << " not found!" << endl;
  return nullptr;
}
void SemiNode::handleServiceResponseMsg(cMessage *msg) {
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
    // double dt =
    //     calculateDirectTrust(requestorId, responderId, simTime().dbl(), 0);
    // General Trust
    // double gt = getNodeById(responderId)->trustScore;

    // respondedProviders[responderId] = (a * dt + b * gt);
    double tt = totalTrust(requestorId, responderId);
    respondedProviders[responderId] = tt;
    pendingResponses.erase(responderId);
    EV << "Received service response from Node " << responderId
       << " with TT = " << tt << endl;

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

void SemiNode::handleFinalServiceRequestMsg(cMessage *msg) {
  // this function is called after i (req) and j (prov) have done a "handshake"
  // and i is "making a formal request of j".
  // therefore the request counter must increment.
  FinalServiceRequest *request = check_and_cast<FinalServiceRequest *>(msg);
  int requesterId = request->getRequesterId();
  std::string requestedService = request->getServiceType();

  // refer above
  incRequestOfIFromJ(requesterId, this->getId());

  if (providedService != requestedService) {
    EV << "Node " << getId() << " received final request for "
       << requestedService << ", but provides " << providedService
       << ". Ignoring." << endl;
    delete request;
    return;
  }

  // wait, this code assumes that j provides the service to i right after
  // receiving the final request!
  // therefore, the response trust should always be 1??
  EV << "Providing FINAL service to Node " << requesterId << endl;

  FinalServiceResponse *response =
      new FinalServiceResponse("finalServiceResponse");
  response->setRequesterId(requesterId);
  response->setProviderId(getId());
  response->setServiceType(requestedService.c_str());
  response->setServiceQuality(calcQuality(
      potency,
      consistency)); // provider kendi potency ve consistency'sini ekliyor

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
void SemiNode::handleFinalServiceResponseMsg(cMessage *msg) {
  // istemci bir cevap ve uzerine hizmet aldi
  // yani ki olumlu donuslerin sayisi arttirilmali!
  FinalServiceResponse *response = check_and_cast<FinalServiceResponse *>(msg);
  int providerId = response->getProviderId();
  double quality = response->getServiceQuality();

  incResponseToIFromJ(this->getId(), providerId);

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
  SemiNode *provider = getNodeById(providerId);
  updateProviderGeneralTrust(provider, trustScore,
                             rating); // şimdilik this kısmını sildim
  // WARN: bu metod normalde pointer degil, objenin kendisni aliyordu
  // cok dusuk de olsa duzgun yazmamis olma ihtimalim var
  // hata olursa buraya bakin

  delete response;
}

void SemiNode::handleServiceRatingMsg(cMessage *msg) {
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

void SemiNode::handleNetworkMessage(cMessage *msg) {
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

void SemiNode::handleSelfMessage(cMessage *msg) {
  const char *msgName = msg->getName();

  if (strcmp(msg->getName(), "badServiceLogger") == 0) {
    if (totalBenevolentNodes > 0) {
      recordScalar(
          ("AverageBadServicesAt_" + std::to_string((int)simTime().dbl()))
              .c_str(),
          (double)totalBadServicesReceived / totalBenevolentNodes);
    }
    scheduleAt(simTime() + 10.0, msg); // repeat every 10s
    // WARN: bunun suresini azaltmak fark yaratir mi acaba?
    return;
  }
  if (strcmp(msgName, "populateServiceTable") == 0) {
    populateServiceTable();
    delete msg;
  } else if (strcmp(msgName, "serviceRequestTimer") == 0) {
    EV << "SemiNode " << getId() << " is initiating a service request." << endl;
    initiateServiceRequest();
    scheduleAt(simTime() + uniform(1, 5), msg); // Reschedule
  }
}

// bu fonksiyonu böldüm içindeki fonksiyonlar yukarıda yazıyor
// çoğu eski haliyle aynı sadece final service request ve response
// fonksiyonlarını ekledim
void SemiNode::handleMessage(cMessage *msg) {
  EV << "My id is: " << getId() << endl;

  if (msg->isSelfMessage()) {
    handleSelfMessage(msg);
  } else {
    handleNetworkMessage(msg);
  }
}

void SemiNode::updateProviderGeneralTrust(SemiNode *provider,
                                          double requestorTrust,
                                          double rating) {
  double oldTS = provider->trustScore;
  double weightedRating = rating * requestorTrust;

  if (rating >= 0) {
    provider->sumOfPositveRatings += weightedRating;
    provider->sumOfAllRatings += weightedRating;
  } else {
    provider->sumOfAllRatings +=
        std::abs(weightedRating) *
        rancorCoef; // mutlak değere çevirdim önceki ile aynı şeyi yapıyor
                    // aslında değiştirmesem de olabilirdi
  }

  provider->trustScore = std::clamp(
      provider->sumOfPositveRatings /
          (provider->sumOfAllRatings +
           1e-6), // NaN olmasın diye ekledim 1e-6'yı
      0.1, 1.0    // böyle yazınca trust skor 0.1'den küçük gelirse 0.1'e
                  // eşitleniyor, 1'den büyük olursa da 1'e (aslında bu mümkün
                  // değil ama ne olur ne olmaz ekledim)
  );

  EV << "ProviderTS " << oldTS << " got a rating " << rating
     << " from a node with TS " << requestorTrust << " and was updated to "
     << provider->trustScore << "\n";
}

void SemiNode::electClusterHeads() {
  // sorts the allNodes array according to the nodes' trustScores
  // third param is a lambda func.
  std::sort(allNodes.begin(), allNodes.end(), [](SemiNode *a, SemiNode *b) {
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
// kiyamam ben buna...
int SemiNode::findRoute(int requesterId, int providerId) {
  // finds a route from requester to provider if they are not in the same
  // cluster
  EV << "Error: No known route to Node " << providerId << endl;
  return 0;
}
void SemiNode::initiateServiceRequest() {
  // Step 1: Choose a random service type
  // std::vector<std::string> serviceTypes = {
  //     "A", "B", "C", "D",
  //     "E"}; // şimdilik böyle yukarıda da var bundan nodeları oluştururken
  // yazmışım teke düşürsek daha kolay olabilir
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

void SemiNode::handleServiceRequest(int requesterId) {
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
bool SemiNode::performsCamouflage(double camouflageRate) {
  double random = uniform_real_distS(genS); // random number in (0,1)
  if (random < camouflageRate) { // performing camouflage,normal service
    return true;
  } else { // giving bad service
    return false;
  }
}
/* With a prob of camouflageRate, perform camouflage and return "normal" rating
 * else, return -10
 */
double SemiNode::calculateRatingCamouflage(double quality, double timeliness,
                                           double rarity) {
  if (performsCamouflage(this->camouflageRate)) { // normal rating
    return calculateRatingBenevolent(quality, timeliness, rarity);
  } else {
    return -10;
  }
}

//"normal" quality if camouflage, -10 else
double SemiNode::calcQualityCamouflage(double potency, double consistency) {
  if (performsCamouflage(this->camouflageRate)) { // normal service
    // return calcQualityBenevolent(potency, consistency);//burada hata var
    // bence çünkü kötü node'un potency'si farklı
    return calcQualityBenevolent(9, 2.0); // bu değer değişebilir
  } else {                                // giving bad service
    return -10;
  }
}
double SemiNode::badMouthingRating() { return 0; }

double SemiNode::calculateRating(double quality, double timeliness,
                                 double rarity) {
  enum AttackerType type = this->attackerType;

  switch (type) {
  case BENEVOLENT:
    return calculateRatingBenevolent(quality, timeliness, rarity);
  case CAMOUFLAGE:
    return calculateRatingCamouflage(quality, timeliness, rarity);
  default:
    EV << "SOMETHING WENT WRONG WITH calculateRating!!\n";
    return 0; // should not defualt to here!
  }
}
double SemiNode::calcQuality(const double potency, const double consistency) {
  enum AttackerType type = this->attackerType;
  switch (type) {
  case BENEVOLENT:
    return calcQualityBenevolent(potency, consistency);
  case CAMOUFLAGE:
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
double SemiNode::calcQualityBenevolent(const double potency,
                                       const double consistency) {
  double quality;
  /* if(consistency==1000){
        EV <<
   "*****************************************************************************"
   ;
   }*/

  double stddev = 1.0 / consistency;
  std::normal_distribution<double> dist{potency, stddev};
  quality = dist(genS);
  if (quality > 10) {
    quality = 10;
  } else if (quality < -10) {
    quality = -10;
  }
  return quality;
}
bool SemiNode::givesService(std::string serviceType) {
  if (providedService == serviceType) {
    return true;
  } else {
    return false;
  }
}
/* traverses ALL NODES IN THE SYSTEM, and returns the reciprocal of the number
 * of ondes that can provide the given service in (0,10)
 */
double SemiNode::calculateRarity(std::string serviceType) {
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
/*
 * uses Quality, Timeliness and Rarity of the service to calculate a rating
 * all of which are in (-10,10)
 * simply takes the _weighted_ average of these three
 */
double SemiNode::calculateRatingBenevolent(double quality, double timeliness,
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
void SemiNode::sendRating(int providerId, double rating) {
  // rating in (-10,10)'e karar verildi
  EV << "IoT Node " << getId() << " gives a score of " << rating << " to Node "
     << providerId << endl;

  ServiceRating *transaction = new ServiceRating("serviceRating");
  transaction->setRequesterId(getId());
  transaction->setProviderId(providerId);
  transaction->setRating(rating);

  sendTransactionToClusterHead(transaction);
}

void SemiNode::sendTransactionToClusterHead(ServiceRating *transaction) {
  // Ensure cluster heads are up-to-date
  // do we have to do this in "each" transaction? Bunu sanki chat yazmis
  // gibi :p
  electClusterHeads();

  std::vector<SemiNode *> clusterHeads;

  // Collect all available Cluster Heads
  for (SemiNode *node : allNodes) {
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
  SemiNode *bestClusterHead = clusterHeads[randomIndex];

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

// double SemiNode::calculateIndirectTrust(int reqId, int provId, double time,
//                                         int depth) {
//   const int MAX_TRUST_RECURSION_DEPTH = 7;
//
//   if (depth > MAX_TRUST_RECURSION_DEPTH) {
//     EV << "[IT] Max recursion depth reached from " << reqId << " to " <<
//     provId
//        << "\n";
//     return 0.1; // Fallback trust
//   }
//
//   EV << "[IT] Indirect trust from " << reqId << " to " << provId
//      << " | depth = " << depth << "\n";
//
//   std::vector<SemiNode *> nodesKnownByRequestor;
//   for (SemiNode *node : allNodes) {
//     int nodeId = node->getId();
//     if (nodeId == provId || nodeId == reqId)
//       continue;
//     if (enoughInteractions(reqId, nodeId)) {
//       nodesKnownByRequestor.push_back(node);
//     }
//   }
//
//   for (SemiNode *node : nodesKnownByRequestor) {
//     int nodeId = node->getId();
//     if (enoughInteractions(nodeId, provId)) {
//       EV << "Node " << reqId << " knows " << nodeId << ", who knows " <<
//       provId
//          << "\n";
//       return std::min(calculateDirectTrust(provId, nodeId, time, depth + 1),
//                       calculateDirectTrust(nodeId, provId, time, depth + 1));
//     }
//   }
//
//   return 0.1;
// }

bool SemiNode::enoughInteractions(int requestorId, int providerId) {
  /*if (blockchain.size() < windowSize) { // pencere bile dolmamis!
    EV << "window is not filled yet!\n";
    return false;
  }
  std::vector<Block> blocksInWindow(blockchain.end() - windowSize,
                                    blockchain.end());*/
  // bu kısımı değiştirdim eski halinde ile window size dolmamışsa eskiden
  // hesaplama yapılmıyordu ama o zamana kadar eklenmiş blokları kullanabiliriz
  // bence çünkü window size çok büyük de olabilir diğer makalelerde 1000
  // yapmışlar
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

/*this is to extract rating and id values from a transaction message in a
 * block give the message as input and the extracted values will be written
 * in the other parameters the input must be in the following format:
 * 'rating: <rating> from <reqId> to <provId>' a disposition of the colon
 * breaks the function
 */
bool SemiNode::extract(const std::string &input, double &rating,
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
void SemiNode::finish() {
  if (badServiceLogger != nullptr) {
    cancelAndDelete(badServiceLogger);
    badServiceLogger = nullptr;
  }

  if (benevolent) {
    recordScalar("FinalBadServicesReceived", badServicesReceived);
  }

  recordScalar("benevolentNodeNumber", totalBenevolentNodes);
  if (totalBenevolentNodes > 0) {
    recordScalar("FinalAverageBadServices",
                 (double)totalBadServicesReceived / totalBenevolentNodes);
  }
}
// bunu kullanmıyorum şu anda
int SemiNode::selectPoTValidator() {
  double totalTrust = 0.0;
  std::vector<SemiNode *> clusterHeads;
  // Collect only Cluster Heads and sum their trust scores
  for (SemiNode *node : allNodes) {
    if (node->isClusterHead) {
      clusterHeads.push_back(node);
      totalTrust += node->trustScore;
    }
  }
  // Generate a weighted random selection
  double randomValue = uniform(0, totalTrust);
  double cumulativeTrust = 0.0;

  for (SemiNode *node : clusterHeads) {
    cumulativeTrust += node->trustScore;
    if (randomValue <= cumulativeTrust) {
      return node->getId(); // Return the selected validator
    }
  }
  return -1; // Should not happen, but included as a fallback
}
