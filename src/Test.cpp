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

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "Log.h"
#include "DocumentManager.h"
#include "XmlCommon.h"
#include "Common.h"

#include "Test.h"

using namespace std;
using namespace xercesc;

TestMap Test::processedTestsMap;
//****************************************************************************************//
//								Test Class												  //	
//****************************************************************************************//
Test::Test() {

	this->SetId("");
	this->SetResult(OvalEnum::RESULT_ERROR);
	this->SetVariableInstance(1);
	this->SetVersion(1);
	this->SetWritten(false);
	this->SetAnalyzed(false);
	this->SetCheckExistence(OvalEnum::EXISTENCE_ALL_EXIST);
	this->SetCheck(OvalEnum::CHECK_ALL);
    this->SetStateOperator(OvalEnum::OPERATOR_AND);
	this->SetObjectId("");
}

Test::~Test() {
	
	TestedItem* item = NULL;
	while(this->testedItems.size() != 0) {
		item = this->testedItems[this->testedItems.size()-1];
	  	this->testedItems.pop_back();
	  	delete item;
	  	item = NULL;
	}

	OvalMessage* currentMessage = NULL;
	while(this->GetMessages()->size() != 0) {
		currentMessage = this->GetMessages()->at(this->GetMessages()->size()-1);
		this->GetMessages()->pop_back();
		delete currentMessage;
		currentMessage = NULL;
	}

    this->stateIds.clear();
}

// ***************************************************************************************	//
//								 Public members												//
// ***************************************************************************************	//

OvalEnum::Check Test::GetCheck() {
	return this->check;
}

void Test::SetCheck(OvalEnum::Check check) {
	this->check = check;
}

OvalEnum::Existence Test::GetCheckExistence() {
	return this->checkExistence;
}

void Test::SetCheckExistence(OvalEnum::Existence checkExistence) {
	this->checkExistence = checkExistence;
}

OvalEnum::Operator Test::GetStateOperator() {
	return this->stateOperator;
}

void Test::SetStateOperator(OvalEnum::Operator stateOperator) {
	this->stateOperator = stateOperator;
}

TestedItemVector* Test::GetTestedItems() {
	return &this->testedItems;
}

void Test::SetTestedItems(TestedItemVector* testedItems) {
	this->testedItems = (*testedItems);
}

void Test::AppendTestedItem(TestedItem* testedItem) {
	this->GetTestedItems()->push_back(testedItem);
}

void Test::AppendTestedVariable(const VariableValue &testedVariable) {
	
	this->testedVariables.push_back(testedVariable);
}

string Test::GetId() {
	
	return this->id;
}

void Test::SetId(string id) {

	this->id = id;
}

string Test::GetObjectId() {

	return this->objectId;
}

void Test::SetObjectId(string objectId) {

	this->objectId = objectId;
}

Object* Test::GetReferencedObject() {

    return Object::GetObjectById(this->GetObjectId());
}

StringSet* Test::GetStateIds() {

	return &this->stateIds;
}

void Test::SetStateIds(StringSet stateIds) {

	this->stateIds = stateIds;
}

void Test::AppendStateId(string stateId) {
    this->stateIds.insert(stateId);
}

bool Test::HasStateReference() {
    if(this->stateIds.size() == 0 ) {
        return false;
    } else {
        return true;
    }
}

string Test::GetName() {

	return this->name;
}

void Test::SetName(string name) {

	this->name = name;
}

OvalEnum::ResultEnumeration Test::GetResult() {

	return this->result;
}

void Test::SetResult(OvalEnum::ResultEnumeration result) {

	this->result = result;
}


OvalMessageVector* Test::GetMessages() {
	return &this->testMessages;
}


void Test::AppendMessage(OvalMessage* message) {
	this->GetMessages()->push_back(message);
}



int Test::GetVariableInstance() {

	return this->variableInstance;
}

void Test::SetVariableInstance(int variableInstance) {

	this->variableInstance = variableInstance;
}

int Test::GetVersion() {

	return this->version;
}

