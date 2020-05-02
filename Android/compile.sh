cd jni/
$AND_NDK/ndk-build -B
cd ../
./InstallAssets.sh
android update project -p . -t android-24
ant debug
adb install -r bin/NativeActivity-debug.apk 
