//
//
//****************************************************************************************//
// Copyright (c) 2002-2014, The MITRE Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice, this list
//       of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright notice, this 
//       list of conditions and the following disclaimer in the documentation and/or other
//       materials provided with the distribution.
//     * Neither the name of The MITRE Corporation nor the names of its contributors may be
//       used to endorse or promote products derived from this software without specific 
//       prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
// SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//****************************************************************************************//

#include <algorithm>
#include <iterator>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "ObjectFactory.h"
#include "Common.h"
#include "XmlCommon.h"

#include "Set.h"

using namespace std;
using namespace xercesc;

//****************************************************************************************//
//									Set Class											  //	
//****************************************************************************************//
Set::Set(DOMElement* setElm) {

	this->SetReferenceOne(NULL);
	this->SetReferenceTwo(NULL);
	this->SetSetOne(NULL);
	this->SetSetTwo(NULL);
	this->SetIsSimpleSet(true);
	this->Parse(setElm);
}

Set::Set() {

	this->SetReferenceOne(NULL);
	this->SetReferenceTwo(NULL);
	this->SetSetOne(NULL);
	this->SetSetTwo(NULL);
	this->SetIsSimpleSet(true);
}

Set::~Set() {

	while(!filters.empty()) {
	  	delete filters.back();
	  	filters.pop_back();
	}

	if(this->referenceOne != NULL) {
		delete(this->referenceOne);
	}
	if(this->referenceTwo != NULL) {
        delete(this->referenceTwo);
	}

	if(this->setOne != NULL) {
        delete(this->setOne);
	}
	if(this->setTwo != NULL){
		delete(this->setTwo);
	}
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//
FilterVector* Set::GetFilters() {
	return &this->filters;
}

void Set::SetFilters(FilterVector* filters) {
	this->filters = (*filters);
}

bool Set::GetIsSimpleSet() {
	return this->isSimpleSet;
}


void Set::SetIsSimpleSet(bool isSimpleSet) {
	this->isSimpleSet = isSimpleSet;	
}

AbsObject* Set::GetReferenceOne() {
	return this->referenceOne;
}

void Set::SetReferenceOne(AbsObject* object) {
	this->referenceOne = object;
}

AbsObject* Set::GetReferenceTwo() {
	return this->referenceTwo;
}

void Set::SetReferenceTwo(AbsObject* object) {
	this->referenceTwo = object;
}

Set* Set::GetSetOne() {
	return this->setOne;
}

void Set::SetSetOne(Set* set) {
	this->setOne = set;
}

Set* Set::GetSetTwo() {
	return this->setTwo;
}

void Set::SetSetTwo(Set* set) {
	this->setTwo = set;
}

OvalEnum::SetOperator Set::GetSetOperator() {
	return this->setOperator;
}

void Set::SetSetOperator(OvalEnum::SetOperator setOperator) {
	this->setOperator = setOperator;
}

void Set::AppendFilter(Filter* filter) {
	this->filters.push_back(filter);
}

VariableValueVector Set::GetVariableValues() {

	VariableValueVector varValues, tmpVarValues;
    
	if(this->GetIsSimpleSet()) {
		// get the variable values used in each filter 
		FilterVector::iterator iterator;
		for(iterator = this->GetFilters()->begin(); iterator != this->GetFilters()->end(); iterator++) {
			Filter* filter = *iterator;
			tmpVarValues = filter->GetVariableValues();
			// copy the state's var values to the set's vector of var values
			copy(tmpVarValues.begin(), tmpVarValues.end(), back_inserter(varValues));
		}

		// get the variable values used by reference one if it exists
		if(this->GetReferenceOne() != NULL) {
			tmpVarValues = this->GetReferenceOne()->GetVariableValues();
			copy(tmpVarValues.begin(), tmpVarValues.end(), back_inserter(varValues));
		}

		// get the variable values used by reference 2 if it exists
		if(this->GetReferenceTwo() != NULL) {
			tmpVarValues = this->GetReferenceTwo()->GetVariableValues();
			copy(tmpVarValues.begin(), tmpVarValues.end(), back_inserter(varValues));
		}

	} else {

		// Get the variable values used by set one if it exists
		if(this->GetSetOne() != NULL) {
			tmpVarValues = this->GetSetOne()->GetVariableValues();
			copy(tmpVarValues.begin(), tmpVarValues.end(), back_inserter(varValues));
		}

		// Get the variable values used by set two if it exists
		if(this->GetSetTwo() != NULL) {
			tmpVarValues = this->GetSetTwo()->GetVariableValues();
			copy(tmpVarValues.begin(), tmpVarValues.end(), back_inserter(varValues));
		}
	}

	return varValues;
}

void Set::Parse(DOMElement* setObjectElm) {

	string setOperatorStr = XmlCommon::GetAttributeByName(setObjectElm, "set_operator");
	this->SetSetOperator(OvalEnum::ToSetOperator(setOperatorStr));

	// loop over all child elements
	DOMNodeList *setObjectChildren = setObjectElm->getChildNodes();
	unsigned int index = 0;
	while(index < setObjectChildren->getLength()) {
		DOMNode *tmpNode = setObjectChildren->item(index);

		//	only concerned with ELEMENT_NODEs
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *setChild = (DOMElement*)tmpNode;

			//	get the name of the child
			string childName = XmlCommon::GetElementName(setChild);
			if(childName.compare("set") == 0) {

				this->SetIsSimpleSet(false);

				// create a new set object based on this element
				if(this->GetSetOne() == NULL) {
					Set* newSetObj = new Set(setChild);
					//newSetObj->SetIsSimpleSet(false);
					this->SetSetOne(newSetObj);
				} else {
					Set* newSetObj = new Set(setChild);
					//newSetObj->SetIsSimpleSet(false);
					this->SetSetTwo(newSetObj);
				}

			} else if(childName.compare("object_reference") == 0) {
				this->SetIsSimpleSet(true);
				string objId = XmlCommon::GetDataNodeValue(setChild);
				AbsObject* tmpObj = ObjectFactory::GetObjectById(objId);
				if(this->GetReferenceOne() == NULL) {
					this->SetReferenceOne(tmpObj);
				} else {
					this->SetReferenceTwo(tmpObj);
				}
			} else if(childName.compare("filter") == 0) {
				this->SetIsSimpleSet(true);
				Filter *tmpFilter = new Filter(setChild);
				this->AppendFilter(tmpFilter);
			}
		}

		index ++;
	}
}

//****************************************************************************************//
//								SetException Class										  //	
//****************************************************************************************//
SetException::SetException(string errMsgIn, int severity, Exception* ex) : Exception(errMsgIn, severity, ex) {

}

SetException::~SetException() {

}
