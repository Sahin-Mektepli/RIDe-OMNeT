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
    int numNodes;                     // Total number of IoT nodes
    double maliciousNodePercentage;  // % of nodes to be malicious
    submodules:
        iotNode[numNodes]: IoTNode{
       	   parameters:
            maliciousNodePercentage = parent.maliciousNodePercentage;}
       // cluster2[numNodes]: IoTNode;


        //iotNode[numNodes]: Semi;
        //cluster2[numNodes]: Semi;
    connections allowunconnected:
        for i=0.. numNodes-1, for j=i+1.. numNodes-1{
        	iotNode[i].inoutGate++ <--> iotNode[j].inoutGate++;
        }
      //  for i=0.. numNodes-1, for j=i+1.. numNodes-1{
      // 	cluster2[i].inoutGate++ <--> cluster2[j].inoutGate++;
      //  }
       
        
}



