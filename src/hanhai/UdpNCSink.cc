//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
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

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/packet/Packet.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/common/packet/chunk/SequenceChunk.h"
#include "inet/applications/udpapp/hanhai/UdpNCSink.h"
#include "inet/common/packet/dissector/ProtocolDissectorRegistry.h"
#include "inet/common/packet/dissector/PacketDissector.h"

#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"

namespace inet {

Define_Module(UdpNCSink);

void UdpNCSink::processPacket(Packet *pk) {

    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(pk)
                   << endl;

    // ������  Ĭ�ϵ�kindֵΪ0
    int kind = pk->getKind();
    EV_INFO << "packet kind = " << kind << endl;

    auto l3Addresses = pk->getTag<L3AddressInd>();
    //auto ports = itsPk->getTag<L4PortInd>();

    //operator==  �����ж�L3Address�����Ƿ����
    L3Address srcAddr = l3Addresses->getSrcAddress();

    // ��Ч
    // bubble("get package!");
    //this->getParentModule()->bubble(("get package!" + srcAddr.str()).c_str());

    // hanhai
    // ȡ������
    auto pData = pk->peekData();
    EV_INFO << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << pData << endl;

    auto pSChunk = dynamic_cast<const SequenceChunk*>(pData.get());
    auto chunks = pSChunk->getChunks();
    auto chunk = dynamic_cast<const BytesChunk*>(chunks[1].get());

    // ��ȡ�����ݳ��� �� ����
    auto dataLen = chunk->getChunkLength();
    auto bytes = chunk->getBytes();

    EV_INFO << "dataLength = " << dataLen << endl;  //1013
    EV_INFO << "data = ";

    for (auto& v : bytes) {
        EV_INFO << (int) v << " ";
    }

    EV_INFO << endl;

    int row = coefMatrix.size();
    vbArray temp_coef;
    temp_coef.assign(coefMatrix.begin(), coefMatrix.end());
    temp_coef.push_back(bytes);
    int rank = NCUtils::getRank(temp_coef);
    if (rank > row) {
        // ��������
        coefMatrix.push_back(bytes);
        EV_INFO << "@@@@@@@@@@ useful data! " << endl;
        if (coefMatrix.size() >= K) {
            this->getParentModule()->bubble("have all data!");

        } else {
            this->getParentModule()->bubble("useful data!");
        }
    } else {
        EV_INFO << "@@@@@@@@@@ useless data! " << endl;
        this->getParentModule()->bubble("useless data!");
    }

    sendPacket(pk);

    emit(packetReceivedSignal, pk);
    delete pk;

    numReceived++;
}

void UdpNCSink::initialize(int stage) {
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        numReceived = 0;
        WATCH(numReceived);

        localPort = par("localPort");
        startTime = par("startTime");
        stopTime = par("stopTime");
        if (stopTime >= SIMTIME_ZERO&& stopTime < startTime)
        throw cRuntimeError("Invalid startTime/stopTime parameters");
        selfMsg = new cMessage("UDPSinkTimer");

        // hanhai �¼�
        K = (int) (par("K"));
        //K = 4;
    }
}

void UdpNCSink::sendPacket(Packet* itsPk) {
    // ȡ��������Ϣ
    auto l3Addresses = itsPk->getTag<L3AddressInd>();
    auto ports = itsPk->getTag<L4PortInd>();

    //operator==  �����ж�L3Address�����Ƿ����
    L3Address srcAddr = l3Addresses->getSrcAddress();

    EV_INFO << "&&&&&&&&&&&&& ip = " << srcAddr.str() << endl;

    //L3Address destAddr = l3Addresses->getDestAddress();
    int srcPort = ports->getSrcPort();
    //int destPort = ports->getDestPort();

    // ��������
    std::ostringstream str;
    str << "return packet";
    Packet *packet = new Packet(str.str().c_str());
    // ���������
    // packet->setKind(1);
    const auto& payload = makeShared<ApplicationPacket>();
    // payload->setChunkLength(B(par("messageLength")));
    payload->setChunkLength(B(1));
    // payload->setSequenceNumber(numSent);
    auto creationTimeTag = payload->addTag<CreationTimeTag>();
    creationTimeTag->setCreationTime(simTime());
    packet->insertAtBack(payload);

    // д��13���ֽ�
    auto rawBytesData = makeShared<BytesChunk>();

    // �ٱ������ɷ�������  ֻ��һ������
    // ���û�����ݻ���ֻ��һ�����ݣ�Ӧ����������
    // һ������ʱ�� �������ٱ���ͷ���
    // û������ʱ���������ͻ᲻������߼�����
//    auto sendData = NCUtils::reencode(coefMatrix);
//    rawBytesData->setBytes(sendData[0]);
    //rawBytesData->setBytes(coefMatrix[(sendIndex++) % K]);

    rawBytesData->setBytes( { 110, 243, 74, 19, 84, 134, 216, 61, 44, 81, 51,
            88, 91 });
    packet->insertAtBack(rawBytesData);

    emit(packetSentSignal, packet);
    socket.sendTo(packet, srcAddr, srcPort);

}

} // namespace inet

