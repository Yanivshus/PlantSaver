from django.urls import path, include
from .views import EntryViewSet, ResultViewSet
from rest_framework.routers import DefaultRouter


entry_router = DefaultRouter()
entry_router.register(r'entry', EntryViewSet)
entry_router.register(r'result', ResultViewSet)

# Define urlpatterns
urlpatterns = [
    path('', include(entry_router.urls)),  # This includes your viewset URLs
]