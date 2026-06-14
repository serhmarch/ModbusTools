
//var rootKey = (installer.value("AllUsers") === "true") ? "HKLM" : "HKCU";
const rootKey = "HKLM"; // Always use HKLM for file associations on Windows
const rootKeyPath = rootKey + "\\Software\\Classes\\";
const rootKeyPath_mbc = rootKeyPath + ".mbc";
const rootKeyPath_mbclient = rootKeyPath + "mbclient.file";
const rootKeyPath_mbs = rootKeyPath + ".mbs";
const rootKeyPath_mbserver = rootKeyPath + "mbserver.file";

function linuxIsQtLibInstalled(libNames)
{
    // 1. Run ldconfig through a shell to capture the output pipeline
    // We look for libQt6Core.so or libQt5Core.so
    var result = installer.execute("sh", ["-c", "ldconfig -p | grep -E 'libQt[56](" + libNames + ")\\.so'"]);
    
    // 2. installer.execute returns an array: [exitCode, outputText]
    var exitCode = result[1];
    var outputText = result[0];
    
    // grep returns exit code 0 if a match is found
    if (exitCode === 0 && outputText.trim() !== "")
	{
        console.log("MBTOOLS: Qt libraries found on system:\n" + outputText);
        return true;
    }
    
    console.log("MBTOOLS: No system-wide Qt5 or Qt6 " + libNames + " libraries detected.");
    return false;
}

function linuxPackagingSystem()
{
	var primaryDistro = systemInfo.productType.toLowerCase();
	
	if (primaryDistro === "ubuntu" ||
		primaryDistro === "debian" ||
		primaryDistro === "linuxmint")
	{
		return "apt";
	}
	else if (primaryDistro === "fedora" || 
			 primaryDistro === "rocky"  ||
			 primaryDistro === "rhel"   ||
			 primaryDistro === "centos")
	{
		return "dnf";
	}
	else if (primaryDistro === "arch")
	{
		return "pacman";
	}
}

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
	else if (systemInfo.kernelType === "linux")
	{
		console.log("MBTOOLS: Installation finished. Setting up file associations for Linux.");

		if ((installer.hasAdminRights()))
		{
			console.log("MBTOOLS: Installer already has administrative rights. Proceeding with file association setup.");
		}
		else
		{
			if (!installer.gainAdminRights())
				return;

			console.log("MBTOOLS: Administrative rights granted. Proceeding with file association setup.");
		}

		// Create .desktop files for mbclient and mbserver
		var desktopEntryClient = "[Desktop Entry]\n" +
			"Name=mbclient\n" +
			"Exec=" + installer.value("TargetDir") + "/mbclient %f\n" +
			"MimeType=application/x-mbclient;\n" +
			"Type=Application\n";

		var desktopEntryServer = "[Desktop Entry]\n" +
			"Name=mbserver\n" +
			"Exec=" + installer.value("TargetDir") + "/mbserver %f\n" +
			"MimeType=application/x-mbserver;\n" +
			"Type=Application\n";
			
		var desktopFileClient = "/usr/share/applications/mbclient.desktop";
		var desktopFileServer = "/usr/share/applications/mbserver.desktop";

		installer.execute("sh", ["-c", "echo \"" + desktopEntryClient.replace(/"/g, '\\"') + "\" | sudo tee " + desktopFileClient]);
		installer.execute("sh", ["-c", "echo \"" + desktopEntryServer.replace(/"/g, '\\"') + "\" | sudo tee " + desktopFileServer]);

		var QtCoreInstalled = linuxIsQtLibInstalled("Core");
		if (QtCoreInstalled)
		{
			console.log("MBTOOLS: Qt libraries detected. Packaging system: " + linuxPackagingSystem());
		}
		else
		{
			var packagingSystem = linuxPackagingSystem();
			console.log("MBTOOLS: No Qt libraries detected. Detected packaging system: " + packagingSystem);
			if (packagingSystem === "apt")
			{
				console.log("MBTOOLS: Suggesting installation of Qt libraries using apt.");
				installer.execute("sh", ["-c", "sudo apt install -y qtbase5-dev qttools5-dev"]);
			}
			else if (packagingSystem === "dnf")
			{
				console.log("MBTOOLS: Suggesting installation of Qt libraries using dnf.");
				installer.execute("sh", ["-c", "sudo dnf install -y qt5-qtbase-devel qt5-qttools-devel"]);
			}
			else if (packagingSystem === "pacman")
			{
				console.log("MBTOOLS: Suggesting installation of Qt libraries using pacman.");
				installer.execute("sh", ["-c", "sudo pacman -S --noconfirm qt5-base qt5-tools"]);
			}
		}
        
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
