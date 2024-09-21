from rest_framework.routers import DefaultRouter
from StatusApp.urls import entry_router
from django.urls import path,include

router = DefaultRouter()
router.registry.extend(entry_router.registry)

urlpatterns = [
    path('', include(router.urls))
]