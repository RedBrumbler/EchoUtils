

$root = "${PSScriptRoot}/.."
$build = "${root}/build"
$yodel = "${yodel}/lib.so"

& ${PSScriptRoot}/build.ps1

& adb push ${yodel} /sdcard/ModData/com.readyatdawn.r15/Modloader/early_mods/libecho-utils.so
