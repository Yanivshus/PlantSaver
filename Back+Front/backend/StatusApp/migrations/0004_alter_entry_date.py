# Generated by Django 4.2.16 on 2024-09-28 11:57

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('StatusApp', '0003_remove_entry_username_entry_device_name'),
    ]

    operations = [
        migrations.AlterField(
            model_name='entry',
            name='date',
            field=models.DateTimeField(auto_now_add=True),
        ),
    ]
