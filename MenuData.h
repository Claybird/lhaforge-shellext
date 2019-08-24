#pragma once
//#include "resource.h"

//#define OUTPUTDIR_DEFAULT		_T(">?*DEFAULT*")	//通常の場所に解凍
#define OUTPUTDIR_DESKTOP		_T(">?*DESKTOP*")	//デスクトップに解凍
#define OUTPUTDIR_SAMEDIR		_T(">?*SAMEDIR*")	//同じ場所に解凍
#define OUTPUTDIR_SPECIFIED		_T(">?*SPECIFIED*")	//指定場所に解凍
//#define OUTPUTDIR_HERE		_T">?*HERE*")		//ここに解凍

//キャプション名とLhaForgeの処理パラメータ
struct SHELL_MENU_ITEM{
	WORD Caption;
	LPCTSTR Param;
};

struct SHELL_MENU_ITEM_CONTAINER{
	const SHELL_MENU_ITEM* pMenu;
	size_t numItems;
};

struct CUserMenuItem{	//ユーザー定義メニューの項目定義
	TCHAR	Caption[_MAX_PATH+1];
	TCHAR	Param[_MAX_PATH+1];
	BOOL	bShellMenu;		//右クリックメニューで表示
	BOOL	bDragMenu;		//右ドラッグメニューで表示
};


//圧縮メニュー
const SHELL_MENU_ITEM g_CompressMenu[]={
	{IDS_MENU_ITEM_CAPTION_LZH,_T("/c:lzh")},
	{IDS_MENU_ITEM_CAPTION_ZIP,_T("/c:zip")},
	{IDS_MENU_ITEM_CAPTION_CAB,_T("/c:cab")},
	{IDS_MENU_ITEM_CAPTION_7Z,_T("/c:7z")},
	{IDS_MENU_ITEM_CAPTION_TAR,_T("/c:tar")},
	{IDS_MENU_ITEM_CAPTION_GZ,_T("/c:gz")},
	{IDS_MENU_ITEM_CAPTION_BZ2,_T("/c:bz2")},
	{IDS_MENU_ITEM_CAPTION_XZ,_T("/c:xz")},
	{IDS_MENU_ITEM_CAPTION_TGZ,_T("/c:tgz")},
	{IDS_MENU_ITEM_CAPTION_TBZ,_T("/c:tbz")},
	{IDS_MENU_ITEM_CAPTION_TXZ,_T("/c:txz")},
	{IDS_MENU_ITEM_CAPTION_JACK,_T("/c:jak")},
	{IDS_MENU_ITEM_CAPTION_YZ1,_T("/c:yz1")},
	{IDS_MENU_ITEM_CAPTION_HKI,_T("/c:hki")},
	{IDS_MENU_ITEM_CAPTION_BZA,_T("/c:bza")},
	{IDS_MENU_ITEM_CAPTION_GZA,_T("/c:gza")},
	{IDS_MENU_ITEM_CAPTION_ISH,_T("/c:ish")},
	{IDS_MENU_ITEM_CAPTION_UUE,_T("/c:uue")},
	{IDS_MENU_ITEM_CAPTION_COMPRESS_B2E,_T("/b2e /c")},
};

