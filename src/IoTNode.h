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
    //*******Yeni model için eklediklerim*****************
    std::map<int, double> localTrustScores;                //Node ID → Trust Score (diğer nodeların trust skorları)
    std::map<int, std::vector<double>> localRatingHistory; // Node ID → Ratings given by this node !! vector içinde tutmak yerine ortalamsı tutulabilir, ya da hepsini tutmak yerine window olabilir, decay de eklenebilir o yüzdeb şimdilik böyle

    double calculateRatingSimilarityCoefficient(int providerId, double newRating);
    //******************************************************


  static int totalBadServicesReceived;
  static int totalBenevolentNodes;
  static std::set<int> maliciousNodeIds;


  int lastProviderId = -1;//collaborative attack için

  int badServicesReceived = 0;
  cMessage *badServiceLogger = nullptr;
  //--parameters--
  int windowSize = 100;          // just for testing purposes90 idi bu
  int enoughEncounterLimit = 1; // TODO: these two parameters are just examples
  double genTrustCoef = 0.01;
  double rancorCoef = 2.0;           // defined to be higher than 1
  double decayFactor = 0.5;          // WARN: bunu 1'de unutmak, decay yok demek!
  std::map<int, int> routingTable; // Maps Node ID → Gate Index
  //  std::map<int, std::string> serviceTable; // private olmalı gibi geldi
  //  TODO: bu eski hali sil
  std::map<std::string, std::vector<int>> serviceTable;
  std::set<int> pendingResponses;
  std::map<int, double> respondedProviders;
  std::string requestedServiceType;
  //--rating calculation--
  double calculateRarity(std::string serviceType); // about how many nodes
                                                   // can provide that service
  double calculateRatingBenevolent(double quality, double timeliness,
                                   double rarity);
  double wQ = 10; // weight of quality
  double wR = 1;  // weight of rarity
  double wT = 0;  // weight of timeliness

  // -- TS coefficients -- (provider secerken kullanilan TS=a*dt + b*gt)
  double a = 1;
  double b = 1;
  //-- attackers --
  enum AttackerType {
    BENEVOLENT, // bunu eklemek sacma olabilir ama bulunsun
    CAMOUFLAGE,
    BAD_MOUTHING,
    MALICIOUS_100,
    COLLABORATIVE

  }; // use this and switch statements to control
  enum AttackerType attackerType = BENEVOLENT; // default
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


  //  helper function to have the handlers modular
   void propagateRatingReplica(ServiceRating *transaction);

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
  double
  calculateDirectTrust(int requestorId, int providerId,
                       double time ); // between a single i,j pair at time t
  double calculateIndirectTrust(int requestor, int provider,
                                double time); // if DT cannot be
                                              // calculated
  bool enoughInteractions(int requestorId, int provider);
  // can I calculate DT for i and j? (this = i)
  double
  calculateDecay(double currentTime,
                 double blockTime); // this can be changed for many reasons!
  void updateProviderGeneralTrust(IoTNode *provider, double requestorTrust,
                                  double rating);
  double getTrust(int nodeId); // returns the general trust of a node given its
                               // id, may be unnecessary

  IoTNode *getNodeById(int nodeId);
  // Node'a mahsus attribute'ler
  double trustScore;
  double sumOfPositveRatings = 5;
  double sumOfAllRatings = 10; // this takes abs of negative ratings.
  bool isClusterHead;
  double potency = 0;
  double consistency = 4;      // draws a 'meaningfull' default curve
  bool benevolent = true;      // WARN: perhaps this is not ideal...
  std::string providedService; // Node'un verdiği servis türü
  // WARN: bu vector'e falan cevirilebilir!

  // -- attack parameters --

  /* change this rate depending on how much camouflage you want the nodes to
   * perform 1 means it never acts malicously and 0 is always malicious
   */
  double camouflageRate = 0.0;
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

  //oppurtunistic attack: trust skoru en yüksek olan iyi node belirli bir zamanadan(opportunisticAttackTime) sonra kötü dvaranmaya başlarsa ne olur onu test ediyoruz
    double opportunisticAttackTime = 600;
    bool opportunisticAttackTriggered = false;
    IoTNode* opportunisticNode = nullptr;

};

#endif
