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

#ifndef POSSIBLERESTRICTIONTYPE_H
#define POSSIBLERESTRICTIONTYPE_H

#include <string>
#include <vector>

#include <xercesc/dom/DOMElement.hpp>

#include "OvalEnum.h"
#include "RestrictionType.h"

class PossibleRestrictionType;

typedef std::vector < PossibleRestrictionType* > PossibleRestrictionTypeVector;

/**
	This class represents an the PossibleRestrictionType related to external variables in the oval definition schema.
*/
class PossibleRestrictionType {
public:

	/** Create a new PossibleRestrictionType. */
	PossibleRestrictionType()
	{}
	~PossibleRestrictionType()
	{}

	void SetHint(std::string hint) {
		this->hint = hint;
	}
	std::string GetHint() const {
		return this->hint;
	}

	/** Parses a valid PossibleRestrictionType element as defined int eh oval definitions schema. */
	void Parse(xercesc::DOMElement* possibleRestrictionElm);

	/** Ensure that the specified value matches the criteria specified by this possible_restriction element. */
	bool ValidateValue(OvalEnum::Datatype datatype, std::string externalValue);

	const RestrictionTypeVector* GetRestrictionTypes() const {
		return &this->restrictionTypes;
	}
	void AppendRestrictionType(RestrictionType* rt) {
		this->restrictionTypes.push_back(rt);
	}

private:

	std::string hint;
	RestrictionTypeVector restrictionTypes;
};


#endif
