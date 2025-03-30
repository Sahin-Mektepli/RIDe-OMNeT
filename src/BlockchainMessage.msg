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
message BlockchainTransaction {
    int requesterId;  // ID of the service requester
    int providerId;   // ID of the service provider
    double rating ;    // Rating given by the requester
}
message ServiceRequest {
    int requesterId;
    int providerId;
    string serviceType;
}

message ServiceResponse {
    int requesterId;
    int providerId;
    string serviceType;
}

message FinalServiceRequest {
    int requesterId;
    int providerId;
    string serviceType;
}

message FinalServiceResponse {
    int requesterId;
    int providerId;
    string serviceType;
    double serviceQuality;
}
message ServiceRating {
    int requesterId;
    int providerId;
    double rating;
}
