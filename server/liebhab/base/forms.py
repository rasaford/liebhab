from django import forms
from django.core.exceptions import RequestAborted

class MessageForm(forms.Form):
    to_user = forms.CharField(label="To User", required=True, max_length=100)
    text = forms.CharField(required=True, max_length=100)
    duration = forms.IntegerField(max_value=5, required=False)