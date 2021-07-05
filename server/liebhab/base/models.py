from django.db import models
from django.contrib.auth.models import User

from datetime import time




class Message(models.Model):
    from_user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True, related_name="from_name")
    to_user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True, related_name="to_name")
    text = models.CharField(max_length=200)
    sent_out = models.BooleanField("Message has been delivered over the API",default=False)
    created_at = models.DateTimeField("Creation Date", auto_now_add=True)
    duration = models.DurationField("Duration the Message will be shown")

    @property
    def is_shown(self):
        return self.created_at and time.now() < self.created_at + self.duration

    def __str__(self) -> str:
        return f"<Message> from_user={self.from_user}, to_user={self.to_user}, text={self.text}, created_at={self.created_at}, duration={self.duration}"
