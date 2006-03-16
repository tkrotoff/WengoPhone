import ftplib
import ConfigParser
import sys
import os
import re, time

if len(sys.argv) < 5:
    print "usage: python upload_nightly.py"\
    "[source|binary|installer] [NG|Classic] [GNULinux|Win32|MacOSX]"\
    "[data_file] [info_file]"
    sys.exit(1)
nightly_type = sys.argv[1]
project_name = sys.argv[2]
nightly_target_platform = sys.argv[3]    

# Get config data
config = ConfigParser.ConfigParser()
config.readfp(open('nightlies.cfg'))
ftp_host = config.get('ftp_server', 'host')
ftp_login = config.get('ftp_server', 'login')
ftp_password = config.get('ftp_server', 'password')

dst_directory = config.get(project_name + '-' + nightly_type,\
                           nightly_target_platform)

# Login to FTP server
ftp_connection = ftplib.FTP(ftp_host, ftp_login, ftp_password)
ftp_connection.connect(ftp_host)
ftp_connection.login(ftp_login, ftp_password)
ftp_connection.set_pasv(True)

# Change working directory to be in the right place
ftp_connection.sendcmd('cwd ' + dst_directory)

# Actually upload files
for filename in sys.argv[4:]:
    index = sys.argv.index(filename)
    ftp_connection.storbinary('STOR ' + re.sub('timestamp',
                                               time.strftime("%Y%m%d%H%M%S",
                                                             time.gmtime()),
                                               sys.argv[index + 1]),
                              open(filename, 'rb'))
    del sys.argv[index + 1]
ftp_connection.quit()

