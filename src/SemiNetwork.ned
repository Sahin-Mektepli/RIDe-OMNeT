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





network SemiNetwork {
  parameters:
    int numNodes;                     // Total number of IoT nodes
    double maliciousNodePercentage;  // % of nodes to be malicious
    int attackerType;
    int clusterCount; //how many clusters there are
    int clusterSize; //how many nodes there are in a cluster
    submodules:
        iotNode[numNodes]: SemiNode{
       	   parameters:
            maliciousNodePercentage = parent.maliciousNodePercentage;
            attackerType = parent.attackerType;}
       // cluster2[numNodes]: IoTNode;


        //iotNode[numNodes]: Semi;
        //cluster2[numNodes]: Semi;
    connections allowunconnected:
        // --- circular cluster code ---
        if(clusterCount != 1),
        for c=0..(clusterCount-1) ,
        	for i=c*clusterSize .. (c+1)*clusterSize - 1,
            	for j=i+1 .. (c+1)*clusterSize - 1 {
                	iotNode[i].inoutGate++ <--> iotNode[j].inoutGate++;}
       	if(clusterCount != 1),
       	for c=0..(clusterCount-1) {
           iotNode[c * clusterSize].inoutGate++ <--> iotNode[((c + 1) % clusterCount) * clusterSize].inoutGate++;
  		}
  		// for fully connected case
  		if(clusterCount == 1),
  			for i=0.. numNodes-1, for j=i+1.. numNodes-1{
       			iotNode[i].inoutGate++ <--> iotNode[j].inoutGate++;}
  		// --- circular cluster code ---
        
        
        
        
        //I don't remember what this was for...
      //  for i=0.. numNodes-1, for j=i+1.. numNodes-1{
      // 	cluster2[i].inoutGate++ <--> cluster2[j].inoutGate++;
      //  }
       
        
}






