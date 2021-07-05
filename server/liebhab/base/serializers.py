from .models import User, Message
from rest_framework import serializers

class MessageSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Message
        fields = ["from_user", "to_user", "text", "created_at", "duration", "sent_out"]

class UserSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = User
        fields = ["username", "email", "first_name", "last_name"]