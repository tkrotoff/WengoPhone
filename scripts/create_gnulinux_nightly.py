import shutil
import tarfile
import os
import sys
import re
import stat

DEBUG = None

if '-d' in sys.argv:
    i = sys.argv.index('-d')
    DEBUG = sys.argv[i+1]
    del sys.argv[i]
    del sys.argv[i]

if DEBUG:
    f = open(DEBUG, 'a')
    sys.stderr = f
    sys.stdout = f

def debug_print(str):
    if DEBUG:
        print str
    
release_mode = sys.argv[1]
debug_print('Using release mode ' + str(release_mode) + '...')

temp_directory = '/tmp/' + str(os.getpid())
debug_print('Using temporary directory: ' + temp_directory + '...')

emoticons_path = 'debug/emoticons'
debug_print('Using emoticons path: ' + emoticons_path + '...')

sounds_path = 'debug/sounds'
debug_print('Using sounds path: ' + sounds_path + '...')

gaim_libs_path = 'libs/gaim/binary-lib/msvc'
debug_print('Using sounds path: ' + gaim_libs_path + '...')

debug_print('Creating temporary directory...')
os.mkdir(temp_directory)
debug_print('Temporary directory created!')

#copy dlls
debug_print('Copying dependent dynamic libraries...')
for file in [f for f in (os.listdir(release_mode)) if f.endswith('.so') or f == "qtwengophone"]:
    debug_print('Copying file ' + str(file) + ' to ' + str(temp_directory + '/' + file))
    shutil.copyfile(release_mode + '/' + file, temp_directory + '/' + file)
    os.chmod(temp_directory + '/' + file, stat.S_IRUSR | stat.S_IXUSR)
debug_print('Done copying dependent dynamic libraries!')

#copy emoticons
debug_print('Copying emoticons...')
shutil.copytree(emoticons_path, temp_directory + '/emoticons')
debug_print('Done copying emoticons!')

#copy sounds
debug_print('Copying sounds...')
shutil.copytree(sounds_path, temp_directory + '/sounds')
debug_print('Done copying sounds!')

#copy gaim plugins
debug_print('Copying gaim\'s plugins...')
for file in [f for f in (os.listdir(gaim_libs_path)) if f.endswith('.so')]:
    debug_print('Copying file ' + str(file) + ' to ' + str(temp_directory + '/' + file))
for file in [f for f in (os.listdir(gaim_libs_path + '/plugins')) if f.endswith('.so')]:
    debug_print('Copying file ' + str(file) + ' to ' + str(temp_directory + '/plugins/' + file))
debug_print('Done copying gaim\'s libs!')

#copy Qt runtime libraries
if os.environ.has_key('QTLIBDIR'):
    qt_libraries_directory = os.environ['QTLIBDIR']
elif os.environ.has_key('QTDIR'):
    qt_libraries_directory = os.environ['QTDIR'] + '/lib'
else:
    print 'Can\'t find Qt libraries directory, aborting!'
    sys.exit(1)
debug_print('Qt libraries directory is ' + qt_libraries_directory)

debug_print('Copying Qt DLL...')
shutil.copyfile(qt_libraries_directory + '/' +  'libQtCore.so.4.1.1', str(temp_directory) + '/libQtCore.so.4.1.1')
shutil.copyfile(qt_libraries_directory + '/' + 'libQtDesigner.so.4.1.1', str(temp_directory) + '/libQtDesigner.so.4.1.1')
shutil.copyfile(qt_libraries_directory + '/' + 'libQtGui.so.4.1.1', str(temp_directory) + '/libQtGui.so.4.1.1')
shutil.copyfile(qt_libraries_directory + '/' + 'libQtXml.so.4.1.1', str(temp_directory) + '/libQtXml.so.4.1.1')
shutil.copyfile(qt_libraries_directory + '/' + 'libQtSvg.so.4.1.1', str(temp_directory) + '/libQtSvg.so.4.1.1')
debug_print('Done copying Qt DLL!')

#Creating tarball and adding files to it
zip_file = tarfile.open('wengophone-ng-GNULinux-binary-latest.tar.bz2', 'w:bz2')
debug_print('Using zip file: ' + str(zip_file) + '...')

debug_print('Adding files to zip archive...')
for root, dirs, files in os.walk(temp_directory):
    for file in files:
        filename_to_add = os.path.join(root, file)
        filename_in_zip = re.sub(re.escape(temp_directory + '/'), '', filename_to_add)
        debug_print('Adding file ' + re.escape(filename_to_add) + ' as ' + filename_in_zip + '...') 
        zip_file.add(filename_to_add, filename_in_zip)
debug_print('Zip file created!')

#Cleaning temporary directory
debug_print('Cleaning temporary...')
shutil.rmtree(temp_directory)
debug_print('Temporary directory cleaned!')
debug_print('Done!')
