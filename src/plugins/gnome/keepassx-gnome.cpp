/***************************************************************************
 *   Copyright (C) 2005-2006 by Tarek Saidi                                *
 *   tarek.saidi@arcor.de                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *

 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include <gtk/gtk.h> 
#include "keepassx-gnome.h"
#define CSTR(x)(x.toUtf8().data())

Q_EXPORT_PLUGIN2(keepassx_gnome, GnomePlugin)
		
bool GnomePlugin::init(int argc, char** argv){
	int t_argc=argc;
	char** t_argv=argv;
	return gtk_init_check(&t_argc,&t_argv);	
}

QString GnomePlugin::openExistingFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters){
	unsigned int NumFilters=Filters.size();
	GtkWidget *FileDlg;
	QString filename;
	FileDlg=gtk_file_chooser_dialog_new(CSTR(title),NULL,
				GTK_FILE_CHOOSER_ACTION_OPEN,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				NULL);

	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(FileDlg),CSTR(dir));
	GtkFileFilter** filters=parseFilterStrings(Filters);
	
	for(int i=0;i<NumFilters;i++){		
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(FileDlg),filters[i]);
	}
	if (gtk_dialog_run(GTK_DIALOG(FileDlg)) == GTK_RESPONSE_ACCEPT){
		char* filename_cstring=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileDlg));
		filename = QString::fromUtf8(filename_cstring);
		g_free(filename_cstring);
 	}	
	gtk_widget_destroy(FileDlg);
	return filename;
}

GtkFileFilter** GnomePlugin::parseFilterStrings(const QStringList& filters){
	if(!filters.size())return NULL;
	GtkFileFilter **f=g_new(GtkFileFilter*,filters.size());
	for(int i=0;i<filters.size();i++){
		f[i]=gtk_file_filter_new();
		QString name;
		int p=0;
		for(p;p<filters[i].size();p++){
			if(filters[i][p]!='(')
				name += filters[i][p];
			else
				break;			
		}
		gtk_file_filter_set_name(f[i],CSTR(name));
		p++;
		QString pattern;
		for(p;p<filters[i].size();p++){
			if(filters[i][p]==' ' || filters[i][p]==')'){
				gtk_file_filter_add_pattern(f[i],CSTR(pattern));
				pattern=QString();
			}
			else{
				pattern += filters[i][p];				
			}
			
		}
	}
	return f;
}

QStringList GnomePlugin::openExistingFilesDialog(QWidget* parent,QString title,QString dir,QStringList Filters){
	unsigned int NumFilters=Filters.size();
	GtkWidget *FileDlg;
	QStringList filenames;
	FileDlg=gtk_file_chooser_dialog_new(CSTR(title),NULL,
				GTK_FILE_CHOOSER_ACTION_OPEN,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				NULL);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(FileDlg),CSTR(dir));
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(FileDlg),true);
	GtkFileFilter** filters=parseFilterStrings(Filters);
	
	for(int i=0;i<NumFilters;i++){		
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(FileDlg),filters[i]);
	}
	if (gtk_dialog_run(GTK_DIALOG(FileDlg)) == GTK_RESPONSE_ACCEPT){
		GSList* FilenameList=gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(FileDlg));
		GSList* front=FilenameList;
		while(FilenameList!=0){
					filenames << QString::fromUtf8((char*)FilenameList->data);
					FilenameList=FilenameList->next;
		}
		g_slist_free(front);
 	}	
	gtk_widget_destroy(FileDlg);
	return filenames;
}

QString GnomePlugin::saveFileDialog(QWidget* parent,QString title,QString dir,QStringList Filters,bool OverWriteWarn){
	unsigned int NumFilters=Filters.size();
	GtkWidget *FileDlg;
	QString filename;
	FileDlg=gtk_file_chooser_dialog_new(CSTR(title),NULL,
				GTK_FILE_CHOOSER_ACTION_SAVE,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				NULL);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(FileDlg),CSTR(dir));
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(FileDlg),OverWriteWarn);
	GtkFileFilter** filters=parseFilterStrings(Filters);
	
	for(int i=0;i<NumFilters;i++){		
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(FileDlg),filters[i]);
	}
	if (gtk_dialog_run(GTK_DIALOG(FileDlg)) == GTK_RESPONSE_ACCEPT){
		char* filename_cstring=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FileDlg));
		filename = QString::fromUtf8(filename_cstring);
		g_free(filename_cstring);
 	}	
	gtk_widget_destroy(FileDlg);
	return filename;}