//発展圧縮メニュー
const SHELL_MENU_ITEM
	g_ExtraMenuLZH[]={
		{IDS_MENU_ITEM_CAPTION_LZH,NULL},
		{IDS_MENU_ITEM_CAPTION_LZH,_T("/c:lzh")},
		{IDS_MENU_ITEM_CAPTION_LZH_SFX,_T("/c:lzhsfx")}
	},
	g_ExtraMenuZIP[]={
		{IDS_MENU_ITEM_CAPTION_ZIP,NULL},
		{IDS_MENU_ITEM_CAPTION_ZIP,_T("/c:zip")},
		{IDS_MENU_ITEM_CAPTION_ZIP_PASS,_T("/c:zippass")},
		{IDS_MENU_ITEM_CAPTION_ZIP_SFX,_T("/c:zipsfx")},
		{IDS_MENU_ITEM_CAPTION_ZIP_PASS_SFX,_T("/c:zippasssfx")},
		{IDS_MENU_ITEM_CAPTION_ZIP_SPLIT,_T("/c:zipsplit")},
		{IDS_MENU_ITEM_CAPTION_ZIP_PASS_SPLIT,_T("/c:zippasssplit")}
	},
	g_ExtraMenuCAB[]={
		{IDS_MENU_ITEM_CAPTION_CAB,NULL},
		{IDS_MENU_ITEM_CAPTION_CAB,_T("/c:cab")},
		{IDS_MENU_ITEM_CAPTION_CAB_SFX,_T("/c:cabsfx")}
	},
	g_ExtraMenu7Z[]={
		{IDS_MENU_ITEM_CAPTION_7Z,NULL},
		{IDS_MENU_ITEM_CAPTION_7Z,_T("/c:7z")},
		{IDS_MENU_ITEM_CAPTION_7Z_PASS,_T("/c:7zpass")},
		{IDS_MENU_ITEM_CAPTION_7Z_SFX,_T("/c:7zsfx")},
		{IDS_MENU_ITEM_CAPTION_7Z_SPLIT,_T("/c:7zsplit")},
		{IDS_MENU_ITEM_CAPTION_7Z_PASS_SPLIT,_T("/c:7zpasssplit")}
	},
	g_ExtraMenuTAR[]={
		{IDS_MENU_ITEM_CAPTION_TAR_RELATED,NULL},
		{IDS_MENU_ITEM_CAPTION_TAR,_T("/c:tar")},
		{IDS_MENU_ITEM_CAPTION_GZ,_T("/c:gz")},
		{IDS_MENU_ITEM_CAPTION_BZ2,_T("/c:bz2")},
		{IDS_MENU_ITEM_CAPTION_XZ,_T("/c:xz")},
		{IDS_MENU_ITEM_CAPTION_LZMA,_T("/c:lzma")},
		{IDS_MENU_ITEM_CAPTION_TGZ,_T("/c:tgz")},
		{IDS_MENU_ITEM_CAPTION_TBZ,_T("/c:tbz")},
		{IDS_MENU_ITEM_CAPTION_TXZ,_T("/c:txz")},
		{IDS_MENU_ITEM_CAPTION_TLZ,_T("/c:tlz")},
	},
	g_ExtraMenuJACK[]={
		{IDS_MENU_ITEM_CAPTION_JACK,NULL},
		{IDS_MENU_ITEM_CAPTION_JACK,_T("/c:jak")},
		{IDS_MENU_ITEM_CAPTION_JACK_SFX,_T("/c:jaksfx")}
	},
	g_ExtraMenuYZ1[]={
		{IDS_MENU_ITEM_CAPTION_YZ1,NULL},
		{IDS_MENU_ITEM_CAPTION_YZ1,_T("/c:yz1")},
		{IDS_MENU_ITEM_CAPTION_YZ1_PASS,_T("/c:yz1pass")},
		{IDS_MENU_ITEM_CAPTION_YZ1_SFX,_T("/c:yz1sfx")},
		{IDS_MENU_ITEM_CAPTION_YZ1_PUB_PASS,_T("/c:yz1pubpass")},
		{IDS_MENU_ITEM_CAPTION_YZ1_PUB_PASS_SFX,_T("/c:yz1pubpasssfx")}
	},
	g_ExtraMenuHKI[]={
		{IDS_MENU_ITEM_CAPTION_HKI,NULL},
		{IDS_MENU_ITEM_CAPTION_HKI,_T("/c:hki")},
		{IDS_MENU_ITEM_CAPTION_HKI_PASS,_T("/c:hkipass")},
		{IDS_MENU_ITEM_CAPTION_HKI_SFX,_T("/c:hkisfx")},
	},
	g_ExtraMenuBGA[]={
		{IDS_MENU_ITEM_CAPTION_BGA,NULL},
		{IDS_MENU_ITEM_CAPTION_BZA,_T("/c:bza")},
		{IDS_MENU_ITEM_CAPTION_BZA_SFX,_T("/c:bzasfx")},
		{IDS_MENU_ITEM_CAPTION_GZA,_T("/c:gza")},
		{IDS_MENU_ITEM_CAPTION_GZA_SFX,_T("/c:gzasfx")}
	},
	g_ExtraMenuAISH[]={
		{IDS_MENU_ITEM_CAPTION_AISH,NULL},
		{IDS_MENU_ITEM_CAPTION_ISH,_T("/c:ish")},
		{IDS_MENU_ITEM_CAPTION_UUE,_T("/c:uue")}
	}
;

#define ITEM_AND_COUNT(x) {x,COUNTOF(x)}

const SHELL_MENU_ITEM_CONTAINER g_ExtraMenuTable[]={
	ITEM_AND_COUNT(g_ExtraMenuLZH),
	ITEM_AND_COUNT(g_ExtraMenuZIP),
	ITEM_AND_COUNT(g_ExtraMenuCAB),
	ITEM_AND_COUNT(g_ExtraMenu7Z),
	ITEM_AND_COUNT(g_ExtraMenuTAR),
	ITEM_AND_COUNT(g_ExtraMenuJACK),
	ITEM_AND_COUNT(g_ExtraMenuYZ1),
	ITEM_AND_COUNT(g_ExtraMenuHKI),
	ITEM_AND_COUNT(g_ExtraMenuBGA),
	ITEM_AND_COUNT(g_ExtraMenuAISH),
};

//解凍メニュー
const SHELL_MENU_ITEM g_ExtractMenu[]={
	{IDS_MENU_CAPTION_EXTRACT_NORMAL,_T("/e")},
	{IDS_MENU_CAPTION_EXTRACT_DESKTOP,_T("/e /o:")OUTPUTDIR_DESKTOP},
	{IDS_MENU_CAPTION_EXTRACT_SAME,_T("/e /o:")OUTPUTDIR_SAMEDIR},
	{IDS_MENU_CAPTION_EXTRACT_TARGET,_T("/e /o:")OUTPUTDIR_SPECIFIED}
};

