#ifndef __IOTNODE_H_
#define __IOTNODE_H_

#include "BlockchainMessage_m.h"
#include <random>

using namespace omnetpp;

struct Block {
  int blockId;
  int validatorId;
  std::string transactionData;
  double timestamp;
};
/**This is to extract rating and id values from a transaction message in a
 * block. Give the message as input and the extracted values will be written
 * in the other parameters the input must be in the following format:
 * 'rating: <rating> from <reqId> to <provId>' a disposition of the colon
 * breaks the function
 */
inline bool extract(const std::string &input, double &rating,
                      int &requesterId, int &providerId) {
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

inline std::default_random_engine genF(std::random_device{}());
inline std::uniform_real_distribution<double> uniform_real_distF{0, 1};
inline double uniform_real_in_range(double lower, double upper){
  return uniform_real_distF(genF)*(upper - lower) + lower;
}
class ForestFire : public omnetpp::cSimpleModule {
private:
    cMessage *badServiceLogger = nullptr;
    omnetpp::cMessage* ffTick = nullptr;
    double trustScore = uniform_real_in_range(0.5,1);
    bool isClusterHead = false;
    double potency; //How good the services generally are
    double consistency; //How consistant the quality is
    static int totalBadServicesReceived; // for logging
     static int totalBenevolentNodes;     // for logging
     static std::set<int> maliciousNodeIds;
     static int totalServicesReceived;
     static int opportunisticNodeId;
    enum AttackerType {
        BENEVOLENT, //0
        CAMOUFLAGE,//1
        BAD_MOUTHING,//2
        MALICIOUS_100,//3
        COLLABORATIVE,//4
        OPPORTUNISTIC//5
    }; // use this and switch statements to control
    void setMalicious(AttackerType type);
    int groupNo;
    double trustThreshold = 0.3;   // .ini'den okunur hale getirebiliriz bunları
    double rho = 0.9;              // .ini'den okunur
    double lambdaScale = 1.0;      // .ini'den okunur
    int compCount = 5;             // .ini'den okunur
protected:
    std::map<int,double> activityRecent; // Act_recent
    std::map<int,double> recentTrust;     // T_recent[targetId]  (yoksa 0.51)
    simtime_t joinTime;
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override;
    static std::vector<ForestFire *> allNodes;
    void setPotencyAndConsistency();
    std::map<int, int> routingTable; // Maps Node ID → Gate Index
    void populateRoutingTable();
    auto handleCoreAndComp();
    int coreNode = -1; //default value
    std::vector<int> compNodes;

    void setCoreNode(int id);
    void setCompNodes(std::vector<int> ids);
    static std::vector<int> allNodeIds;
    ForestFire* getNodeById(int);
    bool banned = false; //a flag to effectively expell the nodes from the system
     double actAlpha      = 0.2;   //  (yakın geçmişe ağırlık)

      // --- Davranış parametreleri
      AttackerType attackerType = BENEVOLENT;
      double pGood = 0.98;           // iyi düğüm servis başarı olasılığı
      double pBad  = 0.05;           // kötü davranışta "yanlışlıkla iyi" olasılığı
      //simtime_t switchTime = 30.0;   // CAMOUFLAGE kötüleşme zamanı
      double camouflageRate = 0.3;


    double calcPsi(double actRec, double rho_,double tSecs);
    auto gettrecent(int targetId)const;
    auto calcZeta(double Trecent);
    auto getActRecent(int targetId) const;
    auto getTime();
    auto calcT_now(double zeta, double psi, double lambda_);
    double computeTfinalFor(int targetId) const;
    void updateTrustForSelected() ;
    void ban(int nodeId, double tfinal);
    bool simulateServiceSuccessFrom(int serverId);
    void probeAndUpdate(int serverId);

        };
#endif
