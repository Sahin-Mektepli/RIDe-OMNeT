/*
 * semi.h
 *
 *  Created on: 30 Mar 2025
 *      Author: ipekm
 */

#ifndef SEMI_H_
#define SEMI_H_

#include <omnetpp.h>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include "BlockchainMessage_m.h"

using namespace omnetpp;

struct Block {
    int blockId;
    int validatorId;
    std::string transactionData;
    double timestamp;
};

struct InteractionRecord {
    double serviceQuality;
    simtime_t timestamp;
};

class Semi : public cSimpleModule {
  public:
    Semi() {}

    static std::vector<Block> blockchain;
    static std::vector<Semi *> allNodes;
    static int numClusterHeads;
    static int globalBlockId;

    std::map<std::string, std::vector<int>> serviceTable;
    std::map<int, int> routingTable;
    std::string providedService;
    std::string requestedServiceType;
    std::set<int> pendingResponses;
    std::map<int, double> respondedProviders;

    bool isClusterHead;
    double trustScore;
    cMessage *serviceRequestEvent;

    std::map<int, std::vector<InteractionRecord>> directInteractions;
    std::map<int, std::map<int, std::vector<std::pair<double, simtime_t>>>> recommendationHistory;
    std::map<int, double> recommendationCredibility;
    std::map<int, double> indirectTrust;
    std::map<int, int> interactionCount;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    void handleSelfMessage(cMessage *msg);
    void handleNetworkMessage(cMessage *msg);

    void handleServiceRequestMsg(cMessage *msg);
    void handleServiceResponseMsg(cMessage *msg);
    void handleFinalServiceRequestMsg(cMessage *msg);
    void handleFinalServiceResponseMsg(cMessage *msg);
    void handleServiceRatingMsg(cMessage *msg);

    void populateServiceTable();
    void printServiceTable();
    void initiateServiceRequest();
    void sendRating(int providerId);
    void sendTransactionToClusterHead(ServiceRating *transaction);
    void handleServiceRequest(int requesterId);

    static void electClusterHeads();
    static int selectPoTValidator();

    double calculateDirectTrust(int targetId, simtime_t now);
    double calculateTotalTrust(int targetId, simtime_t now);
    void handleRecommendation(int recommenderId, int targetId, double value);
    void updateRecommendationCredibility(int recommenderId, simtime_t now);
    void updateIndirectTrust(int targetId);

    bool extract(const std::string &input, double &rating, int &requesterId, int &providerId);
};

#endif /* SEMI_H_ */
