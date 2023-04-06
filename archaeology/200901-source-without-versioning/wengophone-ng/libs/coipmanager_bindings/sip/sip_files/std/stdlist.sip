// SIP support for std::list.
// Based on SIP support for std::vector made by Giovanni Bajo <rasky <at> develer.com>
// Public domain

template<TYPE>
%MappedType std::list<TYPE>
{
%TypeHeaderCode
#include <list>
%End

%ConvertFromTypeCode
    PyObject *l;

	// Create the Python list of the correct length.
	if ((l = PyList_New(sipCpp->size())) == NULL) {
		return NULL;
	}

	// Go through each element in the C++ instance and convert it to a
	// wrapped P2d.
	int i = 0;
    for (std::list<TYPE>::const_iterator it = sipCpp->begin();
		it != sipCpp->end(); ++it) {
		
		TYPE *cpp = new TYPE(*it);
		PyObject *pobj;

		// Get the Python wrapper for the Type instance, creating a new
		// one if necessary, and handle any ownership transfer.
		if ((pobj = sipConvertFromInstance(cpp, sipClass_TYPE, sipTransferObj)) == NULL) {
			// There was an error so garbage collect the Python list.
			Py_DECREF(l);
			return NULL;
		}

		// Add the wrapper to the list.
		PyList_SET_ITEM(l, i, pobj);
		i++;
	}

	// Return the Python list.
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

	// Iterate over the object
	PyObject *iterator = PyObject_GetIter(sipPy);
	PyObject *item;

	std::list<TYPE> *V = new std::list<TYPE>();

	while ((item = PyIter_Next(iterator)))
	{
		if (!sipCanConvertToInstance(item, sipClass_TYPE, SIP_NOT_NONE)) {
			PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to TYPE");
			*sipIsErr = 1;
			break;
		}

		int state;
		TYPE *p =
			reinterpret_cast<TYPE *>(sipConvertToInstance(item, sipClass_TYPE,
				0, SIP_NOT_NONE, &state, sipIsErr));

		if (!*sipIsErr) {
			V->push_back(*p);
		}

		sipReleaseInstance(p, sipClass_TYPE, state);
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
