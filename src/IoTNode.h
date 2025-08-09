/*
 * IoTNode.h
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm, sahinm
 */

#ifndef __IOTNODE_H_
#define __IOTNODE_H_

#include "BlockchainMessage_m.h"
#include <map>
#include <omnetpp.h>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>
// Add this at the top of IoTNode.h
using namespace omnetpp;

struct Block {
  int blockId;
  int validatorId;
  std::string transactionData;
  double timestamp;
};

//static const double epsilon = 0.2; //choose random provider with an epsilon chance



class IoTNode : public omnetpp::cSimpleModule {
private:
    double epsilon;//0.2
      double minEpsilon;//0.1
      double epsilonDecay;
  //*******Yeni model için eklediklerimiz*****************
  std::map<int, double> localTrustScores; // Node ID → Trust Score (diğer
                                          // nodeların trust skorları)
  std::map<int, std::vector<double>>
      localRatingHistory; // Node ID → Ratings given by this node !! vector
                          // içinde tutmak yerine ortalamsı tutulabilir, ya da
                          // hepsini tutmak yerine window olabilir, decay de
                          // eklenebilir o yüzdeb şimdilik böyle

  /** A helper struct to store positive and all ratings of this node to some
   * other node; This is used to calculate the similarity of a rating to us.
   *
   *Average of ratings, which are in (-10,10), so its value is in that range*/
  struct myRatings {
    double sumRatings = 0;
    int count = 0;

    double value() {
      if (count > 0)
        return sumRatings / count;
      else
        return 0.0;
    }
  };

  // A helper structure to store the TS to a node.
  // This is the trust TO some node of THIS node
  struct trustScore {
    double sumOfPositiveRatings, sumOfAllRatings = 0;
    double value() {
      if (sumOfPositiveRatings > sumOfAllRatings)
        return NAN; // SHOULD NOT BE SET TO 1 AND BE HANDLED!
      else if (sumOfAllRatings != 0)
        return sumOfPositiveRatings / sumOfAllRatings;
      else
        return 0.0;
      // TODO THIS IS AN ARBITRARY DEFAULT TRUST VALUE
      // CALL ANOTHER FUNCTION TO CALCULATE WITH NEIGHBOURS
    }
  };

  // NodeID --> Trust Score struct
  // real trust score to the nodes of THIS node
  std::unordered_map<int, trustScore> trustMap;
  /**NodeID --> Avg of past ratings
   * Used to calculate similarity of a rating.*/
  std::unordered_map<int, myRatings> myRatingMap;

  // Update trustScore to a node with the given rating and its effect coef
  double updateTrustScore(int providerId, double rating, double alpha);
  double updateMyRating(int providerId, double rating);


  double calculateRatingSimilarityCoefficient(int providerId, double newRating);
  void setPotencyAndConsistency();
  //******************************************************

  static int totalBadServicesReceived; // for logging
  static int totalBenevolentNodes;     // for logging
  static std::set<int> maliciousNodeIds;

  void recordLocalTrust();
  void recordAbility();

  int lastProviderId = -1; // collaborative attack için

  int badServicesReceived = 0; // for logging
  cMessage *badServiceLogger = nullptr;
  //--parameters--
  int windowSize = 100;         // just for testing purposes90 idi bu
  int enoughEncounterLimit = 1; // TODO: these two parameters are just examples
  double genTrustCoef = 0.01;
  double rancorCoef = 2.0;         // defined to be higher than 1
  double decayFactor = 0.5;        // WARN: bunu 1'de unutmak, decay yok demek!
  std::map<int, int> routingTable; // Maps Node ID → Gate Index
  std::map<std::string, std::vector<int>> serviceTable;
  std::set<int> pendingResponses;
  std::map<int, double> respondedProviders;
  std::string requestedServiceType;
  //--rating calculation--
  double calculateRatingBenevolent(double quality, double timeliness,
                                   double rarity);
  double wQ = 1; // weight of quality
  double wR = 0;  // weight of rarity
  double wT = 0;  // weight of timeliness

  // -- TS coefficients -- (provider secerken kullanilan TS=a*dt + b*gt)
  double a = 1;
  double b = 1;
  //-- attackers --
  enum AttackerType {
    BENEVOLENT, //0
    CAMOUFLAGE,//1
    BAD_MOUTHING,//2
    MALICIOUS_100,//3
    COLLABORATIVE,//4
    OPPORTUNISTIC//5

  }; // use this and switch statements to control
  enum AttackerType attackerType = BENEVOLENT; // default
  double calculateMalRating(enum AttackerType);
  double calculateRatingCamouflage(double quality, double timeliness,
                                   double rarity);

protected:
  void updateEpsilon();
  auto epsilonGreedyMaxPair(const std::map<int, double> &mapping);
  virtual void finish() override;
  virtual void initialize() override;
    // helper for initalize
    void setMalicious(AttackerType type);
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

  //  helper function to have the handlers modular
  void propagateRatingReplica(ServiceRating *transaction);
  int addBlockToBC(double rating, int requesterId, int providerId);

  // Setup
  void populateServiceTable();
  void electClusterHeads();
  void processClusterHeadDuties();
  void sendTransactionToClusterHead(
      ServiceRating *transaction); // Fix this declaration
  // WARN: what is the problem to be fixed here?
  int selectPoTValidator();
  void initiateServiceRequest();
  void handleServiceRequest(int requesterId);
  void sendRating(int providerId, double rating);
  void printServiceTable();

  bool extract(const std::string &input, double &rating, int &requesterId,
               int &providerId);
  bool enoughInteractions(int requestorId, int provider);
  // can I calculate DT for i and j? (this = i)
  double
  calculateDecay(double currentTime,
                 double blockTime); // this can be changed for many reasons!
  double getTrust(int nodeId); // returns the general trust of a node given its
                               // id, may be unnecessary

  IoTNode *getNodeById(int nodeId);
  // Node'a mahsus attribute'ler
  [[deprecated("NO GENERAL TRUST SCORES!!")]]
  double trustScore = 0.5;
  bool isClusterHead;
  double potency = 0;
  double consistency = 4; // draws a 'meaningfull' default curve
  bool benevolent = true;
  std::string providedService; // Node'un verdiği servis türü

  void populateRoutingTable();

  // -- attack parameters --

  /* change this rate depending on how much camouflage you want the nodes to
   * perform 1 means it never acts malicously and 0 is always malicious
   */
  double camouflageRate = 0.3;
  // -- deciding on the rating of a service --
  double calculateRating(double quality, double timeliness, double rarity);

  // -- sending service, and determining the quality of it --
  double calcQuality(double potency, double consistency);
  double calcQualityBenevolent(double potency, double consistency);
  double calcQualityCamouflage(double potency, double consistency);
  static std::vector<IoTNode *> allNodes;
  static int numClusterHeads;
  static int globalBlockId;
  static std::vector<Block> blockchain;

  cMessage *serviceRequestEvent; // Add this line

  // oppurtunistic attack: trust skoru en yüksek olan iyi node belirli bir
  // zamanadan(opportunisticAttackTime) sonra kötü dvaranmaya başlarsa ne olur
  // onu test ediyoruz
  double opportunisticAttackTime = 500;
  bool opportunisticAttackTriggered = false;
  IoTNode *opportunisticNode = nullptr;
  bool isOpportunisticNode = false;
  static int opportunisticNodeId;
  cMessage *opportunisticTriggerMsg = nullptr;
};

#endif
