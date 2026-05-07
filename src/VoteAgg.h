#ifndef VOTE_AGG_H_
#define VOTE_AGG_H_

#include <omnetpp.h>
#include "BlockchainMessage_m.h"

#include <map>
#include <set>
#include <string>
#include <vector>

using namespace omnetpp;

struct Block {
  int blockId = 0;
  int validatorId = 0;
  std::string transactionData;
  double timestamp = 0.0;
};

struct trustScore {
  double sumOfPositiveRatings = 0.0;
  double sumOfAllRatings = 0.0;

  double value() const {
    if (sumOfAllRatings <= 0.0)
      return 0.5;  // neutral default until enough ratings exist
    return sumOfPositiveRatings / sumOfAllRatings;
  }
};

struct myRatings {//now we do not need this because ı store DT in trustmap and trustScore
  double sumRatings = 0.0;
  int count = 0;

  double value() const {
    if (count <= 0)
      return 0.0;
    return sumRatings / count;
  }
};

enum AttackerType {
  BENEVOLENT = 0,
  MALICIOUS_100 = 1,
  CAMOUFLAGE = 2,
  BAD_MOUTHING = 3,
  OPPORTUNISTIC = 4,
  COLLABORATIVE = 5,
  HYBRID = 6
};

class VoteAgg : public cSimpleModule {
public:
  enum AggregationMethod {
    AGG_ADDITIVE = 0,
    AGG_MULTIPLICATIVE = 1,
    AGG_BORDA = 2
  };

  using DirectTrustMatrix = std::map<int, std::map<int, double>>;

  static std::vector<Block> blockchain;
  static std::vector<VoteAgg *> allNodes;
  static int numClusterHeads;
  static int globalBlockId;
  static std::set<int> maliciousNodeIds;
  static int totalBadServicesReceived;
  static int totalBenevolentNodes;
  static int opportunisticNodeId;
  static int totalServicesReceived;
  static std::map<int, double> globalTrustScores;
  static std::vector<int> globalTrustRanking;

protected:
  std::map<int, int> routingTable;
  std::map<std::string, std::vector<int>> serviceTable;
  std::map<int, trustScore> trustMap;
  std::map<int, myRatings> myRatingMap;
  std::map<int, double> respondedProviders;
  std::set<int> pendingResponses;

  bool isClusterHead = false;
  bool benevolent = true;
  bool isOpportunisticNode = false;
  bool opportunisticAttackTriggered = false;
  bool hybridHasSwitched = false;

  AttackerType attackerType = BENEVOLENT;
  AggregationMethod aggregationMethod = AGG_ADDITIVE;

  double epsilon = 0.0;//epsilon greedy değil şu anda!!!
  double minEpsilon = 0.01;
  double epsilonDecay = 0.90;
  double camouflageRate = 0.0;
  double potency = 0.0;
  double consistency = 1.0;
  double opportunisticAttackTime = 50.0;
  double hybridSwitchTime = 50.0;
  double globalTrustUpdateInterval = 10.0;

  double wQ = 1.0;
  double wR = 1.0;
  double wT = 1.0;

  int badServicesReceived = 0;
  int lastProviderId = -1;
  int windowSize = 10;
  int enoughEncounterLimit = 3;

  double trustScore = 0.5;

  std::string requestedServiceType;
  std::string providedService;

  cMessage *serviceRequestEvent = nullptr;
  cMessage *badServiceLogger = nullptr;
  cMessage *opportunisticTriggerMsg = nullptr;
  cMessage *globalTrustUpdateEvent = nullptr;

protected:
  virtual void initialize() override;
  virtual void handleMessage(cMessage *msg) override;
  virtual void finish() override;

  void populateRoutingTable();
  void updateEpsilon();
  bool noMalDominatedClusters();
  void setMalicious(AttackerType type);
  void setPotencyAndConsistency();
  void populateServiceTable();
  double calculateRatingSimilarityCoefficient(int providerId, double newRating);
  void printServiceTable();
  void handleServiceRequestMsg(cMessage *msg);
  static VoteAgg *getNodeById(int targetId);
  std::map<int, double>::const_iterator epsilonGreedyMaxPair(const std::map<int, double> &mapping);
  void handleServiceResponseMsg(cMessage *msg);
  void handleFinalServiceRequestMsg(cMessage *msg);
  double updateMyRating(int providerId, double rating);
  void handleFinalServiceResponseMsg(cMessage *msg);
  void propagateRatingReplica(ServiceRating *transaction);
  int addBlockToBC(double rating, int requesterId, int providerId);
  double updateTrustScore(int providerId, double rating, double alpha);
  void handleServiceRatingMsg(cMessage *msg);
  void handleNetworkMessage(cMessage *msg);
  void handleSelfMessage(cMessage *msg);
  void electClusterHeads();
  void initiateServiceRequest();
  void handleServiceRequest(int requesterId);
  double calculateRatingCamouflage(double quality, double timeliness, double rarity);
  double calcQualityCamouflage(double potency, double consistency);
  double calculateRatingBadMouthing(double quality, double timeliness, double rarity);
  double calculateRating(double quality, double timeliness, double rarity);
  double calcQuality(double potency, double consistency);
  double calcQualityBenevolent(double potency, double consistency);
  double calculateRatingBenevolent(double quality, double timeliness = 10, double rarity = 10);
  void sendRating(int providerId, double rating);
  void sendTransactionToClusterHead(ServiceRating *transaction);
  bool enoughInteractions(int requestorId, int providerId);
  double calculateDecay(double currentTime, double blockTime);
  bool extract(const std::string &input, double &rating, int &requesterId, int &providerId);
  void recordLocalTrust();
  void recordAbility();

  AggregationMethod parseAggregationMethod(const char *methodName);
  static std::vector<int> sortNodesByScore(const std::map<int, double> &scores);
  static DirectTrustMatrix buildDirectTrustMatrix();
  static void updateGlobalTrustList();
  static void updateGlobalTrustAdditive();
  static void updateGlobalTrustMultiplicative();
  static void updateGlobalTrustBorda();
  static int getRankPointFromGlobalOrdering(int nodeId, const std::vector<int> &candidates);
  double mergeTrustScore(int candidateId);
};

#endif  // VOTE_AGG_H_