void Test::SetVersion(int version) {

	this->version = version;
}

bool Test::GetWritten() {

	return this->written;
}

void Test::SetWritten(bool written) {

	this->written = written;
}

bool Test::GetAnalyzed() {

	return this->analyzed;
}

void Test::SetAnalyzed(bool analyzed) {

	this->analyzed = analyzed;
}

Test* Test::SearchCache(string id) {

	Test* cachedTest = NULL;

	TestMap::iterator iterator;
	iterator = Test::processedTestsMap.find(id);
	if(iterator != Test::processedTestsMap.end()) {
		cachedTest = iterator->second;
	} 

	return cachedTest;
}

void Test::ClearCache() {

	TestMap::iterator iterator;
	for(iterator = Test::processedTestsMap.begin(); iterator != Test::processedTestsMap.end(); iterator++) {
		
		Test* test = iterator->second;
		delete test;
	}
	
	Test::processedTestsMap.clear();
}

void Test::Write(DOMElement* parentElm) {

	if(!this->GetWritten()) {
		this->SetWritten(true);

		// get the parent document
		DOMDocument* resultDoc = parentElm->getOwnerDocument();

		// create a new Test element
		DOMElement* testElm = XmlCommon::AddChildElementNS(resultDoc, parentElm, XmlCommon::resNS, "test");

		// add the attributes
		XmlCommon::AddAttribute(testElm, "test_id", this->GetId());
		XmlCommon::AddAttribute(testElm, "version", Common::ToString(this->GetVersion()));
		XmlCommon::AddAttribute(testElm, "check_existence", OvalEnum::ExistenceToString(this->GetCheckExistence()));
		XmlCommon::AddAttribute(testElm, "check", OvalEnum::CheckToString(this->GetCheck()));
        XmlCommon::AddAttribute(testElm, "state_operator", OvalEnum::OperatorToString(this->GetStateOperator()));
		XmlCommon::AddAttribute(testElm, "result", OvalEnum::ResultToString(this->GetResult()));

		if(this->GetVariableInstance() != 1) {
			XmlCommon::AddAttribute(testElm, "variable_instance", Common::ToString(this->GetVariableInstance()));
		}	

		OvalMessage* currentMessage = NULL;
		unsigned int sizeOfMessageList = this->GetMessages()->size();
		unsigned int msgCounter = 0;
		while(msgCounter < sizeOfMessageList) {
			currentMessage = this->GetMessages()->at(msgCounter);
			currentMessage->Write(resultDoc,testElm,"oval-res", XmlCommon::resNS);
			msgCounter++;
		}

		TestedItem* currentElement = NULL;
		unsigned int sizeOfItemList = this->GetTestedItems()->size();
		unsigned int itemCounter = 0;
		while(itemCounter < sizeOfItemList) {
			currentElement = this->GetTestedItems()->at(itemCounter);
			currentElement->Write(testElm);	  		
	  		itemCounter++;
		}

		// loop through all variable values and call write method
		VariableValueVector::iterator iterator1;
		for(iterator1 = this->testedVariables.begin(); iterator1 != this->testedVariables.end(); iterator1++) {
			iterator1->WriteTestedVariable(testElm);
		}

		// loop through all vars in the states
        for(StringSet::iterator it = this->GetStateIds()->begin(); it != this->GetStateIds()->end(); it++) {
	    
		    State* tmpState = State::SearchCache((*it));
		    if(tmpState != NULL) { 
			    VariableValueVector::iterator iterator2;
			    VariableValueVector stateVars = tmpState->GetVariableValues();
			    for(iterator2 = stateVars.begin(); iterator2 != stateVars.end(); iterator2++) {
				    iterator2->WriteTestedVariable(testElm);
			    }
		    }		    
        }
	}
}

