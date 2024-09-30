from rest_framework.serializers import ModelSerializer
from .models import Entry
from .models import Result


class EntrySerializer(ModelSerializer):
    class Meta:
        model = Entry
        fields = ('device_name','temp', 'humidity', 'hasWater', 'date', 'lightAmount')

class ResultSerializer(ModelSerializer):
    class Meta:
        model = Result
        fields = ('score', 'sentence', 'date')

