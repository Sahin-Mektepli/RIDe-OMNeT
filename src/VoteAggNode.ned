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

// Separate node type for the vote aggregation model.
// It uses the C++ class VoteAgg and does not depend on IoTNode.
simple VoteAggNode 
{
    parameters:
        @class(VoteAgg);
        @display("i=block/circle");

        // Kept for compatibility with the old ini style.
        int inoutGateSize = default(10);
        volatile double serviceInterval @unit(s) = default(uniform(1s, 5s));

        // Old trust / attacker parameters kept in the new module.
        double maliciousNodePercentage = default(0.3);
        double camouflageRate = default(0.0);

        // New vote aggregation parameters.
		int aggregationMethod = default(0); // 0 additive, 1 multiplicative, 2 borda        double globalTrustUpdateInterval @unit(s) = default(10s);
        double interactionWindow @unit(s) = default(100s);
        double providerAvailabilityProbability = default(1.0);
          
        int selectionMode = default(0);       // 0:DT+GT merge, 1:DT-only, 2:GT-only
        int directTrustMethod = default(0);   // 0:Simple, 1:Weighted, 2:Rancorous, 3:Decay, 4:Rancorous+Decay

        int minDirectEvidence = default(1);
        double directTrustPriorMass = default(0.0);
        double rancorFactor = default(2.0);
        double directTrustDecayRate = default(0.05);
        
    gates:
        inout inoutGate[];
}

