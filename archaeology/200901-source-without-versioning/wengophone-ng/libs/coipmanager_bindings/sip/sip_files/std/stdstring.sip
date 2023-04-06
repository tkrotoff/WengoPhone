%MappedType std::string {

%TypeHeaderCode
#include <string>
%End

%ConvertFromTypeCode
	return PyString_FromString(sipCpp->c_str());
%End

%ConvertToTypeCode
	if (sipIsErr == 0)
	    return PyString_Check(sipPy);

	*sipCppPtr = new std::string(PyString_AsString(sipPy));
	return sipGetState(sipTransferObj);
%End

};
