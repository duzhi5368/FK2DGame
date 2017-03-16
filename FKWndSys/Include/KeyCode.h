/**
*	created:		2012-6-23   23:30
*	filename: 		KeyCode
*	author:			FreeKnight
*	Copyright (C): 	
*	purpose:		
*/
//------------------------------------------------------------------------
#include "../../FKGame/WndSys/WndSys.h"
//------------------------------------------------------------------------
// 键码类型
#define unuse_key		0x1
#define num_key			0x2
#define alpha_key		0x4
#define sign_key		0x8
#define func_key		0x10
//------------------------------------------------------------------------
// 上档键依赖于哪种方式
#define dp_capslock		0x20
#define dp_shift		0x40
#define dp_numlock		0x80
//------------------------------------------------------------------------
static unsigned char KEY_CODE[256] = 
{
	unuse_key,// 0
	func_key,// 1 (VK_LBUTTON)(0x01)
	func_key,// 2 (VK_RBUTTON)(0x02)
	func_key,// 3 (VK_CANCEL)(0x03)
	func_key,// 4 (VK_MBUTTON)(0x04)    /* NOT contiguous with L & RBUTTON */
	unuse_key,// 5
	unuse_key,// 6
	unuse_key,// 7
	func_key,// 8 (VK_BACK)(0x08)
	func_key,// 9 (VK_TAB)(0x09)
	func_key,// 10
	unuse_key,// 11
	func_key,// 12 (VK_CLEAR)(0x0C)
	func_key,// 13 (VK_RETURN)(0x0D)
	unuse_key,// 14
	unuse_key,// 15
	func_key,// 16 (VK_SHIFT)(0x10)
	func_key,// 17 (VK_CONTROL)(0x11)
	func_key,// 18 (VK_MENU)(0x12)
	func_key,// 19 (VK_PAUSE)(0x13)
	func_key,// 20 (VK_CAPITAL)(0x14)
	func_key,// 21 (VK_KANA,VK_HANGEUL,VK_HANGUL)(0x15)
	unuse_key,// 22
	func_key,// 23 (VK_JUNJA)(0x17)
	func_key,// 24 (VK_FINAL)(0x18)
	func_key,// 25 (VK_HANJA,VK_KANJI)(0x19)
	unuse_key,// 26
	func_key,// 27 (VK_ESCAPE)(0x1B)
	func_key,// 28 (VK_CONVERT)(0x1C)
	func_key,// 29 (VK_NONCONVERT)(0x1D)
	func_key,// 30 (VK_ACCEPT)(0x1E)
	func_key,// 31 (VK_MODECHANGE)(0x1F)
	alpha_key,// 32 (VK_SPACE)(0x20)
	func_key,// 33 (VK_PRIOR)(0x21)
	func_key,// 34 (VK_NEXT)(0x22)
	func_key,// 35 (VK_END)(0x23)
	func_key,// 36 (VK_HOME)(0x24)
	func_key,// 37 (VK_LEFT)(0x25)
	func_key,// 38 (VK_UP)(0x26)
	func_key,// 39 (VK_RIGHT)(0x27)
	func_key,// 40 (VK_DOWN)(0x28)
	func_key,// 41 (VK_SELECT)(0x29)
	func_key,// 42 (VK_PRINT)(0x2A)
	func_key,// 43 (VK_EXECUTE 0x2B)
	func_key,// 44 (VK_SNAPSHOT)(0x2C)
	func_key,// 45 (VK_INSERT)(0x2D)
	func_key,// 46 (VK_DELETE)(0x2E)
	func_key,// 47 (VK_HELP)(0x2F)

	// 48->57大键盘数字键
	num_key|dp_shift,// 48
	num_key|dp_shift,
	num_key|dp_shift,
	num_key|dp_shift,
	num_key|dp_shift,
	num_key|dp_shift,
	num_key|dp_shift,
	num_key|dp_shift,
	num_key|dp_shift,
	num_key|dp_shift,// 57
	unuse_key,// 58
	unuse_key,// 59
	unuse_key,// 60
	unuse_key,// 61
	unuse_key,// 62
	unuse_key,// 63
	unuse_key,// 64
	// 65->90英文子母键
	alpha_key|dp_capslock|dp_shift,	// 65
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift,
	alpha_key|dp_capslock|dp_shift, // 90
	func_key,// 91 (VK_LWIN)()
	func_key,// 92 (VK_RWIN)
	func_key,// 93 (VK_APPS)	// 具有弹出鼠标右键菜单的功能
	unuse_key,// 94
	unuse_key,// 95

	// 小键盘
	num_key|dp_numlock,// 96 (VK_NUMPAD0)(0x60)
	num_key|dp_numlock,// 97 (VK_NUMPAD1)(0x61)
	num_key|dp_numlock,// 98 (VK_NUMPAD2)(0x62)
	num_key|dp_numlock,// 99 (VK_NUMPAD3)(0x63)
	num_key|dp_numlock,// 100 (VK_NUMPAD4)(0x64)
	num_key|dp_numlock,// 101 (VK_NUMPAD5)(0x65)
	num_key|dp_numlock,// 102 (VK_NUMPAD6)(0x66)
	num_key|dp_numlock,// 103 (VK_NUMPAD7)(0x67)
	num_key|dp_numlock,// 104 (VK_NUMPAD8)(0x68)
	num_key|dp_numlock,// 105 (VK_NUMPAD9)(0x69)
	sign_key,// 106 (VK_MULTIPLY)(0x6A) 乘号
	sign_key,// 107 (VK_ADD)(0x6B) 加号
	sign_key,// 108 (VK_SEPARATOR)(0x6C)
	sign_key,// 109 (VK_SUBTRACT)(0x6D) 减号
	sign_key,// 110 (VK_DECIMAL)(0x6E) 点号
	sign_key,// 111 (VK_DIVIDE)(0x6F) 除号

	// 功能键
	func_key,// 112 (VK_F1)(0x70)
	func_key,// 113 (VK_F2)(0x71)
	func_key,// 114 (VK_F3)(0x72)
	func_key,// 115 (VK_F4)(0x73)
	func_key,// 116 (VK_F5)(0x74)
	func_key,// 117 (VK_F6)(0x75)
	func_key,// 118 (VK_F7)(0x76)
	func_key,// 119 (VK_F8)(0x77)
	func_key,// 120 (VK_F9)(0x78)
	func_key,// 121 (VK_F10)(0x79)
	func_key,// 122 (VK_F11)(0x7A)
	func_key,// 123 (VK_F12)(0x7B)
	func_key,// 124 (VK_F13)(0x7C)
	func_key,// 125 (VK_F14)(0x7D)
	func_key,// 126 (VK_F15)(0x7E)
	func_key,// 127 (VK_F16)(0x7F)
	func_key,// 128 (VK_F17)(0x80)
	func_key,// 129 (VK_F18)(0x81)
	func_key,// 130 (VK_F19)(0x82)
	func_key,// 131 (VK_F20)(0x83)
	func_key,// 132 (VK_F21)(0x84)
	func_key,// 133 (VK_F22)(0x85)
	func_key,// 134 (VK_F23)(0x86)
	func_key,// 135 (VK_F24)(0x87)
	unuse_key,// 136
	unuse_key,// 137
	unuse_key,// 138
	unuse_key,// 139
	unuse_key,// 140
	unuse_key,// 141
	unuse_key,// 142
	unuse_key,// 143
	func_key,// 144 (VK_NUMLOCK)(0x90)
	func_key,// 145 (VK_SCROLL)(0x91)
	unuse_key,// 146
	unuse_key,// 147
	unuse_key,// 148
	unuse_key,// 149
	unuse_key,// 150
	unuse_key,// 151
	unuse_key,// 152
	unuse_key,// 153
	unuse_key,// 154
	unuse_key,// 155
	unuse_key,// 156
	unuse_key,// 157
	unuse_key,// 158
	unuse_key,// 159

	/*
	 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
	 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
	 * No other API or message will distinguish left and right keys in this way.
	 */
	unuse_key,// 160 (VK_LSHIFT)(0xA0)
	unuse_key,// 161 (VK_RSHIFT)(0xA1)
	unuse_key,// 162 (VK_LCONTROL)(0xA2)
	unuse_key,// 163 (VK_RCONTROL)(0xA3)
	unuse_key,// 164 (VK_LMENU)(0xA4)
	unuse_key,// 165 (VK_RMENU)(0xA5)
	unuse_key,// 166
	unuse_key,// 167
	unuse_key,// 168
	unuse_key,// 169
	unuse_key,// 170
	unuse_key,// 171
	unuse_key,// 172
	unuse_key,// 173
	unuse_key,// 174
	unuse_key,// 175
	unuse_key,// 176
	unuse_key,// 177
	unuse_key,// 178
	unuse_key,// 179
	unuse_key,// 180
	unuse_key,// 181
	unuse_key,// 182
	unuse_key,// 183
	unuse_key,// 184
	unuse_key,// 185
	sign_key|dp_shift,// 186 ;:
	sign_key|dp_shift,// 187 =+
	sign_key|dp_shift,// 188 ,<
	sign_key|dp_shift,// 189 -_
	sign_key|dp_shift,// 190 .>
	sign_key|dp_shift,// 191 /?
	sign_key|dp_shift,// 192 `~
	unuse_key,// 193 
	unuse_key,// 194
	unuse_key,// 195
	unuse_key,// 196
	unuse_key,// 197
	unuse_key,// 198
	unuse_key,// 199
	unuse_key,// 200
	unuse_key,// 201
	unuse_key,// 202
	unuse_key,// 203
	unuse_key,// 204
	unuse_key,// 205
	unuse_key,// 206
	unuse_key,// 207
	unuse_key,// 208
	unuse_key,// 209
	unuse_key,// 210
	unuse_key,// 211
	unuse_key,// 212
	unuse_key,// 213
	unuse_key,// 214
	unuse_key,// 215
	unuse_key,// 216
	unuse_key,// 217
	unuse_key,// 218
	sign_key|dp_shift,// 219 [{
	sign_key|dp_shift,// 220 \|
	sign_key|dp_shift,// 221 ]}
	sign_key|dp_shift,// 222 '"
	unuse_key,// 223
	unuse_key,// 224
	unuse_key,// 225
	unuse_key,// 226
	unuse_key,// 227
	unuse_key,// 228
	func_key,// 229 (VK_PROCESSKEY)(0xE5)
	unuse_key,// 230
	unuse_key,// 231
	unuse_key,// 232
	unuse_key,// 233
	unuse_key,// 234
	unuse_key,// 235
	unuse_key,// 236
	unuse_key,// 237
	unuse_key,// 238
	unuse_key,// 239
	unuse_key,// 240
	unuse_key,// 241
	unuse_key,// 242
	unuse_key,// 243
	unuse_key,// 244
	unuse_key,// 245
	func_key,// 246 (VK_ATTN)(0xF6)
	func_key,// 247 (VK_CRSEL)(0xF7)
	func_key,// 248 (VK_EXSEL)(0xF8)
	func_key,// 249 (VK_EREOF)(0xF9)
	func_key,// 250 (VK_PLAY)(0xFA)
	func_key,// 251 (VK_ZOOM)(0xFB)
	func_key,// 252 (VK_NONAME)(0xFC)
	func_key,// 253 (VK_PA1)(0xFD)
	func_key,// 254 (VK_OEM_CLEAR)(0xFE)
	unuse_key,// 255
};
//------------------------------------------------------------------------