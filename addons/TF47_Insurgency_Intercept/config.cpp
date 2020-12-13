class CfgPatches {
	class tf47_insurgency_intercept { //Change this
		name = "TF47 Insurgency Intercept"; //Change this
		units[] = {};
		weapons[] = {};
		requiredVersion = 2.0;
		requiredAddons[] = {"intercept_core"};
		author = "TF47 Dragon"; //Change this
		authors[] = {"Dragon"}; //Change this
		url = "https://taskforce47.com"; //Change this
		version = "1.0";
		versionStr = "1.0";
		versionAr[] = {1,0};
	};
};
class Intercept {
    class TF47 { //Change this. It's either the name of your project if you have more than one plugin. Or just your name.
        class tf47_insurgency_intercept { //Change this.
            pluginName = "tf47_insurgency_intercept"; //Change this.
        };
    };
};
