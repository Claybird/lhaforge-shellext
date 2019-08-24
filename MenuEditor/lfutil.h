#pragma once

//複数階層のディレクトリを一気に作成する
BOOL UtilMakeSureDirectoryPathExists(LPCTSTR lpszPath);
//INIに指定されたセクションがあるならtrueを返す
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName);
