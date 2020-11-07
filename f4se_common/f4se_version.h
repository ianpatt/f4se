#ifndef __F4SE_VERSION_H__
#define __F4SE_VERSION_H__

// these have to be macros so they can be used in the .rc
#define F4SE_VERSION_INTEGER		0
#define F4SE_VERSION_INTEGER_MINOR	6
#define F4SE_VERSION_INTEGER_BETA	21
#define F4SE_VERSION_VERSTRING		"0, 0, 6, 21"
#define F4SE_VERSION_PADDEDSTRING	"0023"
#define F4SE_VERSION_RELEASEIDX		23

#define MAKE_EXE_VERSION_EX(major, minor, build, sub)	((((major) & 0xFF) << 24) | (((minor) & 0xFF) << 16) | (((build) & 0xFFF) << 4) | ((sub) & 0xF))
#define MAKE_EXE_VERSION(major, minor, build)			MAKE_EXE_VERSION_EX(major, minor, build, 0)

#define GET_EXE_VERSION_MAJOR(a)	(((a) & 0xFF000000) >> 24)
#define GET_EXE_VERSION_MINOR(a)	(((a) & 0x00FF0000) >> 16)
#define GET_EXE_VERSION_BUILD(a)	(((a) & 0x0000FFF0) >> 4)
#define GET_EXE_VERSION_SUB(a)		(((a) & 0x0000000F) >> 0)

