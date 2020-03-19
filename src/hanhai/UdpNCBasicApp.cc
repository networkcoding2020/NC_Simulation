//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 Andras Varga
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
#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"

#include "inet/applications/udpapp/hanhai/UdpNCBasicApp.h"

namespace inet {

Define_Module(UdpNCBasicApp);

void UdpNCBasicApp::sendPacket() {
    std::ostringstream str;
    str << packetName << "-" << numSent;
    Packet *packet = new Packet(str.str().c_str());
    packet->setKind(11);
    const auto& payload = makeShared<ApplicationPacket>();
    //payload->setChunkLength(B(par("messageLength")));
    // �����������һ������  ���򱨴�  ����0������
    payload->setChunkLength(B(1));

    payload->setSequenceNumber(numSent);
    auto creationTimeTag = payload->addTag<CreationTimeTag>();
    creationTimeTag->setCreationTime(simTime());
    packet->insertAtBack(payload);

    // д��13���ֽ�
    auto rawBytesData = makeShared<BytesChunk>();

    // �ٱ������ɷ�������  ֻ��һ������
    // ���û�����ݻ���ֻ��һ�����ݣ�Ӧ����������
    // һ������ʱ�� �������ٱ���ͷ���
    // û������ʱ���������ͻ᲻������߼�����
    auto sendData = NCUtils::reencode(coefMatrix);
    rawBytesData->setBytes(sendData[0]);

    //rawBytesData->setBytes(coefMatrix[(sendIndex++) % K]);

    //rawBytesData->setBytes({110, 243, 74, 19, 84, 134, 216, 61, 44, 81, 51, 88, 91});
    packet->insertAtBack(rawBytesData);


    L3Address destAddr = chooseDestAddr();
    emit(packetSentSignal, packet);

    // ��Ч
//    socket.connect(destAddr, destPort);
//    socket.send(packet);

    socket.sendTo(packet, destAddr, destPort);

//    // ���������õĵ�ַһһ����  ����
//    for (L3Address& destAddr : destAddresses) {
//        Packet pk = *packet;
//        socket.sendTo(&pk, destAddr, destPort);
//    }
//    emit(packetSentSignal, packet);

    numSent++;
}

void UdpNCBasicApp::initialize(int stage) {
    ApplicationBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);

        localPort = par("localPort");
        destPort = par("destPort");
        startTime = par("startTime");
        stopTime = par("stopTime");
        packetName = par("packetName");
        if (stopTime >= SIMTIME_ZERO&& stopTime < startTime)
        throw cRuntimeError("Invalid startTime/stopTime parameters");
        selfMsg = new cMessage("sendTimer");

        // hanhai ���
        // ��ʼ��ϵ������
        // sendIndex = 0;
        K = (int) (par("K"));
        if (K < 2)
            K = 2;
        //K = 4;
        //coefMatrix = NCUtils::generateRandMatrix(K, K);
        //EV_INFO<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
        for (int i = 0; i < K; i++) {
            vector<byte> coef(K, 0);
            coef[i] = 1;
            coefMatrix.push_back(coef);
//            for(auto& b:coef){
//                EV_INFO<<(int)b<<" ";
//            }
//            EV_INFO<<endl;
        }
    }
}

void UdpNCBasicApp::processPacket(Packet* msg) {
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(msg)
                   << endl;

    int kind = msg->getKind();
    EV_INFO << "packet kind = " << kind << endl;

    // ȡ��������Ϣ
    auto l3Addresses = msg->getTag<L3AddressInd>();
    //auto ports = itsPk->getTag<L4PortInd>();

    //operator==  �����ж�L3Address�����Ƿ����
    L3Address srcAddr = l3Addresses->getSrcAddress();

    // ��Ч
    // bubble("get package!");
    this->getParentModule()->bubble(("get package!" + srcAddr.str()).c_str());

    EV_INFO << "########## return packet from " + srcAddr.str() << endl;
    // hanhai
    // ȡ������
    auto pData = msg->peekData();
    EV_INFO << "#################################" << pData << endl;

    // �ͷŰ�
    delete msg;
}

} // namespace inet

