//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
//
//

#include "inet/linklayer/ieee80211/mac/coordinationfunction/Edcaf.h"
#include "inet/linklayer/ieee80211/mac/duplicatedetector/QosDuplicateDetector.h"
#include "inet/linklayer/ieee80211/mac/framesequence/HcfFs.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Mac.h"
#include "inet/linklayer/ieee80211/mac/lifetime/EdcaTransmitLifetimeHandler.h"
#include "inet/linklayer/ieee80211/mac/originator/OriginatorQoSMpduHandler.h"

namespace inet {
namespace ieee80211 {

Define_Module(Edcaf);

inline double fallback(double a, double b) {return a!=-1 ? a : b;}
inline simtime_t fallback(simtime_t a, simtime_t b) {return a!=-1 ? a : b;}

void Edcaf::initialize(int stage)
{
    CafBase::initialize(stage);
    if (stage == INITSTAGE_LINK_LAYER) {
        contentionCallback = this;
        ac = getAccessCategory(par("accessCategory"));
        originatorMpduHandler = check_and_cast<IOriginatorMpduHandler *>(getModuleByPath(par("originatorMpduHandlerModule")));
        contention->setTxIndex(ac + 1); // TODO
    }
    else if (stage == INITSTAGE_LAST) {
        auto rateSelection = check_and_cast<IRateSelection *>(getModuleByPath(par("rateSelectionModule")));
        const IIeee80211Mode *referenceMode = rateSelection->getSlowestMandatoryMode(); // or any other; slotTime etc must be the same for all modes we use
        slotTime = referenceMode->getSlotTime();
        sifs = referenceMode->getSifsTime();
        int aifsn = fallback(par("aifsn"), getAifsNumber());
        simtime_t aifs = sifs + aifsn * slotTime;
        ifs = aifs;
        eifs = sifs + aifs + referenceMode->getDuration(LENGTH_ACK);
    }
}

int Edcaf::getAifsNumber()
{
    switch (ac)
    {
        case AC_BK: return 7;
        case AC_BE: return 3;
        case AC_VI: return 2;
        case AC_VO: return 2;
        default: throw cRuntimeError("Unknown access category = %d", ac);
    }
}

// TODO: remove
int Edcaf::getCwMax(int aCwMax, int aCwMin)
{
    switch (ac)
    {
        case AC_BK: return aCwMax;
        case AC_BE: return aCwMax;
        case AC_VI: return aCwMin;
        case AC_VO: return (aCwMin + 1) / 2 - 1;
        default: throw cRuntimeError("Unknown access category = %d", ac);
    }
}

// TODO: remove
int Edcaf::getCwMin(int aCwMin)
{
    switch (ac)
    {
        case AC_BK: return aCwMin;
        case AC_BE: return aCwMin;
        case AC_VI: return (aCwMin + 1) / 2 - 1;
        case AC_VO: return (aCwMin + 1) / 4 - 1;
        default: throw cRuntimeError("Unknown access category = %d", ac);
    }
}

void Edcaf::channelAccessGranted()
{
    if (!isSequenceRunning()) {
        frameSequence = new HcfFs();
        context = originatorMpduHandler->buildContext();
        frameSequence->startSequence(context, 0);
        startFrameSequenceStep();
    }
    else
        throw cRuntimeError("Channel access granted while a frame sequence is running");
}

AccessCategory Edcaf::getAccessCategory(const char *ac)
{
    if (strcmp("AC_BK", ac) == 0)
        return AC_BK;
    if (strcmp("AC_VI", ac) == 0)
        return AC_VI;
    if (strcmp("AC_VO", ac) == 0)
        return AC_VO;
    if (strcmp("AC_BE", ac) == 0)
        return AC_BE;
    throw cRuntimeError("Unknown Access Category = %s", ac);
}

void Edcaf::internalCollision()
{
    // Take the first frame from queue assuming it would have been sent
    OriginatorQoSMpduHandler *qosMpduHandler = check_and_cast<OriginatorQoSMpduHandler*>(originatorMpduHandler);
    if (qosMpduHandler->internalCollision())
        startContention();
}

} // namespace ieee80211
} // namespace inet
