#include <VCard.h>

#include <qstring.h>

#include <iostream>
using namespace std;

int main(int, char **)
{
  cout << "Test Write VCard" << endl;

  using namespace VCARD;
  
  VCard v;

  ContentLine cl1;
  cl1.setName(EntityTypeToParamName(EntityName));
  cl1.setValue(new TextValue("Hans Wurst"));
  v.add(cl1);

  ContentLine cl2;
  cl2.setName(EntityTypeToParamName(EntityTelephone));
  cl2.setValue(new TelValue("12345"));
  ParamList p;
  p.append( new TelParam("home") );
  p.append( new TelParam("fax") );
  cl2.setParamList( p );
  v.add(cl2);

  QCString str = v.asString();

  cout << "--- VCard begin ---" << endl
            << str
            << "--- VCard  end  ---" << endl;
}
