# Qt编译Webengineview
在安装的qt源码文件中有个readme文件，里面有编译qt所需要的环境以及编译步骤。先按照其中的教程试一试。我这点内存也不知道够用不够。

命令
-- -webengine-proprietary-codecs


./configure -confirm-license -opensource -platform win64-msvc -debug-and-release -prefix "./build" -qt-sqlite -qt-pcre -qt-zlib -qt-libpng -qt-libjpeg -opengl desktop -qt-freetype -nomake tests -no-compile-examples -nomake examples -webengine-proprietary-codecs



文件
C:\Qt\5.15.2\Src\qtwebengine\build\src\core\release\obj\third_party\angle\libANGLE.ninja
C:\Qt\5.15.2\Src\qtwebengine\build\src\core\debug\obj\third_party\angle\angle_common.ninja

删除/we4244

