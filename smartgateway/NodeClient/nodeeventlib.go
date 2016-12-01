package nodeclient

import "fmt"

// get energy meter data, only for test
func GetEnergyMeterData() (int, error) {
	cmd := RequestInfo{REQUEST, GETMETREDATA, ""}
	data, err := DoNodeClientGwMessage(cmd)
	if err != nil {
		fmt.Println("can't get the EnergyMeter data")
	} else {
		fmt.Println("EnergyMeter Value:", data)
	}

	return data.(int), nil
}

//get Temperature, only for test
func GetTemperatureData() (int, error) {
	cmd := RequestInfo{REQUEST, GETTEMPERATURE, ""}
	data, err := DoNodeClientUiMessage(cmd)
	if err != nil {
		fmt.Println("can't get the temperature")
	} else {
		fmt.Println("Temperature Value:", data)
	}
	return data.(int), nil
}

// get humidity, only for test
func GetHumidityData() (int, error) {
	cmd := RequestInfo{REQUEST, GETHUMIDITY, ""}
	data, err := DoNodeClientGwMessage(cmd)
	if err != nil {
		fmt.Println("can't get the humidity")
	} else {
		fmt.Println("Humidity Value:", data)
	}
	return data.(int), nil
}

//get gateway discovery，only for test
func GetGatewayDiscovery() ([]string, error) {
	cmd := RequestInfo{REQUEST, GWDISCOVERY, ""}

	data, err := DoNodeClientGwMessage(cmd)
	if err != nil {
		fmt.Println("can't get the gateway")
	} else {
		fmt.Println("Gateway:", data)
	}
	return data.([]string), err

}

//get node add, only for test
func GetGatewayNodeAdd() ([]string, error) {
	cmd := RequestInfo{REQUEST, NODEADD, ""}

	data, err := DoNodeClientGwMessage(cmd)
	if err != nil {
		fmt.Println("can't get the node")
	} else {
		fmt.Println("Gateway:", data)
	}
	return data.([]string), err
}
