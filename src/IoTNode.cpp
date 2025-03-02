/*
 * IoTNode.cpp
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm
 */
#include "IoTNode.h"
#include "BlockchainMessage_m.h"
#include <string>
#include <random>
#include <vector>
#include <algorithm>
#include <map>

// Some notes:
// TODO is a nice tag to have, with which we can follow what is left "to do" :p
// We should have a ClusterHeads vector ready. Creating one each time we need is costly
using namespace omnetpp;

Define_Module(IoTNode);
//!!!!!
std::vector<IoTNode*> IoTNode::allNodes;
// TODO: why do we have an allNodes vector in the IoTNode class?
// if I am not mistaken this should be in the blockchain class or sth
int IoTNode::numClusterHeads = 3;
int IoTNode::globalBlockId = 0;
// this is because I want to be able to print the table for debugging. not really necessary
void printRoutingTable(const std::map<int, int>& routingTable){
    EV << "Routing Table:\n";
    EV << "NodeID --> Gate Index\n";
    for(const auto& entry: routingTable){
        EV << entry.first << "-->" << entry.second << "\n";
    }
}
void IoTNode::initialize() {
    serviceRequestEvent = new cMessage("serviceRequestTimer");  // Give a distinct name
       scheduleAt(simTime() + uniform(1, 5), serviceRequestEvent); //bunun indentationi neden bi garip ansdfk
    trustScore = uniform(50, 100); // Initial random trust score
				   // this can stay like this for the time being but
				   // TODO: in the real sim, this should be decided by a "higher level"
    isClusterHead = false;
    allNodes.push_back(this);

    // Populate Routing Table
       for (int i = 0; i < gateSize("inoutGate"); i++) {
           cGate* outGate = gate("inoutGate$o", i);
           if (outGate->isConnected()) {
               cModule* connectedModule = outGate->getNextGate()->getOwnerModule();
               if (connectedModule) {
                   int destNodeId = connectedModule->getId();
                   routingTable[destNodeId] = i;  // Store the gate index
                   EV << "Node " << getId() << " connected to Node " << destNodeId << " via gate index " << i << endl;
               }
           }
       }
    printRoutingTable(routingTable);
   }

void IoTNode::handleMessage(cMessage *msg) {

    if (msg->isSelfMessage()) {
            if (strcmp(msg->getName(), "serviceRequestTimer") == 0) {
                EV << "IoTNode " << getId() << " is initiating a service request." << endl;
                initiateServiceRequest();
                scheduleAt(simTime() + uniform(1, 5), msg);  // Reschedule next request
            }
            return;
        }
    EV << "Received message: " << msg->getName() << " (Type: " << msg->getClassName() << ")" << endl;

    if (strcmp(msg->getName(), "serviceRequest") == 0) {
        ServiceRequest *transaction = dynamic_cast<ServiceRequest *>(msg);
        if (!transaction) {
            EV << "Error: Message is not actually a ServiceRequest!" << endl;
            return;
        }
        handleServiceRequest(transaction->getRequesterId());

    } else if (strcmp(msg->getName(), "serviceResponse") == 0) {
        ServiceResponse *transaction = dynamic_cast<ServiceResponse *>(msg);
        if (!transaction) {
            EV << "Error: Message is not actually a ServiceResponse!" << endl;
            return;
        }
        EV << "IoT Node " << getId() << " received service from Node " << transaction->getProviderId() << endl;
        sendRating(transaction->getProviderId());

    } else if (strcmp(msg->getName(), "serviceRating") == 0) {
        ServiceRating *transaction = dynamic_cast<ServiceRating *>(msg);
        if (!transaction) {
            EV << "Error: Message is not actually a ServiceRating!" << endl;
            return;
        }

        if (isClusterHead) {
            EV << "Cluster Head Node " << getId() << " adding rating to blockchain." << endl;
            int blockId = ++globalBlockId;//burada validator için ekstra bir seçim yok çünkü zaten en yüksek trust skora sahipler cluster head
            Block newBlock = {
                .blockId = blockId,
                .validatorId = getId(),
                .transactionData = "Rating: " + std::to_string(transaction->getRating()) +
                                   " from " + std::to_string(transaction->getRequesterId()) +
                                   " to " + std::to_string(transaction->getProviderId()),
                .timestamp = simTime().dbl()
            };

            blockchain.push_back(newBlock);
            EV << "Block " << blockId << " added to blockchain." << endl;
        } else {
            EV << "IoT Node " << getId() << " forwarding rating to a Cluster Head" << endl;
            sendTransactionToClusterHead(transaction);
        }
    }

    delete msg;
}




