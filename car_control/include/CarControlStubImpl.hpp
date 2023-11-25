#ifndef CarControlStubImpl_H_
#define CarControlStubImpl_H_

#include <CommonAPI/CommonAPI.hpp>
#include "../src-gen/core/v0/commonapi/CarControlStubDefault.hpp"

#include "PiRacer.hpp"
#include <Python.h>

class CarControlStubImpl: public v0_1::commonapi::CarControlStubDefault {
    public:
        CarControlStubImpl();
        virtual ~CarControlStubImpl();
        virtual void gearSelectionHeadUnit(const std::shared_ptr<CommonAPI::ClientId> _client, 
                                               std::string _selectedGear, 
                                               gearSelectionHeadUnitReply_t _reply);
};

#endif //CarControlStubImpl_H