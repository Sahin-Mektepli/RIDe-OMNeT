//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package src;

network VoteAggNetwork
{
    parameters:
        int numNodes = default(30);                    // Total number of VoteAgg nodes
        double maliciousNodePercentage = default(0.3); // Ratio of malicious nodes
        int attackerType = default(6);                 // See voteAgg.ini comments
        int clusterCount = default(1);                 // Number of clusters
        int clusterSize = default(int(numNodes / clusterCount));

    submodules:
        voteAggNode[numNodes]: VoteAggNode {
            parameters:
                maliciousNodePercentage = parent.maliciousNodePercentage;
        }

    connections allowunconnected:
        // Clustered topology: fully connect inside each cluster.
        if clusterCount != 1,
            for c = 0..clusterCount-1,
                for i = c * clusterSize .. (c + 1) * clusterSize - 1,
                    for j = i + 1 .. (c + 1) * clusterSize - 1 {
                        voteAggNode[i].inoutGate++ <--> voteAggNode[j].inoutGate++;
                    }

        // Ring connection between clusters.
        if clusterCount != 1,
            for c = 0..clusterCount-1 {
                voteAggNode[c * clusterSize].inoutGate++ <--> voteAggNode[((c + 1) % clusterCount) * clusterSize].inoutGate++;
            }

        // Fully connected topology when clusterCount == 1.
        if clusterCount == 1,
            for i = 0..numNodes-1,
                for j = i + 1..numNodes-1 {
                    voteAggNode[i].inoutGate++ <--> voteAggNode[j].inoutGate++;
                }
}

