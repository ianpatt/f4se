#include "f4se/GameForms.h"
#include "f4se/GameUtilities.h"

RelocAddr <_LookupFormByID> LookupFormByID(0x00152C90);

// 19AD05F2961D07B65E7987D210D6A47199FC0EFA+21
RelocPtr <IFormFactory*> g_formFactoryList(0x058D3BF0);

// 7156C2CD12E2BEAE5271E10ECFC738D8D24050B8+F1
RelocPtr <tHashSet<ObjectModMiscPair, BGSMod::Attachment::Mod*>> g_modAttachmentMap(0x036E2148-0x08);
