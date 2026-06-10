
//var rootKey = (installer.value("AllUsers") === "true") ? "HKLM" : "HKCU";
const rootKey = "HKLM"; // Always use HKLM for file associations on Windows
const rootKeyPath = rootKey + "\\Software\\Classes\\";
const rootKeyPath_mbc = rootKeyPath + ".mbc";
const rootKeyPath_mbclient = rootKeyPath + "mbclient.file";
const rootKeyPath_mbs = rootKeyPath + ".mbs";
const rootKeyPath_mbserver = rootKeyPath + "mbserver.file";

function Controller()
{
}

// Keep existing IntroductionPageCallback for backward compatibility
Controller.prototype.IntroductionPageCallback = function()
{
	if (installer.isInstaller())
	{
    	console.log("MBTOOLS: Installation started. Registering callback for installation finished.");
		installer.installationFinished.connect(finishInstallation);
	}
	else if (installer.isUninstaller())
	{
    	console.log("MBTOOLS: Uninstallation started. Registering callback for uninstallation finished.");
		installer.uninstallationFinished.connect(finishUninstallation);
	}
}

finishInstallation = function()
{
	if (systemInfo.kernelType === "winnt")
    {
		var targetDir = installer.value("TargetDir").replace(/\//g, "\\"); // Change slashes for Windows

		console.log("MBTOOLS: Installation finished. Target directory: " + targetDir);
		console.log("MBTOOLS: Registering file associations in the registry under " + rootKey);

		// Keys for .mbc
		installer.execute("reg", ["add", rootKeyPath_mbc, "/ve", "/t", "REG_SZ", "/d", "mbclient.file", "/f"]);
		installer.execute("reg", ["add", rootKeyPath_mbclient + "\\shell\\open\\command", "/ve", "/t", "REG_SZ", "/d", "\"" + targetDir + "\\mbclient.exe\" \"%1\"", "/f"]);

		// Keys for .mbs
		installer.execute("reg", ["add", rootKeyPath_mbs, "/ve", "/t", "REG_SZ", "/d", "mbserver.file", "/f"]);
		installer.execute("reg", ["add", rootKeyPath_mbserver + "\\shell\\open\\command", "/ve", "/t", "REG_SZ", "/d", "\"" + targetDir + "\\mbserver.exe\" \"%1\"", "/f"]);
	}
}

finishUninstallation = function()
{
	if (systemInfo.kernelType === "winnt")
	{
		console.log("MBTOOLS: Uninstallation finished. Removing file associations from the registry under " + rootKey);

		// Remove keys for .mbc
		installer.execute("reg", ["delete", rootKeyPath_mbc, "/f"]);
		installer.execute("reg", ["delete", rootKeyPath_mbclient, "/f"]);

		// Remove keys for .mbs
		installer.execute("reg", ["delete", rootKeyPath_mbs, "/f"]);
		installer.execute("reg", ["delete", rootKeyPath_mbserver, "/f"]);
	}
}
