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

#ifndef LOCALVARIABLE_H
#define LOCALVARIABLE_H

#include <xercesc/dom/DOMElement.hpp>

#include "AbsVariable.h"
#include "ComponentFactory.h"
#include "VariableFactory.h"

/**
	This class represents an local_variable in an oval definition schema.
*/
class LocalVariable : public AbsVariable {
public:

	/** Create a complete LocalVariable. */
	LocalVariable(std::string id = "", std::string name = "local_variable", int version = 1, OvalEnum::Datatype datatype = OvalEnum::DATATYPE_STRING, StringVector* msgs = new StringVector())
		: AbsVariable (id, name, version, datatype, msgs),
		component(NULL)
	{}
	virtual ~LocalVariable() {
		if (component)
			delete component;
	}

	/** Parse the provided local_variable element into a LocalVariable. */
	virtual void Parse(xercesc::DOMElement* localVariableElm);

    /** Compute the value of the component.
        Create a VariableValue for each value in the returned ComponentValue
        if the flag is set to complete or incomplete. 
    */
	void ComputeValue();

    /** Return the variable values used to compute this variable's value.
        Here we can simply return the values used by the component.
    */
	virtual VariableValueVector GetVariableValues() const {
		return this->GetComponent()->GetVariableValues();
	}
	
	/** Get the AbsComponent. */
	AbsComponent* GetComponent() const {
		return component;
	}
	/** Set the AbsComponent. */
	void SetComponent(AbsComponent* component) {
		if (this->component)
			delete this->component;
		this->component = component;
	}

private:
	AbsComponent* component;
};

#endif
