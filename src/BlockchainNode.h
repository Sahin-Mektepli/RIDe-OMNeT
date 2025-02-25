/*
 * BlockchainNode.h
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm
 */

#ifndef BLOCKCHAINNODE_H
#define BLOCKCHAINNODE_H

#include <omnetpp.h>
#include <string>
#include <vector>
#include <map>

using namespace omnetpp;

class BlockchainNode : public cSimpleModule
{
  private:
	  //why aren't we defining Block as a class. I am not really sure about the cons and pros though...
    struct Block {
        int blockId;
        int validatorId;
        std::string transactionData;
	//I may be a little annoying but should we have a separate class/struct for transactionData?
	//writing everything in it will most probably require a parser in the future.
        simtime_t timestamp;
    };
    static int globalBlockId; // Global block ID counter
    std::vector<Block> blockchain;
    int blockCount; // this is not an attribute of a BC node so this should be static too right? I might be trippin tho
    int reward;
    std::map<int, double> trustScores;
    //are we storing the trustScores as a map from int to double?
    //that's rad!

  public://hata aldığım için şimdilik böyle
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void createBlock();
    void processServiceRating(int requesterId, int providerId, double rating);
    int selectValidator();
    void updateTrustScore(int validatorId);
};

#endif // BLOCKCHAINNODE_H
