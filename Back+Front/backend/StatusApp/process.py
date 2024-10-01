from .models import Entry
from math import isclose

def checkPropperEnviroment(enteries):
    avgTemp = 0
    avgHumidity = 0
    tempMsg = ""
    humidityMsg = ""
    for entry in enteries:
        if entry.humidity < 0:
            continue
        else:
            avgTemp += entry.temp
            avgHumidity += entry.humidity

    if 20 <= avgTemp // len(enteries) and avgTemp // len(enteries) <= 30:
        tempMsg = f"Propper environment temperature, current avg: {avgTemp // len(enteries)}"
    else:
        tempMsg = f"Not propper environment temperature: should keep between 20 and 30 degrees, current avg: {avgTemp // len(enteries)}"

    if 60 <= avgHumidity // len(enteries) and avgHumidity // len(enteries) <= 80:
        humidityMsg = f"Propper environment humidity, current avg: {avgHumidity // len(enteries)}"
    else:
        humidityMsg = f"Not propper environment humidity: should keep between 60 and 80 precent, current avg: {avgHumidity // len(enteries)}"

    return (tempMsg, humidityMsg)


def checkWaterLevel(enteries):
    enoughWater = 0
    for entry in enteries:
        if entry.hasWater:
            enoughWater += 1

    amountEnteries = len(enteries)  

    supposedAmount = amountEnteries // 2
    return isclose(enoughWater, supposedAmount, abs_tol=10) 
    