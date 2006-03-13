#ifndef EDITMYPROFILE_H
#define EDITMYPROFILE_H

#include <QtGui>

#include <Logger.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>


class QtEditMyProfile : public QDialog
{
	Q_OBJECT
	
public:
	
	QtEditMyProfile (QWidget * parent =0, Qt::WFlags f=0);
	
protected:
	
	void init();
	
	void changeGroupBoxStat(QGroupBox * box, bool stat);
	
	void readFromConfig();
	
	void writeToConfig();
public Q_SLOTS:
	void saveClicked();
	void cancelClicked();
protected:
	
	QWidget * _widget;
	
	QGridLayout * layout;
	
	QLineEdit * _wengoNickName;
	
	QPushButton * _changePassword;
	
	QLineEdit * _firstName;
	
	QLineEdit * _lastName;
	
	QDateEdit * _birthDate;
	
	QLineEdit * _city;
	
	QLineEdit * _altSip;
	
	QPushButton * _addSipAccount;
	
	QComboBox * _gender;
	
	QComboBox * _country;
	
	QComboBox * _state;
	
	QList<QLabel *> _imAccountsPic;
	
	QList<QLineEdit *> _imAccountLineEdit;
	
	QList<QString> _imAccountsPicPath;
	
	QPushButton * _addIMAccount;
	
	QPushButton * _modifyIMAccount;
	
	QLineEdit * _cellphone;
	
	QLineEdit * _wengoPhone;
	
	QLineEdit * _homePhone;
	
	QLineEdit * _workPhone;
	
	QLineEdit * _email;
	
	QLineEdit * _blog;
	
	QLineEdit * _web;
	
	QLabel * _avatar;
	
	QString  _avatarPath;
	
	QTextEdit * _aboutYou;
	
	QPushButton * _saveChange;
	
	QPushButton * _cancelChange;
	
	QGroupBox * _wengoGeneralDetailsGroup;
	
	QGroupBox * _IMAccountGroup;
	
	QGroupBox * _personalDetailsGroup;
	
	QGroupBox * _advancedDetailsGroup;
	
	QGroupBox * _detailsSeenWPhoneGroup;
	
	QMap<QString,QVariant> _data;
	


};

#endif
