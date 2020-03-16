#pragma once
#include <string>
#include <vector>
#include "KeyMaker.h"
#include "FileCryptor.h"

class GatekeeperResult
{
public:
	GatekeeperResult();
	GatekeeperResult(int in_code, std::string in_command,std::string in_reason);
public:
	int code = 0;
	std::string command = "none";
	std::string reason = "none";
};

class Gatekeeper
{
public:
	void showHelp();
	GatekeeperResult Run(std::vector<std::string> args);
private:
	GatekeeperResult MakeSetupFile(std::vector<std::string> args);
	GatekeeperResult EncryptFile(std::vector<std::string> args);	
	GatekeeperResult RunFolder(std::vector<std::string> args);	
	GatekeeperResult GenRSAKeyPair(std::vector<std::string> args);
	GatekeeperResult GenUserEncryptKey(std::vector<std::string> args);	
	GatekeeperResult RSAEncodeFEK(std::vector<std::string> args);
	std::vector<std::string> LoadTargetConfigFile();
private:
	GatekeeperResult Raw_EncryptFile(std::__cxx11::string sourceFile, std::__cxx11::string aeskeyFile, 
		std::__cxx11::string publickeyFile, std::__cxx11::string privatekeyFile,std::__cxx11::string outputFolder="");
	GatekeeperResult Raw_RunFolder(std::__cxx11::string targetFolder);
	GatekeeperResult Raw_GenRSAKeyPair(std::__cxx11::string outDir);
	GatekeeperResult Raw_GenUserEncryptKey(std::__cxx11::string hdCode, std::__cxx11::string authKey, std::__cxx11::string keyFile);
private:
	KeyMaker keyMaker;
};

