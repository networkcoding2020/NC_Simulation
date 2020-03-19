#include "RunModeFactory.h"

namespace inet {
AbstractRunMode* RunModeFactory::createRunMode(string modeType,
        bool isSourceNode, int K) {
    AbstractRunMode* runMode = nullptr;

    if (modeType == NC_MODE) {
        runMode = new NCRunMode(isSourceNode, K);
    } else if (modeType == RS_MODE) {
        runMode = new RSRunMode(isSourceNode, K);
    } else if (modeType == OD_MODE) {
        runMode = new ODRunMode(isSourceNode, K);
    }

    return runMode;
}
}
