/*
 * IoTNode.h
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm
 */

#ifndef __IOTNODE_H_
#define __IOTNODE_H_

#include <omnetpp.h>
#include "BlockchainMessage_m.h"
#include <vector>
#include <map>
// Add this at the top of IoTNode.h
using namespace omnetpp;


struct Block {
    int blockId;
    int validatorId;
    std::string transactionData;
    double timestamp;
};

class IoTNode : public omnetpp::cSimpleModule {
private:
    std::map<int, int> routingTable; // Maps Node ID → Gate Index
    std::map<int, std::string> serviceTable; //private olmalı gibi geldi

protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    void electClusterHeads();
    void processClusterHeadDuties();
    void sendTransactionToClusterHead(ServiceRating* transaction);  // Fix this declaration
    int selectPoTValidator();
    void initiateServiceRequest();
    void handleServiceRequest(int requesterId);
    void sendRating(int providerId);

    double trustScore;
    bool isClusterHead;
    static std::vector<IoTNode*> allNodes;
    static int numClusterHeads;
    static int globalBlockId;
    std::vector<Block> blockchain;
    std::string providedService;  // Node'un verdiği servis türü

    cMessage *serviceRequestEvent;  // Add this line
};

#endif
