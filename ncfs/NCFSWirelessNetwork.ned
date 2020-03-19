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


network NCFSWirelessNetwork extends NCFSNetworkBase
{
    parameters:
        @figure[rcvdPkText](type=indicatorText; pos=380,20; anchor=w; font=,12; textFormat="hostB packets received: %g"; initialValue=0);
        @statistic[packetReceived](source=hostB.app[0].packetReceived; record=figure(count); targetFigure=rcvdPkText);
		totalNodes = default(2);
    submodules:
        hostB: <default("WirelessHost")> like INetworkNode {
            @display("p=265.104,341.712;i=misc/smartphone");

        }
}
