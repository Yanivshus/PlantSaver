from django.shortcuts import render
from django.http import HttpResponse
from rest_framework.viewsets import ModelViewSet
from .models import Entry
from .models import Result
from .serializers import EntrySerializer
from .serializers import ResultSerializer
from datetime import timedelta
from django.utils import timezone


class EntryViewSet(ModelViewSet):
    queryset = Entry.objects.all()
    serializer_class = EntrySerializer

class ResultViewSet(ModelViewSet):
    queryset = Result.objects.all()
    serializer_class = ResultSerializer
    http_method_names = ['get']

    def list(self, request):
        last_week = timezone.now() - timedelta(days=7)
        entries = Entry.objects.filter(date=last_week)

    def process_and_save_results(self, enteries):
        pass

    


