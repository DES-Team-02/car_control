#include "CarControlStubImpl.hpp"

CarControlStubImpl::CarControlStubImpl() {
}

CarControlStubImpl::~CarControlStubImpl() {
}
/*Method that can be invoked by client */
void CarControlStubImpl::gearSelectionHeadUnit(const std::shared_ptr<CommonAPI::ClientId> _client, 
                                               std::string _selectedGear, 
                                               gearSelectionHeadUnitReply_t _reply)
{
    std::cout << " ---------------------------" << std::endl;
    std::cout << "Head Unit invoked service method & tries to set gear." << std::endl;
    std::cout << "  Input Gear: " << _selectedGear << std::endl;
    // singleton instance of PiRacer
    PiRacer* piracer = PiRacer::getInstance();
    // lock GIL
    PyGILState_STATE gilState = PyGILState_Ensure();
    // try to set gear to piracer
    bool accepted = piracer->setGear(_selectedGear);
    // if gear is set by gamepad, set gear attribute to service
    if(accepted) {
        std::cout << (accepted ? "->Gear change accepted." : "->Gear change declined.") << std::endl;
        setGearAttribute(_selectedGear);
    }
    std::cout << " ---------------------------" << std::endl;
    // release GIL
    PyGILState_Release(gilState);
    // send reply to client
    _reply(accepted);
}
