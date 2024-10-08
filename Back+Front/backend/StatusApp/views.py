from django.shortcuts import render
from django.http import HttpResponse
from rest_framework.viewsets import ModelViewSet
from .models import Entry
from .models import Result
from .serializers import EntrySerializer
from .serializers import ResultSerializer
from datetime import timedelta
from django.utils import timezone
from rest_framework.response import Response


class EntryViewSet(ModelViewSet):
    queryset = Entry.objects.all()
    serializer_class = EntrySerializer

class ResultViewSet(ModelViewSet):
    queryset = Result.objects.all()
    serializer_class = ResultSerializer
    http_method_names = ['get']

    def list(self, request):
        last_week = timezone.now() - timedelta(days=7)
        entry_type = request.query_params.get('device_name', None)
        entries = Entry.objects.filter(date__gte=last_week) # filter only entreis from last week
        Entry.objects.filter(date__lt=last_week).delete() # delete all entreis not from last week

        if entry_type:
            entries = entries.filter(device_name=entry_type) # check if provided device name.

        if not entries.exists():
            return Response({"error": "No entries found for the specified device name or date range."}, status=404)

            
        proc_data = self.process_and_save_results(entries) # if device name provided we will filter by it too.
        
        return Response(proc_data)




    def process_and_save_results(self, enteries):
        score, msg = self.mainProcessing(enteries)
        result = Result.objects.create(score=score, sentence= msg) # create result and save model
        result_serializer = ResultSerializer(result) # derialize the get response
        return result_serializer.data
    
    
    def mainProcessing(self, enteries):
        score = 100

        tempMsg, humidityMsg, envScr = self.checkPropperEnviroment(enteries)
        score += envScr

        waterMsg, waterScr = self.checkWaterLevel(enteries)
        score += waterScr

        lightMsg, lightScore = self.checkLightAmount(enteries)
        score += lightScore

        msg = tempMsg + " | " + humidityMsg + " | " + waterMsg + " | " + lightMsg
        return (score, msg)


    def checkPropperEnviroment(self,enteries):
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
        # ranges are taken from sources online.
        if 20 <= avgTemp // len(enteries) and avgTemp // len(enteries) <= 30:
            tempMsg = f"Propper environment temperature, current avg: {avgTemp // len(enteries)}"
            score += 0
        else:
            tempMsg = f"Not propper environment temperature: should keep between 20 and 30 degrees, current avg: {avgTemp // len(enteries)}"
            score += -25

        if 60 <= avgHumidity // len(enteries) and avgHumidity // len(enteries) <= 80:
            humidityMsg = f"Propper environment humidity, current avg: {avgHumidity // len(enteries)}"
            score += 0
        else:
            humidityMsg = f"Not propper environment humidity: should keep between 60 and 80 precent, current avg: {avgHumidity // len(enteries)}"
            score += -25

        return (tempMsg, humidityMsg, score)


    def checkWaterLevel(self, enteries):
        enoughWater = 0
        for entry in enteries:
            if entry.hasWater != 0:
                enoughWater += 1

        amountEnteries = len(enteries)  

        supposedAmount = amountEnteries // 2
        supposedAmountPrecent = 50

        enoughtWaterPrecent = (enoughWater // amountEnteries) * 100
        
        if enoughtWaterPrecent - 20 >= supposedAmountPrecent :
            return ("You are using too much water, try lowering it down", -25)
        elif enoughtWaterPrecent + 10  < supposedAmountPrecent:
            return ("You are not using enough water, try to pour more", -25)
        else:
            return ("You are providing enough water", 0)
        
    def checkLightAmount(self, enteries):
        # by precent
        amount = 0

        for entry in enteries:
            # not dark
            if entry.lightAmount > 13:
                amount += 1
            
        precent = ( amount / len(enteries) ) * 100

        # equels to 6-8 hours of direct light exposere
        if 25 <= precent and precent <= 33.33333333:
            return (f"You are using the correct light precentage : which is {precent}%", 0)
        
        return (f"You are using the incorrect light precentage : which is {precent}%, you should expose the plant to 6-8 hours of direct sunlight per day which will come to 25-33.3 %", -25)
