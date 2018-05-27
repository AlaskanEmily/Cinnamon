/*
 * Copyright (c) 2018 AlaskanEmily
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cin_driver.h"
#include "cin_driver_oss.hpp"
#include "cin_loader.h"
#include "cin_loader_soft.h"

#include <sys/soundcard.h>

unsigned Cin_StructDriverSize(){
    return sizeof(struct Cin_Driver);
}

enum Cin_DriverError Cin_CreateDriver(struct Cin_Driver *drv){
    new (drv) Cin_Driver();
    return Cin_eDriverSuccess;
}

void Cin_DestroyDriver(struct Cin_Driver *drv){
    drv->~Cin_Driver();
}