//XacRettメニュー
const SHELL_MENU_ITEM g_XacRettMenu[]={
	{IDS_MENU_CAPTION_EXTRACT_NORMAL,_T("/xacrett /e")},
	{IDS_MENU_CAPTION_EXTRACT_DESKTOP,_T("/xacrett /e /o:")OUTPUTDIR_DESKTOP},
	{IDS_MENU_CAPTION_EXTRACT_SAME,_T("/xacrett /e /o:")OUTPUTDIR_SAMEDIR},
	{IDS_MENU_CAPTION_EXTRACT_TARGET,_T("/xacrett /e /o:")OUTPUTDIR_SPECIFIED},
	{IDS_MENU_CAPTION_LIST_XACRETT,_T("/xacrett /l")}
};

//B2Eメニュー
const SHELL_MENU_ITEM g_B2EMenu[]={
	{IDS_MENU_ITEM_CAPTION_COMPRESS_B2E,_T("/b2e /c")},
	{IDS_MENU_CAPTION_EXTRACT_NORMAL,_T("/b2e /e")},
	{IDS_MENU_CAPTION_EXTRACT_DESKTOP,_T("/b2e /e /o:")OUTPUTDIR_DESKTOP},
	{IDS_MENU_CAPTION_EXTRACT_SAME,_T("/b2e /e /o:")OUTPUTDIR_SAMEDIR},
	{IDS_MENU_CAPTION_EXTRACT_TARGET,_T("/b2e /e /o:")OUTPUTDIR_SPECIFIED},
	{IDS_MENU_CAPTION_LIST_XACRETT,_T("/b2e /l")}
};

#ifndef MENUEDITOR
//B2E(Drag)メニュー
const SHELL_MENU_ITEM g_B2EDragMenu[]={
	{IDS_MENU_CAPTION_COMPRESS_DRAG,_T("/b2e /c")},
	{IDS_MENU_CAPTION_EXTRACT_DRAG,_T("/b2e /e")},
};
#endif
//-------------------------------------------------
//ユーザーによる圧縮メニューカスタマイズ用テーブル

struct SHELL_MENU_ITEM_USER{
	TCHAR Identifier;				//メニューアイテム識別文字
	const SHELL_MENU_ITEM *lpMenuItem;	//メニューアイテムへのポインタ
};

const SHELL_MENU_ITEM_USER g_UserMenuTable[]={
	{_T('l'),&g_ExtraMenuLZH[1]},
	{_T('L'),&g_ExtraMenuLZH[2]},

	{_T('z'),&g_ExtraMenuZIP[1]},
	{_T('Z'),&g_ExtraMenuZIP[2]},
	{_T('s'),&g_ExtraMenuZIP[3]},
	{_T('S'),&g_ExtraMenuZIP[4]},
	{_T('p'),&g_ExtraMenuZIP[5]},
	{_T('P'),&g_ExtraMenuZIP[6]},

	{_T('c'),&g_ExtraMenuCAB[1]},
	{_T('C'),&g_ExtraMenuCAB[2]},

	{_T('7'),&g_ExtraMenu7Z[1]},
	{_T('6'),&g_ExtraMenu7Z[2]},
	{_T('5'),&g_ExtraMenu7Z[3]},
	{_T('4'),&g_ExtraMenu7Z[4]},
	{_T('3'),&g_ExtraMenu7Z[5]},

	{_T('t'),&g_ExtraMenuTAR[1]},	//tar
	{_T('q'),&g_ExtraMenuTAR[2]},	//gz
	{_T('Q'),&g_ExtraMenuTAR[3]},	//bz2
	{_T('a'),&g_ExtraMenuTAR[4]},	//xz
	{_T('A'),&g_ExtraMenuTAR[5]},	//lzma
	{_T('v'),&g_ExtraMenuTAR[6]},	//tgz
	{_T('V'),&g_ExtraMenuTAR[7]},	//tbz
	{_T('d'),&g_ExtraMenuTAR[8]},	//txz
	{_T('D'),&g_ExtraMenuTAR[9]},	//tlz

	{_T('j'),&g_ExtraMenuJACK[1]},
	{_T('J'),&g_ExtraMenuJACK[2]},

	{_T('y'),&g_ExtraMenuYZ1[1]},
	{_T('Y'),&g_ExtraMenuYZ1[2]},
	{_T('x'),&g_ExtraMenuYZ1[3]},
	{_T('X'),&g_ExtraMenuYZ1[4]},
	{_T('1'),&g_ExtraMenuYZ1[5]},

	{_T('h'),&g_ExtraMenuHKI[1]},
	{_T('H'),&g_ExtraMenuHKI[2]},
	{_T('k'),&g_ExtraMenuHKI[3]},

	{_T('b'),&g_ExtraMenuBGA[1]},
	{_T('B'),&g_ExtraMenuBGA[2]},
	{_T('g'),&g_ExtraMenuBGA[3]},
	{_T('G'),&g_ExtraMenuBGA[4]},

	{_T('i'),&g_ExtraMenuAISH[1]},
	{_T('u'),&g_ExtraMenuAISH[2]},

	{_T('/'),&g_B2EMenu[0]},
};
const LPCTSTR USER_MENU_DEFAULT=_T("lzc7tqQavVdjyhbgiu/");
