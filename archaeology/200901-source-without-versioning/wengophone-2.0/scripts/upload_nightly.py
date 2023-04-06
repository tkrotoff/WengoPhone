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

# Get svn revision from which this version comes

revision_number = ''
try:
    (stdin, stdout) = os.popen2("svnversion .")
    revision_number = stdout.readline().split(':')[0].rstrip(os.linesep)
    revision_number = re.sub('(M|S)', '', revision_number)
except IOError:
    print "Couldn't get revision number, aborting."
    ftp_connection.quit()
    sys.exit(1)


# Actually upload files
index = 4
while index < len(sys.argv):
    remote_filename = sys.argv[index + 1]
    local_filename = sys.argv[index]
    print 'Uploading remote filename: ' + str(re.sub('revnumber',
                                                     revision_number,
                                                     remote_filename))
    ftp_connection.storbinary('STOR ' +  re.sub('revnumber',
                                               revision_number,
                                               remote_filename),
                              open(local_filename, 'rb'))
    index += 2
ftp_connection.quit()

