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

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <string>
#include <vector>
#include <xercesc/dom/DOMDocument.hpp>

#include "Exception.h"

/**
	This class stores interface infocmetion as strings.
	the interface name, mac address and ip address are stored.
*/
class IfData {
public:
	IfData(){};
	~IfData(){};
	IfData(std::string ifn, std::string ipAddr, std::string macAddr) : ifName(ifn), ipAddress(ipAddr), macAddress(macAddr) {}

	std::string ifName;
	std::string ipAddress;
	std::string macAddress;
};

/**	
	A vector for storing interface data dobjects. 
	Stores only pointers to the objects. 
*/
typedef std::vector < IfData* > IfDataVector;

/**
	This class stores system info as defined in the oval system characteristics schema.
	A write method is provide for writing out the system infor element as defined in the oval 
	system characteristics schema.
*/
class SystemInfo {

public:
	/** Initialize data memebers. */
	SystemInfo();
	/** Delete all objects in the interfaces vector. */
	~SystemInfo();

	/** Write the system_info node to the sc file. */
	void Write(xercesc::DOMDocument *scDoc);
		
	std::string os_name;
	std::string os_version;
	std::string architecture;
	std::string primary_host_name;
	IfDataVector interfaces;
};

/**
	This class is responsible for collecting system information.
*/
class SystemInfoCollector {
	public:
		/** Run the system info collector. Return a SystemInfo object. */
		static SystemInfo* CollectSystemInfo();
		
	private:
		/** Get the OS name and version, the architecture, and the primary host name for the system. */
		static void GetOSInfo(SystemInfo*);

		/** Create a vector of IfData object that will represent all the available
		 *	interfaces on the system. 		
	     *	Must get interface_name, ip_address, and mac_address for each interface
         */
		static IfDataVector GetInterfaces();
};

/** 
	This class represents an Exception that occured while collecting system info.
*/
class SystemInfoException : public Exception {
	public:
		SystemInfoException(std::string errMsgIn = "", int severity = ERROR_FATAL, Exception* ex = NULL);
		~SystemInfoException();
};

#endif
