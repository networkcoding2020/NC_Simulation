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
    //hanhai �Զ�������
private:
    // �����洢����ϵ��  ����
    //vbArray coefMatrix;

    AbstractRunMode* runMode;
    double ber;

    // msg kind
    // ��������ļ��������߼�
    // COEFMATRIX_MSG �ļ�������Ϣ ϵ���������Ϣ
    // QUEST_MSG  �ļ�������Ϣ
    // FILE_MSG   �ļ�������Ϣ
    enum OtherMsgKind {
        COEFMATRIX_MSG = 10, QUEST_MSG, FILE_MSG
    };
    //lyl--ģʽѡ��
    enum ModeKind {
        Copy_Mode = 101, RLNC_Mode, RRLNC_Mode
    };

    // ��Ӧ���ٷ�����һ������ϵ����
    bool hasAnswer = true;

    // ÿ�����ĳ���
    int packetLength;
    // �����ļ����ܳ���
    int fileLength;
    // �������Kֵ
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
    //��д�̳���UdpBasicApp �ĺ���   ��ʼ������
    //UdpBasicAppģ����ڽڵ������� ��������ϢhandleNodeStart���� ���� ��ʼstart���͵���Ϣ
    //Ȼ��handleMessageWhenUp���� ��������Ϣ���� ���ò�ͬ��process����
    //���ǵ���processStart()���� �����ͷ�IP��Ϣ�󶨷�����  Ȼ���ж��Ƿ���send���͵���Ϣ
    //processSend�������� ������  ���͸�Ŀ�Ľڵ�  packet��Ϣ
    //��������ͬ�ڵ����processPacket(Packet *msg)���������ڵ㣩 ��ɺ��������
    virtual void initialize(int stage) override;
    virtual void sendPacket() override;
    virtual void processPacket(Packet *msg) override;
    virtual void processSend() override;
    virtual void refreshDisplay() const override;
private:
    // ���нڵ���������ʱ�����͸�������
    void sendNCPacket(L3Address sendAddr, int sendPort, vector<byte>& bytes);
    // ��������
    void saveNCData(Packet *pk, vector<byte>& bytes);
    // ������յ����ļ�������Ϣ      ����ϵ������
    void solveCoefMatrixMsg(Packet *pk, vector<byte>& bytes);
    bool hasProbabilisticError(b length, double ber);

    // ˢ��network�����е���Ϣ��ʾ
    void setTextFigure(string labelName,string labelMsg);
    // ����network����ֵ
    //void updateNetworkParam(string  )
    virtual void clearZero(int flag[],int size);
    virtual bool checkArray(int flag[],int size);
};

} // namespace inet

#endif // ifndef __INET_NCFSUDPBASICAPP_H

