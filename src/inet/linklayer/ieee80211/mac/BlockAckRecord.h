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

#ifndef __INET_BLOCKACKRECORD_H
#define __INET_BLOCKACKRECORD_H

#include "inet/common/INETDefs.h"
#include "inet/linklayer/ieee80211/mac/common/Ieee80211Defs.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"

namespace inet {
namespace ieee80211 {

//
// The recipient shall maintain a Block Ack record consisting of originator address, TID, and a record of
// reordering buffer size indexed by the received MPDU sequence control value. This record holds the
// acknowledgment state of the data frames received from the originator.
//
class INET_API BlockAckRecord
{
    protected:
        MACAddress originatorAddress = MACAddress::UNSPECIFIED_ADDRESS;
        Tid tid = -1;
        std::map<std::pair<SequenceNumber, FragmentNumber>, bool> acknowledgmentState;

    public:
        BlockAckRecord(MACAddress originatorAddress, Tid tid);

        void blockAckPolicyFrameReceived(Ieee80211DataFrame* frame);
        bool getAckState(SequenceNumber sequenceNumber, FragmentNumber fragmentNumber);

        MACAddress getOriginatorAddress() { return originatorAddress; }
        Tid getTid() { return tid; }
};

} /* namespace ieee80211 */
} /* namespace inet */

#endif // __INET_BLOCKACKRECORD_H