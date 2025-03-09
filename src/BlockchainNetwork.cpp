/*
 * BlockchainNetwork.cpp
 *
 *  Created on: 4 Oca 2025
 *      Author: ipekm
 */

#include "BlockchainNetwork.h"

Define_Module(BlockchainNetwork);

void BlockchainNetwork::initialize() {
    // Get the number of blockchain and IoT nodes
    int numBlockchainNodes = getSubmoduleVectorSize("blockchainNodes");
    int numIoTNodes = getSubmoduleVectorSize("iotNodes");

    // Connect blockchain nodes in a circular topology
    for (int i = 0; i < numBlockchainNodes; i++) {
        cModule *node1 = getSubmodule("blockchainNodes", i);
        cModule *node2 = getSubmodule("blockchainNodes", (i + 1) % numBlockchainNodes);

        cGate *outGate = node1->gate("inoutGate$o", 0);
        cGate *inGate = node2->gate("inoutGate$i", 0);

        outGate->connectTo(inGate);
    }

    // Connect IoT nodes to blockchain nodes
    for (int i = 0; i < numIoTNodes; i++) {
        cModule *iotNode = getSubmodule("iotNodes", i);
        cModule *blockchainNode = getSubmodule("blockchainNodes", i % numBlockchainNodes);

        cGate *outGate = iotNode->gate("inoutGate$o", 0);
        cGate *inGate = blockchainNode->gate("inoutGate$i", 1);

        outGate->connectTo(inGate);
    }

    // Fully connect IoT nodes to each other (mesh topology)
    for (int i = 0; i < numIoTNodes; i++) {
        for (int j = i + 1; j < numIoTNodes; j++) {
            cModule *node1 = getSubmodule("iotNodes", i);
            cModule *node2 = getSubmodule("iotNodes", j);

            cGate *outGate1 = node1->gate("inoutGate$o", j - i - 1);
            cGate *inGate1 = node2->gate("inoutGate$i", i);
            outGate1->connectTo(inGate1);

            cGate *outGate2 = node2->gate("inoutGate$o", i);
            cGate *inGate2 = node1->gate("inoutGate$i", j - i - 1);
            outGate2->connectTo(inGate2);
        }
    }

}
