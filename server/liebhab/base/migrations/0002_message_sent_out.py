# Generated by Django 3.2.5 on 2021-10-06 16:30

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('base', '0001_initial'),
    ]

    operations = [
        migrations.AddField(
            model_name='message',
            name='sent_out',
            field=models.BooleanField(default=False, verbose_name='Message has been delivered over the API'),
        ),
    ]
