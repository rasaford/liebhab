from django.urls import path
from django.urls.conf import include
from . import views
from rest_framework import routers


router = routers.DefaultRouter()
router.register(r"messages", views.MessageViewSet, basename="Message")
router.register(r"users", views.UserViewSet)


urlpatterns = [
    path('', views.index, name="index"),
    path("api/", include(router.urls)),
    # path("api/", include("rest_framework.urls", namespace="rest_framework")),
]
