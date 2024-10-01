from django.db import models

# Create your models here.

class Result(models.Model):
    score = models.IntegerField(default=0)
    sentence = models.CharField(default="N/A", max_length=400)
    date = models.DateTimeField(auto_now_add=True)


class Entry(models.Model):
    device_name = models.CharField(default="N/A",max_length=200)
    temp = models.IntegerField(default=0)
    humidity = models.IntegerField(default=0)
    hasWater = models.BooleanField(default=True)
    date = models.DateTimeField(auto_now_add=True)
    lightAmount = models.IntegerField(default=0)

    def getTemp(self):
        return self.temp
    
    def getHumidity(self):
        return self.humidty
    
    def getHasWater(self):
        return self.hasWater
    
    def getDate(self):
        return self.date
    
    def getLIght(self):
        return self.lightAmount
    
    def __str__(self):
        return f"Entry[Device name: {self.device_name},  Temp: {self.temp}, Humidity : {self.humidity}, has Water : {self.hasWater}, date : {self.date}]"


