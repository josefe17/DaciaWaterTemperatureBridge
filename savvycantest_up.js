var incrementSteps = 1;
var lowerLimit = -40;
var upperLimit = 150;

var cycleTemperatureDelay = 100; //In ms

var temperatureDecimal = lowerLimit;
var isIncrementing = 1;

function setup()
{
	host.log("Testing coolant temperature gauge");
	host.log("Update period set to " + tick.toString() + " ms");
	host.setTickInterval(cycleTemperatureDelay);
}

function tick()
{
	cycleTemperature();
}


function cycleTemperature()
{	
	var temperatureRaw = temperatureDecimal + 40;
	host.log("Sending coolant temperature: " + temperatureDecimal.toString() + " ºC");
	can.sendFrame(0, 0x5DA, 8, [temperatureRaw, 0, 0, 0, 0, 0, 0, 0]);
	if (isIncrementing == 1)
	{
		if ((temperatureDecimal + incrementSteps) <= upperLimit)
		{
			temperatureDecimal  += incrementSteps;
		}
		else
		{
			temperatureDecimal  -= incrementSteps;	
			isIncrementing = 0;
		}
	}
	else
	{
		if ((temperatureDecimal -  incrementSteps) >= lowerLimit)
		{
			temperatureDecimal  -= incrementSteps;
		}
		else
		{
			temperatureDecimal  += incrementSteps;
			isIncrementing = 1;
		}
	}
}


/*
function cycleTemperature()
{	
	if ((temperatureDecimal + incrementSteps) <= upperLimit + 1)
	{
		var temperatureRaw = temperatureDecimal + 40;
		host.log("Sending coolant temperature: " + temperatureDecimal.toString() + " ºC");
		can.sendFrame(0, 0x5DA, 8, [temperatureRaw, 0, 0, 0, 0, 0, 0, 0]);
		temperatureDecimal  += incrementSteps;
	}
}
*/