#include "BlockchainNode.h"
#include "BlockchainMessage_m.h"
#include <string>
#include <random>

using namespace omnetpp;

int BlockchainNode::globalBlockId = 0; // Initialize global block ID counter

Define_Module(BlockchainNode);

void BlockchainNode::initialize() {

    reward = par("reward");

    // Initialize trust scores for simulation
    for (int i = 0; i < 2; i++) {
        trustScores[i] = uniform(50, 100); // Random initial trust scores
    }

    EV << "BlockchainNode initialized with " << trustScores.size() << " trust scores.\n";
}

void BlockchainNode::handleMessage(cMessage *msg) {
    // Handle service ratings (BlockchainTransaction messages)
    BlockchainTransaction *transaction = dynamic_cast<BlockchainTransaction *>(msg);
    if (transaction) {
        EV << "Received service rating from IoT Node: " << transaction->getRequesterId() << "\n";
        processServiceRating(transaction->getRequesterId(), transaction->getProviderId(), transaction->getRating());
        delete msg;
    } else {
        throw cRuntimeError("Unexpected message received.");
    }
}

void BlockchainNode::processServiceRating(int requesterId, int providerId, double rating) {
    // Create a block with the transaction data
    int uniqueBlockId = ++globalBlockId;

    Block newBlock = {
        .blockId = uniqueBlockId,
        .validatorId = selectValidator(),
        .transactionData = "Requester: " + std::to_string(requesterId) +
                           ", Provider: " + std::to_string(providerId) +
                           ", Rating: " + std::to_string(rating),
        .timestamp = simTime()
    };

    blockchain.push_back(newBlock);
    EV << "Block " << newBlock.blockId << " created with service rating.\n";
}

int BlockchainNode::selectValidator() {
    double totalTrust = 0;
    for (const auto &pair : trustScores) {
        totalTrust += pair.second;
    }

    double randomValue = uniform(0, totalTrust);
    double cumulativeTrust = 0;

    for (const auto &pair : trustScores) {
        cumulativeTrust += pair.second;
        if (randomValue <= cumulativeTrust) {
            updateTrustScore(pair.first); // Reward the selected validator
            return pair.first;
        }
    }
    return -1; // Should not reach here
}

void BlockchainNode::updateTrustScore(int validatorId) {
    trustScores[validatorId] += reward;
    EV << "Validator Node " << validatorId << " rewarded. New Trust Score: " << trustScores[validatorId] << "\n";
}
