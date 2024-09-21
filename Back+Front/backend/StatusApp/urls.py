from django.urls import path, include
from .views import EntryViewSet
from rest_framework.routers import DefaultRouter


entry_router = DefaultRouter()
entry_router.register(r'entry', EntryViewSet)

# Define urlpatterns
urlpatterns = [
    path('', include(entry_router.urls)),  # This includes your viewset URLs
]