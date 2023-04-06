# 
# WengoPhone, a voice over Internet phone
#
# Copyright (C) 2004-2005  Wengo
# Copyright (C) 2005 David Ferlier <david.ferlier@wengo.fr>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from wengoscons.common import wengoenv

def images_descr_build(target, source, env):
    dst = target[0]
    images_handle = file(str(dst), "w")
    for src in source:
        images_handle.write(str(src) + "\n")
    images_handle.close()

def WengoQt3ImagesEmbed(env, target, images):
    return env.Command(
        target, images, "$QT_UIC -embed project $SOURCES -o $TARGET")

def WengoQt3WriteImagesToFile(env, target, images):
    return env.Command(target, images, images_descr_build)

def WengoQt3ImagesEmbedFromFile(env, target, filename):
    return env.Command(target, filename, "$QT_UIC -embed project -f $SOURCES -o $TARGET")

wengoenv.WengoEnvInstallFunction("WengoQt3ImagesEmbed", WengoQt3ImagesEmbed)
wengoenv.WengoEnvInstallFunction("WengoQt3ImagesEmbedFromFile", WengoQt3ImagesEmbedFromFile)
wengoenv.WengoEnvInstallFunction("WengoQt3WriteImagesToFile", WengoQt3WriteImagesToFile)
