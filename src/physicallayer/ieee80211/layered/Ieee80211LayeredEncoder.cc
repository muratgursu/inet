//
// Copyright (C) 2014 OpenSim Ltd.
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
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "Ieee80211LayeredEncoder.h"

namespace inet {
namespace physicallayer {

void Ieee80211LayeredEncoder::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL)
    {
        serializer = check_and_cast<ISerializer *>(getSubmodule("serializer"));
        scrambler = check_and_cast<IScrambler *>(getSubmodule("scrambler"));
        dataFECEncoder = check_and_cast<IForwardErrorCorrection *>(getSubmodule("forwardErrorCorrection"));
        signalFECEncoder = check_and_cast<IForwardErrorCorrection *>(getSubmodule("signalforwardErrorCorrection"));
        interleaver = check_and_cast<IInterleaver *>(getSubmodule("interleaver"));
    }
}

BitVector Ieee80211LayeredEncoder::signalFieldEncode(const BitVector& signalField) const
{
    // NOTE: The contents of the SIGNAL field are not scrambled.
    BitVector fecEncodedBits = signalFECEncoder->encode(signalField);
    BitVector interleavedBits = interleaver->interleaving(fecEncodedBits);
    return interleavedBits;
}

BitVector Ieee80211LayeredEncoder::dataFieldEncode(const BitVector& dataField) const
{
   BitVector scrambledBits = scrambler->scramble(dataField);
   BitVector fecEncodedBits = dataFECEncoder->encode(scrambledBits);
   BitVector interleavedBits = interleaver->interleaving(fecEncodedBits);
   return interleavedBits;
}

const ITransmissionBitModel* Ieee80211LayeredEncoder::encode(const ITransmissionPacketModel* packetModel) const
{
    const cPacket *packet = packetModel->getPacket();
    BitVector serializedPacket = serializer->serialize(packet);
    // The SIGNAL field is composed of RATE (4), Reserved (1), LENGTH (12), Parity (1), Tail (6),
    // fields, so the SIGNAL field is 24 bits long.
    BitVector signalField;
    for (int i = 0; i < 24; i++)
        signalField.appendBit(serializedPacket.getBit(i));
    BitVector dataField;
    // NOTE: the SERVICE field, which is part of the PLCP header goes to the DATA field.
    // Then we apply different FEC encodings (and later modulations) - not for the whole PLCP header
    // but for the SIGNAL field - then for the DATA field.
    for (int i = 24; i < serializedPacket.getSize(); i++)
        dataField.appendBit(serializedPacket.getBit(i));
    BitVector encodedSignalField = signalFieldEncode(signalField);
    BitVector encodedDataField = dataFieldEncode(dataField);
    BitVector encodedBits;
    for (int i = 0; i < encodedSignalField.getSize(); i++)
        encodedBits.appendBit(encodedSignalField.getBit(i));
    for (int i = 0; i < encodedDataField.getSize(); i++)
        encodedBits.appendBit(encodedDataField.getBit(i));
    // TODO: bitrate
    return new TransmissionBitModel(encodedBits.getSize(), bitRate, encodedBits, dataFECEncoder->getInfo(), scrambler->getInfo(), interleaver->getInfo());

}

} /* namespace physicallayer */
} /* namespace inet */