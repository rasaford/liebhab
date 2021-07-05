from django.http.response import HttpResponseServerError
from django.shortcuts import render
from django.http import HttpResponse
from django.template import loader
from django.contrib.auth.mixins import LoginRequiredMixin
from django.views.generic.list import ListView
from .models import Message, User
from .forms import MessageForm
from datetime import datetime
from datetime import timedelta

from rest_framework import viewsets, permissions, generics
from .serializers import MessageSerializer, UserSerializer


def index(request):
    if request.method == "POST":
        form = MessageForm(request.POST)
        if form.is_valid():
            cleaned = form.cleaned_data
            print(cleaned)
            message = Message()
            message.from_user = request.user
            message.to_user = User.objects.filter(
                username=cleaned["to_user"]).first()
            if "text" in cleaned:
                message.text = cleaned["text"]
            message.created_at = datetime.now()
            print("setting duration")
            message.duration = timedelta(
                seconds=cleaned["duration"] *
                60) if cleaned["duration"] is not None else timedelta(
                    seconds=60)
            print(message.duration)
            print(message)
            message.save()

    template = loader.get_template("base/index.html")
    message_list = Message.objects.order_by("created_at").reverse()[:10]
    user_list = User.objects.order_by("username")

    return HttpResponse(
        template.render({
            "message_list": message_list,
            "user_list": user_list
        }, request))


class MessageViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows messages to be set or edited
    """

    serializer_class = MessageSerializer
    permission_classes = [permissions.IsAuthenticated]

    def get_queryset(self):
        user = self.request.user
        # return latest non-sent message
        message = Message.objects.filter(to_user=user).filter(
            sent_out=False).order_by("created_at").last()

        if message:
            # set shown on set out queries
            message.sent_out = True
            message.save()
            return [message]
        return []


class UserViewSet(viewsets.ModelViewSet):
    """
    API endpoint that allows users to be set or edited
    """
    queryset = User.objects.all().order_by("username").reverse()
    serializer_class = UserSerializer
    permission_classes = [permissions.IsAuthenticated]