#define RUNTIME_VERSION_1_1_29	MAKE_EXE_VERSION(1, 1, 29)	// 0x010101D0	initial version released on steam
#define RUNTIME_VERSION_1_1_30	MAKE_EXE_VERSION(1, 1, 30)	// 0x010101E0	day1 patch to fix xaudio problem
#define RUNTIME_VERSION_1_2		MAKE_EXE_VERSION(1, 2, 33)	// 0x01020210	beta, removed mini-mod-manager from the launcher
#define RUNTIME_VERSION_1_2_37	MAKE_EXE_VERSION(1, 2, 37)	// 0x01020250	beta - unclear
#define RUNTIME_VERSION_1_3_45	MAKE_EXE_VERSION(1, 3, 45)	// 0x010302D0	beta - compiled with optimization enabled (finally), added hbao+ and cuda-based impact particles (shame)
#define RUNTIME_VERSION_1_3_47	MAKE_EXE_VERSION(1, 3, 47)	// 0x010302F0	release, originally pushed as beta
#define RUNTIME_VERSION_1_4_124	MAKE_EXE_VERSION(1, 4, 124)	// 0x010407C0	beta - preliminary internal mod manager
#define RUNTIME_VERSION_1_4_125	MAKE_EXE_VERSION(1, 4, 125)	// 0x010407D0	beta - released same day as previous version. about 300K smaller
#define RUNTIME_VERSION_1_4_131	MAKE_EXE_VERSION(1, 4, 131)	// 0x01040830	beta - 
#define RUNTIME_VERSION_1_4_132	MAKE_EXE_VERSION(1, 4, 132)	// 0x01040840	released without a beta version 
#define RUNTIME_VERSION_1_5_141	MAKE_EXE_VERSION(1, 5, 141)	// 0x010508D0	beta - survival mode improvements
#define RUNTIME_VERSION_1_5_147	MAKE_EXE_VERSION(1, 5, 147)	// 0x01050930	beta - survival mode improvements
#define RUNTIME_VERSION_1_5_151	MAKE_EXE_VERSION(1, 5, 151)	// 0x01050970	beta - survival mode improvements
#define RUNTIME_VERSION_1_5_154	MAKE_EXE_VERSION(1, 5, 154)	// 0x010509A0	beta - public editor beta
#define RUNTIME_VERSION_1_5_157	MAKE_EXE_VERSION(1, 5, 157)	// 0x010509D0	beta - released the same day
#define RUNTIME_VERSION_1_5_205	MAKE_EXE_VERSION(1, 5, 205)	// 0x01050CD0	beta - released to main
#define RUNTIME_VERSION_1_5_210	MAKE_EXE_VERSION(1, 5, 210)	// 0x01050D20	beta - no whatsnew
#define RUNTIME_VERSION_1_5_307	MAKE_EXE_VERSION(1, 5, 307)	// 0x01051330	release - exe almost identical
#define RUNTIME_VERSION_1_5_412	MAKE_EXE_VERSION(1, 5, 412)	// 0x010519C0	beta - that's a big number. added GetOrbisModInfo console cmd (does nothing)
#define RUNTIME_VERSION_1_5_414	MAKE_EXE_VERSION(1, 5, 414)	// 0x010519E0	beta - why so many releases all at once
#define RUNTIME_VERSION_1_5_416	MAKE_EXE_VERSION(1, 5, 416)	// 0x01051A00	release - why so many releases all at once
#define RUNTIME_VERSION_1_6_0	MAKE_EXE_VERSION(1, 6, 0)	// 0x01060000	beta
#define RUNTIME_VERSION_1_6_3	MAKE_EXE_VERSION(1, 6, 3)	// 0x01060030	beta - promoted to release
#define RUNTIME_VERSION_1_6_9	MAKE_EXE_VERSION(1, 6, 9)	// 0x01060090	release - no interim beta
#define RUNTIME_VERSION_1_7_7	MAKE_EXE_VERSION(1, 7, 7)	// 0x01070070	beta
#define RUNTIME_VERSION_1_7_9	MAKE_EXE_VERSION(1, 7, 9)	// 0x01070090	beta - promoted to release
#define RUNTIME_VERSION_1_7_10	MAKE_EXE_VERSION(1, 7, 10)	// 0x010700A0	release - no interim beta
#define RUNTIME_VERSION_1_7_12	MAKE_EXE_VERSION(1, 7, 12)	// 0x010700C0	release - no interim beta, wtf bethesda
#define RUNTIME_VERSION_1_7_15	MAKE_EXE_VERSION(1, 7, 15)	// 0x010700F0	release - no interim beta, released on a holiday weekend, wtf bethesda
#define RUNTIME_VERSION_1_7_19	MAKE_EXE_VERSION(1, 7, 19)	// 0x01070130	release - rolled back
#define RUNTIME_VERSION_1_7_22	MAKE_EXE_VERSION(1, 7, 22)	// 0x01070160	release - bugfix for 1.7.19
#define RUNTIME_VERSION_1_8_7	MAKE_EXE_VERSION(1, 8, 7)	// 0x01080070	release - poking at the built-in mod manager
#define RUNTIME_VERSION_1_9_4	MAKE_EXE_VERSION(1, 9, 4)	// 0x01090040	release - high-resolution texture pack
#define RUNTIME_VERSION_1_10_20	MAKE_EXE_VERSION(1, 10, 20)	// 0x010A0140	beta/release - creation club
#define RUNTIME_VERSION_1_10_26	MAKE_EXE_VERSION(1, 10, 26)	// 0x010A01A0	creation club update 2
#define RUNTIME_VERSION_1_10_40	MAKE_EXE_VERSION(1, 10, 40)	// 0x010A0280	creation club update 3 (thanks for cleaning up plugin identification)
#define RUNTIME_VERSION_1_10_50	MAKE_EXE_VERSION(1, 10, 50)	// 0x010A0320	creation club update 4
#define RUNTIME_VERSION_1_10_64	MAKE_EXE_VERSION(1, 10, 64)	// 0x010A0400	creation club update 5
#define RUNTIME_VERSION_1_10_75	MAKE_EXE_VERSION(1, 10, 75)	// 0x010A04B0	creation club update 6
#define RUNTIME_VERSION_1_10_82	MAKE_EXE_VERSION(1, 10, 82)	// 0x010A0520	creation club update 7 (startup speed?)
#define RUNTIME_VERSION_1_10_89	MAKE_EXE_VERSION(1, 10, 89)	// 0x010A0590	creation club update 8
#define RUNTIME_VERSION_1_10_98	MAKE_EXE_VERSION(1, 10, 98)	// 0x010A0620	creation club update 9
#define RUNTIME_VERSION_1_10_106	MAKE_EXE_VERSION(1, 10, 106)	// 0x010A06A0	creation club update 10 (no addresses changed)
#define RUNTIME_VERSION_1_10_111	MAKE_EXE_VERSION(1, 10, 111)	// 0x010A06F0	creation club update 11 (no addresses changed)
#define RUNTIME_VERSION_1_10_114	MAKE_EXE_VERSION(1, 10, 114)	// 0x010A0720	creation club update 12 (no addresses changed)
#define RUNTIME_VERSION_1_10_120	MAKE_EXE_VERSION(1, 10, 120)	// 0x010A0780	creation club update 13 (no addresses changed)
#define RUNTIME_VERSION_1_10_130	MAKE_EXE_VERSION(1, 10, 130)	// 0x010A0820	creation club update 14
#define RUNTIME_VERSION_1_10_138	MAKE_EXE_VERSION(1, 10, 138)	// 0x010A08A0	creation club update 15
#define RUNTIME_VERSION_1_10_162	MAKE_EXE_VERSION(1, 10, 162)	// 0x010A0A20	creation club update 16
#define RUNTIME_VERSION_1_10_163	MAKE_EXE_VERSION(1, 10, 163)	// 0x010A0A30	creation club update 17

#define PACKED_F4SE_VERSION		MAKE_EXE_VERSION(F4SE_VERSION_INTEGER, F4SE_VERSION_INTEGER_MINOR, F4SE_VERSION_INTEGER_BETA)

// information about the state of the game at the time of release
#define F4SE_TARGETING_BETA_VERSION	0
#define CURRENT_RELEASE_RUNTIME		RUNTIME_VERSION_1_10_163
#define CURRENT_RELEASE_F4SE_STR	"0.6.21"

#endif /* __F4SE_VERSION_H__ */
