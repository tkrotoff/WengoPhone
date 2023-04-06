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
 
 /**
 * tests for iconset
 * 
 * @author Mathieu Stute
 */

#include <qapplication.h>
#include <stdio.h>

#include <iconset.h>
#include <assert.h>

#define OK "ok\n"
#define NOK "failed\n"

void unit_test(char * test_name, bool expr) {
	printf("\t* %s: ", test_name);
	expr ? printf(OK) : printf(NOK);
}

bool Test_Load(QString name, Iconset *iconset) {
	iconset->clear();
	return iconset->load(name);
}

bool Test_count(Iconset * iconset) {
	Icon *icon;
	int count = 0;
	
	QPtrListIterator<Icon> it = iconset->iterator();
	while ( (icon = it.current()) != 0 ) {
		it++;
		count++;
	}
	
	if(iconset->count() != count) {
		printf("\t\t* iconset count announced: %i / found: %i\n", iconset->count(), count);
		return false;
	}
	return true;
}

int main(int argc, char * argv[]) {
	QApplication app(argc, argv);
	Iconset i;
	
	printf("%s\n", "iconset tests...\n");

	unit_test("Load test nok", !Test_Load("doesnotexists", &i));
	i.clear();
	
	unit_test("Load test ok", Test_Load("emoticons/default", &i));
	unit_test("Count test ok", Test_count(&i));
	i.clear();
	
	unit_test("Load test nok", !Test_Load("emoticons/default2", &i));
	
	return 1;
}