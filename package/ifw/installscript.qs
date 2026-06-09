function Controller()
{
}

Controller.prototype.IntroductionPageCallback = function()
{
	console.log("IntroductionPageCallback begin");
	// Keep default behavior; hook exists for future custom install actions.
	arr = installer.components();
	c = arr[0];
	QMessageBox.information("IntroductionPageCallback", "Installation complete! Component: " + c.name);
};

