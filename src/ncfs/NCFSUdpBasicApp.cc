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
//初始化 stage=0 没有做任何工作
void NCFSUdpBasicApp::initialize(int stage) {
    ApplicationBase::initialize(stage);
//    if (stage == INITSTAGE_LOCAL) {
        //以下未定义的变量 均是继承自UdpBasicApp中
        numSent = 0;
        numReceived = 0;
        countres= 0;
        //ressize 是冗余数量
        ressize = (int) (this->getParentModule()->getParentModule()->par("ressize"));
        lossrate = (double) (this->getParentModule()->getParentModule()->par("lossrate"));
        selectMode = (int)(this->getParentModule()->getParentModule()->par("selectMode"));
        EV_INFO<<"-----ressize----"<<ressize<<endl;
        //EV_INFO<<"-----time is----"<<mtm<<endl;
        clearZero(flagp,KSize);
        //在节点上显示相应的发送和接收的 数据包数 watch
        //这里给每个节点加上两个变量
        if (stage == INITSTAGE_LOCAL) {
        WATCH(numSent);
        WATCH(numReceived);
        //par是获得后面中变量名的具体数据信息
        localPort = par("localPort");
        destPort = par("destPort");
        startTime = par("startTime");
        stopTime = par("stopTime");
        packetName = par("packetName");

        if (stopTime >= SIMTIME_ZERO&& stopTime < startTime)
        throw cRuntimeError("Invalid startTime/stopTime parameters");
        selfMsg = new cMessage("sendTimer");
        //hanhai 添加  这里的getParentMoudle 是Network中的变量
        K = (int) (this->getParentModule()->getParentModule()->par("K"));
        fileLength = (int) (this->getParentModule()->getParentModule()->par("fileLength"));
        // 每个包的长度  编码数据的长度再加上编码系数
        //改变后--事实上--我们只需要编码系数和K值
        packetLength = ceil((double) fileLength / K) + K + 1;
        bool isBootNode = (bool) (par("isBootNode"));
        ber = (double) par("ber");
        string runModeString = this->getParentModule()->getParentModule()->par("runModeString");
        //这里是判断哪个模式运行的地方 我可以在这里改成自己的模式
        //每个模式都定义在RunMode.h里面
        //这里可以越过 模式创建
        if (runModeString == NC_MODE || runModeString == RS_MODE
                || runModeString == OD_MODE) {
            runMode = RunModeFactory::createRunMode(runModeString, isBootNode,
                    K);
        } else {
            // 运行模式参数设置错误
            throw cRuntimeError(
                    "Invalid runModeString! runModeString = NC/RS/OD");
        }
    }
}

