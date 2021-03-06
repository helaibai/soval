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

#ifndef ENDFUNCTION_H
#define ENDFUNCTION_H

#include <xercesc/dom/DOMElement.hpp>

#include "AbsFunctionComponent.h"
#include "ComponentFactory.h"

/**
	This class represents a EndFunction component in a local_variable in the oval definition schema.

    The end function takes a single string component and defines a character (or string) that the 
    component string should end with. The character attribute defines the specific character 
    (or string). The character (or string) is only added to the component string if the component 
    string doesn't already end with the specified character (or string).
*/
class EndFunction : public AbsFunctionComponent {
public:

	/** Create a complete EndFunction object. */
	EndFunction(std::string charIn = "") : character(charIn)
	{}
	virtual ~EndFunction()
	{}

	/** Parse the begin element and its child component element. */
	virtual void Parse(xercesc::DOMElement* componentElm); 

	/** Compute and return the value. */
	virtual ComponentValue* ComputeValue();

	/** Return the variable values used to compute this function's value. */
	virtual VariableValueVector GetVariableValues();

	/** Get the character field's value. */
	std::string GetCharacter() const {
		return this->character;
	}
	/** Set the character field's value. */
	void SetCharacter(std::string charIn) {
		this->character = charIn;
	}

private:
	std::string character;
};

#endif
