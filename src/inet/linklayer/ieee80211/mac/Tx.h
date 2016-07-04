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
// Author: Andras Varga
//

#ifndef __INET_TX_H
#define __INET_TX_H

#include "inet/linklayer/ieee80211/mac/contract/ITx.h"

namespace inet {
namespace ieee80211 {

class IUpperMac;
class IMacRadioInterface;
class IRx;
class IStatistics;

/**
 * The default implementation of ITx.
 */
class INET_API Tx : public cSimpleModule, public ITx
{
    protected:
        MACAddress address;
        ITx::ICallback *txCallback = nullptr;
        IMacRadioInterface *mac;
        IRx *rx;
        IStatistics *statistics;
        Ieee80211Frame *frame = nullptr;
        cMessage *endIfsTimer = nullptr;
        simtime_t durationField;
        bool transmitting = false;

    protected:
        virtual int numInitStages() const override { return NUM_INIT_STAGES; }
        virtual void initialize(int stage) override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void updateDisplayString();

    public:
        Tx() {}
        ~Tx();

        virtual void transmitFrame(Ieee80211Frame *frame, ITx::ICallback *txCallback) override;
        virtual void transmitFrame(Ieee80211Frame *frame, simtime_t ifs, ITx::ICallback *txCallback) override;
        virtual void radioTransmissionFinished() override;
};

} // namespace ieee80211
} // namespace inet

#endif

