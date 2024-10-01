from .models import Entry
from math import isclose

def mainProcessing(enteries):
    score = 100

    tempMsg, humidityMsg, envScr = checkPropperEnviroment(enteries)
    score += envScr

    waterMsg, waterScr = checkWaterLevel(enteries)
    score += waterScr

    msg = tempMsg + "|" + humidityMsg + "|" + waterMsg
    return (score, msg)


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

    score = 0
    if 20 <= avgTemp // len(enteries) and avgTemp // len(enteries) <= 30:
        tempMsg = f"Propper environment temperature, current avg: {avgTemp // len(enteries)}"
        score = 0
    else:
        tempMsg = f"Not propper environment temperature: should keep between 20 and 30 degrees, current avg: {avgTemp // len(enteries)}"
        score = -25

    if 60 <= avgHumidity // len(enteries) and avgHumidity // len(enteries) <= 80:
        humidityMsg = f"Propper environment humidity, current avg: {avgHumidity // len(enteries)}"
        score = 0
    else:
        humidityMsg = f"Not propper environment humidity: should keep between 60 and 80 precent, current avg: {avgHumidity // len(enteries)}"
        score = -25

    return (tempMsg, humidityMsg, score)


def checkWaterLevel(enteries):
    enoughWater = 0
    for entry in enteries:
        if entry.hasWater:
            enoughWater += 1

    amountEnteries = len(enteries)  

    supposedAmount = amountEnteries // 2
    if enoughWater - 10 > supposedAmount :
        return ("You are using too much water, try lowering it down", -25)
    elif enoughWater + 10  < supposedAmount:
        return ("You are not using enough water, try to pour more", -25)
    else:
        return ("You are providing enough water", 0)
    


    
    
    