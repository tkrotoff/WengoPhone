#include "QtUserList.h"

QtUserList * QtUserList::_instance = 0;

QtUserList * QtUserList::getInstance()
{
	if (_instance == 0)
	{
		_instance = new QtUserList;
	}
	return _instance;
}

QtUserList::QtUserList()
{
}

void QtUserList::paintUser(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index)
{
	QtUser * user;
	user = _userList[index.data().toString()];
	if (user)
		user->paint(painter, option, index);
}

void QtUserList::addUser(QtUser * user)
{
	if (user)
	{
		_userList[user->getId()] = user;
	}
}

void QtUserList::removeUser(const QString & userid)
{
	_userList.remove(userid);
}

void QtUserList::mouseOn(const QString & userid)
{
	QtUser * user;
	user = _userList[userid];
	
	if (user)
		user->setMouseOn(true);
		
	user = _userList[_lastMouseOn];
	if (user)
		user->setMouseOn(false);
	_lastMouseOn = userid;
}

void QtUserList::setUserStatus(const QString & userid,QtContactPixmap::contactPixmap status)
{
	QtUser * user;
	user = _userList[userid];
	if (user)
		user->setStatus(status);
}

QtUser * QtUserList::getUser(const QString & userid)
{
	QtUser * user;
	user = _userList[userid];
	return user;
}

void QtUserList::haveIM(const QString & userid, bool value){
	QtUser * user = _userList[userid];
	if (user)
		user->haveIM(value);
}

bool QtUserList::haveIM(const QString & userid){
	QtUser * user = _userList[userid];
	if (user)
		return user->haveIM();
	return false;
}

bool QtUserList::haveCall(const QString & userid){

	QtUser * user = _userList[userid];
	if (user)
		return user->haveCall();
	return false;
}
void QtUserList::haveCall(const QString & userid,bool value){
	QtUser * user = _userList[userid];
	if (user)
		user->haveCall(value);
}
	
bool QtUserList::haveVideo(const QString & userid){
	QtUser * user = _userList[userid];
	if (user)
		return user->haveVideo();
	return false;
}

void QtUserList::haveVideo(const QString & userid,bool value){
	QtUser * user = _userList[userid];
	if (user)
		user->haveVideo(value);
}

void QtUserList::mouseClicked(const QString & userid,const QPoint pos, const QRect & rect)
{
	QtUser * user = _userList[userid];
	if (user)
		user->mouseClicked(pos,rect);
}

int	QtUserList::getIconsStartPosition(const QString & userid)
{
	QtUser * user = _userList[userid];
	if (user)
		return user->getIconsStartPosition();
	return -1;
}

void QtUserList::setButton(const QString & userid,const Qt::MouseButton button)
{
	QtUser * user = _userList[userid];
	if (user)
		user->setButton(button);
}

Qt::MouseButton QtUserList::getButton(const QString & userid)
{
	QtUser * user = _userList[userid];
	if (user)
		return user->getButton();
	return Qt::NoButton;
}
void QtUserList::setOpenStatus(const QString & userid,bool value)
{
	QtUser * user = _userList[userid];
	if (user)
		user->setOpenStatus(value);
}