void Test::Parse(DOMElement* testElm) {

	// get id
	string id = XmlCommon::GetAttributeByName(testElm, "id");

	// get the attributes
	this->SetId(XmlCommon::GetAttributeByName(testElm, "id"));
	this->SetName(XmlCommon::GetElementName(testElm));
	int vers = 0;
	Common::FromString(XmlCommon::GetAttributeByName(testElm, "version"), &vers);
	this->SetVersion(vers);
	this->SetCheckExistence(OvalEnum::ToExistence(XmlCommon::GetAttributeByName(testElm, "check_existence")));
	this->SetCheck(OvalEnum::ToCheck(XmlCommon::GetAttributeByName(testElm, "check")));
    this->SetStateOperator(OvalEnum::ToOperator(XmlCommon::GetAttributeByName(testElm, "state_operator")));
    

	// to support version 5.3 it is best to just look for the deprected check = none exist 
	// and replace it with the correct pair of check = any and check_existence = none_exist
	if(this->GetCheck() == OvalEnum::CHECK_NONE_EXIST) {
		Log::Info("Converting deprected check=\'none exist\' attribute value to check_existence=\'none_exist\' and check=\'none satisfy\'. The \'none exist\' CheckEnumeration value has been deprecated and will be removed with the next major version of the language. One should use the other possible values in addition to the existence attributes instead of the \'none exist\' value here.");
		this->SetCheckExistence(OvalEnum::EXISTENCE_NONE_EXIST);
		this->SetCheck(OvalEnum::CHECK_NONE_SATISFY);
		this->AppendMessage(new OvalMessage("Converting deprecated check=\'none exist\' attribute value to check_existence=\'none_exist\' and check=\'none satisfy\'. The \'none exist\' CheckEnumeration value has been deprecated and will be removed with the next major version of the language. One should use the other possible values in addition to the existence attributes instead of the \'none exist\' value here."));
	}

	// get the object element and the object id if it exists
	DOMElement* objectElm = XmlCommon::FindElementNS(testElm, "object");
	if(objectElm != NULL) {
		this->SetObjectId(XmlCommon::GetAttributeByName(objectElm, "object_ref"));
	}
    
	// get the state elements and the state id if it exists
    DOMNodeList *testElmChildren = testElm->getChildNodes();
	unsigned int index = 0;
	while(index < testElmChildren->getLength()) {
		DOMNode *tmpNode = testElmChildren->item(index);

		//	only concerned with ELEMENT_NODEs
		if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
			DOMElement *testChildElm = (DOMElement*)tmpNode;
			
			//	get the name of the child
			string childName = XmlCommon::GetElementName(testChildElm);
			if(childName.compare("state") == 0) {				
				// get the state's id
				string stateId = XmlCommon::GetAttributeByName(testChildElm, "state_ref");
                this->AppendStateId(stateId);
			} 
		}
		index ++;
	}

	Test::Cache(this);
}

