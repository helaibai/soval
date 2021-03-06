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

#ifndef ABSFUNCTIONCOMPONENT_H
#define ABSFUNCTIONCOMPONENT_H

#include "AbsComponent.h"

/**
	This class represents abstract function component in a local_variable in the oval definition schema.
	The oval definition schema defines a set of funtions for use in local_variables. All functions have 
	some common aspects. This calls encaplusates those commonalities.
*/
class AbsFunctionComponent : public AbsComponent {
public:

    /** Create a complete AbsFunctionComponent object. */
	AbsFunctionComponent()
	{}
	virtual ~AbsFunctionComponent()	{
		DestroyComponents();
	}

    /** Return the components field's value. */
	AbsComponentVector* GetComponents() {
		return &this->components;
	}
    /** Set the components field's value. */
	void SetComponents(AbsComponentVector* components) {
		DestroyComponents();
		this->components = (*components);
	}

    /** Append the input componenet to the list of componenets. */
	void AppendComponent(AbsComponent* component) {
		this->GetComponents()->push_back(component);
	}

private:
	void DestroyComponents() {
		for (AbsComponentVector::iterator iter = components.begin();
			iter != components.end();
			++iter)
			if (*iter)
				delete *iter;
	}

	AbsComponentVector components;
};

#endif
