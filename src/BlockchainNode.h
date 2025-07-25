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
    struct Block {
        int blockId;
        int validatorId;
        std::string transactionData;
        simtime_t timestamp;
    };
    static int globalBlockId; // Global block ID counter
    std::vector<Block> blockchain;
    int blockCount;
    int reward;
    std::map<int, double> trustScores;

  public://hata aldığım için şimdilik böyle
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void createBlock();
    void processServiceRating(int requesterId, int providerId, double rating);
    int selectValidator();
    void updateTrustScore(int validatorId);
};

#endif // BLOCKCHAINNODE_H
