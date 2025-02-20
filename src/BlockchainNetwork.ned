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





network BlockchainNetwork {
    parameters:
        int numNodes = 10;  // Total number of IoT nodes

    submodules:
        iotNode[10]: IoTNode;

    connections allowunconnected:
        // Manually connecting every node to every other node
        iotNode[0].inoutGate++ <--> iotNode[1].inoutGate++;
        iotNode[0].inoutGate++ <--> iotNode[2].inoutGate++;
        iotNode[0].inoutGate++ <--> iotNode[3].inoutGate++;
        iotNode[0].inoutGate++ <--> iotNode[4].inoutGate++;
        iotNode[0].inoutGate++ <--> iotNode[5].inoutGate++;
        iotNode[0].inoutGate++ <--> iotNode[6].inoutGate++;
        iotNode[0].inoutGate++ <--> iotNode[7].inoutGate++;
        iotNode[0].inoutGate++ <--> iotNode[8].inoutGate++;
        iotNode[0].inoutGate++ <--> iotNode[9].inoutGate++;

        iotNode[1].inoutGate++ <--> iotNode[2].inoutGate++;
        iotNode[1].inoutGate++ <--> iotNode[3].inoutGate++;
        iotNode[1].inoutGate++ <--> iotNode[4].inoutGate++;
        iotNode[1].inoutGate++ <--> iotNode[5].inoutGate++;
        iotNode[1].inoutGate++ <--> iotNode[6].inoutGate++;
        iotNode[1].inoutGate++ <--> iotNode[7].inoutGate++;
        iotNode[1].inoutGate++ <--> iotNode[8].inoutGate++;
        iotNode[1].inoutGate++ <--> iotNode[9].inoutGate++;

        iotNode[2].inoutGate++ <--> iotNode[3].inoutGate++;
        iotNode[2].inoutGate++ <--> iotNode[4].inoutGate++;
        iotNode[2].inoutGate++ <--> iotNode[5].inoutGate++;
        iotNode[2].inoutGate++ <--> iotNode[6].inoutGate++;
        iotNode[2].inoutGate++ <--> iotNode[7].inoutGate++;
        iotNode[2].inoutGate++ <--> iotNode[8].inoutGate++;
        iotNode[2].inoutGate++ <--> iotNode[9].inoutGate++;

        iotNode[3].inoutGate++ <--> iotNode[4].inoutGate++;
        iotNode[3].inoutGate++ <--> iotNode[5].inoutGate++;
        iotNode[3].inoutGate++ <--> iotNode[6].inoutGate++;
        iotNode[3].inoutGate++ <--> iotNode[7].inoutGate++;
        iotNode[3].inoutGate++ <--> iotNode[8].inoutGate++;
        iotNode[3].inoutGate++ <--> iotNode[9].inoutGate++;

        iotNode[4].inoutGate++ <--> iotNode[5].inoutGate++;
        iotNode[4].inoutGate++ <--> iotNode[6].inoutGate++;
        iotNode[4].inoutGate++ <--> iotNode[7].inoutGate++;
        iotNode[4].inoutGate++ <--> iotNode[8].inoutGate++;
        iotNode[4].inoutGate++ <--> iotNode[9].inoutGate++;

        iotNode[5].inoutGate++ <--> iotNode[6].inoutGate++;
        iotNode[5].inoutGate++ <--> iotNode[7].inoutGate++;
        iotNode[5].inoutGate++ <--> iotNode[8].inoutGate++;
        iotNode[5].inoutGate++ <--> iotNode[9].inoutGate++;

        iotNode[6].inoutGate++ <--> iotNode[7].inoutGate++;
        iotNode[6].inoutGate++ <--> iotNode[8].inoutGate++;
        iotNode[6].inoutGate++ <--> iotNode[9].inoutGate++;

        iotNode[7].inoutGate++ <--> iotNode[8].inoutGate++;
        iotNode[7].inoutGate++ <--> iotNode[9].inoutGate++;

        iotNode[8].inoutGate++ <--> iotNode[9].inoutGate++;
}


