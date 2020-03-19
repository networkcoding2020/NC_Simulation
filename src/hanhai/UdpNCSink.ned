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


package inet.applications.udpapp.hanhai;

import inet.applications.udpapp.UdpSink;

//
// Consumes and prints packets received from the ~Udp module.
//
simple UdpNCSink extends UdpSink
{
    parameters:    
        // 若是没有这句话  依旧执行的是 UdpSink C++类
        @class("UdpNCSink");    
		//	hanhai
		// 网络编码K值
		int K = default(4);
		volatile int messageLength @unit(B); // length of messages to generate, in bytes
}

