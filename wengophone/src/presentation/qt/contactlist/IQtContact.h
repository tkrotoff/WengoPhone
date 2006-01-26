/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 #ifndef IQTCONTACT_H
 #define IQTCONTACT_H
 
 class IQtContact {
 public:
	IQtContact(Contact * contact) {
		_displayDetails = false;
		_contact = contact;
	}
		
	/**
	 * Set if details must be displayed.
	 *
	 * @param disp true if the details must be displayed.
	 */
	void setDisplayDetails(bool disp) {
		_displayDetails = disp;
	}
	
	/**
	 * @return true if details must be displayed
	 */
	bool isDisplayDetailsEnabled() {
		return _displayDetails;
	}

	Contact * getContact() {
		return _contact;
	}
	
private:
	bool _displayDetails;
	Contact * _contact;
};

#endif //IQTCONTACT_H
