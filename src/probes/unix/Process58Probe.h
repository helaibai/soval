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

#ifndef PROCESS58PROBE_H
#define PROCESS58PROBE_H

#include "AbsProbe.h"

#include <cerrno>
#include <strings.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#ifdef SUNOS
#include <procfs.h>
#include <ftw.h>
#include <algorithm>
#include <cctype>
#endif

#ifdef DARWIN
#include <sys/sysctl.h>
#endif

// Define some buffer lengths
#define CMDLINE_LEN 1024
#define TTY_LEN PATH_MAX

/**
	Data collector for process test.

	The following command produces process information suitable for this probe.
    ps -eo class,etime,pid,uid,start_time,tty,priority,cmd
*/
class Process58Probe : public AbsProbe {
public:
	virtual ~Process58Probe();
    
	virtual ItemVector* CollectItems(Object* object);

	/** Ensure that the Process58Probe is a singleton. */
	static AbsProbe* Instance();
	  
private:

	Process58Probe();

	/** Return a new Item created for storing process information. */
	virtual Item* CreateItem();

	/** 
		Return the set of all process commands on the ssytem that match the specified object entities criteria.
		All process command names that are return have been checked and exist on the syytem.
		@param command an ObjectEntity* that represents the objects to collect on the ssytem
		@return The matching commands
	*/
	StringPairVector* GetCommands(ObjectEntity *pid, ObjectEntity* command);
	
	/**
		Get all commands on the system that match the specified pattern.
		@param pattern a string used that commands are compared against.
		@param isRegex a bool that is indicates how system commands should be compared against the specified pattern
		@return The set of matching commands.
	*/
	StringPairVector* GetMatchingCommands(std::string pattern, bool isRegex);

	/**
		Return true if the specified command exists on the system, and gets its pid.
		@deprecated This exists only until Dan fixes the MacOSX port to collect all
		processes.  Once this method is no longer used, it will be deleted.
		@param[in] command a string that hold the name of the command to check for.
		@param[out] pid the pid for the first process found for the given command.
		@return The result of checking for the specified command.  true=>found, false=>not found
	*/
	bool CommandExists(std::string command, std::string &pid);

	/**
		Return true if the specified command exists on the system, and gets its pids.
		@param[in] command a string that hold the name of the command to check for.
		@param[out] pids a vector which will be populated with the pids of all processes
		found for the given command.
		@return The result of checking for the specified command.  true=>found, false=>not found
	*/
	bool CommandExists(std::string command, StringVector &pids);

	/**
		Get all the information for the command.
		@param command a string representing the command to collect information about.
		@param items a vector of items that matched the command.
	*/
	void GetPSInfo(std::string command, std::string pidStr, ItemVector* items);

	/**
		Read /proc/<pid>/cmdline to gather the application name and startup arguments
	*/
	int RetrieveCommandLine(const char *process, char *cmdline, std::string *errMsg);

#ifdef LINUX

	/**
	 * Every time we try to read something from /proc/<pid>, there is a chance
	 * that the process terminated (which means the /proc/<pid> directory
	 * disappeared).  That's ok, and should cause the item to be omitted, rather
	 * than an error to be generated.  I use these constants to distinguish
	 * between the two cases.
	 */
	enum ProcStatus {
		PROC_OK, ///< no error
		PROC_TERMINATED, ///< the process terminated
		PROC_ERROR ///< some other error occurred
	};

	/**
		Read the stat file for a specific process
	*/
	ProcStatus RetrieveStatFile(const std::string &process, pid_t *ppid, 
						 long *priority, unsigned long *starttime,
				    pid_t *session, unsigned long *policy, std::string *errMsg);