void NCFSUdpBasicApp::sendPacket() {
// 这里循环发送自己的编码系数   现在我们只需要编码系数
    //用的是单位矩阵  我可以把帧数设为100  K=10  为此生成10*10的单位矩阵
//    int currentNum = runMode->currentDataNum();//拥有的系数矩阵的大小
//    int totalNum = runMode->totalDataNum();//初始的K值
//    if (currentNum == 0) {
//        // 不发送
//        return;
//    }
//    if (currentNum == totalNum) {
//        //这里是最初的节点 显示拥有的数据量
////        cDisplayString& dispStr = this->getParentModule()->getDisplayString();
////        string showPro = "i=misc/smartphone,red;t=" + std::to_string(currentNum)
////                + "/" + std::to_string(totalNum);
////        dispStr.parse(showPro.c_str());
//        if (runMode->isBootNode()) {
//            // 设置当前运行模式   NC/RS/OD
//            // obtain the figure pointer by hierarchical name, and change the text:
//            // 显示运行模式
//            string modeString = "Run Mode/K: " + runMode->getRunModeString()
//                    + "/" + std::to_string(K);
//            setTextFigure("runModelLabel", modeString);
//            // 显示数据信息  数据总长度  每个数据包的长度
//            float fileLen = (float) fileLength / (1024 * 1024);
//            float pkLen = (float) packetLength / (1024 * 1024);
//            //类，用于@c char混合输入和输出内存流。
//            stringstream stream1;
//            stream1 << std::fixed << std::setprecision(2) << fileLen;
//            string str_fileLen = stream1.str() + "MB";
//            stringstream stream2;
//            stream2 << std::fixed << std::setprecision(2) << pkLen;
//            string str_pkLen = stream2.str() + "MB";
//            string dataMsg = "fileLen/packetLen: " + str_fileLen + "/"
//                    + str_pkLen;
//            setTextFigure("dataMsg", dataMsg);
//            // 显示当前已经完成的节点数
//            int numCount = this->getParentModule()->getParentModule()->par(
//                    "numOfCompleted");
//            int totalNodes = this->getParentModule()->getParentModule()->par(
//                    "totalNodes");
//            string completeMsg = "Completed/Total: " + std::to_string(numCount)
//                    + "/" + std::to_string(totalNodes);
//            setTextFigure("numCompleted", completeMsg);
//            if (numCount == totalNodes) {
//                // 所有节点都收到足够多的数据   仿真结束
//                throw cRuntimeError("All nodes have enough data! ");
//            }
//        }
//    }
    //前面部分是显示数据 这里开始 才是发送部分
    // 发送编码系数
    // 等待有节点回复时，开始给对方传输数数据
//    std::ostringstream str;
//    //str << "coef matrix msg";
//    str << "video frame packet";
//    Packet *packet = new Packet(str.str().c_str());
//    // 表示发送的本地文件的编码系数  即本地文件配置信息
//    // messageKind值无法再接收节点中正常取出
//    // 把数据的最后一位作为信息类型的标志位
//    //packet->setKind(COEFMATRIX_MSG);
//    const auto& payload = makeShared<ApplicationPacket>();
//    // 这里必须设置一个长度  否则报错  设置0，报错
//    payload->setChunkLength(B(1)); // 单位为B
//    //payload->setSequenceNumber(numSent);
//    auto creationTimeTag = payload->addTag<CreationTimeTag>();
//    creationTimeTag->setCreationTime(simTime());
//    packet->insertAtBack(payload);
//
//    auto rawBytesData = makeShared<BytesChunk>();
//    // 放入系数矩阵
//    vector<byte> data = runMode->getCoefMatrix_1();
//    // 把数据的最后一位作为信息类型的标志位  这里表示为编码系数
//    // 这里需要放入每一个数据帧的编号 和 子序号
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
//    //在包里添加数据
//    packet->insertAtBack(rawBytesData);
//    L3Address destAddr = chooseDestAddr();
//    // 广播配置信息  广播发送
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
        //这里可以换成 随机的矩阵 试试
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
            // 表示发送的本地文件的编码系数  即本地文件配置信息
            const auto& payload = makeShared<ApplicationPacket>();
            // 这里必须设置一个长度  否则报错  设置0，报错
            payload->setChunkLength(B(1)); // 单位为B
            //payload->setSequenceNumber(numSent);
            auto creationTimeTag = payload->addTag<CreationTimeTag>();
            creationTimeTag->setCreationTime(simTime());
            packet->insertAtBack(payload);
            auto rawBytesData = makeShared<BytesChunk>();
            // 放入系数矩阵  这个是一个10*10的单位阵  不应该全部放进去  这里应该分割开来  分成10份
            //应该生成随机矩阵 是一个(10+ressize)*10的随机矩阵
            //在循环外面生成  每次去一个行向量 放进去  获取一次就可以了
            //vector<byte> data = runMode->getCoefMatrix_1();
            //每次获取10个字节 放入data1中
            data1.assign(data.begin()+K*(j-1),data.begin()+K*j);
            // 把数据的最后一位作为信息类型的标志位  这里表示为编码系数
            // 这里需要放入每一个数据帧的编号 和 子序号
            data1.push_back(numofframe);
            data1.push_back(packetnum);
            data1.push_back(COEFMATRIX_MSG);
            //将10字节的矩阵向量
            rawBytesData->setBytes(data1);
            //rawBytesData->setBytes(data);
            //rawBytesData->setBytes({110, 243, 74, 19, 84, 134, 216, 61, 44, 81, 51, 88, 91});
            //在包里添加数据
            packet->insertAtBack(rawBytesData);
            L3Address destAddr = chooseDestAddr();
            // 广播配置信息  广播发送
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
//下面是发送的网络包的处理过程 主要真多三种不同类型的消息进行处理
//可以在这里进行相应的更改  变成我需要的
void NCFSUdpBasicApp::processPacket(Packet* pk) {
//    if (pk->hasBitError()) {
//        this->getParentModule()->bubble("bit error");
//        return;
//    }
    //使用默认日志级别记录信息的伪流
    EV_INFO << "@@@Received packet: "<< UdpSocket::getReceivedPacketInfo(pk) << endl;
    //messageKind 取出后恒为0无法作为packet标识
    //int msgKind = pk->getKind();
    // 取出数据
    auto pData = pk->peekData();
    auto pSChunk = dynamic_cast<const SequenceChunk*>(pData.get());
    auto chunks = pSChunk->getChunks();
    auto chunk = dynamic_cast<const BytesChunk*>(chunks[1].get());
    // 这里取到的是多个编码系数数据
    auto bytes = chunk->getBytes();
    //auto dataLen = chunk->getChunkLength();
    int size = bytes.size();
    // 数据的最后一位为msgKind值
    int msgKind = bytes[size - 1];
    // 删除最后一位
    bytes.pop_back();
    // 取出连接信息
    auto l3Addresses = pk->getTag<L3AddressInd>();
    L3Address srcAddr = l3Addresses->getSrcAddress();
    //这里 应该先删去 回头再加上
    if (msgKind == QUEST_MSG) {
        // 有应答   继续广播编码系数
        hasAnswer = true;
        // 处理文件请求
        // 获取源数据来自于哪个端口
        auto ports = pk->getTag<L4PortInd>();
        int srcPort = ports->getSrcPort();
        // 向文件请求者发送NC编码数据
        sendNCPacket(srcAddr, srcPort, bytes);

    } else if (msgKind == FILE_MSG) {
        // 有应答   继续广播编码系数
        hasAnswer = true;
        //接收到文件   存入本地
        // 取出数据
        saveNCData(pk, bytes);
    } else if (msgKind == COEFMATRIX_MSG) {
        //编码系数检查  应该变成我需要的RLNC编码系数检查  一直发送编码系数即可 可不要具体的数据
        // 检查接收到文件编码系数信息
        // 检查是否对自己有用，如果有用，则发送请求
        //EV_INFO<<"---------------COEFMATRIX_MSG---------------"<<endl;
        if(runMode->isBootNode())
        {
            return;
        }
        solveCoefMatrixMsg(pk, bytes);
    }

    // 释放包
    delete pk;
}
/**
 * 处理3类包信息
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

    // 写入13个字节
    auto rawBytesData = makeShared<BytesChunk>();

    //
    auto sendData = runMode->getData(bytes);

    sendData.push_back(FILE_MSG);
    rawBytesData->setBytes(sendData);

    packet->insertAtBack(rawBytesData);

    if (hasProbabilisticError(B(1), ber)) {
        // 设置误包
        packet->setBitError(true);
    }

    socket.sendTo(packet, sendAddr, sendPort);

    emit(packetSentSignal, packet);

    numSent++;
}

void NCFSUdpBasicApp::saveNCData(Packet* pk, vector<byte>& bytes) {
    if (runMode->haveAllData()) {
        // 证明已经获取到所有的数据    不用再检查对方的数据对自己是否有用
        return;
    }
    //接收到文件   存入本地
    EV_INFO << "data = ";
    for (auto& v : bytes) {
        EV_INFO << (int) v << " ";
    }
    EV_INFO << endl;

    bool ret = runMode->saveData(bytes);
    if (ret) {
        // 有新的数据包了   继续发送
        //hasAnswer = true;
        int currentNum = runMode->currentDataNum();
        int totalNum = runMode->totalDataNum();
        string msg = "save success!(" + std::to_string(currentNum) + "/"+ std::to_string(totalNum) + ")";
        //this->getParentModule()->bubble(msg.c_str());
        cDisplayString& dispStr = this->getParentModule()->getDisplayString();
        string showPro = "i=misc/smartphone;t=" + std::to_string(currentNum)+ "/" + std::to_string(totalNum);
        if (currentNum == totalNum) {
            // 不再接收广播   无效
//            EV_INFO << "***************************" << endl;
//            localPort = -1;
//            socket.setBroadcast(false);
            // 变色
            string s = "i=misc/smartphone,red;" + showPro;
            //parse 将显示字符串设置为给定值。如果解析字符串时出错，则抛出错误。
            //这是 节点头上的0/10  把这个去掉或者改变成 我们需要的样子
            dispStr.parse(s.c_str());
            // 记录接收到所有数据使用的时间
            auto receiveFinishTime = simTime();
            recordScalar("hanhai: receive finish time", receiveFinishTime);
            // 尝试修改ned文件变量值    已经完成的手机数量
            cPar& numOfCompleted =
                    this->getParentModule()->getParentModule()->par("numOfCompleted");
            int numCount = this->getParentModule()->getParentModule()->par(
                    "numOfCompleted");
            numOfCompleted.setIntValue(++numCount);

            // 显示当前已经完成的节点数
            int totalNodes = this->getParentModule()->getParentModule()->par(
                    "totalNodes");
            string completeMsg = "Completed/Total: " + std::to_string(numCount)
                    + "/" + std::to_string(totalNodes);
            //显示当前已经完成的节点数函数 前面是图形的ID
            setTextFigure("numCompleted", completeMsg);
            if (numCount == totalNodes) {
                // 所有节点都收到足够多的数据   仿真结束
                throw cRuntimeError("All nodes have enough data! ");
            }

        } else {
            dispStr.parse(showPro.c_str());
        }
        EV_INFO << msg << endl;
        // 更新数据显示
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
//消息类型 为COEFMATRIX_MSG时  调用这个函数
void NCFSUdpBasicApp::solveCoefMatrixMsg(Packet* pk, vector<byte>& bytes) {
    //这里是确定丢包率的地方
    //int randnum = runMode->randNumber(100);
    if(uniform(0, 1)< lossrate)
    {
        return;
    }
//    if (runMode->haveAllData()) {
//        // 证明已经获取到所有的数据    不用再检查对方的数据对自己是否有用
//        return;
//    }
    // 连接成功
//    cPar& totalConnectParam = this->getParentModule()->getParentModule()->par(
//            "totalConnect");
//    int totalConnect = this->getParentModule()->getParentModule()->par(
//            "totalConnect");
//    totalConnectParam.setIntValue(++totalConnect);
//    // 显示当前有效连接和总连接数
//    int usefulConnect = this->getParentModule()->getParentModule()->par(
//            "usefulConnect");
//    string connectMsg = "usefulConnect/totalConnect: "
//            + std::to_string(usefulConnect) + "/"
//            + std::to_string(totalConnect);
//    // 显示当前有效连接和总连接数  第一个参数为图形@figure的的ID 第二个参数为显示的具体内容
//    setTextFigure("connectMsg", connectMsg);
//    EV_INFO << "data = ";
//    //这里的是将接收的编码系数全部打印出来
//    for (auto& v : bytes) {
//        EV_INFO << (int) v << " ";
//    }
//    EV_INFO << endl;
    //现在我在数组后面增添了两个标记用于 标记 帧的编号  和 包的编号
    //下面是 包处理部分
    int size = bytes.size();
    int packetflag=bytes[size - 1];
    if(packetflag>10 && packetflag!=reencodenum)
    {
        packetflag %= 10;
        if(packetflag==0){packetflag=10;}
    }
    int frameflag=bytes[size - 2];
    //EV_INFO<<size<<"--"<<frameflag<<endl;
    // 删除最后两位
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
                    payload->setChunkLength(B(1)); // 单位为B
                    auto creationTimeTag = payload->addTag<CreationTimeTag>();
                    creationTimeTag->setCreationTime(simTime());
                    packet->insertAtBack(payload);
                    auto rawBytesData = makeShared<BytesChunk>();
                    data1.push_back(numofframe);
                    data1.push_back(reencodenum);
                    data1.push_back(COEFMATRIX_MSG);
                    //将10字节的矩阵向量
                    rawBytesData->setBytes(data1);
                    packet->insertAtBack(rawBytesData);
                    L3Address destAddr = chooseDestAddr();
                    // 广播配置信息  广播发送
                    countres++;
                    socket.sendTo(packet, destAddr, destPort);
                }
               //要在数据包数和接收帧数 增加前将再编码数据发送出去
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


      // 这里是RLNC的部分
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

    //下面是copy的处理部分
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


    //lyl加上显示的部分
    cDisplayString& dispStr = this->getParentModule()->getDisplayString();
//    string showPro = "i=misc/smartphone,red;t=" + std::to_string(numReceived)
//                    + "/100" + std::to_string(numSent);
    string showPro = "i=misc/smartphone,red;t=" + std::to_string(numReceived)+ "/100/"+ std::to_string(countres);
    dispStr.parse(showPro.c_str());

    //这里用来判断矩阵的秩 lyl  下面的暂时注销  只是增加 接收数据包的数量 ---lyl暂时注销下面

//    auto requestData = runMode->hasUsefulData(bytes);
//    if (requestData.size() == 0) {
//        return;
//    } else {
//        // 连接有效  刷新有效连接的数目
//        cPar& usefulConnectParam =
//                this->getParentModule()->getParentModule()->par("usefulConnect");
//        int usefulConnect = this->getParentModule()->getParentModule()->par(
//                "usefulConnect");
//        usefulConnectParam.setIntValue(++usefulConnect);
//        // 显示当前有效连接和总连接数
//        int totalConnect =this->getParentModule()->getParentModule()->par("totalConnect");
//        string connectMsg = "usefulConnect/totalConnect: "
//                + std::to_string(usefulConnect) + "/"
//                + std::to_string(totalConnect);
//        setTextFigure("connectMsg", connectMsg);
//    }

       //暂时注销下面的请求数据发送部分
//    requestData.push_back(QUEST_MSG);
//    // 计算秩  即查看对方的数据是否对自己有用
//    // 请求数据
//    std::ostringstream str;
//    str << "file request!";
//    Packet *packet = new Packet(str.str().c_str());
//    // 表示发送的本地文件的编码系数  即本地文件配置信息
//    //packet->setKind(QUEST_MSG);
//    const auto& payload = makeShared<ApplicationPacket>();
//    // 这里必须设置一个长度  否则报错  设置0，报错
//    payload->setChunkLength(B(1)); // 单位为B
//    //payload->setSequenceNumber(numSent);
//
//    auto creationTimeTag = payload->addTag<CreationTimeTag>();
//    creationTimeTag->setCreationTime(simTime());
//    packet->insertAtBack(payload);
//    // 写入13个字节
//    auto rawBytesData = makeShared<BytesChunk>();
//    rawBytesData->setBytes(requestData);
//    packet->insertAtBack(rawBytesData);
//    // 取出连接信息
//    auto l3Addresses = pk->getTag<L3AddressInd>();
//    L3Address srcAddr = l3Addresses->getSrcAddress();
//    // 获取源数据来自于哪个端口
//    auto ports = pk->getTag<L4PortInd>();
//    int srcPort = ports->getSrcPort();
//    // 发送文件请求
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
    //初始发送
    double rate = 1.0;
    if (!hasAnswer) {
        //rate = 5.0;
        rate = 0.05;
    }
    sendPacket();
    hasAnswer = false;
    // 动态设置发送时间间隔
    // 根据已完成节点数目的多少设置发送间隔
    int numCount = this->getParentModule()->getParentModule()->par(
            "numOfCompleted");
    int totalNodes = this->getParentModule()->getParentModule()->par(
            "totalNodes");
    double completeRate = (double) numCount / totalNodes;
    //simtime_t d = simTime() + (double) par("sendInterval") * completeRate;
    //d是发送自我消息时的 发送间隔
    //simtime_t d = simTime() + (double) par("sendInterval") * (rate + completeRate);
    simtime_t d = simTime() + (double) par("sendInterval") * (rate);
    // 消息循环  此处很重要 不停的sendPacket()  应该改成有数量限制
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
    // 刷新显示
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

