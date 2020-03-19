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

#ifndef INET_APPLICATIONS_UDPAPP_NCFS_RUNMODE_RUNMODE_H_
#define INET_APPLICATIONS_UDPAPP_NCFS_RUNMODE_RUNMODE_H_
#include "AbstractRunMode.h"

#include <string>

using std::string;

const string NC_MODE = "NC";
const string RS_MODE = "RS";
const string OD_MODE = "OD";

namespace inet {

// NCģʽ
class NCRunMode: public AbstractRunMode {
public:
    NCRunMode(bool isSourceNode, int K) :
            AbstractRunMode(isSourceNode, K) {
    }
    virtual vector<byte> getData(vector<byte>& itsNeedCoef) override {
        //��ȡ�������
        auto ret = NCUtils::reencode(coefMatrix);
        return ret[0];
    }

    virtual string getRunModeString() override{
        return NC_MODE;
    }
};

// ODģʽ
class ODRunMode: public AbstractRunMode {
public:
    ODRunMode(bool isSourceNode, int K) :
            AbstractRunMode(isSourceNode, K) {

    }
    virtual vector<byte> getData(vector<byte>& itsNeedCoef) override {
        return itsNeedCoef;
    }

    virtual string getRunModeString() override{
        return OD_MODE;
    }
};

// RSģʽ
class RSRunMode: public AbstractRunMode {
public:
    RSRunMode(bool isSourceNode, int K) :
            AbstractRunMode(isSourceNode, K) {

    }

    virtual vector<byte> getData(vector<byte>& itsNeedCoef) override {
        // RSģʽֻ��Դ�ڵ��ٱ���
        if (isSourceNode) {
            auto ret = NCUtils::reencode(coefMatrix);
            return ret[0];
        } else {
            return itsNeedCoef;
        }
    }
    virtual string getRunModeString() override{
            return RS_MODE;
    }
};

} /* namespace inet */

#endif /* INET_APPLICATIONS_UDPAPP_NCFS_RUNMODE_RUNMODE_H_ */
