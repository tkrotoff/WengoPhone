import ftplib
import ConfigParser

# Get config data
config = ConfigParser.ConfigParser()
config.readfp(open('nightlies.cfg'))
ftp_host = config.get('ftp_server', 'host')
ftp_login = config.get('ftp_server', 'login')
ftp_password = config.get('ftp_server', 'password')

# Login to FTP server
ftp_connection = ftplib.FTP(ftp_host, ftp_login, ftp_password)
ftp_connection.connect(ftp_host)
ftp_connection.login(ftp_login, ftp_password)
ftp_connection.set_pasv(True)

# Actually upload files
for filename in sys.argv:
    ftp_connection.storbinary('STOR ' + filename, open(filename))

ftp_connection.quit()

