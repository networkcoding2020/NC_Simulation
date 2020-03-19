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

package inet.examples.hanhai.ncfsmove;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;

import inet.networklayer.configurator.ipv4.HostAutoConfigurator;
import inet.applications.udpapp.ncfsmove.CarManager;
import inet.examples.hanhai.ncfsmove.MoveNetwork;
import inet.node.inet.WirelessHost;


module DynamicHost extends WirelessHost
{
    parameters:
        ipv4.configurator.networkConfiguratorModule = "";
    submodules:
        autoConfigurator: HostAutoConfigurator {
//            @display("p=125,640;is=s");
        }
}


network MoveNetwork
{
    
    
    
    parameters:
        @display("bgb=650,500;bgg=100,1,grey95;bgt=haha");
        @figure[title](type=label; pos=0,-1; anchor=sw; color=darkblue);

        @figure[label](type=text; pos=0,30; font=,12);


        @figure[rcvdPkText](type=indicatorText; pos=380,20; anchor=w; font=,12; textFormat="hostB packets received: %g"; initialValue=0);
        @statistic[packetReceived](source=hostB.app[0].packetReceived; record=figure(count); targetFigure=rcvdPkText);

    submodules:
        visualizer: <default("IntegratedCanvasVisualizer")> like IIntegratedVisualizer if hasVisualizer() {
            @display("p=580,125");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=580,200");
        }
        radioMedium: <default("UnitDiskRadioMedium")> like IRadioMedium {
            @display("p=580,275");
        }
        carManager: CarManager {
            @display("p=579.60004,58.464");
        }

        hostA: <default("WirelessHost")> like INetworkNode {
            @display("p=50,325;i=misc/smartphone");
        }
        hostB: <default("WirelessHost")> like INetworkNode {
            @display("p=49.392002,199.584;i=misc/smartphone");
        }

}
