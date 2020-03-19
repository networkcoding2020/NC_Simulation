//
// Copyright (C) 2004 Andras Varga
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

#ifndef __INET_UDPNCSINK_H
#define __INET_UDPNCSINK_H

#include "inet/applications/udpapp/UdpSink.h"
#include "inet/applications/udpapp/hanhai/nc/NCUtils.h"
#include <vector>
using std::vector;

namespace inet {

/**
 * Consumes and prints packets received from the Udp module. See NED for more info.
 */
class INET_API UdpNCSink: public UdpSink {
    //hanhai 自定义数据
private:
    int K;
    //int sendIndex;
    vbArray coefMatrix;
public:
    UdpNCSink() {
        // 初始化系数矩阵
        //sendIndex = 0;
    }
protected:
    virtual void processPacket(Packet *msg) override;
    virtual void initialize(int stage) override;
    // 利用packet包里面的信息给发送者返回信息
    void sendPacket(Packet* itsPk);
};

} // namespace inet

#endif // ifndef __INET_UDPNCSINK_H

