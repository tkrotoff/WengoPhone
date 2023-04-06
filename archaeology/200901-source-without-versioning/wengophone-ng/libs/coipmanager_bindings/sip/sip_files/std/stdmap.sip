// SIP support for std::map.
// Code refactored from QMap

template<TYPE1, TYPE2>
%MappedType std::map<TYPE1, TYPE2>
{
%TypeHeaderCode
#include <map>
%End

%ConvertFromTypeCode
    // Create the dictionary.
    PyObject *d = PyDict_New();

    if (!d)
        return NULL;

    // Set the dictionary elements.
    std::map<TYPE1, TYPE2>::const_iterator i = sipCpp->begin();

    while (i != sipCpp->end())
    {
        TYPE1 *t1 = new TYPE1(i.first);
        TYPE2 *t2 = new TYPE2(i.second);

        PyObject *t1obj = sipConvertFromNewInstance(t1, sipClass_TYPE1, sipTransferObj);
        PyObject *t2obj = sipConvertFromNewInstance(t2, sipClass_TYPE2, sipTransferObj);

        if (t1obj == NULL || t2obj == NULL || PyDict_SetItem(d, t1obj, t2obj) < 0)
        {
            Py_DECREF(d);

            if (t1obj)
                Py_DECREF(t1obj);
            else
                delete t1;

            if (t2obj)
                Py_DECREF(t2obj);
            else
                delete t2;

            return NULL;
        }

        Py_DECREF(t1obj);
        Py_DECREF(t2obj);

        ++i;
    }

    return d;
%End

%ConvertToTypeCode
    PyObject *t1obj, *t2obj;
#if PY_VERSION_HEX >= 0x02050000
    Py_ssize_t i = 0;
#else
    int i = 0;
#endif

    // Check the type if that is all that is required.
    if (sipIsErr == NULL)
    {
        if (!PyDict_Check(sipPy))
            return 0;

        while (PyDict_Next(sipPy, &i, &t1obj, &t2obj))
        {
            if (!sipCanConvertToInstance(t1obj, sipClass_TYPE1, SIP_NOT_NONE))
                return 0;

            if (!sipCanConvertToInstance(t2obj, sipClass_TYPE2, SIP_NOT_NONE))
                return 0;
        } 

        return 1;
    }

    std::map<TYPE1, TYPE2> *result = new std::map<TYPE1, TYPE2>;
 
    while (PyDict_Next(sipPy, &i, &t1obj, &t2obj))
    {
        int state1, state2;

        TYPE1 *t1 = reinterpret_cast<TYPE1 *>(sipConvertToInstance(t1obj, sipClass_TYPE1, sipTransferObj, SIP_NOT_NONE, &state1, sipIsErr));
        TYPE2 *t2 = reinterpret_cast<TYPE2 *>(sipConvertToInstance(t2obj, sipClass_TYPE2, sipTransferObj, SIP_NOT_NONE, &state2, sipIsErr));
 
        if (*sipIsErr)
        {
            sipReleaseInstance(t1, sipClass_TYPE1, state1);
            sipReleaseInstance(t2, sipClass_TYPE2, state2);

            delete result;
            return 0;
        }

        result->insert(*t1, *t2);

        sipReleaseInstance(t1, sipClass_TYPE1, state1);
        sipReleaseInstance(t2, sipClass_TYPE2, state2);
    }
 
    *sipCppPtr = result;
 
    return sipGetState(sipTransferObj);
%End
};

%MappedType std::map<std::string, std::string>
{
%TypeHeaderCode
#include <map>
%End

%ConvertFromTypeCode
    // Create the dictionary.
    PyObject *d = PyDict_New();

    if (!d)
        return NULL;

    // Set the dictionary elements.
    std::map<std::string, std::string>::const_iterator i = sipCpp->begin();

    while (i != sipCpp->end())
    {
        PyObject *t1obj = PyString_FromString((*i).first.c_str());
        PyObject *t2obj = PyString_FromString((*i).second.c_str());

        if (t1obj == NULL || t2obj == NULL || PyDict_SetItem(d, t1obj, t2obj) < 0)
        {
            Py_DECREF(d);

            if (t1obj)
                Py_DECREF(t1obj);

            if (t2obj)
                Py_DECREF(t2obj);

            return NULL;
        }

        Py_DECREF(t1obj);
        Py_DECREF(t2obj);

        ++i;
    }

    return d;
%End

%ConvertToTypeCode
    PyObject *t1obj, *t2obj;
#if PY_VERSION_HEX >= 0x02050000
    Py_ssize_t i = 0;
#else
    int i = 0;
#endif

	// Check the type if that is all that is required.
	if (sipIsErr == NULL)
	{
		if (!PyDict_Check(sipPy))
			return 0;

		while (PyDict_Next(sipPy, &i, &t1obj, &t2obj))
		{
			if (!PyString_Check(t1obj))
				return 0;

			if (!PyString_Check(t2obj))
				return 0;
		} 

		return 1;
	}

	std::map<std::string, std::string> *result = new std::map<std::string, std::string>;

	while (PyDict_Next(sipPy, &i, &t1obj, &t2obj)) {
		result->insert(std::pair<std::string, std::string>(std::string(PyString_AsString(t1obj)), std::string(PyString_AsString(t2obj))));
	}

	*sipCppPtr = result;

	return sipGetState(sipTransferObj);
%End
};
