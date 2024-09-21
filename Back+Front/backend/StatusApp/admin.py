from django.contrib import admin

# Register your models here.
from .models import Entry 

admin.site.admin_view(Entry)
admin.site.register(Entry)
