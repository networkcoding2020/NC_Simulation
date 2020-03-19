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

#ifndef __INET_UDPNCBASICAPP_H
#define __INET_UDPNCBASICAPP_H

#include "inet/applications/udpapp/UdpBasicApp.h"
#include "inet/applications/udpapp/hanhai/nc/NCUtils.h"

#include <vector>
using std::vector;

namespace inet {

/**
 * UDP application. See NED for more info.
 */
class INET_API UdpNCBasicApp : public UdpBasicApp
{
  //hanhai 自定义数据
  private:
    int K;
    //int sendIndex;
    vbArray coefMatrix;

  public:
    UdpNCBasicApp() {

    }
  protected:
    virtual void initialize(int stage) override;
    virtual void sendPacket() override;
    virtual void processPacket(Packet *msg) override;
};

} // namespace inet

#endif // ifndef __INET_UDPNCBASICAPP_H