OvalEnum::ResultEnumeration Test::Analyze() {

	if(!this->GetAnalyzed()) {

		// Does the test have a object ref?
		if(this->GetObjectId().compare("") == 0) {
			// Assumes it is only unknown tests that do not have an object specifier and sets result to unknown
			this->SetResult(OvalEnum::RESULT_UNKNOWN);
		} else {
			// get the collected object from the sc file
			DOMElement* collectedObjElm = XmlCommon::FindElement(DocumentManager::GetSystemCharacteristicsDocument(), "object", "id", this->GetObjectId());
			OvalEnum::Flag collectedObjFlag = OvalEnum::FLAG_NOT_COLLECTED;

			if(collectedObjElm == NULL) {
				
                // If there are no collected objects available, the interpreter will try to find corresponding
				// items in the system_data section.
                Log::Info(" Note: No collected objects found for test " + this->GetId() + ". Assuming that the input system characteristics file is complete. Searching system_data with in the input system characteristics.");


                // Get the component name from the first part of the test name
				// NOTE: Due to the inconsistent OVAL object, test, items names this won't work for inetlisteningserver(s)
				string componentName;
                string::size_type loc = this->name.find_last_of('_');
				if( loc != string::npos ) {
					componentName = this->name.substr(0, loc);
				}

				// Find potential matching items in the system_data section
				ElementVector* dataElems = XmlCommon::FindAllElements(DocumentManager::GetSystemCharacteristicsDocument(), componentName + "_item");
                if(dataElems->size() == 0) {
					
                    // No potential matching items found
					collectedObjFlag = OvalEnum::FLAG_NOT_COLLECTED;
					this->SetResult(OvalEnum::RESULT_UNKNOWN);

				} else {

                    /**
                     *  Loop through all potentially matching items.
                     *  Turn each potential match into an Item
                     *  Than call the Analyze() method on the current Object. 
                     *  If the return is a TRUE result then the Item should be considered to be a match for the Object
                     *  The loop here needs to identify ALL matches so it will loop through all possible matches all the time.
                     *  If any matches are found it will be assumed that the collected Object flag should be complete.
                     *  If no matches are found it is assumed that no items on the system were found that matched the Object.
                     *  
                     */

                    // get the object referenced by the test.
                    Object* referencedObject = this->GetReferencedObject();
                    
				    ElementVector::iterator iterator;
                    for(iterator = dataElems->begin(); iterator != dataElems->end(); iterator++) {
                        DOMElement *itemElm = (*iterator);
                        string itemId = XmlCommon::GetAttributeByName(itemElm, "id");

                        // get the element as an item
                        Item* currentItem = Item::GetItemById(itemId);

                        if(referencedObject->Analyze(currentItem) == true) {
                            TestedItem* testedItem = new TestedItem();
                            testedItem->SetItem(currentItem);
                            this->AppendTestedItem(testedItem);
                        }
                    }
                }

                if(this->GetTestedItems()->size() > 0) {
                    collectedObjFlag = OvalEnum::FLAG_COMPLETE;
                } else {
                    collectedObjFlag = OvalEnum::FLAG_DOES_NOT_EXIST;
                }		

			// The collected object was not NULL
			} else {

				// get the flag on the collected object
				string flagStr = XmlCommon::GetAttributeByName(collectedObjElm, "flag");
				collectedObjFlag = OvalEnum::ToFlag(flagStr);

				// Copy all variables in the collected object into VariableValues for the results file
				// Copy all item references into TestedItems for the results file
				// loop over all child elements and call tested object
				DOMNodeList *collectedObjChildren = collectedObjElm->getChildNodes();
				unsigned int index = 0;
				while(index < collectedObjChildren->getLength()) {
					DOMNode *tmpNode = collectedObjChildren->item(index);

					//	only concerned with ELEMENT_NODEs
					if (tmpNode->getNodeType() == DOMNode::ELEMENT_NODE) {
						DOMElement *collectedObjChildElm = (DOMElement*)tmpNode;
						
						//	get the name of the child
						string childName = XmlCommon::GetElementName(collectedObjChildElm);
						if(childName.compare("reference") == 0) {
							
							// get the reference's id
							string itemId = XmlCommon::GetAttributeByName(collectedObjChildElm, "item_ref");
							
							// create a new tested item
						    TestedItem* testedItem = new TestedItem();
							Item* item = Item::GetItemById(itemId);
							testedItem->SetItem(item);

							this->AppendTestedItem(testedItem);
						
						} else if(childName.compare("variable_value") == 0) {
							// create a new tested variable
							VariableValue testedVar;
							testedVar.Parse(collectedObjChildElm);
							this->AppendTestedVariable(testedVar);
						} 
					}
					index ++;
				}
            }

			// determine how to proceed based on flag value
			if(collectedObjFlag == OvalEnum::FLAG_ERROR) {
				
				// the result should be error unless the check existence is set to any exist
				// and the test does not have a state reference. In this case the result should be true
				if(this->GetCheckExistence() == OvalEnum::EXISTENCE_ANY_EXIST && !this->HasStateReference()) {
					
					this->SetResult(OvalEnum::RESULT_TRUE);

				} else {
					this->SetResult(OvalEnum::RESULT_ERROR);
				}

				// since we did not look at the state set the tested item result to not evaluated
				this->MarkTestedItemsNotEvaluated();

			} else if(collectedObjFlag == OvalEnum::FLAG_NOT_APPLICABLE) {

				// the result should be not applicable unless the check existence is set to any exist
				// and the test does not have a state reference. In this case the result should be true
				if(this->GetCheckExistence() == OvalEnum::EXISTENCE_ANY_EXIST && !this->HasStateReference()) {
					
					this->SetResult(OvalEnum::RESULT_TRUE);

				} else {
					this->SetResult(OvalEnum::RESULT_NOT_APPLICABLE);
				}

				// since we did not look at the state set the tested item result to not evaluated
				this->MarkTestedItemsNotEvaluated();

			} else if(collectedObjFlag == OvalEnum::FLAG_NOT_COLLECTED) {

				// the result should be unknown unless the check existence is set to any exist
				// and the test does not have a state reference. In this case the result should be true
				if(this->GetCheckExistence() == OvalEnum::EXISTENCE_ANY_EXIST && !this->HasStateReference()) {
					
					this->SetResult(OvalEnum::RESULT_TRUE);

				} else {
					this->SetResult(OvalEnum::RESULT_UNKNOWN);
				}
				
				// since we did not look at the state set the tested item result to not evaluated
				this->MarkTestedItemsNotEvaluated();

			} else if(collectedObjFlag == OvalEnum::FLAG_INCOMPLETE) {

				OvalEnum::ResultEnumeration overallResult = OvalEnum::RESULT_UNKNOWN;

				// get the count of items with a status of exists
				int existsCount = 0;
				TestedItemVector::iterator iterator;
				for(iterator = this->GetTestedItems()->begin(); iterator != this->GetTestedItems()->end(); iterator++) {
					OvalEnum::SCStatus itemStatus = (*iterator)->GetItem()->GetStatus();
					if(itemStatus == OvalEnum::STATUS_EXISTS) {
						existsCount++;
					} 
				}

				OvalEnum::ResultEnumeration existenceResult = OvalEnum::RESULT_UNKNOWN;

				if(this->GetCheckExistence() == OvalEnum::EXISTENCE_NONE_EXIST && existsCount > 0) {

					// if more than 0 then false	
					existenceResult = OvalEnum::RESULT_FALSE;

				} else if(this->GetCheckExistence() == OvalEnum::EXISTENCE_ONLY_ONE_EXISTS && existsCount > 1) {
					
					// if more than 1 then false					
					existenceResult = OvalEnum::RESULT_FALSE;

				} else if(this->GetCheckExistence() == OvalEnum::EXISTENCE_AT_LEAST_ONE_EXISTS && existsCount > 0) {

					// if more than 0 then true					
					existenceResult = OvalEnum::RESULT_TRUE;

				} else if(this->GetCheckExistence() == OvalEnum::EXISTENCE_ANY_EXIST) {

					// always true				
					existenceResult = OvalEnum::RESULT_TRUE;

				} 

				if(existenceResult == OvalEnum::RESULT_TRUE) {

					// consider the check_state if true so far...
					OvalEnum::ResultEnumeration stateResult = this->EvaluateCheckState();

					if(stateResult == OvalEnum::RESULT_FALSE) {
						overallResult = OvalEnum::RESULT_FALSE;
					} else if(stateResult == OvalEnum::RESULT_TRUE && this->GetCheck() == OvalEnum::CHECK_AT_LEAST_ONE) {

						overallResult = OvalEnum::RESULT_TRUE;
					}

				} else {
					overallResult =	existenceResult;

					// since we did not look at the state set the tested item result to not evaluated
					this->MarkTestedItemsNotEvaluated();
				}

				this->SetResult(overallResult);

			} else if(collectedObjFlag == OvalEnum::FLAG_DOES_NOT_EXIST) {

				// if the check_existence is set to none_exist or 
				// any_exist the result is true
				// otherwise the result is false
				if(this->GetCheckExistence() == OvalEnum::EXISTENCE_NONE_EXIST || this->GetCheckExistence() == OvalEnum::EXISTENCE_ANY_EXIST) {
					this->SetResult(OvalEnum::RESULT_TRUE);

					// since we did not look at the state set the tested item result to not evaluated
					this->MarkTestedItemsNotEvaluated();

				} else {
					this->SetResult(OvalEnum::RESULT_FALSE);
				}

			} else if(collectedObjFlag == OvalEnum::FLAG_COMPLETE) {
				
				OvalEnum::ResultEnumeration overallResult = OvalEnum::RESULT_ERROR;

				// Evaluate the check existence attribute.
				OvalEnum::ResultEnumeration existenceResult = this->EvaluateCheckExistence();

				// if the existence result is true evaluate the check_state attribute if there is a state
				if(existenceResult == OvalEnum::RESULT_TRUE) {
					if(this->HasStateReference()) {
						overallResult = this->EvaluateCheckState();
					} else {
						overallResult = existenceResult;

						// since we did not look at the state set the tested item result to not evaluated
						this->MarkTestedItemsNotEvaluated();
					}

				} else {
					overallResult = existenceResult;

					// since we did no look at the state set the tested item result to not evaluated
                    this->MarkTestedItemsNotEvaluated();
				}

                this->SetResult(overallResult);

            }				
		}
		this->SetAnalyzed(true);
	}

	return this->GetResult();
}

