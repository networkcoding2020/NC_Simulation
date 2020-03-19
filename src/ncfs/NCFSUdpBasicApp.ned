//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.applications.udpapp.ncfs;

import inet.applications.udpapp.UdpBasicApp;

simple NCFSUdpBasicApp extends UdpBasicApp
{
    parameters:
        // 若是没有这句话  依旧执行的是 UdpBasicApp C++类
        @class("NCFSUdpBasicApp");
        // 是否是启动节点
        bool isBootNode = default(false);
        
        // 不再使用此参数设置包长度
        messageLength = 1B;
        
        //	hanhai
        // 网络编码K值
		// int K = default(4);	
		// 设置误包率
		double ber = default(0);	
}