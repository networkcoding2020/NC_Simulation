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

package inet.examples.hanhai.ncfs;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;

import inet.physicallayer.errormodel.packetlevel.StochasticErrorModel;

network NCFSNetworkBase
{
    parameters:
        //        一个像素代表1分米
        @display("bgb=950,700;bgg=100,1,grey95;bgs=1,dm");
        //@figure[title](type=label; pos=0,-1; anchor=sw; color=darkblue);
		// 显示运行模式信息  K值
        @figure[runModelLabel](type=text; pos=900,350; font=,12; text=);
        // 显示多少个手机已经获取到所有的数据
        @figure[numCompleted](type=text; pos=900,400; font=,12);
        // 显示数据长度和 每个包的长度
        @figure[dataMsg](type=text; pos=900,450; font=,12; text=);
        // 显示连接总数和有效的次数     usefulConnect/totalConnect
        @figure[connectMsg](type=text; pos=900,500; font=,12; text=);
        // 已完成的节点数
        volatile int numOfCompleted = default(1);
        // 节点总数
        int totalNodes = default(1);
        // 运行模式  NC/RS/OD
        string runModeString;
        // 网络编码K值
        int K;
        // 文件的大小
        int fileLength @unit(B);
        // 连接成功的次数
        volatile int totalConnect = default(0);
        // 有效连接数
        volatile int usefulConnect = default(0);
        int selectMode;
        int ressize;
        double lossrate;       
    submodules:
        visualizer: <default("IntegratedCanvasVisualizer")> like IIntegratedVisualizer if hasVisualizer() {
            @display("p=900,90.72");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=900,176.40001");
        }
        radioMedium: <default("UnitDiskRadioMedium")> like IRadioMedium {
            @display("p=900,260.064");
        }
        hostA: <default("WirelessHost")> like INetworkNode {
            @display("p=400,350;i=misc/smartphone");
        }
}
