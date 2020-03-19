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

import inet.node.inet.INetworkNode;

network NCFSnNode extends NCFSNetworkBase
{
    parameters:
//        @figure[road] (type=polyline; points=100,200, 500,200; smooth=true; lineWidth=100; zoomLineWidth=true; lineColor=grey);
//        @figure[paint](type=polyline; points=100,150, 500,200; smooth=true; lineWidth=3; zoomLineWidth=true; lineColor=white; lineStyle=dashed);
        int numHosts;
        totalNodes = this.numHosts + 1;
    submodules:
        hostC[numHosts]: <default("WirelessHost")> like INetworkNode {
            parameters:                                
                @display("i=misc/smartphone");
              //@display("p=200,200,m,5,50,50");
        }

}
