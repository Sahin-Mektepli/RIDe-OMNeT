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
           // maliciousNodePercentage = parent.maliciousNodePercentage;
            //attackerType = parent.attackerType;//burayı değiştirmemişiz semiNode.cpp'nin içinde
            }
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