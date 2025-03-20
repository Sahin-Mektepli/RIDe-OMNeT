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
  int windowSize = 3; //3 is just for testing purposes
  std::map<int, int> routingTable;         // Maps Node ID → Gate Index
  std::map<std::string, std::vector<int>> serviceTable; // Store service type with node IDs


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
  void printServiceTable(); // Prints the service table

  bool extract(const std::string &input, double &rating, int &requesterId,
          int &providerId);
  double calculateDirectTrust(IoTNode& provider, double time); //between a single i,j pair at time t
  double calculateIndirectTrust(IoTNode& provider); //if DT cannot be calculated
  bool enoughInteractions(IoTNode& provider); //can I calculate DT for i and j? (this = i)
  double calculateDecay(double currentTime, double blockTime); //this can be changed for many reasons!

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