OvalEnum::ResultEnumeration Test::NotEvaluated() {

	if(!this->GetAnalyzed()) {
		this->SetResult(OvalEnum::RESULT_NOT_EVALUATED);
		this->SetAnalyzed(true);
	}

	return this->GetResult();
}

OvalEnum::ResultEnumeration Test::EvaluateCheckExistence() {

	OvalEnum::ResultEnumeration existenceResult = OvalEnum::RESULT_ERROR;

	// get the count of each status value
	int errorCount = 0;
	int existsCount = 0;
	int doesNotExistCount = 0;
	int notCollectedCount = 0;

	TestedItemVector::iterator iterator;
	for(iterator = this->GetTestedItems()->begin(); iterator != this->GetTestedItems()->end(); iterator++) {
		OvalEnum::SCStatus itemStatus = (*iterator)->GetItem()->GetStatus();
		if(itemStatus == OvalEnum::STATUS_ERROR) {
			errorCount++;
		} else if(itemStatus == OvalEnum::STATUS_EXISTS) {
			existsCount++;
		} else if(itemStatus == OvalEnum::STATUS_DOES_NOT_EXIST) {
			doesNotExistCount++;
		} else if(itemStatus == OvalEnum::STATUS_NOT_COLLECTED) {
			notCollectedCount++;
		} 
	}

	if(this->GetCheckExistence() == OvalEnum::EXISTENCE_ALL_EXIST) {

		if(existsCount >= 1 && doesNotExistCount == 0 && errorCount == 0 && notCollectedCount == 0) {
			existenceResult = OvalEnum::RESULT_TRUE;			
		} else if(existsCount >= 0 && doesNotExistCount >= 1 && errorCount >= 0 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_FALSE;
		} else if(existsCount == 0 && doesNotExistCount == 0 && errorCount == 0 && notCollectedCount == 0) {
			existenceResult = OvalEnum::RESULT_FALSE;
		} else if(existsCount >= 0 && doesNotExistCount == 0 && errorCount >= 1 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_ERROR;
		} else if(existsCount >= 0 && doesNotExistCount == 0 && errorCount == 0 && notCollectedCount >= 1) {
			existenceResult = OvalEnum::RESULT_UNKNOWN;
		} else {
			string msg = "Unexpected set of item statuses found while evaluating the check existence value for a test. check_existence='all_exist' Found";
			msg.append(" exists count=" + Common::ToString(existsCount));
			msg.append(" does not exist count=" + Common::ToString(doesNotExistCount));
			msg.append(" error count=" + Common::ToString(errorCount));
			msg.append(" not collected count=" + Common::ToString(notCollectedCount));
			Log::Info(msg);
		}
		
	} else if(this->GetCheckExistence() == OvalEnum::EXISTENCE_ANY_EXIST) {
		
		if(existsCount >= 0 && doesNotExistCount >= 0 && errorCount == 0 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_TRUE;	
		} else if(existsCount >= 1 && doesNotExistCount >= 0 && errorCount >= 1 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_TRUE;	
		} else if(existsCount == 0 && doesNotExistCount >= 0 && errorCount >= 1 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_ERROR;
		} else {
			string msg = "Unexpected set of item statuses found while evaluating the check existence value for a test. check_existence='any_exist' Found";
			msg.append(" exists count=" + Common::ToString(existsCount));
			msg.append(" does not exist count=" + Common::ToString(doesNotExistCount));
			msg.append(" error count=" + Common::ToString(errorCount));
			msg.append(" not collected count=" + Common::ToString(notCollectedCount));
			Log::Info(msg);
		}

	} else if(this->GetCheckExistence() == OvalEnum::EXISTENCE_AT_LEAST_ONE_EXISTS) {
		
		if(existsCount >= 1 && doesNotExistCount >= 0 && errorCount >= 0 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_TRUE;			
		} else if(existsCount == 0 && doesNotExistCount >= 1 && errorCount == 0 && notCollectedCount == 0) {
			existenceResult = OvalEnum::RESULT_FALSE;
		} else if(existsCount == 0 && doesNotExistCount >= 0 && errorCount >= 1 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_ERROR;
		} else if(existsCount == 0 && doesNotExistCount >= 0 && errorCount == 0 && notCollectedCount >= 1) {
			existenceResult = OvalEnum::RESULT_UNKNOWN;
		} else {
			string msg = "Unexpected set of item statuses found while evaluating the check existence value for a test. check_existence='at_least_one_exists' Found";
			msg.append(" exists count=" + Common::ToString(existsCount));
			msg.append(" does not exist count=" + Common::ToString(doesNotExistCount));
			msg.append(" error count=" + Common::ToString(errorCount));
			msg.append(" not collected count=" + Common::ToString(notCollectedCount));
			Log::Info(msg);
		}

	} else if(this->GetCheckExistence() == OvalEnum::EXISTENCE_NONE_EXIST) {
		
		if(existsCount == 0 && doesNotExistCount >= 0 && errorCount == 0 && notCollectedCount == 0) {
			existenceResult = OvalEnum::RESULT_TRUE;			
		} else if(existsCount >= 1 && doesNotExistCount >= 0 && errorCount >= 0 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_FALSE;
		} else if(existsCount == 0 && doesNotExistCount >= 0 && errorCount >= 1 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_ERROR;
		} else if(existsCount == 0 && doesNotExistCount >= 0 && errorCount == 0 && notCollectedCount >= 1) {
			existenceResult = OvalEnum::RESULT_UNKNOWN;
		} else {
			string msg = "Unexpected set of item statuses found while evaluating the check existence value for a test. check_existence='none_exist' Found";
			msg.append(" exists count=" + Common::ToString(existsCount));
			msg.append(" does not exist count=" + Common::ToString(doesNotExistCount));
			msg.append(" error count=" + Common::ToString(errorCount));
			msg.append(" not collected count=" + Common::ToString(notCollectedCount));
			Log::Info(msg);
		}

	} else if(this->GetCheckExistence() == OvalEnum::EXISTENCE_ONLY_ONE_EXISTS) {
		
		if(existsCount == 1 && doesNotExistCount >= 0 && errorCount == 0 && notCollectedCount == 0) {
			existenceResult = OvalEnum::RESULT_TRUE;			
		} else if(existsCount >= 2 && doesNotExistCount >= 0 && errorCount >= 0 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_FALSE;
		} else if(existsCount == 0 && doesNotExistCount >= 0 && errorCount == 0 && notCollectedCount == 0) {
			existenceResult = OvalEnum::RESULT_FALSE;
		} else if(existsCount == 0 && doesNotExistCount >= 0 && errorCount >= 1 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_ERROR;
		} else if(existsCount == 1 && doesNotExistCount >= 0 && errorCount >= 1 && notCollectedCount >= 0) {
			existenceResult = OvalEnum::RESULT_ERROR;
		} else if(existsCount == 0 && doesNotExistCount >= 0 && errorCount == 0 && notCollectedCount >= 1) {
			existenceResult = OvalEnum::RESULT_UNKNOWN;
		} else if(existsCount == 1 && doesNotExistCount >= 0 && errorCount == 0 && notCollectedCount >= 1) {
			existenceResult = OvalEnum::RESULT_UNKNOWN;
		} else {
			string msg = "Unexpected set of item statuses found while evaluating the check existence value for a test. check_existence='only_one_exists' Found";
			msg.append(" exists count=" + Common::ToString(existsCount));
			msg.append(" does not exist count=" + Common::ToString(doesNotExistCount));
			msg.append(" error count=" + Common::ToString(errorCount));
			msg.append(" not collected count=" + Common::ToString(notCollectedCount));
			Log::Info(msg);
		}
	}

	return existenceResult;
}

