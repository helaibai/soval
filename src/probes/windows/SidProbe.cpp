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

#include <iterator>
#include <algorithm>

#include "WindowsCommon.h"
#include "Log.h"
#include <VectorPtrGuard.h>

#include "SidProbe.h"

using namespace std;

//****************************************************************************************//
//								SidProbe Class											  //	
//****************************************************************************************//
SidProbe* SidProbe::instance = NULL;

SidProbe::SidProbe() : AbsProbe() {

}

SidProbe::~SidProbe() {
  instance = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Public Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
AbsProbe* SidProbe::Instance() {

	// Use lazy initialization
	if(instance == NULL) 
		instance = new SidProbe();

	return instance;	
}

ItemVector* SidProbe::CollectItems(Object *object) {
	// -----------------------------------------------------------------------
	//
	//  ABSTRACT
	//
	//  Run the sid probe. Return a vector of Items
	//
	//	if not using variables
	//		if operation == equals
	//			- call LookupAccountName and return collected item
	//				http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/lookupaccountname.asp
	//
	//		if operation == not equals || operation == pattern match
	//			- get all sids see note below...
	//			- determine set of sids that match the specified criteria
	//			- for each match call LookupAccountName and return collected item
	//				http://msdn.microsoft.com/library/default.asp?url=/library/en-us/secauthz/security/lookupaccountname.asp
	//
	//	Behaviors:
	//	The following behaviors are supported when collecting sid objects:
	//	 - include_group: should be paired with the "resolve_group" behavior.
	//	   When true, include the group in the set of sids. When false, do not
	//	   include the group in the set of sids.
	//
	//	 - resolve_group: when true, if the trustee name specifies a group 
	//	   then return all users in the group. When false just return sid 
	//	   for the group.
	//	
	//
	//
	//	NOTE:
	//	I have not yet found a way to get all the SIDs on a system. To handle this
	//	for now I am getting all the sids of the local users, local groups, and
	//	global groups. Then expanding all the groups found above to get all of their
	//	members. Additionally I look up all well known trustee names by their sid string
	//	to ensure all well know sids are captured. For now this will have to suffice as
	//	the set of all SIDs on a system. 
	//
	//	NOTE: 
	//	Trustee name is expected to be a fully qualified name like:
	//		domain_name\user_name
	//	or a well known account name like:
	//		Administrators, or SYSTEM, or Administrator, or Users
	//
	//	TODO: Ensure that a given account is only looked up once
	//		
	//	Get All Local users with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netuserenum.asp
	//	
	//	Get all global groups with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netgroupenum.asp
	//	Get global group members with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netgroupgetusers.asp
	//
	//	Get all local groups with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netlocalgroupenum.asp
	//	Get local group members with:
	//	http://msdn.microsoft.com/library/default.asp?url=/library/en-us/netmgmt/netmgmt/netlocalgroupgetmembers.asp
	//
	// -----------------------------------------------------------------------

	// get the trustee_name from the provided object
	ObjectEntity* trusteeName = object->GetElementByName("trustee_name");

	// check datatypes - only allow string
	if(trusteeName->GetDatatype() != OvalEnum::DATATYPE_STRING) {
		throw ProbeException("Error: invalid data type specified on trustee_name. Found: " + OvalEnum::DatatypeToString(trusteeName->GetDatatype()));
	}	

	VectorPtrGuard<Item> collectedItems(new ItemVector());

	// support behaviors - init with defaults.
	bool includeGroupBehavior = true;
	bool resolveGroupBehavior = false;
	if(object->GetBehaviors()->size() != 0) {
		BehaviorVector* behaviors = object->GetBehaviors();
		BehaviorVector::iterator iterator;
        for(iterator = behaviors->begin(); iterator != behaviors->end(); iterator++) {
			Behavior* behavior = (*iterator);
		    if(behavior->GetName().compare("include_group") == 0)  {
                if(behavior->GetValue().compare("false") == 0) {
			        includeGroupBehavior = false;
                }
            } else if(behavior->GetName().compare("resolve_group") == 0) {
                if(behavior->GetValue().compare("true") == 0) {
			        resolveGroupBehavior = true;
                }
		    } else {
                Log::Info("Unsupported behavior found when collecting " + object->GetId() + " Found behavior: " + behavior->GetName() + " = " + behavior->GetValue());
		    }
        }
	}

	StringVector specifiedTrusteeNames;
	StringVector candidateTrusteeNames;
	StringSet *allTrusteeNames;
	string normFormattedName;

	switch (trusteeName->GetOperation()) {
	case OvalEnum::OPERATION_EQUALS:
	case OvalEnum::OPERATION_CASE_INSENSITIVE_EQUALS:
		// In this case, we don't restrict ourselves to the narrower scope
		// implemented in WindowsCommon::GetAllTrusteeNames().  If the trustee
		// name matches and exists, we'll get the info.
		trusteeName->GetEntityValues(specifiedTrusteeNames);

		// normalize what the user gave us...
		for (StringVector::iterator iter = specifiedTrusteeNames.begin();
			iter != specifiedTrusteeNames.end();
			++iter) {
				if (WindowsCommon::NormalizeTrusteeName(*iter, NULL, NULL,
					&normFormattedName, NULL, NULL))
					candidateTrusteeNames.push_back(normFormattedName);
				else {
					Item* item = this->CreateItem();
					item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
					item->AppendElement(new ItemEntity("trustee_name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
					collectedItems->push_back(item);
				}
		}

		break;

	default:
		// We rely on GetAllTrusteeNames() returning properly formatted/
		// capitalized trustee names.  It also scopes searches narrowly,
		// to avoid hammering active directory servers.
		allTrusteeNames = WindowsCommon::GetAllTrusteeNames();
		copy(allTrusteeNames->begin(), allTrusteeNames->end(), back_inserter(candidateTrusteeNames));

		break;
	}

	ItemEntity trusteeNameIe("trustee_name");
	for (StringVector::iterator iter = candidateTrusteeNames.begin();
		iter != candidateTrusteeNames.end();
		++iter) {
		trusteeNameIe.SetValue(*iter);
		if (trusteeName->Analyze(&trusteeNameIe) == OvalEnum::RESULT_TRUE) {
			this->GetAccountInformation(*iter,
				resolveGroupBehavior, includeGroupBehavior, collectedItems.get());
		}
	}

	return collectedItems.release();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  Private Members  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Item* SidProbe::CreateItem() {

	Item* item = new Item(0, 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows", 
						"win-sc", 
						"http://oval.mitre.org/XMLSchema/oval-system-characteristics-5#windows windows-system-characteristics-schema.xsd", 
						OvalEnum::STATUS_ERROR, 
						"sid_item");

	return item;
}

void SidProbe::GetAccountInformation(const string &accountName,
	bool resolveGroupBehavior, bool includeGroupBehavior, ItemVector* items) {

	// lookup the trustee name
	string sidStr, normDomain;
	bool isGroup;
	if (!WindowsCommon::NormalizeTrusteeName(accountName, NULL, &normDomain, 
		NULL, &sidStr, &isGroup))
		return;

	try {

		// if a group
		// handle behaviors
		if(isGroup && resolveGroupBehavior) {

			if(includeGroupBehavior) {
				Item* item = this->CreateItem();
				item->SetStatus(OvalEnum::STATUS_EXISTS);
				item->AppendElement(new ItemEntity("trustee_name", accountName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("trustee_sid", sidStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				item->AppendElement(new ItemEntity("trustee_domain", normDomain, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
				items->push_back(item);
			} 
			
			// Get all the accounts in the group
			StringSet* groupMembers = new StringSet();
			WindowsCommon::ExpandGroup(accountName, groupMembers, includeGroupBehavior, resolveGroupBehavior);
			StringSet::iterator iterator;
			for(iterator = groupMembers->begin(); iterator != groupMembers->end(); iterator++) {
				// make recursive call...
				try {
					this->GetAccountInformation((*iterator), resolveGroupBehavior, includeGroupBehavior, items);
				} catch (Exception ex) {
					Log::Debug(ex.GetErrorMessage());
				}
			}
			delete groupMembers;

		} else {
			Item* item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_EXISTS);
			item->AppendElement(new ItemEntity("trustee_name", accountName, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("trustee_sid", sidStr, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			item->AppendElement(new ItemEntity("trustee_domain", normDomain, OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_EXISTS));
			items->push_back(item);
		}

	} catch(ProbeException ex) {
		// only way to have a notice level exception thrown here 
		// is for the account to not be found. In that case return an 
		// item with a status of does not exist.
		if(ex.GetSeverity() == ERROR_NOTICE) {
			Item* item = this->CreateItem();
			item->SetStatus(OvalEnum::STATUS_DOES_NOT_EXIST);
			item->AppendElement(new ItemEntity("trustee_name", "", OvalEnum::DATATYPE_STRING, OvalEnum::STATUS_DOES_NOT_EXIST));
			items->push_back(item);
		} else {
			throw;
		}
	}
}
