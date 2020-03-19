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
#include "inet/applications/udpapp/ncfs/NCFSUdpBasicApp.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <time.h>

using std::string;
using std::stringstream;

namespace inet {
Define_Module(NCFSUdpBasicApp);
//��ʼ�� stage=0 û�����κι���
void NCFSUdpBasicApp::initialize(int stage) {
    ApplicationBase::initialize(stage);
//    if (stage == INITSTAGE_LOCAL) {
        //����δ����ı��� ���Ǽ̳���UdpBasicApp��
        numSent = 0;
        numReceived = 0;
        countres= 0;
        //ressize ����������
        ressize = (int) (this->getParentModule()->getParentModule()->par("ressize"));
        lossrate = (double) (this->getParentModule()->getParentModule()->par("lossrate"));
        selectMode = (int)(this->getParentModule()->getParentModule()->par("selectMode"));
        EV_INFO<<"-----ressize----"<<ressize<<endl;
        //EV_INFO<<"-----time is----"<<mtm<<endl;
        clearZero(flagp,KSize);
        //�ڽڵ�����ʾ��Ӧ�ķ��ͺͽ��յ� ���ݰ��� watch
        //�����ÿ���ڵ������������
        if (stage == INITSTAGE_LOCAL) {
        WATCH(numSent);
        WATCH(numReceived);
        //par�ǻ�ú����б������ľ���������Ϣ
        localPort = par("localPort");
        destPort = par("destPort");
        startTime = par("startTime");
        stopTime = par("stopTime");
        packetName = par("packetName");

        if (stopTime >= SIMTIME_ZERO&& stopTime < startTime)
        throw cRuntimeError("Invalid startTime/stopTime parameters");
        selfMsg = new cMessage("sendTimer");
        //hanhai ���  �����getParentMoudle ��Network�еı���
        K = (int) (this->getParentModule()->getParentModule()->par("K"));
        fileLength = (int) (this->getParentModule()->getParentModule()->par("fileLength"));
        // ÿ�����ĳ���  �������ݵĳ����ټ��ϱ���ϵ��
        //�ı��--��ʵ��--����ֻ��Ҫ����ϵ����Kֵ
        packetLength = ceil((double) fileLength / K) + K + 1;
        bool isBootNode = (bool) (par("isBootNode"));
        ber = (double) par("ber");
        string runModeString = this->getParentModule()->getParentModule()->par("runModeString");
        //�������ж��ĸ�ģʽ���еĵط� �ҿ���������ĳ��Լ���ģʽ
        //ÿ��ģʽ��������RunMode.h����
        //�������Խ�� ģʽ����
        if (runModeString == NC_MODE || runModeString == RS_MODE
                || runModeString == OD_MODE) {
            runMode = RunModeFactory::createRunMode(runModeString, isBootNode,
                    K);
        } else {
            // ����ģʽ�������ô���
            throw cRuntimeError(
                    "Invalid runModeString! runModeString = NC/RS/OD");
        }
    }
}

void NCFSUdpBasicApp::sendPacket() {
// ����ѭ�������Լ��ı���ϵ��   ��������ֻ��Ҫ����ϵ��
    //�õ��ǵ�λ����  �ҿ��԰�֡����Ϊ100  K=10  Ϊ������10*10�ĵ�λ����
//    int currentNum = runMode->currentDataNum();//ӵ�е�ϵ������Ĵ�С
//    int totalNum = runMode->totalDataNum();//��ʼ��Kֵ
//    if (currentNum == 0) {
//        // ������
//        return;
//    }
//    if (currentNum == totalNum) {
//        //����������Ľڵ� ��ʾӵ�е�������
////        cDisplayString& dispStr = this->getParentModule()->getDisplayString();
////        string showPro = "i=misc/smartphone,red;t=" + std::to_string(currentNum)
////                + "/" + std::to_string(totalNum);
////        dispStr.parse(showPro.c_str());
//        if (runMode->isBootNode()) {
//            // ���õ�ǰ����ģʽ   NC/RS/OD
//            // obtain the figure pointer by hierarchical name, and change the text:
//            // ��ʾ����ģʽ
//            string modeString = "Run Mode/K: " + runMode->getRunModeString()
//                    + "/" + std::to_string(K);
//            setTextFigure("runModelLabel", modeString);
//            // ��ʾ������Ϣ  �����ܳ���  ÿ�����ݰ��ĳ���
//            float fileLen = (float) fileLength / (1024 * 1024);
//            float pkLen = (float) packetLength / (1024 * 1024);
//            //�࣬����@c char������������ڴ�����
//            stringstream stream1;
//            stream1 << std::fixed << std::setprecision(2) << fileLen;
//            string str_fileLen = stream1.str() + "MB";
//            stringstream stream2;
//            stream2 << std::fixed << std::setprecision(2) << pkLen;
//            string str_pkLen = stream2.str() + "MB";
//            string dataMsg = "fileLen/packetLen: " + str_fileLen + "/"
//                    + str_pkLen;
//            setTextFigure("dataMsg", dataMsg);
//            // ��ʾ��ǰ�Ѿ���ɵĽڵ���
//            int numCount = this->getParentModule()->getParentModule()->par(
//                    "numOfCompleted");
//            int totalNodes = this->getParentModule()->getParentModule()->par(
//                    "totalNodes");
//            string completeMsg = "Completed/Total: " + std::to_string(numCount)
//                    + "/" + std::to_string(totalNodes);
//            setTextFigure("numCompleted", completeMsg);
//            if (numCount == totalNodes) {
//                // ���нڵ㶼�յ��㹻�������   �������
//                throw cRuntimeError("All nodes have enough data! ");
//            }
//        }
//    }
    //ǰ�沿������ʾ���� ���￪ʼ ���Ƿ��Ͳ���
    // ���ͱ���ϵ��
    // �ȴ��нڵ�ظ�ʱ����ʼ���Է�����������
//    std::ostringstream str;
//    //str << "coef matrix msg";
//    str << "video frame packet";
//    Packet *packet = new Packet(str.str().c_str());
//    // ��ʾ���͵ı����ļ��ı���ϵ��  �������ļ�������Ϣ
//    // messageKindֵ�޷��ٽ��սڵ�������ȡ��
//    // �����ݵ����һλ��Ϊ��Ϣ���͵ı�־λ
//    //packet->setKind(COEFMATRIX_MSG);
//    const auto& payload = makeShared<ApplicationPacket>();
//    // �����������һ������  ���򱨴�  ����0������
//    payload->setChunkLength(B(1)); // ��λΪB
//    //payload->setSequenceNumber(numSent);
//    auto creationTimeTag = payload->addTag<CreationTimeTag>();
//    creationTimeTag->setCreationTime(simTime());
//    packet->insertAtBack(payload);
//
//    auto rawBytesData = makeShared<BytesChunk>();
//    // ����ϵ������
//    vector<byte> data = runMode->getCoefMatrix_1();
//    // �����ݵ����һλ��Ϊ��Ϣ���͵ı�־λ  �����ʾΪ����ϵ��
//    // ������Ҫ����ÿһ������֡�ı�� �� �����
//    if(runMode->isBootNode())
//    {
//        if(packetnum==11+ressize)
//        {
//            numofframe++;
//            packetnum=1;
//            cDisplayString& dispStr = this->getParentModule()->getDisplayString();
//            if(numofframe<=100)
//            {
//                string showPro = "i=misc/smartphone,red;t=" + std::to_string(numofframe)+ "/100";
//                dispStr.parse(showPro.c_str());
//            }
//        }
//
//    }
//    data.push_back(numofframe);
//    data.push_back(packetnum);
//    data.push_back(COEFMATRIX_MSG);
//    rawBytesData->setBytes(data);
//    //rawBytesData->setBytes({110, 243, 74, 19, 84, 134, 216, 61, 44, 81, 51, 88, 91});
//    //�ڰ����������
//    packet->insertAtBack(rawBytesData);
//    L3Address destAddr = chooseDestAddr();
//    // �㲥������Ϣ  �㲥����
//    socket.sendTo(packet, destAddr, destPort);
//    packetnum++;
    if(runMode->isBootNode())
    {
        packetnum=1;
        cDisplayString& dispStr = this->getParentModule()->getDisplayString();
        if(numofframe<=100)
           {
             string showPro = "i=misc/smartphone,red;t=" + std::to_string(numofframe)+ "/100";
             dispStr.parse(showPro.c_str());
           }
        //������Ի��� ����ľ��� ����
        //vector<byte> data = runMode->getCoefMatrix_1();
        time_t begin = time(NULL);
        vector<byte> data = runMode->getCoefMatrix_lyl(10+ressize);
        time_t end = time(NULL);
        EV_INFO<<"-----begin time is----"<<begin<<endl;
        EV_INFO<<"-----end time is----"<<end<<endl;
        EV_INFO<<"-----minus time is----"<<end-begin<<endl;
        vector<byte> data1;

        for(int j=1;j<=10+ressize;j++)
        {
            std::ostringstream str;
            str << "video frame packet";
            Packet *packet = new Packet(str.str().c_str());
            // ��ʾ���͵ı����ļ��ı���ϵ��  �������ļ�������Ϣ
            const auto& payload = makeShared<ApplicationPacket>();
            // �����������һ������  ���򱨴�  ����0������
            payload->setChunkLength(B(1)); // ��λΪB
            //payload->setSequenceNumber(numSent);
            auto creationTimeTag = payload->addTag<CreationTimeTag>();
            creationTimeTag->setCreationTime(simTime());
            packet->insertAtBack(payload);
            auto rawBytesData = makeShared<BytesChunk>();
            // ����ϵ������  �����һ��10*10�ĵ�λ��  ��Ӧ��ȫ���Ž�ȥ  ����Ӧ�÷ָ��  �ֳ�10��
            //Ӧ������������� ��һ��(10+ressize)*10���������
            //��ѭ����������  ÿ��ȥһ�������� �Ž�ȥ  ��ȡһ�ξͿ�����
            //vector<byte> data = runMode->getCoefMatrix_1();
            //ÿ�λ�ȡ10���ֽ� ����data1��
            data1.assign(data.begin()+K*(j-1),data.begin()+K*j);
            // �����ݵ����һλ��Ϊ��Ϣ���͵ı�־λ  �����ʾΪ����ϵ��
            // ������Ҫ����ÿһ������֡�ı�� �� �����
            data1.push_back(numofframe);
            data1.push_back(packetnum);
            data1.push_back(COEFMATRIX_MSG);
            //��10�ֽڵľ�������
            rawBytesData->setBytes(data1);
            //rawBytesData->setBytes(data);
            //rawBytesData->setBytes({110, 243, 74, 19, 84, 134, 216, 61, 44, 81, 51, 88, 91});
            //�ڰ����������
            packet->insertAtBack(rawBytesData);
            L3Address destAddr = chooseDestAddr();
            // �㲥������Ϣ  �㲥����
            socket.sendTo(packet, destAddr, destPort);
            packetnum++;
        }
        numofframe++;
        if(numofframe>100)
           {
               endSimulation();
           }
    }

}
//�����Ƿ��͵�������Ĵ������ ��Ҫ������ֲ�ͬ���͵���Ϣ���д���
//���������������Ӧ�ĸ���  �������Ҫ��
void NCFSUdpBasicApp::processPacket(Packet* pk) {
//    if (pk->hasBitError()) {
//        this->getParentModule()->bubble("bit error");
//        return;
//    }
    //ʹ��Ĭ����־�����¼��Ϣ��α��
    EV_INFO << "@@@Received packet: "<< UdpSocket::getReceivedPacketInfo(pk) << endl;
    //messageKind ȡ�����Ϊ0�޷���Ϊpacket��ʶ
    //int msgKind = pk->getKind();
    // ȡ������
    auto pData = pk->peekData();
    auto pSChunk = dynamic_cast<const SequenceChunk*>(pData.get());
    auto chunks = pSChunk->getChunks();
    auto chunk = dynamic_cast<const BytesChunk*>(chunks[1].get());
    // ����ȡ�����Ƕ������ϵ������
    auto bytes = chunk->getBytes();
    //auto dataLen = chunk->getChunkLength();
    int size = bytes.size();
    // ���ݵ����һλΪmsgKindֵ
    int msgKind = bytes[size - 1];
    // ɾ�����һλ
    bytes.pop_back();
    // ȡ��������Ϣ
    auto l3Addresses = pk->getTag<L3AddressInd>();
    L3Address srcAddr = l3Addresses->getSrcAddress();
    //���� Ӧ����ɾȥ ��ͷ�ټ���
    if (msgKind == QUEST_MSG) {
        // ��Ӧ��   �����㲥����ϵ��
        hasAnswer = true;
        // �����ļ�����
        // ��ȡԴ�����������ĸ��˿�
        auto ports = pk->getTag<L4PortInd>();
        int srcPort = ports->getSrcPort();
        // ���ļ������߷���NC��������
        sendNCPacket(srcAddr, srcPort, bytes);

    } else if (msgKind == FILE_MSG) {
        // ��Ӧ��   �����㲥����ϵ��
        hasAnswer = true;
        //���յ��ļ�   ���뱾��
        // ȡ������
        saveNCData(pk, bytes);
    } else if (msgKind == COEFMATRIX_MSG) {
        //����ϵ�����  Ӧ�ñ������Ҫ��RLNC����ϵ�����  һֱ���ͱ���ϵ������ �ɲ�Ҫ���������
        // �����յ��ļ�����ϵ����Ϣ
        // ����Ƿ���Լ����ã�������ã���������
        //EV_INFO<<"---------------COEFMATRIX_MSG---------------"<<endl;
        if(runMode->isBootNode())
        {
            return;
        }
        solveCoefMatrixMsg(pk, bytes);
    }

    // �ͷŰ�
    delete pk;
}
/**
 * ����3�����Ϣ
 */
void NCFSUdpBasicApp::sendNCPacket(L3Address sendAddr, int sendPort,
        vector<byte>& bytes) {

    //this->getParentModule()->bubble("received file quest!");

    std::ostringstream str;
    str << packetName << "-" << numSent;

    Packet *packet = new Packet(str.str().c_str());

    //packet->setKind(FILE_MSG);

    const auto& payload = makeShared<ApplicationPacket>();
    //payload->setChunkLength(B(par("messageLength")));
    payload->setChunkLength(B(packetLength));
    payload->setSequenceNumber(numSent);
    auto creationTimeTag = payload->addTag<CreationTimeTag>();
    creationTimeTag->setCreationTime(simTime());
    packet->insertAtBack(payload);

    // д��13���ֽ�
    auto rawBytesData = makeShared<BytesChunk>();

    //
    auto sendData = runMode->getData(bytes);

    sendData.push_back(FILE_MSG);
    rawBytesData->setBytes(sendData);

    packet->insertAtBack(rawBytesData);

    if (hasProbabilisticError(B(1), ber)) {
        // �������
        packet->setBitError(true);
    }

    socket.sendTo(packet, sendAddr, sendPort);

    emit(packetSentSignal, packet);

    numSent++;
}

void NCFSUdpBasicApp::saveNCData(Packet* pk, vector<byte>& bytes) {
    if (runMode->haveAllData()) {
        // ֤���Ѿ���ȡ�����е�����    �����ټ��Է������ݶ��Լ��Ƿ�����
        return;
    }
    //���յ��ļ�   ���뱾��
    EV_INFO << "data = ";
    for (auto& v : bytes) {
        EV_INFO << (int) v << " ";
    }
    EV_INFO << endl;

    bool ret = runMode->saveData(bytes);
    if (ret) {
        // ���µ����ݰ���   ��������
        //hasAnswer = true;
        int currentNum = runMode->currentDataNum();
        int totalNum = runMode->totalDataNum();
        string msg = "save success!(" + std::to_string(currentNum) + "/"+ std::to_string(totalNum) + ")";
        //this->getParentModule()->bubble(msg.c_str());
        cDisplayString& dispStr = this->getParentModule()->getDisplayString();
        string showPro = "i=misc/smartphone;t=" + std::to_string(currentNum)+ "/" + std::to_string(totalNum);
        if (currentNum == totalNum) {
            // ���ٽ��չ㲥   ��Ч
//            EV_INFO << "***************************" << endl;
//            localPort = -1;
//            socket.setBroadcast(false);
            // ��ɫ
            string s = "i=misc/smartphone,red;" + showPro;
            //parse ����ʾ�ַ�������Ϊ����ֵ����������ַ���ʱ�������׳�����
            //���� �ڵ�ͷ�ϵ�0/10  �����ȥ�����߸ı�� ������Ҫ������
            dispStr.parse(s.c_str());
            // ��¼���յ���������ʹ�õ�ʱ��
            auto receiveFinishTime = simTime();
            recordScalar("hanhai: receive finish time", receiveFinishTime);
            // �����޸�ned�ļ�����ֵ    �Ѿ���ɵ��ֻ�����
            cPar& numOfCompleted =
                    this->getParentModule()->getParentModule()->par("numOfCompleted");
            int numCount = this->getParentModule()->getParentModule()->par(
                    "numOfCompleted");
            numOfCompleted.setIntValue(++numCount);

            // ��ʾ��ǰ�Ѿ���ɵĽڵ���
            int totalNodes = this->getParentModule()->getParentModule()->par(
                    "totalNodes");
            string completeMsg = "Completed/Total: " + std::to_string(numCount)
                    + "/" + std::to_string(totalNodes);
            //��ʾ��ǰ�Ѿ���ɵĽڵ������� ǰ����ͼ�ε�ID
            setTextFigure("numCompleted", completeMsg);
            if (numCount == totalNodes) {
                // ���нڵ㶼�յ��㹻�������   �������
                throw cRuntimeError("All nodes have enough data! ");
            }

        } else {
            dispStr.parse(showPro.c_str());
        }
        EV_INFO << msg << endl;
        // ����������ʾ
        emit(packetReceivedSignal, pk);
        numReceived++;
    } else {
        //this->getParentModule()->bubble("useless data!");
        EV_WARN << "-------------save failed!-------------" << endl;
        auto coefMatrix = runMode->getCoefMatrix_2();
        EV_INFO << "print coef matrix" << endl;
        for (auto& v : coefMatrix) {
            for (byte& b : v) {
                EV_INFO << (int) b << " ";
            }
            EV_INFO << endl;
        }
        EV_INFO << "useless data = ";
        for (auto& v : bytes) {
            EV_INFO << (int) v << " ";
        }
        EV_INFO << endl;
    }
}
//��Ϣ���� ΪCOEFMATRIX_MSGʱ  �����������
void NCFSUdpBasicApp::solveCoefMatrixMsg(Packet* pk, vector<byte>& bytes) {
    //������ȷ�������ʵĵط�
    //int randnum = runMode->randNumber(100);
    if(uniform(0, 1)< lossrate)
    {
        return;
    }
//    if (runMode->haveAllData()) {
//        // ֤���Ѿ���ȡ�����е�����    �����ټ��Է������ݶ��Լ��Ƿ�����
//        return;
//    }
    // ���ӳɹ�
//    cPar& totalConnectParam = this->getParentModule()->getParentModule()->par(
//            "totalConnect");
//    int totalConnect = this->getParentModule()->getParentModule()->par(
//            "totalConnect");
//    totalConnectParam.setIntValue(++totalConnect);
//    // ��ʾ��ǰ��Ч���Ӻ���������
//    int usefulConnect = this->getParentModule()->getParentModule()->par(
//            "usefulConnect");
//    string connectMsg = "usefulConnect/totalConnect: "
//            + std::to_string(usefulConnect) + "/"
//            + std::to_string(totalConnect);
//    // ��ʾ��ǰ��Ч���Ӻ���������  ��һ������Ϊͼ��@figure�ĵ�ID �ڶ�������Ϊ��ʾ�ľ�������
//    setTextFigure("connectMsg", connectMsg);
//    EV_INFO << "data = ";
//    //������ǽ����յı���ϵ��ȫ����ӡ����
//    for (auto& v : bytes) {
//        EV_INFO << (int) v << " ";
//    }
//    EV_INFO << endl;
    //�������������������������������� ��� ֡�ı��  �� ���ı��
    //������ ��������
    int size = bytes.size();
    int packetflag=bytes[size - 1];
    if(packetflag>10 && packetflag!=reencodenum)
    {
        packetflag %= 10;
        if(packetflag==0){packetflag=10;}
    }
    int frameflag=bytes[size - 2];
    //EV_INFO<<size<<"--"<<frameflag<<endl;
    // ɾ�������λ
    bytes.pop_back();
    bytes.pop_back();

    //R-RLNC
    if(selectMode == RRLNC_Mode)
    {
     if(frameflag==numofframe && packetcount<=10)
        {
            if(packetflag==101)
            {
                reencodeflag=false;
            }
            int msize = runMode->checkData(bytes,checkflag);
            EV_INFO<<"---------packetflag---------"<<packetflag<<endl;
            EV_INFO<<"---------msize---------"<<msize<<endl;
            packetcount=msize;
            if(packetcount==10)
            {
                if(reencodeflag==true)
                {
                    vector<byte> data1=runMode->getCoefMatrix_recode();
//                    EV_INFO<<"-----data1----";
//                    for (auto& v : data1) {
//                        EV_INFO<<int(v)<<" ";
//                        }
//                    EV_INFO<<endl;
                    std::ostringstream str;
                    str << "reencode frame packet";
                    Packet *packet = new Packet(str.str().c_str());
                    const auto& payload = makeShared<ApplicationPacket>();
                    payload->setChunkLength(B(1)); // ��λΪB
                    auto creationTimeTag = payload->addTag<CreationTimeTag>();
                    creationTimeTag->setCreationTime(simTime());
                    packet->insertAtBack(payload);
                    auto rawBytesData = makeShared<BytesChunk>();
                    data1.push_back(numofframe);
                    data1.push_back(reencodenum);
                    data1.push_back(COEFMATRIX_MSG);
                    //��10�ֽڵľ�������
                    rawBytesData->setBytes(data1);
                    packet->insertAtBack(rawBytesData);
                    L3Address destAddr = chooseDestAddr();
                    // �㲥������Ϣ  �㲥����
                    countres++;
                    socket.sendTo(packet, destAddr, destPort);
                }
               //Ҫ�����ݰ����ͽ���֡�� ����ǰ���ٱ������ݷ��ͳ�ȥ
               numReceived++;
               packetcount++;
            }
         }else
         {
            if(frameflag<=numofframe)
            {
                return;
            }
            reencodeflag=true;
            numofframe=frameflag;
            int msize2 = runMode->checkData(bytes,true);
            packetcount=msize2;
         }
    }


      // ������RLNC�Ĳ���
    if(selectMode == RLNC_Mode)
    {
     if(frameflag==numofframe && packetcount<=10)
     {
        int msize = runMode->checkData(bytes,checkflag);
        EV_INFO<<"---------packetflag---------"<<packetflag<<endl;
        EV_INFO<<"---------msize---------"<<msize<<endl;
        packetcount=msize;
        if(packetcount==10)
        {
           numReceived++;
           packetcount++;
        }
     }else
     {
        numofframe=frameflag;
        int msize2 = runMode->checkData(bytes,true);
        packetcount=msize2;
     }
    }

    //������copy�Ĵ�����
    if(selectMode == Copy_Mode)
    {
     if(frameflag==numofframe && flagp[packetflag-1]!=1 && packetcount<=10)
     {
        flagp[packetflag-1]=1;
        packetcount++;
        if(packetcount==10)
        {
            flag=checkArray(flagp,KSize);
            if(flag==true)
            {
                numReceived++;
                packetcount++;
            }
        }
     }else
     {
        numofframe=frameflag;
        clearZero(flagp,KSize);
        flagp[packetflag-1]=1;
        packetcount=1;
     }
    }


    //lyl������ʾ�Ĳ���
    cDisplayString& dispStr = this->getParentModule()->getDisplayString();
//    string showPro = "i=misc/smartphone,red;t=" + std::to_string(numReceived)
//                    + "/100" + std::to_string(numSent);
    string showPro = "i=misc/smartphone,red;t=" + std::to_string(numReceived)+ "/100/"+ std::to_string(countres);
    dispStr.parse(showPro.c_str());

    //���������жϾ������ lyl  �������ʱע��  ֻ������ �������ݰ������� ---lyl��ʱע������

//    auto requestData = runMode->hasUsefulData(bytes);
//    if (requestData.size() == 0) {
//        return;
//    } else {
//        // ������Ч  ˢ����Ч���ӵ���Ŀ
//        cPar& usefulConnectParam =
//                this->getParentModule()->getParentModule()->par("usefulConnect");
//        int usefulConnect = this->getParentModule()->getParentModule()->par(
//                "usefulConnect");
//        usefulConnectParam.setIntValue(++usefulConnect);
//        // ��ʾ��ǰ��Ч���Ӻ���������
//        int totalConnect =this->getParentModule()->getParentModule()->par("totalConnect");
//        string connectMsg = "usefulConnect/totalConnect: "
//                + std::to_string(usefulConnect) + "/"
//                + std::to_string(totalConnect);
//        setTextFigure("connectMsg", connectMsg);
//    }

       //��ʱע��������������ݷ��Ͳ���
//    requestData.push_back(QUEST_MSG);
//    // ������  ���鿴�Է��������Ƿ���Լ�����
//    // ��������
//    std::ostringstream str;
//    str << "file request!";
//    Packet *packet = new Packet(str.str().c_str());
//    // ��ʾ���͵ı����ļ��ı���ϵ��  �������ļ�������Ϣ
//    //packet->setKind(QUEST_MSG);
//    const auto& payload = makeShared<ApplicationPacket>();
//    // �����������һ������  ���򱨴�  ����0������
//    payload->setChunkLength(B(1)); // ��λΪB
//    //payload->setSequenceNumber(numSent);
//
//    auto creationTimeTag = payload->addTag<CreationTimeTag>();
//    creationTimeTag->setCreationTime(simTime());
//    packet->insertAtBack(payload);
//    // д��13���ֽ�
//    auto rawBytesData = makeShared<BytesChunk>();
//    rawBytesData->setBytes(requestData);
//    packet->insertAtBack(rawBytesData);
//    // ȡ��������Ϣ
//    auto l3Addresses = pk->getTag<L3AddressInd>();
//    L3Address srcAddr = l3Addresses->getSrcAddress();
//    // ��ȡԴ�����������ĸ��˿�
//    auto ports = pk->getTag<L4PortInd>();
//    int srcPort = ports->getSrcPort();
//    // �����ļ�����
//    //this->getParentModule()->bubble("send file quest!");
//    socket.sendTo(packet, srcAddr, srcPort);
}

bool NCFSUdpBasicApp::hasProbabilisticError(b length, double ber) {
    ASSERT(0.0 < ber && ber <= 1.0);
    //return dblrand() < 1 - std::pow((1 - ber), length.get());
    return dblrand() < ber;
}

void NCFSUdpBasicApp::processSend() {
    if(!runMode->isBootNode())
    {
        return ;
        EV_INFO<<"not boortnode!"<<endl;
    }
    //��ʼ����
    double rate = 1.0;
    if (!hasAnswer) {
        //rate = 5.0;
        rate = 0.05;
    }
    sendPacket();
    hasAnswer = false;
    // ��̬���÷���ʱ����
    // ��������ɽڵ���Ŀ�Ķ������÷��ͼ��
    int numCount = this->getParentModule()->getParentModule()->par(
            "numOfCompleted");
    int totalNodes = this->getParentModule()->getParentModule()->par(
            "totalNodes");
    double completeRate = (double) numCount / totalNodes;
    //simtime_t d = simTime() + (double) par("sendInterval") * completeRate;
    //d�Ƿ���������Ϣʱ�� ���ͼ��
    //simtime_t d = simTime() + (double) par("sendInterval") * (rate + completeRate);
    simtime_t d = simTime() + (double) par("sendInterval") * (rate);
    // ��Ϣѭ��  �˴�����Ҫ ��ͣ��sendPacket()  Ӧ�øĳ�����������
    if (stopTime < SIMTIME_ZERO||d < stopTime) {
        selfMsg->setKind(SEND);
        scheduleAt(d, selfMsg);
    }
    else {
        selfMsg->setKind(STOP);
        scheduleAt(stopTime, selfMsg);
    }
}

void NCFSUdpBasicApp::setTextFigure(string labelName, string labelMsg) {
    cCanvas *canvas = this->getParentModule()->getParentModule()->getCanvas();
    // obtain the figure pointer by hierarchical name, and change the text:
    cFigure *figure = canvas->getFigureByPath(labelName.c_str());
    cTextFigure *textFigure = check_and_cast<cTextFigure *>(figure);
    // ˢ����ʾ
    textFigure->setText(labelMsg.c_str());
}

void NCFSUdpBasicApp::refreshDisplay() const
{
    char buf[40];
    sprintf(buf, "send: %d-receive:%d ",numSent,numReceived);
    getDisplayString().setTagArg("t", 0, buf);
}

void NCFSUdpBasicApp::clearZero(int flag[],int size)
{
    for(int j=0;j<size;j++)
    {
        flag[j]=0;
    }
}
bool NCFSUdpBasicApp::checkArray(int flag[],int size)
{
    for(int j=0;j<size;j++)
    {
        if(flag[j]==0)
        {
            return false;
        }
    }
    return true;
}
}
// namespace inet

