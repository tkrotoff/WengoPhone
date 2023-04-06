
export LIB="$LIB;C:\tools\boost_1_33_1\lib"

# Change the Makefile for
# LOCAL_INCLUDES = -I$(srcdir) -IC:\tools\boost_1_33_1 -IC:\dev\coipmanager\libs\owutil -IC:\dev\coipmanager\libs\coipmanager\include -IC:\dev\coipmanager\libs\coipmanager_base\include


export MYPROJECT="C:\dev\wax\trunk\wax\\$MYPROJECT"

echo "Copying files for the $MYPROJECT project"

cp "C:\dev\build-xulrunner-2.0\extensions\xulconnection\src\xulconnection.dll" "$MYPROJECT\components"
cp "C:\dev\build-xulrunner-2.0\extensions\xulconnection\src\_xpidlgen\xulconnection.xpt" "$MYPROJECT\components"
cp "C:\dev\build-xulrunner-2.0\extensions\xulconnection\src\_xpidlgen\ICommand.xpt" "$MYPROJECT\components"
cp "C:\dev\build-xulrunner-2.0\extensions\xulconnection\src\_xpidlgen\IListener.xpt" "$MYPROJECT\components"
cp "C:\dev\build-xulrunner-2.0\extensions\xulconnection\src\_xpidlgen\ISession.xpt" "$MYPROJECT\components"
cp "C:\dev\build-xulrunner-2.0\extensions\xulconnection\src\_xpidlgen\IConfigurator.xpt" "$MYPROJECT\components"
cp "C:\dev\build-xulrunner-2.0\extensions\xulconnection\src\_xpidlgen\nsISystemTools.xpt" "$MYPROJECT\components"

