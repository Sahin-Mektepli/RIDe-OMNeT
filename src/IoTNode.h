/*
 * IoTNode.h
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm,sahinm
 */

#ifndef __IOTNODE_H_
#define __IOTNODE_H_

#include "BlockchainMessage_m.h"
#include <map>
#include <omnetpp.h>
#include <set>
#include <vector>
using namespace omnetpp;

struct Block {
  int blockId;
  int validatorId;
  std::string transactionData;
  double timestamp;
};

class IoTNode : public omnetpp::cSimpleModule {
private:
  bool banished = false;
  static int totalBadServicesReceived;
  static int totalBenevolentNodes;
  static std::set<int> maliciousNodeIds;

  int badServicesReceived = 0;
  cMessage *badServiceLogger = nullptr;
  //--parameters--
  int windowSize = 50;
  int enoughEncounterLimit = 1; // TODO: these two parameters are just examples
  double genTrustCoef = 0.01;
  double rancorCoef = 2.0; // defined to be higher than 1
  double decayFactor = 0.9;
  std::map<int, int> routingTable; // Maps Node ID → Gate Index
  std::map<std::string, std::vector<int>> serviceTable;
  std::set<int> pendingResponses;
  std::map<int, double> respondedProviders;
  std::string requestedServiceType;
  //--rating calculation--

  // about how many nodes can provide that service
  double calculateRarity(std::string serviceType);
  double calculateRatingBenevolent(double quality, double timeliness,
                                   double rarity);
  double wQ = 1;   // weight of quality
  double wR = 0.3; // weight of rarity
  double wT = 0;   // weight of timeliness FIXME this should be non-zero

  // -- TS coefficients -- (provider secerken kullanilan TS=a*dt + b*gt)
  double a = 0.2;
  double b = 0.8;
  //-- attackers --
  enum AttackerType {
    BENEVOLENT, // bunu eklemek sacma olabilir ama bulunsun
    CAMOUFLAGE,
    BAD_MOUTHING,
    MALICIOUS_100,
    MALFUNCTION
  }; // use this and switch statements to control
  enum AttackerType attackerType = BENEVOLENT; // default value is BENEVOLENT
  double calculateMalRating(enum AttackerType);
  double calculateRatingCamouflage(double quality, double timeliness,
                                   double rarity);

protected:
  virtual void finish() override;
  virtual void initialize() override;
  virtual void handleMessage(omnetpp::cMessage *msg) override;
  // --- Message Handling ---

  void handleSelfMessage(cMessage *msg);

  void handleNetworkMessage(cMessage *msg);

  // Service-level handlers

  void handleServiceRequestMsg(cMessage *msg);

  void handleServiceResponseMsg(cMessage *msg);

  void handleFinalServiceRequestMsg(cMessage *msg);

  void handleFinalServiceResponseMsg(cMessage *msg);

  void handleServiceRatingMsg(cMessage *msg);

  // Setup
  void populateServiceTable();
  void electClusterHeads();
  void processClusterHeadDuties();
  void sendTransactionToClusterHead(ServiceRating *transaction);
  int selectPoTValidator();
  void initiateServiceRequest();
  void handleServiceRequest(int requesterId);
  void sendRating(int providerId, double rating);
  void printServiceTable();

  bool extract(const std::string &input, double &rating, int &requesterId,
               int &providerId);
  // DT between a single i,j pair at time t
  double calculateDirectTrust(int requestorId, int providerId, double time,
                              int depth);

  // if DT cannot be calculated
  double calculateIndirectTrust(int requestor, int provider, double time,
                                int depth);
  // can I calculate DT for i and j? (this = i)
  bool enoughInteractions(int requestorId, int provider);
  // this can be changed for many reasons!
  double calculateDecay(double currentTime, double blockTime);
  void updateProviderGeneralTrust(IoTNode *provider, double requestorTrust,
                                  double rating);
// returns the general trust of a node given its id, may be unnecessary
  double getTrust(int nodeId);


  IoTNode *getNodeById(int nodeId);
  // Node'a mahsus attribute'ler
  double trustScore;
  double sumOfPositveRatings = 5;
  double sumOfAllRatings = 10; // this takes abs of negative ratings.
  bool isClusterHead;
  double potency = 0;
  double consistency = 4;      // draws a 'meaningfull' default curve
  bool benevolent = true;      // XXX perhaps this is not ideal...
  std::string providedService; // DEPRECATED Node'un verdiği servis türü

  // -- attack parameters --

  /* change this rate depending on how much camouflage you want the nodes to
   * perform 1 means it never acts malicously and 0 is always malicious
   */
  double camouflageRate = 0.3;
  // -- deciding on the rating of a service --
  double calculateRating(double quality, double timeliness, double rarity);

  // -- sending service, and determining the quality of it --
  bool givesService(std::string serviceType);
  double calcQuality(double potency, double consistency);
  double calcQualityBenevolent(double potency, double consistency);
  double calcQualityCamouflage(double potency, double consistency);
  static std::vector<IoTNode *> allNodes;
  static int numClusterHeads;
  static int globalBlockId;
  static std::vector<Block> blockchain;

  cMessage *serviceRequestEvent; // Add this line
};

#endif
