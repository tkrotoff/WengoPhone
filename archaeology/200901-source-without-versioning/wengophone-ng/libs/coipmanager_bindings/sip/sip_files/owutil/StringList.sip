%MappedType StringList {

%TypeHeaderCode
#include <util/StringList.h>
%End

%ConvertFromTypeCode
	PyObject *l = NULL;

	if ((l = PyList_New(sipCpp->size())) == NULL)
		return NULL;

	for (int i = 0; i < (int)sipCpp->size(); ++i) {
		PyObject *pobj = PyString_FromString(sipCpp->at(i).c_str());
		PyList_SET_ITEM(l, i, pobj);
	}

	return l;
%End

%ConvertToTypeCode
	// Check if type is compatible
	if (sipIsErr == NULL) {
		// Must be any iterable
		PyObject *i = PyObject_GetIter(sipPy);
		bool iterable = (i != NULL);
		Py_XDECREF(i);
		return iterable;
	}

	PyObject *iterator = PyObject_GetIter(sipPy);
	PyObject *item = NULL;

	StringList *V = new StringList();

	while ((item = PyIter_Next(iterator))) {
		if (!sipCanConvertToMappedType(item, sipFindMappedType("std::string"), SIP_NOT_NONE)) {
			PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to std::string");
			*sipIsErr = 1;
			break;
		}

		int state;
		std::string* p = 
			reinterpret_cast<std::string*>(sipConvertToMappedType(
				item, sipFindMappedType("std::string"), 0, SIP_NOT_NONE, &state, sipIsErr));

		if (!*sipIsErr) {
			V->push_back(*p);
		}

		sipReleaseMappedType(p, sipFindMappedType("std::string"), state);
		Py_DECREF(item);
	}

	Py_DECREF(iterator);

	if (*sipIsErr) {
		delete V;
		return 0;
	}

	*sipCppPtr = V;
	return sipGetState(sipTransferObj);
%End
};