	/**
	 * Reads uid's from the /proc/pid/status file.  This file supposedly contains
	 * a more human readable form of what's in /proc/pid/stat.  But it seems like
	 * it contains more than that: as far as I've been able to figure out, process
	 * uid's are only available here.
	 * <p>
	 * I wanted to return an error indication if the required values weren't able
	 * to be located in the status file.  A plain old special flag value won't
	 * work, since uid_t's are unsigned, and afaik there are no illegal values
	 * throughout its range.  (maybe UINT_MAX or some such might be good enough?
	 * But I wanted something more fool-proof.)  My idea is have the caller
	 * initialize pointers to the actual variables, and then pass the addresses
	 * of the pointers into this method.  So that is a precondition for calling
	 * this method: the double-pointer parameters \em must be properly
	 * initialized!  Then, if the values can be found, this method simply stores
	 * them in **var.  If they are not found, *var is set to NULL.  The caller
	 * can then check their pointers.  If the pointer is NULL, the value was not
	 * found; if non-NULL, correct values have been stored in the pointees.
	 * (I think boost::optional would have been perfect for this :( )
	 * <p>
	 * Whether or not the required values were found inside the status file, if
	 * there were no I/O or parse errors reading the file, PROC_OK is returned.
	 * If PROC_ERROR or PROC_TERMINATED is returned, pointer and pointee values
	 * are undefined.
	 */
	ProcStatus RetrieveStatusFile(const std::string &process, uid_t **ruid, 
								  uid_t **euid, uint64_t **effCap, 
								  std::string *errMsg);

	/**
		 Since there appears to be no simple way to convert the 'tty' value contained in
		 '/proc/<pid>/stat' into a device name, we instead use '/proc/<pid>/fd/0', which is
		 normally linked to a device.  Note, the 'fd' directory is set read-only user, so
		 if this probe is not run as root, many of these reads will fail.  In that case, we
		 return '?' as the tty value.		
	*/
	ProcStatus RetrieveTTY(const std::string &process, char *ttyName, std::string *err);

	/**
		Read the value contained in '/proc/uptime/' so that we can calculate
		the start time and exec time of the running processes.
	*/
	bool RetrieveUptime(unsigned long *uptime, std::string *errMsg);

	/**
	 * Get the contents of /proc/<pid>/loginuid
	 */
	ProcStatus RetrieveLoginUid(pid_t pid, uid_t *loginUid, std::string *err);

	/**
	 * Adds posix_capability entities to \p item according to the given bit set.
	 */
	void AddCapabilities(Item *item, uint64_t effCap);

	/**
	 * Gets the selinux domain for the given pid, and stores it in \p label.
	 */
	bool RetrieveSelinuxDomainLabel(pid_t pid, std::string *label, std::string *err);

#endif

#ifdef SUNOS

	/**
	 * Solaris-specific: gets a device path for the given device.  This will
	 * take advantage of /etc/ttysrch, if the file exists.
	 */
	std::string GetDevicePath(dev_t tty);

	/**
	 * Solaris-specific: Gets the directories listed in /etc/ttysrch.  An empty
	 * vector is returned if the file wasn't found or couldn't be read.
	 */
	StringVector GetDeviceSearchDirs();

	/**
	 * Solaris-specific: Reads info from the specified file into \p info.
	 * \param[in] psinfoFileName the full path to the file to read
	 * \param[out] info Receives info about the process
	 * \param[out] errMsg Will receive any error message.  If the file was not found,
	 *             it will remain unmodified.  Other errors produce a message.
	 * \return false if the file was not found or an error occurred, true otherwise.
	 */
	bool ReadPSInfoFromFile(const std::string psinfoFileName, psinfo_t &info, std::string &errMsg);

#endif

	/**
		Convert the input seconds and conveert to a string format for exec time.
	*/
	std::string FormatExecTime(time_t execTime);

	/**
		Convert the input seconds and convert to a string format for start time.
	*/
	std::string FormatStartTime(time_t startTime);

	/**
	 * A convenience method to delete the commands vector.
	 */
	void DeleteCommands(StringPairVector *commands);

	static Process58Probe *instance;
};

#endif
