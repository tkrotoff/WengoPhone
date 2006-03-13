#include "QtEditMyProfile.h"

#include <WidgetFactory.h>

QtEditMyProfile::QtEditMyProfile(QWidget * parent, Qt::WFlags f) : QDialog(parent,f){

	_widget = qobject_cast<QWidget *>(WidgetFactory::create(":/forms/login/profileWindow.ui", this));
	layout = new QGridLayout(this);
	layout->addWidget(_widget);
	// Config _config = ConfigManager::getInstance().getCurrentConfig();
	init();
	readFromConfig();
	connect(_saveChange,SIGNAL(clicked()),this,SLOT(saveClicked()));
	connect(_cancelChange,SIGNAL(clicked()),this,SLOT(cancelClicked()));
}

void QtEditMyProfile::saveClicked(){
	writeToConfig();
	accept();
}

void QtEditMyProfile::cancelClicked(){
	reject();
}

void QtEditMyProfile::writeToConfig(){
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	
	config.set(config.PROFILE_NICKNAME, _wengoNickName->text().toStdString());
	config.set(config.PROFILE_FIRSTNAME, _firstName->text().toStdString());
	config.set(config.PROFILE_LASTNAME, _lastName->text().toStdString());
	config.set(config.PROFILE_BIRTHDATE, _birthDate->date().toString().toStdString());
	config.set(config.PROFILE_CITY,_city->text().toStdString());

	config.set(config.PROFILE_ALTSIP,_altSip->text().toStdString());
	config.set(config.PROFILE_GENDER, _gender->currentText().toStdString());
	config.set(config.PROFILE_COUNTRY, _country->currentText().toStdString());
	config.set(config.PROFILE_STATE, _state->currentText().toStdString());

	config.set(config.PROFILE_IMEMAIL1,_imAccountLineEdit[0]->text().toStdString());
	config.set(config.PROFILE_IMEMAIL2,_imAccountLineEdit[1]->text().toStdString());
	config.set(config.PROFILE_IMEMAIL3,_imAccountLineEdit[2]->text().toStdString());
	config.set(config.PROFILE_IMEMAIL4,_imAccountLineEdit[3]->text().toStdString());
	config.set(config.PROFILE_IMEMAIL5,_imAccountLineEdit[4]->text().toStdString());
	config.set(config.PROFILE_IMEMAIL6,_imAccountLineEdit[5]->text().toStdString());
	config.set(config.PROFILE_IMEMAIL7,_imAccountLineEdit[6]->text().toStdString());
	config.set(config.PROFILE_IMEMAIL8,_imAccountLineEdit[7]->text().toStdString());
	config.set(config.PROFILE_IMEMAIL9,_imAccountLineEdit[8]->text().toStdString());
	
	config.set(config.PROFILE_IMPIC1,_imAccountsPicPath[0].toStdString());
	config.set(config.PROFILE_IMPIC2,_imAccountsPicPath[1].toStdString());
	config.set(config.PROFILE_IMPIC3,_imAccountsPicPath[2].toStdString());
	config.set(config.PROFILE_IMPIC4,_imAccountsPicPath[3].toStdString());
	config.set(config.PROFILE_IMPIC5,_imAccountsPicPath[4].toStdString());
	config.set(config.PROFILE_IMPIC6,_imAccountsPicPath[5].toStdString());
	config.set(config.PROFILE_IMPIC7,_imAccountsPicPath[6].toStdString());
	config.set(config.PROFILE_IMPIC8,_imAccountsPicPath[7].toStdString());
	config.set(config.PROFILE_IMPIC9,_imAccountsPicPath[8].toStdString());
	
	config.set(config.PROFILE_CELLPHONE,_cellphone->text().toStdString());
	config.set(config.PROFILE_HOMEPHONE,_homePhone->text().toStdString());
	config.set(config.PROFILE_WENGOPHONE,_wengoPhone->text().toStdString());
	config.set(config.PROFILE_WORKPHONE,_workPhone->text().toStdString());

	config.set(config.PROFILE_EMAIL, _email->text().toStdString());
	config.set(config.PROFILE_BLOG, _blog->text().toStdString());
	config.set(config.PROFILE_WEB, _web->text().toStdString());

	config.set(config.PROFILE_AVATAR,_avatarPath.toStdString());
	config.set(config.PROFILE_ABOUT,_aboutYou->toPlainText().toStdString());
}

