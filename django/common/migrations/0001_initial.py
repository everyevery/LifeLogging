# -*- coding: utf-8 -*-
import datetime
from south.db import db
from south.v2 import SchemaMigration
from django.db import models


class Migration(SchemaMigration):

    def forwards(self, orm):
        # Adding model 'DataType'
        db.create_table(u'common_datatype', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('description', self.gf('django.db.models.fields.CharField')(max_length=100)),
        ))
        db.send_create_signal(u'common', ['DataType'])

        # Adding model 'Target'
        db.create_table(u'common_target', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('name', self.gf('django.db.models.fields.CharField')(max_length=30)),
            ('description', self.gf('django.db.models.fields.CharField')(max_length=100)),
        ))
        db.send_create_signal(u'common', ['Target'])

        # Adding M2M table for field source on 'Target'
        m2m_table_name = db.shorten_name(u'common_target_source')
        db.create_table(m2m_table_name, (
            ('id', models.AutoField(verbose_name='ID', primary_key=True, auto_created=True)),
            ('target', models.ForeignKey(orm[u'common.target'], null=False)),
            ('source', models.ForeignKey(orm[u'common.source'], null=False))
        ))
        db.create_unique(m2m_table_name, ['target_id', 'source_id'])

        # Adding model 'Source'
        db.create_table(u'common_source', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('name', self.gf('django.db.models.fields.CharField')(max_length=30)),
            ('datatype', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['common.DataType'])),
        ))
        db.send_create_signal(u'common', ['Source'])

        # Adding model 'WeightData'
        db.create_table(u'common_weightdata', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('target', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['common.Target'])),
            ('datetime', self.gf('django.db.models.fields.DateTimeField')()),
            ('value', self.gf('django.db.models.fields.FloatField')()),
        ))
        db.send_create_signal(u'common', ['WeightData'])


    def backwards(self, orm):
        # Deleting model 'DataType'
        db.delete_table(u'common_datatype')

        # Deleting model 'Target'
        db.delete_table(u'common_target')

        # Removing M2M table for field source on 'Target'
        db.delete_table(db.shorten_name(u'common_target_source'))

        # Deleting model 'Source'
        db.delete_table(u'common_source')

        # Deleting model 'WeightData'
        db.delete_table(u'common_weightdata')


    models = {
        u'common.datatype': {
            'Meta': {'object_name': 'DataType'},
            'description': ('django.db.models.fields.CharField', [], {'max_length': '100'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'})
        },
        u'common.source': {
            'Meta': {'object_name': 'Source'},
            'datatype': ('django.db.models.fields.related.ForeignKey', [], {'to': u"orm['common.DataType']"}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '30'})
        },
        u'common.target': {
            'Meta': {'object_name': 'Target'},
            'description': ('django.db.models.fields.CharField', [], {'max_length': '100'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '30'}),
            'source': ('django.db.models.fields.related.ManyToManyField', [], {'to': u"orm['common.Source']", 'symmetrical': 'False'})
        },
        u'common.weightdata': {
            'Meta': {'object_name': 'WeightData'},
            'datetime': ('django.db.models.fields.DateTimeField', [], {}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'target': ('django.db.models.fields.related.ForeignKey', [], {'to': u"orm['common.Target']"}),
            'value': ('django.db.models.fields.FloatField', [], {})
        }
    }

    complete_apps = ['common']