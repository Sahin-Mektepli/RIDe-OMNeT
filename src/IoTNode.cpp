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

std::default_random_engine gen; // WARN: bunu buraya koyabilir miyim??
std::uniform_real_distribution<double> uniform_real_dist{
    0, 1}; // bu da burda dursun madem
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
std::set<int> IoTNode::maliciousNodeIds;
int IoTNode::totalBadServicesReceived = 0;
int IoTNode::totalBenevolentNodes = 0;
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
  trustScore = 0.5; // start with moderate to high trust between 0.5 to 1.0 !!burayı değiştirdim
 // Initial random trust score
            // this can stay like this for the time being but
            // TODO: in the real sim, this should be decided by a "higher level"
  isClusterHead = false;
  allNodes.push_back(this);

  // Define possible service types
  std::vector<std::string> serviceTypes = {"A", "B","C","D","E"};//bunu azalttım şimdilik!!! initiateServiceRequest'e bak orada da servis tipleri var

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
  EV << "******************************"<<numMalicious;
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
    attackerType = COLLABORATIVE; // saldırının adı değişecek
    potency = -10; // bunlar kaç olmalı bilmiyorum burada chatgpt'nin yazdığını
                   // bıraktım buraya bakalım
    consistency = 1000;
  } else {
      benevolent = true;
         totalBenevolentNodes++;
         potency = 9;        // quality 8-9 arası olacak diye düşündüm
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
    double dt = calculateDirectTrust(requestorId, responderId, simTime().dbl());
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
  lastProviderId = providerId;  // New global member needed

  double rating =
      calculateRating(quality, timeliness, rarity); // handles attacks too

  sendRating(providerId, rating);
  IoTNode *provider = getNodeById(providerId);
  updateProviderGeneralTrust(provider, trustScore, rating);//şimdilik this kısmını sildim
  // WARN: bu metod normalde pointer degil, objenin kendisni aliyordu
  // cok dusuk de olsa duzgun yazmamis olma ihtimalim var
  // hata olursa buraya bakin

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
    //belirli sürede bir(şu anda 10 saniye) tekrar ettiği için badServiceLogger'ın içine yazdım bu opportunistic saldırıyı başlatan kısmı
    if (!opportunisticAttackTriggered && simTime().dbl() >= opportunisticAttackTime) {
        opportunisticAttackTriggered = true;
        IoTNode* mostTrusted = *std::max_element(allNodes.begin(), allNodes.end(),
            [](IoTNode* a, IoTNode* b) { return a->trustScore < b->trustScore; });

        mostTrusted->attackerType = CAMOUFLAGE;
        mostTrusted->camouflageRate = 0.0; // hep kötücül davranıyor şu anda aslında kamuflaj dememeliydik o yüzden ama böyle yapıp %100 kötücüle çevirmek daha kolay geldi
        mostTrusted->benevolent = false;
        mostTrusted->getDisplayString().setTagArg("i", 1, "orange");//rengi değişiyor


        EV << "Opportunistic attack triggered! Node " << mostTrusted->getId() << " is now malicious.\n";
        opportunisticNode = mostTrusted;
    }
    if (opportunisticAttackTriggered) {//kötü davranmaya başlayan node'un trust skorunu kaydetmek için
        recordScalar(("OpportunisticNodeTrustScoreAt_" + std::to_string((int)simTime().dbl())).c_str(),
                     opportunisticNode->trustScore);
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

void IoTNode::updateProviderGeneralTrust(IoTNode *provider,
                                         double requestorTrust, double rating) {
  double oldTS = provider->trustScore;
  double weightedRating = rating * requestorTrust;

  if (rating >= 0) {
    provider->sumOfPositveRatings += weightedRating;
    provider->sumOfAllRatings += weightedRating;
  } else {
    provider->sumOfAllRatings += std::abs(weightedRating) * rancorCoef;//mutlak değere çevirdim önceki ile aynı şeyi yapıyor aslında değiştirmesem de olabilirdi
  }

  provider->trustScore = std::clamp(
      provider->sumOfPositveRatings / (provider->sumOfAllRatings + 1e-6),//NaN olmasın diye ekledim 1e-6'yı
      0.1, 1.0 //böyle yazınca trust skor 0.1'den küçük gelirse 0.1'e eşitleniyor, 1'den büyük olursa da 1'e (aslında bu mümkün değil ama ne olur ne olmaz ekledim)
  );

  EV << "ProviderTS " << oldTS << " got a rating " << rating
     << " from a node with TS " << requestorTrust
     << " and was updated to " << provider->trustScore << "\n";
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
  std::vector<std::string> serviceTypes = {"A", "B","C","D","E"};//şimdilik böyle yukarıda da var bundan nodeları oluştururken yazmışım teke düşürsek daha kolay olabilir
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
    //return calcQualityBenevolent(potency, consistency);//burada hata var bence çünkü kötü node'un potency'si farklı
    return calcQualityBenevolent(9, 2.0); //bu değer değişebilir
  } else { // giving bad service
    return -10;
  }
}
double badMouthingRating() { return 0; }

double IoTNode::calculateRating(double quality, double timeliness,
                                double rarity) {
  enum AttackerType type = this->attackerType;

  switch (type) {
  case BENEVOLENT:
    return calculateRatingBenevolent(quality, timeliness, rarity);
  case CAMOUFLAGE:
    return calculateRatingCamouflage(quality, timeliness, rarity);
  case COLLABORATIVE: {
       IoTNode* provider = getNodeById(lastProviderId);
       if (!provider) return 0;
       return provider->benevolent ?calculateRatingBenevolent(-10, timeliness, rarity) :calculateRatingBenevolent(8, timeliness, rarity); // punish honest, reward malicious
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
 /* if(consistency==1000){
       EV << "*****************************************************************************" ;
  }*/

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

double IoTNode::calculateIndirectTrust(int reqId, int provId, double time) {
  // WARN: This works for two layers only for now...

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
      return std::min((calculateDirectTrust(reqId, nodeId, time)),
                      (calculateDirectTrust(nodeId, provId, time)));
    }
  }

  return 0.1;
  // TODO: bunu buyuk oranda debug icin 0.1 koydum
}

bool IoTNode::enoughInteractions(int requestorId, int providerId) {
  /*if (blockchain.size() < windowSize) { // pencere bile dolmamis!
    EV << "window is not filled yet!\n";
    return false;
  }
  std::vector<Block> blocksInWindow(blockchain.end() - windowSize,
                                    blockchain.end());*/
    //bu kısımı değiştirdim eski halinde ile window size dolmamışsa eskiden hesaplama yapılmıyordu
    //ama o zamana kadar eklenmiş blokları kullanabiliriz bence çünkü window size çok büyük de olabilir diğer makalelerde 1000 yapmışlar
    std::vector<Block> blocksInWindow;
      if (blockchain.size() < windowSize) {
        EV << "Blockchain not full — using available " << blockchain.size() << " blocks.\n";
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
  if (encounterCounter > enoughEncounterLimit) {// bu kısıma bağlı aslında DT mi yoksa IT mi çalışacağı
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
 * WARN: bu su an kullanilmiyor!!
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
  // TODO: divide this method if you want. Too long in this format.
  if (!enoughInteractions(requestorId, providerId))
    return calculateIndirectTrust(requestorId, providerId, time);
  // so there are enough interactions, we calculate DT
  double dt = 0; // initialise DT
  double positiveRatings = 0;
  double all_ratings = 0;
  // bu ikisi decay'e tabii olacaklar

  // WARN: DT icin zincirin tamamina mi bakacagiz yoksa yalnizca pencereye
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
    // double decayFactor = calculateDecay(time, blockTime); //
    // WARN: su an decay yok
    //   dt += rating * decayFactor;
    double addendum = rating * decayFactor;
    // DT_ij icin j'nin TS'i onemli degil, gormezden geliniyor
    EV << "rating " << rating << '\n';
    if (rating >= 0) { // positive rating
      positiveRatings += addendum;
    } else {                         // negative rating;
      addendum *= (-1) * rancorCoef; // negatif oyun mutlak degeri
      // olumsuzsa kin katsayisiyla carp ki fazla tesir etsin
    }
    all_ratings += addendum;
  }
  EV << "positive ratings :" << positiveRatings
     << "\nall ratings: " << all_ratings << '\n';
  dt = positiveRatings / all_ratings;
  EV << "DT = " << dt << '\n';
  return dt;
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

    //Accuracy için
    if (getId() == 2) {//tek bir node içinde hesplamak için yazdım bu kısmı node id'leri 2'den başlıyor omnet'te
        std::vector<std::pair<double, bool>> trustAndLabel;

        for (IoTNode* node : allNodes) {
            trustAndLabel.emplace_back(node->trustScore, node->benevolent);
        }

        double bestF1 = 0.0;
        double bestThreshold = 0.5;  // default threshold
        double bestPrecision = 0.0, bestRecall = 0.0, bestAccuracy = 0.0;
        double threshold = 0.5;//!!

       // for (double threshold = 0.0; threshold <= 1.0; threshold += 0.01) {
            int TP = 0, TN = 0, FP = 0, FN = 0;

            for (const auto& [score, isBenevolent] : trustAndLabel) {
                bool predictedBenevolent = (score >= threshold);
                //bu önemli!!!!
                //positive= malicious bu testlerde çünkü amacımız kötüyü bulmak
                if (!isBenevolent && !predictedBenevolent) TP++; // kötü olana kötü demiş
                else if (isBenevolent && predictedBenevolent) TN++;// iyi olana iyi demiş
                else if (!isBenevolent && predictedBenevolent) FN++;// kötüye iyi demiş
                else if (isBenevolent && !predictedBenevolent) FP++;// iyiye kötü demiş
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
    }





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