void QtEditMyProfile::readFromConfig(){

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	
	_wengoNickName->setText(QString().fromStdString(config.getProfileNickName()));
	_firstName->setText(QString().fromStdString(config.getProfileFirstName()));
	_lastName->setText(QString().fromStdString(config.getProfileLastName()));
	_birthDate->setDate(QDate::fromString(QString().fromStdString(config.getProfileBirthDate())));
	_city->setText(QString().fromStdString(config.getProfileCity()));

	_altSip->setText(QString().fromStdString(config.getProfileAltSip()));
	_gender->setCurrentIndex(_gender->findText(QString().fromStdString(config.getProfileGender())));
	_country->setCurrentIndex(_country->findText(QString().fromStdString(config.getProfileCountry())));
	_state->setCurrentIndex(_state->findText(QString().fromStdString(config.getProfileState())));
	
	_imAccountLineEdit[0]->setText(QString().fromStdString(config.getProfileIMEmail1()));
	_imAccountLineEdit[1]->setText(QString().fromStdString(config.getProfileIMEmail2()));
	_imAccountLineEdit[2]->setText(QString().fromStdString(config.getProfileIMEmail3()));
	_imAccountLineEdit[3]->setText(QString().fromStdString(config.getProfileIMEmail4()));
	_imAccountLineEdit[4]->setText(QString().fromStdString(config.getProfileIMEmail5()));
	_imAccountLineEdit[5]->setText(QString().fromStdString(config.getProfileIMEmail6()));
	_imAccountLineEdit[6]->setText(QString().fromStdString(config.getProfileIMEmail7()));
	_imAccountLineEdit[7]->setText(QString().fromStdString(config.getProfileIMEmail8()));
	_imAccountLineEdit[8]->setText(QString().fromStdString(config.getProfileIMEmail9()));
	
	_imAccountsPic[0]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic1())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic1());
	_imAccountsPic[1]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic2())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic2());
	_imAccountsPic[2]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic3())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic3());
	_imAccountsPic[3]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic4())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic4());
	_imAccountsPic[4]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic5())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic5());
	_imAccountsPic[5]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic6())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic6());
	_imAccountsPic[6]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic7())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic7());
	_imAccountsPic[7]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic8())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic8());
	_imAccountsPic[8]->setPixmap(QPixmap(QString().fromStdString(config.getProfileIMPic9())));
	_imAccountsPicPath << QString().fromStdString(config.getProfileIMPic9());
	
	_cellphone->setText(QString().fromStdString(config.getProfileCellPhone()));
	_homePhone->setText(QString().fromStdString(config.getProfileHomePhone()));
	_wengoPhone->setText(QString().fromStdString(config.getProfileWengoPhone()));
	_workPhone->setText(QString().fromStdString(config.getProfileWorkPhone()));
	
	_email->setText(QString().fromStdString(config.getProfileEmail()));
	_blog->setText(QString().fromStdString(config.getProfileBlog()));
	_web->setText(QString().fromStdString(config.getProfileWeb()));
	
	_avatar->setPixmap(QPixmap(QString().fromStdString(config.getProfileAvatar())));
	_avatarPath = QString().fromStdString(config.getProfileAvatar());
	_aboutYou->clear();
	_aboutYou->insertPlainText(QString().fromStdString(config.getProfileAbout()));
	
}

void QtEditMyProfile::init(){

	_wengoNickName = _widget->findChild<QLineEdit *>("wengoNickName");
	_changePassword = _widget->findChild<QPushButton *>("changePassword");
	_firstName = _widget->findChild<QLineEdit *>("firstName");
	_lastName = _widget->findChild<QLineEdit *>("lastName");
	_birthDate = _widget->findChild<QDateEdit *>("birthDate");
	_city = _widget->findChild<QLineEdit *>("city");
	_altSip = _widget->findChild<QLineEdit *>("altSip");
	_addSipAccount = _widget->findChild<QPushButton *>("addSipAcount");
	
	_gender = _widget->findChild<QComboBox *>("gender");
	_country = _widget->findChild<QComboBox *>("country");
	_state = _widget->findChild<QComboBox *>("state");
	
	_cellphone = _widget->findChild<QLineEdit *>("cellPhone");
	_wengoPhone = _widget->findChild<QLineEdit *>("wengoPhone");
	_homePhone = _widget->findChild<QLineEdit *>("homePhone");
	_workPhone = _widget->findChild<QLineEdit *>("workPhone");
	
	// Advanced details
	_email = _widget->findChild<QLineEdit *>("email");
	_blog = _widget->findChild<QLineEdit *>("blog");
	_web = _widget->findChild<QLineEdit *>("web");
	
	_saveChange = _widget->findChild<QPushButton *>("saveChange");
	_cancelChange = _widget->findChild<QPushButton *>("cancelChange");

	
	// IM accounts pics
	QLabel * imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic1");
	_imAccountsPic << imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic2");
	_imAccountsPic << imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic3");
	_imAccountsPic << imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic4");
	_imAccountsPic << imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic5");
	_imAccountsPic << imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic6");
	_imAccountsPic << imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic7");
	_imAccountsPic << imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic8");
	_imAccountsPic << imAccountsPic;
	imAccountsPic = _widget->findChild<QLabel *>("imPic9");
	_imAccountsPic << imAccountsPic;
	// IM accounts data
	QLineEdit * imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount1");
	_imAccountLineEdit << imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount2");
	_imAccountLineEdit << imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount3");
	_imAccountLineEdit << imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount4");
	_imAccountLineEdit << imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount5");
	_imAccountLineEdit << imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount6");
	_imAccountLineEdit << imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount7");
	_imAccountLineEdit << imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount8");
	_imAccountLineEdit << imAccountData;
	imAccountData = _widget->findChild<QLineEdit *>("imAccount9");
	_imAccountLineEdit << imAccountData;

	_addIMAccount = _widget->findChild<QPushButton *>("addIMAccount");
	_modifyIMAccount = _widget->findChild<QPushButton *>("modityIMAccount");

	_avatar = _widget->findChild<QLabel *>("avatar");
	if (!_avatar)
		LOG_DEBUG("******************** NO AVATAR WIDGET ****************************");
	
	_aboutYou = _widget->findChild<QTextEdit *>("aboutYou");
	
	// Group box
	_wengoGeneralDetailsGroup = _widget->findChild<QGroupBox *>("wengoGeneralDetailsGroup");
	_IMAccountGroup=_widget->findChild<QGroupBox *>("IMAccountGroup");
	_personalDetailsGroup=_widget->findChild<QGroupBox *>("personalDetailsGroup");
	_advancedDetailsGroup=_widget->findChild<QGroupBox *>("advancedDetailsGroup");
	_detailsSeenWPhoneGroup=_widget->findChild<QGroupBox *>("detailsSeenWPhoneGroup");

	// Connections
}

void QtEditMyProfile::changeGroupBoxStat(QGroupBox * box, bool stat)
{
	QList<QWidget *> allWidgets = box->findChildren<QWidget *>();
	for (int i=0;i<allWidgets.size();i++){
		allWidgets[i]->setEnabled(stat);
	}
}

