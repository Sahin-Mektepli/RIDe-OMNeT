/*
 * IoTNode.h
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm
 */

#ifndef __IOTNODE_H_
#define __IOTNODE_H_

#include "BlockchainMessage_m.h"
#include <map>
#include <omnetpp.h>
#include <vector>
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
  //--parameters--
  int windowSize = 3;           // 3 is just for testing purposes
  int enoughEncounterLimit = 2; // TODO: these two parameters are just examples
  double genTrustCoef = 0.01;
  std::map<int, int> routingTable;         // Maps Node ID → Gate Index
  std::map<int, std::string> serviceTable; // private olmalı gibi geldi

protected:
  virtual void initialize() override;
  virtual void handleMessage(omnetpp::cMessage *msg) override;
  void electClusterHeads();
  void processClusterHeadDuties();
  void sendTransactionToClusterHead(
      ServiceRating *transaction); // Fix this declaration
  int selectPoTValidator();
  void initiateServiceRequest();
  void handleServiceRequest(int requesterId);
  void sendRating(int providerId);

  bool extract(const std::string &input, double &rating, int &requesterId,
               int &providerId);
  double
  calculateDirectTrust(int requestor, int provider,
                       double time); // between a single i,j pair at time t
  double calculateIndirectTrust(int requestor, int provider,
                                double time); // if DT cannot be
                                              // calculated
  bool enoughInteractions(int requestorId, int provider);
  // can I calculate DT for i and j? (this = i)
  double
  calculateDecay(double currentTime,
                 double blockTime); // this can be changed for many reasons!
  void updateProviderGeneralTrust(IoTNode &provider, double requestorTrust,
                                  double rating);
  double getTrust(int nodeId); // returns the general trust of a node given its
                               // id, may be unnecessary

  double trustScore;
  bool isClusterHead;
  static std::vector<IoTNode *> allNodes;
  static int numClusterHeads;
  static int globalBlockId;
  static std::vector<Block> blockchain;
  std::string providedService; // Node'un verdiği servis türü

  cMessage *serviceRequestEvent; // Add this line
};

#endif
