from django.db import models

class DataType(models.Model):
    description = models.CharField(max_length=100)

class Target(models.Model):
    name = models.CharField(max_length=30)
    description = models.CharField(max_length=100)
    source = models.ManyToManyField('Source')

class Source(models.Model):
    name = models.CharField(max_length=30)
    datatype = models.ForeignKey('DataType')

class WeightData(models.Model):
    target = models.ForeignKey('Target')
    datetime = models.DateTimeField()
    value = models.FloatField()
