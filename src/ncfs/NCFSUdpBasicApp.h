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

#ifndef __INET_NCFSUDPBASICAPP_H
#define __INET_NCFSUDPBASICAPP_H

#include "inet/applications/udpapp/UdpBasicApp.h"
#include "inet/applications/udpapp/ncfs/runmode/RunModeFactory.h"

#include <vector>
#include <time.h>
using std::vector;
namespace inet {

/**
 * UDP application. See NED for more info.
 */
class INET_API NCFSUdpBasicApp: public UdpBasicApp {
    //hanhai 自定义数据
private:
    // 用来存储编码系数  数组
    //vbArray coefMatrix;

    AbstractRunMode* runMode;
    double ber;

    // msg kind
    // 网络编码文件分享处理逻辑
    // COEFMATRIX_MSG 文件配置信息 系数矩阵的信息
    // QUEST_MSG  文件请求信息
    // FILE_MSG   文件内容信息
    enum OtherMsgKind {
        COEFMATRIX_MSG = 10, QUEST_MSG, FILE_MSG
    };
    //lyl--模式选择
    enum ModeKind {
        Copy_Mode = 101, RLNC_Mode, RRLNC_Mode
    };

    // 有应答再发送下一个编码系数包
    bool hasAnswer = true;

    // 每个包的长度
    int packetLength;
    // 发送文件的总长度
    int fileLength;
    // 网络编码K值
    int K;
    int sumframes=100;
    int numofframe=1;
    int packetnum=1;
    int fflag=1;
    int flagp[10];
    const int KSize=10;
    int packetcount=0;
    bool flag=false;
    int ressize;
    int checkflag=false;
    bool reencodeflag=true;
    int  reencodenum=101;
    double lossrate;
    int selectMode;
    int countres;
public:
    NCFSUdpBasicApp() {

    }
    ~NCFSUdpBasicApp() {
        if (runMode != nullptr) {
            delete runMode;
        }
    }

protected:
    //重写继承自UdpBasicApp 的函数   初始化函数
    //UdpBasicApp模块会在节点启动是 发送自消息handleNodeStart（） 发送 开始start类型的消息
    //然后handleMessageWhenUp（） 根据自消息类型 调用不同的process函数
    //先是调用processStart()函数 处理发送方IP信息绑定发送门  然后判断是否发送send类型的消息
    //processSend（）函数 被调用  发送给目的节点  packet消息
    //最后各个不同节点调用processPacket(Packet *msg)（包括根节点） 完成后结束仿真
    virtual void initialize(int stage) override;
    virtual void sendPacket() override;
    virtual void processPacket(Packet *msg) override;
    virtual void processSend() override;
    virtual void refreshDisplay() const override;
private:
    // 当有节点请求数据时，发送给他数据
    void sendNCPacket(L3Address sendAddr, int sendPort, vector<byte>& bytes);
    // 保存数据
    void saveNCData(Packet *pk, vector<byte>& bytes);
    // 处理接收到的文件配置信息      编码系数矩阵
    void solveCoefMatrixMsg(Packet *pk, vector<byte>& bytes);
    bool hasProbabilisticError(b length, double ber);

    // 刷新network布局中的信息显示
    void setTextFigure(string labelName,string labelMsg);
    // 更新network参数值
    //void updateNetworkParam(string  )
    virtual void clearZero(int flag[],int size);
    virtual bool checkArray(int flag[],int size);
};

} // namespace inet

#endif // ifndef __INET_NCFSUDPBASICAPP_H