void IoTNode::electClusterHeads() {
    //sorts the allNodes array according to the nodes' trustScores
    //third param is a lambda func.
    std::sort(allNodes.begin(), allNodes.end(), [](IoTNode* a, IoTNode* b) {
        return a->trustScore > b->trustScore;
    });

    for (size_t i = 0; i < allNodes.size(); i++) {//bunu da daha farklı yazabiliriz böyle biraz saçma oldu ama doğru çalışıyor olmalı
						  //en üstteki i node'u seçmek daha hızlı olur sanırım ama meh, ne fark eder...
        allNodes[i]->isClusterHead = (i < numClusterHeads);
    }
    EV << "Updated Cluster Head selection." << endl;
}
int findRoute(int requesterId, int providerId){
    //finds a route from requester to provider if they are not in the same cluster
    EV << "Error: No known route to Node " << providerId << endl;
    return 0;
}
void IoTNode::initiateServiceRequest() {
    IoTNode* provider = nullptr;
    int attempts = 0;
    int randomIndex=0;
    while ((!provider || provider == this) && attempts < 10) {
       randomIndex = intuniform(0, allNodes.size() - 1);  // Select random node
        provider = allNodes[randomIndex];  // Get random provider
        attempts++;
    }

    if (!provider || provider == this) {
        EV << "IoT Node " << getId() << " could not find a valid provider!" << endl;
        return;
    }

    int providerId = provider->getId();

    int gateIndex;
      // Check if there is a recorded gate for this provider
      if (routingTable.find(providerId) == routingTable.end()) {
        //if not in the table. find a route
        gateIndex = findRoute(getId(), providerId); //I think this should work like this, no?
          return;
      }
    //TODO: this should change in accordance to the findRoute() func used above
    //findRoute() should return the next node, which is not the provider node but one of the "bridge" nodes to that node
      gateIndex = routingTable[providerId];

      EV << "IoT Node " << getId() << " is sending service request to Node " << providerId
         << " via gate index " << gateIndex << endl;

      ServiceRequest *serviceRequest = new ServiceRequest("serviceRequest");
      serviceRequest->setRequesterId(getId());
      serviceRequest->setProviderId(providerId);

      // Send through the correct gate
      send(serviceRequest, "inoutGate$o", gateIndex);
  }





void IoTNode::handleServiceRequest(int requesterId) {
    EV << "IoT Node " << getId() << " is providing service to Node " << requesterId << endl;

    ServiceResponse *serviceResponse = new ServiceResponse("serviceResponse");
    serviceResponse->setRequesterId(requesterId);
    serviceResponse->setProviderId(getId());

    if (routingTable.find(requesterId) != routingTable.end()) {
           int gateIndex = routingTable[requesterId];
           send(serviceResponse, "inoutGate$o", gateIndex);
       } else {
           EV << "Error: No route to send service response to Node " << requesterId << endl;
           delete serviceResponse;
       }
   }

void IoTNode::sendRating(int providerId) {
    double rating = uniform(1, 5); // Generate random rating
    EV << "IoT Node " << getId() << " gives a score of " << rating << " to Node " << providerId << endl;

    ServiceRating *transaction = new ServiceRating("serviceRating");
    transaction->setRequesterId(getId());
    transaction->setProviderId(providerId);
    transaction->setRating(rating);

    sendTransactionToClusterHead(transaction);
}

void IoTNode::sendTransactionToClusterHead(ServiceRating* transaction) {
    // Ensure cluster heads are up-to-date
    // do we have to do this in "each" transaction? Bunu sanki chat yazmis gibi :p
    electClusterHeads();

    std::vector<IoTNode*> clusterHeads;

    // Collect all available Cluster Heads
    for (IoTNode* node : allNodes) {
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
    // TODO: This should not be random. Each node must have one and only one CH to which it sends.
    int randomIndex = intuniform(0, clusterHeads.size() - 1);
    IoTNode* bestClusterHead = clusterHeads[randomIndex];

    int clusterHeadId = bestClusterHead->getId();

    // Check if there's a known route to the selected Cluster Head
    if (routingTable.find(clusterHeadId) == routingTable.end()) {
        EV << "Error: No known route to Cluster Head " << clusterHeadId << endl;
        delete transaction; // Prevent memory leak
			    // this is not ideal.
			    // TODO: it should create a new route or sth if there is none!
        return;
    }

    int gateIndex = routingTable[clusterHeadId];

    EV << "IoT Node " << getId() << " forwarding transaction to Cluster Head " << clusterHeadId
       << " via gate index " << gateIndex << endl;

    // Send the transaction via the correct output gate
    send(transaction, "inoutGate$o", gateIndex);
}

//bunu kullanmıyorum şu anda
int IoTNode::selectPoTValidator() {
    double totalTrust = 0.0;
    std::vector<IoTNode*> clusterHeads;

    // Collect only Cluster Heads and sum their trust scores
    for (IoTNode* node : allNodes) {
        if (node->isClusterHead) {
            clusterHeads.push_back(node);
            totalTrust += node->trustScore;
        }
    }

    // Generate a weighted random selection
    double randomValue = uniform(0, totalTrust);
    double cumulativeTrust = 0.0;

    for (IoTNode* node : clusterHeads) {
        cumulativeTrust += node->trustScore;
        if (randomValue <= cumulativeTrust) {
            return node->getId();  // Return the selected validator
        }
    }

    return -1;  // Should not happen, but included as a fallback
}


