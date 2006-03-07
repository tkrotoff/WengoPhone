import shutil
import zipfile
import os
import sys
import re

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

zip_file = zipfile.ZipFile('c:\\temp\qtwengophone-latest.zip', 'w')
debug_print('Using zip file: ' + str(zip_file) + '...')

temp_directory = 'c:\\temp\\' + str(os.getpid())
debug_print('Using temporary directory: ' + temp_directory + '...')

emoticons_path = 'debug\emoticons'
debug_print('Using emoticons path: ' + emoticons_path + '...')

sounds_path = 'debug\sounds'
debug_print('Using sounds path: ' + sounds_path + '...')

gaim_libs_path = 'libs\\gaim\\binary-lib\\msvc'
debug_print('Using sounds path: ' + gaim_libs_path + '...')

debug_print('Creating temporary directory...')
os.mkdir(temp_directory)
debug_print('Temporary directory created!')

#copy dlls
debug_print('Copying dependent dynamic libraries...')
for file in [f for f in (os.listdir(release_mode)) if f.endswith('.dll') or f.endswith('.exe')]:
    debug_print('Copying file ' + str(file) + ' to ' + str(temp_directory + '\\' + file))
    shutil.copyfile(release_mode + '\\' + file, temp_directory + '\\' + file)
debug_print('Done copying dependent dynamic libraries!')

#copy emoticons
debug_print('Copying emoticons...')
shutil.copytree(emoticons_path, temp_directory + '\\emoticons')
debug_print('Done copying emoticons!')

#copy sounds
debug_print('Copying sounds...')
shutil.copytree(sounds_path, temp_directory + '\sounds')
debug_print('Done copying sounds!')

#copy gaim plugins
debug_print('Copying gaim\'s plugins...')
for file in [f for f in (os.listdir(gaim_libs_path)) if f.endswith('.dll')]:
    debug_print('Copying file ' + str(file) + ' to ' + str(temp_directory + '\\' + file))
for file in [f for f in (os.listdir(gaim_libs_path + '\plugins')) if f.endswith('.dll')]:
    debug_print('Copying file ' + str(file) + ' to ' + str(temp_directory + '\plugins\\' + file))
debug_print('Done copying gaim\'s libs!')

#copy MS C and C++ runtime
debug_print('Copying MS C and C++ runtime DLLs...')
shutil.copyfile('c:\\windows\\system32\\msvcr71.dll', str(temp_directory) + '\\msvcr71.dll')
shutil.copyfile('c:\\windows\\system32\\msvcr71d.dll', str(temp_directory) + '\\msvcr71d.dll')
shutil.copyfile('c:\\windows\\system32\\msvcp71.dll', str(temp_directory) + '\\msvcp71.dll')
debug_print('Done copying MS C and C++ runtime DLLs!')

#copy Qt runtime libraries
if os.environ.has_key('QTLIBDIR'):
    qt_libraries_directory = os.environ['QTLIBDIR']
elif os.environ.has_key('QTDIR'):
    qt_libraries_directory = os.environ['QTDIR'] + '\\lib'
else:
    print 'Can\'t find Qt libraries directory, aborting!'
    sys.exit(1)
debug_print('Qt libraries directory is ' + qt_libraries_directory)

debug_print('Copying Qt DLL...')
shutil.copyfile(qt_libraries_directory + '\\' + 'QtCored4.dll', str(temp_directory) + '\\QtCored4.dll')
shutil.copyfile(qt_libraries_directory + '\\' + 'QtDesignerd4.dll', str(temp_directory) + '\\QtDesignerd4.dll')
shutil.copyfile(qt_libraries_directory + '\\' + 'QtGuid4.dll', str(temp_directory) + '\\QtGuid4.dll')
shutil.copyfile(qt_libraries_directory + '\\' + 'QtXmld4.dll', str(temp_directory) + '\\QtXmld4.dll')
debug_print('Done copying Qt DLL!')

debug_print('Adding files to zip archive...')
for root, dirs, files in os.walk(temp_directory):
    for file in files:
        filename_to_add = os.path.join(root, file)
        filename_in_zip = re.sub(re.escape(temp_directory + '\\'), '', filename_to_add)
        debug_print('Adding file ' + re.escape(filename_to_add) + ' as ' + filename_in_zip + '...') 
        zip_file.write(filename_to_add, filename_in_zip)
debug_print('Zip file created!')

debug_print('Cleaning temporary...')
shutil.rmtree(temp_directory)
debug_print('Temporary directory cleaned!')
debug_print('Done!')
