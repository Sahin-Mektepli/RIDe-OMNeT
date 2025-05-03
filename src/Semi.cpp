
#include "Semi.h"
#include "BlockchainMessage_m.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <random>
#include <string>
#include <vector>

#include <omnetpp/distrib.h>

using namespace omnetpp;

std::vector<Block> Semi::blockchain;
std::vector<Semi *> Semi::allNodes;

int Semi::numClusterHeads = 3;
int Semi::globalBlockId = 0;
// gecici bir sey yaziyorum

void Semi::initialize() {
  serviceRequestEvent =
      new cMessage("serviceRequestTimer"); // Give a distinct name
  scheduleAt(simTime() + uniform(1, 5),
             serviceRequestEvent); // bunun indentationi neden bi garip ansdfk
  trustScore = uniform(
      50,
      100); // Initial random trust score
            // this can stay like this for the time being but
            // TODO: in the real sim, this should be decided by a "higher level"
  isClusterHead = false;
  allNodes.push_back(this);

  // Define possible service types
  std::vector<std::string> serviceTypes = {"A", "B", "C", "D"};

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

  scheduleAt(simTime() + 0.1, new cMessage("populateServiceTable"));
}

// ************************************************************
void Semi::populateServiceTable() {
  EV << "Node " << getId()
     << " is now filling its service table with direct neighbors..." << endl;
  for (int i = 0; i < gateSize("inoutGate"); i++) {
    cGate *outGate = gate("inoutGate$o", i);
    if (outGate->isConnected()) {
      cModule *connectedModule = outGate->getNextGate()->getOwnerModule();
      Semi *neighborNode = dynamic_cast<Semi *>(connectedModule);
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

void Semi::printServiceTable() {
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

void Semi::handleServiceRequestMsg(cMessage *msg) {
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
void Semi::handleServiceResponseMsg(cMessage *msg) {
  ServiceResponse *response = check_and_cast<ServiceResponse *>(msg);
  int responderId = response->getProviderId();
  std::string serviceType = response->getServiceType();

  if (requestedServiceType == serviceType) {
    double dt = calculateTotalTrust(responderId,
                                    simTime()); // burası farklı bizimkisinden

    respondedProviders[responderId] = dt;
    pendingResponses.erase(responderId);
    EV << "Received service response from Node " << responderId
       << " with DT = " << dt << endl;

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
void Semi::handleFinalServiceRequestMsg(cMessage *msg) {
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
  response->setServiceQuality(
      uniform(3.0, 5.0)); // burası şu anda random ama random kalmayacak
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
void Semi::handleFinalServiceResponseMsg(cMessage *msg) {
  FinalServiceResponse *response = check_and_cast<FinalServiceResponse *>(msg);
  int providerId = response->getProviderId();
  double quality = response->getServiceQuality();

  EV << "Node " << getId() << " received final service from " << providerId
     << " with quality: " << quality << endl;

  sendRating(providerId);

  delete response;
}
void Semi::handleServiceRatingMsg(cMessage *msg) {
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

void Semi::handleNetworkMessage(cMessage *msg) {
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

void Semi::handleSelfMessage(cMessage *msg) {
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

// assiri uzun bu metod; mumkunse kisalmali aslinda...
// bu fonksiyonu böldüm içindeki fonksiyonlar yukarıda yazıyor
// çoğu eski haliyle aynı sadece final service request ve response
// fonksiyonlarını ekledim
void Semi::handleMessage(cMessage *msg) {
  EV << "My id is: " << getId() << endl;

  if (msg->isSelfMessage()) {
    handleSelfMessage(msg);
  } else {
    handleNetworkMessage(msg);
  }
}

void Semi::electClusterHeads() {
  // sorts the allNodes array according to the nodes' trustScores
  // third param is a lambda func.
  std::sort(allNodes.begin(), allNodes.end(),
            [](Semi *a, Semi *b) { return a->trustScore > b->trustScore; });

  for (size_t i = 0; i < allNodes.size();
       i++) { // bunu da daha farklı yazabiliriz böyle biraz saçma oldu ama
              // doğru çalışıyor olmalı
    // en üstteki i node'u seçmek daha hızlı olur sanırım ama meh, ne fark
    // eder...
    allNodes[i]->isClusterHead = (i < numClusterHeads);
  }
  // EV << "Updated Cluster Head selection." << endl;
}
void Semi::initiateServiceRequest() {
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

void Semi::handleServiceRequest(int requesterId) {
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

void Semi::sendRating(int providerId) {
  double rating = uniform(1, 5); // Generate random rating
  EV << "IoT Node " << getId() << " gives a score of " << rating << " to Node "
     << providerId << endl;

  ServiceRating *transaction = new ServiceRating("serviceRating");
  transaction->setRequesterId(getId());
  transaction->setProviderId(providerId);
  transaction->setRating(rating);

  sendTransactionToClusterHead(transaction);
}

void Semi::sendTransactionToClusterHead(ServiceRating *transaction) {
  // Ensure cluster heads are up-to-date
  // do we have to do this in "each" transaction? Bunu sanki chat yazmis gibi :p
  electClusterHeads();

  std::vector<Semi *> clusterHeads;

  // Collect all available Cluster Heads
  for (Semi *node : allNodes) {
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
  // TODO: This should not be random. Each node must have one and only one CH to
  // which it sends.
  int randomIndex = intuniform(0, clusterHeads.size() - 1);
  Semi *bestClusterHead = clusterHeads[randomIndex];

  int clusterHeadId = bestClusterHead->getId();

  // Check if there's a known route to the selected Cluster Head
  if (routingTable.find(clusterHeadId) == routingTable.end()) {
    EV << "Error: No known route to Cluster Head " << clusterHeadId << endl;
    delete transaction; // Prevent memory leak
                        // this is not ideal.
                        // TODO: it should create a new route or sth if there is
                        // none!
    return;
  }

  int gateIndex = routingTable[clusterHeadId];

  EV << "IoT Node " << getId() << " forwarding transaction to Cluster Head "
     << clusterHeadId << " via gate index " << gateIndex << endl;

  // Send the transaction via the correct output gate
  send(transaction, "inoutGate$o", gateIndex);
}

// Trust kısmı buradan sonra öncesi IoTNode ile birebir
// aynı***********************************************************
double Semi::calculateDirectTrust(int targetId, simtime_t now) {
  const double decay = 0.8;
  auto it = directInteractions.find(targetId);
  if (it == directInteractions.end())
    return 0;

  double sum = 0, weight = 0;
  for (const auto &r : it->second) {
    double w = decay * exp(-0.05 * (now.dbl() - r.timestamp.dbl()));
    sum += w * r.serviceQuality;
    weight += w;
  }
  return weight > 0 ? sum / weight : 0;
}

double Semi::calculateTotalTrust(int targetId, simtime_t now) {
  double DT = calculateDirectTrust(targetId, now);
  double IT = indirectTrust[targetId];
  int n = interactionCount[targetId];
  double alpha = std::min(1.0, n / 10.0);
  double beta = 1.0 - alpha;
  return alpha * DT + beta * IT;
}

void Semi::updateRecommendationCredibility(int recommenderId, simtime_t now) {
  const auto &recs = recommendationHistory[recommenderId];
  double totalError = 0;
  int count = 0;
  for (const auto &[target, vec] : recs) {
    double dt = calculateDirectTrust(target, now);
    for (const auto &[val, ts] : vec) {
      totalError += fabs(val - dt);
      count++;
    }
  }
  recommendationCredibility[recommenderId] =
      count > 0 ? std::max(0.0, 1.0 - (totalError / count)) : 0.5;
}

void Semi::handleRecommendation(int recommenderId, int targetId, double value) {
  recommendationHistory[recommenderId][targetId].emplace_back(value, simTime());
  if (recommendationHistory[recommenderId][targetId].size() > 10)
    recommendationHistory[recommenderId][targetId].erase(
        recommendationHistory[recommenderId][targetId].begin());
  updateRecommendationCredibility(recommenderId, simTime());
  updateIndirectTrust(targetId);
}

void Semi::updateIndirectTrust(int targetId) {
  double total = 0, weight = 0;
  for (const auto &[recommender, map] : recommendationHistory) {
    auto it = map.find(targetId);
    if (it != map.end() && !it->second.empty()) {
      double latest = it->second.back().first;
      double cred = recommendationCredibility[recommender];
      total += cred * latest;
      weight += cred;
    }
  }
  indirectTrust[targetId] = weight > 0 ? total / weight : 0;
}
/****************************************Trust burada
bitiyor****************************************************************

/*this is to extract rating and id values from a transaction message in a block
 * give the message as input and the extracted values will be written in the
 * other parameters
 * the input must be in the following format: 'rating: <rating>
 * from <reqId> to <provId>' a disposition of the colon breaks the function
 */
bool Semi::extract(const std::string &input, double &rating, int &requesterId,
                   int &providerId) {
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
int Semi::selectPoTValidator() {
  double totalTrust = 0.0;
  std::vector<Semi *> clusterHeads;

  // Collect only Cluster Heads and sum their trust scores
  for (Semi *node : allNodes) {
    if (node->isClusterHead) {
      clusterHeads.push_back(node);
      totalTrust += node->trustScore;
    }
  }

  // Generate a weighted random selection hata verdiği için böyle yazmak zorunda
  // kaldım
  double randomValue = omnetpp::uniform(getEnvir()->getRNG(0), 0.0, totalTrust);

  double cumulativeTrust = 0.0;

  for (Semi *node : clusterHeads) {
    cumulativeTrust += node->trustScore;
    if (randomValue <= cumulativeTrust) {
      return node->getId(); // Return the selected validator
    }
  }

  return -1; // Should not happen, but included as a fallback
}
Define_Module(Semi);
