from django.db import models

# Create your models here.

class User(models.Model):
    username = models.CharField(max_length=200)
    password = models.CharField(max_length=200)
    device_name = models.CharField(max_length=200)

    

class Entry(models.Model):
    device_name = models.CharField(default="N/A",max_length=200)#models.ForeignKey(User, on_delete=models.CASCADE)
    temp = models.IntegerField(default=0)
    humidity = models.IntegerField(default=0)
    hasWater = models.BooleanField(default=True)
    date = models.DateTimeField("Entry date")

    def getTemp(self):
        return self.temp
    
    def getHumidity(self):
        return self.humidty
    
    def getHasWater(self):
        return self.hasWater
    
    def getDate(self):
        return self.date
    
    def __str__(self):
        return f"Entry[Device name: {self.device_name},  Temp: {self.temp}, Humidity : {self.humidity}, has Water : {self.hasWater}, date : {self.date}]"


