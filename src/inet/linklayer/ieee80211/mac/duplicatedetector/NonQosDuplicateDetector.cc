//
// Copyright (C) 2015 Andras Varga
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
// Author: Andras Varga, Benjamin Seregi
//

#include "inet/common/stlutils.h"
#include "inet/linklayer/ieee80211/mac/duplicatedetector/NonQosDuplicateDetector.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"

namespace inet {
namespace ieee80211 {

void NonQoSDuplicateDetector::assignSequenceNumber(Ieee80211DataOrMgmtFrame *frame)
{
    ASSERT(frame->getType() != ST_DATA_WITH_QOS);
    lastSeqNum = (lastSeqNum + 1) % 4096;
    const MACAddress& address = frame->getReceiverAddress();
    auto it = lastSentSeqNums.find(address);
    if (it == lastSentSeqNums.end())
        lastSentSeqNums[address] = lastSeqNum;
    else {
        if (it->second == lastSeqNum)
            lastSeqNum = (lastSeqNum + 1) % 4096; // make it different from the last sequence number sent to that RA (spec: "add 2")
        it->second = lastSeqNum;
    }
    frame->setSequenceNumber(lastSeqNum);
}

} // namespace ieee80211
} // namespace inet
