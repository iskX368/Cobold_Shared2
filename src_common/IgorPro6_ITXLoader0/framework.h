#pragma once

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
// Windows ヘッダー ファイル
#define NOMINMAX //numelic_limits<T>::min()のエラー (C2589) 回避
#include <windows.h>
