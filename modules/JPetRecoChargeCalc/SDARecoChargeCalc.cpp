/**
 *  @copyright Copyright 2016 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @file SDARecoChargeCalc.cpp
 */

#include "../../tools/JPetRecoSignalTools/JPetRecoSignalTools.h"
#include "SDARecoChargeCalc.h"

SDARecoChargeCalc::SDARecoChargeCalc(const char* name, const char* description) 
: JPetTask(name, description),
fBadSignals(0),
fCurrentEventNumber(0)
{}

SDARecoChargeCalc::~SDARecoChargeCalc(){}

void SDARecoChargeCalc::init(const JPetTaskInterface::Options&){
	INFO(Form("Starting charge calculation"));
	fBadSignals =0;
}

void SDARecoChargeCalc::exec(){
	if(auto signal = dynamic_cast<const JPetRecoSignal*const>(getEvent())){
		double charge = JPetRecoSignalTools::calculateAreaFromStartingIndex(*signal);
		if (charge == JPetRecoSignalTools::ERRORS::badCharge) {
			WARNING( Form("Something went wrong when calculating charge for event: %d", fCurrentEventNumber) );
			JPetRecoSignalTools::saveBadSignalIntoRootFile(*signal, fBadSignals, "badCharges.root");
			fBadSignals++;
		}else{
			auto signalWithCharge = *signal;
			signalWithCharge.setCharge(charge);
			fWriter->write(signalWithCharge);
		}
		fCurrentEventNumber++;
	}
}

void SDARecoChargeCalc::terminate(){
	int fEventNb = fCurrentEventNumber; 
	double goodPercent = (fEventNb-fBadSignals) * 100.0/fEventNb;
	INFO(Form("Charge` calculation complete \nAmount of bad signals: %d \n %f %% of data is good" , fBadSignals, goodPercent));
}
void SDARecoChargeCalc::setWriter(JPetWriter* writer){
	fWriter=writer;
}