OvalEnum::ResultEnumeration Test::EvaluateCheckState() {

	OvalEnum::ResultEnumeration stateResult = OvalEnum::RESULT_ERROR;

	// is there a state associated with this test?
	if(!this->HasStateReference()) {
		// no state specified
		// just report true...
		stateResult = OvalEnum::RESULT_TRUE;

	} else {

        string currentStateId = "";
		try {
		    // analyze each tested item
		    IntVector itemResults;
		    for(TestedItemVector::iterator iterator = this->GetTestedItems()->begin(); iterator != this->GetTestedItems()->end(); iterator++) {

                // Compare the item to each state
                IntVector stateResults;
                for(StringSet::iterator it = this->GetStateIds()->begin(); it != this->GetStateIds()->end(); it++) {
                    currentStateId = (*it);
		            State* state =  State::GetStateById(currentStateId);
			        OvalEnum::ResultEnumeration stateResult = state->Analyze((*iterator)->GetItem());				        
			        stateResults.push_back(stateResult);
                }
                
                // combine results based on the state_operator attribute
                OvalEnum::ResultEnumeration itemResult = OvalEnum::CombineResultsByOperator(&stateResults, this->GetStateOperator());
                (*iterator)->SetResult(itemResult);
                itemResults.push_back(itemResult);
		    }

		    // combine results based on the check attribute
		    stateResult = OvalEnum::CombineResultsByCheck(&itemResults, this->GetCheck());

		} catch(Exception ex) {
			this->SetResult(OvalEnum::RESULT_ERROR);
			Log::Fatal("Unable to evaluate test " + this->GetId() + ". An error occured while processing the associated state " + currentStateId + ". " + ex.GetErrorMessage());
		}
	}

	return stateResult;
}

void Test::Cache(Test* test) {

	Test::processedTestsMap.insert(TestPair(test->GetId(), test));
}

Test* Test::GetTestById(string testId) {

	Test* test = NULL;
	
	// Search the cache
	test = Test::SearchCache(testId);

	// if not found try to parse it.
	if(test == NULL) {

		DOMElement* testsElm = XmlCommon::FindElementNS(DocumentManager::GetDefinitionDocument(),"tests");
		DOMElement* testElm = XmlCommon::FindElementByAttribute(testsElm, "id", testId);

		if(testElm == NULL) {
			throw Exception("Unable to find specified test in oval-definition document. Test id: " + testId);
		}

		test = new Test();
		test->Parse(testElm);
	}
	
	return test;
}

void Test::MarkTestedItemsNotEvaluated() {
    
    TestedItemVector::iterator iterator;
	for(iterator = this->GetTestedItems()->begin(); iterator != this->GetTestedItems()->end(); iterator++) {
		(*iterator)->SetResult(OvalEnum::RESULT_NOT_EVALUATED);
	}
}
